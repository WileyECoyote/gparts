/* gEDA - GPL Electronic Design Automation
 * gparts - gEDA Parts Manager
 * Copyright (C) 2010 Edward C. Hennessy
 * Copyright (C) 2010 gEDA Contributors (see ChangeLog for details)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 */

/*! \file schgui-cairo-pin.c
 */

#include <math.h>

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "schgui.h"

#define SCHGUI_CAIRO_PIN_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCHGUI_TYPE_CAIRO_PIN,SchGUICairoPinPrivate))

typedef struct _SchGUICairoPinPrivate SchGUICairoPinPrivate;

struct _SchGUICairoPinPrivate
{
    double red;
    double green;
    double blue;
    double alpha;

    double x[2];
    double y[2];

    double line_width;

    int    solid;
    int    visible;
};

static void
schgui_cairo_pin_bounds(SchGUICairoDrawItem *item, cairo_t *cairo, GeomBounds *bounds);

static void
schgui_cairo_pin_class_init(void *g_class, void *g_class_data);

static void
schgui_cairo_pin_draw(SchGUICairoDrawItem *shape, cairo_t *cairo);

static void
schgui_cairo_pin_mirror_y(SchGUICairoDrawItem *item);

static void
schgui_cairo_pin_rotate(SchGUICairoDrawItem *item, double dt);

static void
schgui_cairo_pin_translate(SchGUICairoDrawItem *item, double dx, double dy);


static void
schgui_cairo_pin_bounds(SchGUICairoDrawItem *item, cairo_t *cairo, GeomBounds *bounds)
{
    if (bounds != NULL) {

        SchGUICairoPinPrivate *privat = SCHGUI_CAIRO_PIN_GET_PRIVATE(item);

        if (privat != NULL) {

            GeomBounds temp;

            if (privat->x[0] < privat->x[1]) {

                temp.min_x = privat->x[0];
                temp.max_x = privat->x[1];
            }
            else {

                temp.min_x = privat->x[1];
                temp.max_x = privat->x[0];
            }

            if (privat->y[0] < privat->y[1]) {

                temp.min_y = privat->y[0];
                temp.max_y = privat->y[1];
            }
            else {

                temp.min_y = privat->y[1];
                temp.max_y = privat->y[0];
            }

            geom_bounds_expand(&temp, &temp, privat->line_width);

            geom_bounds_union(bounds, bounds, &temp);
        }
    }

}

static void
schgui_cairo_pin_class_init(void *g_class, void *g_class_data)
{
   //SchGUICairoPinClass *klasse = SCHGUI_CAIRO_PIN_CLASS(g_class);

    g_type_class_add_private(G_OBJECT_CLASS(g_class), sizeof(SchGUICairoPinPrivate));

    SCHGUI_CAIRO_DRAW_ITEM_CLASS(g_class)->bounds    = schgui_cairo_pin_bounds;
    SCHGUI_CAIRO_DRAW_ITEM_CLASS(g_class)->draw      = schgui_cairo_pin_draw;
    SCHGUI_CAIRO_DRAW_ITEM_CLASS(g_class)->mirror_y  = schgui_cairo_pin_mirror_y;
    SCHGUI_CAIRO_DRAW_ITEM_CLASS(g_class)->rotate    = schgui_cairo_pin_rotate;
    SCHGUI_CAIRO_DRAW_ITEM_CLASS(g_class)->translate = schgui_cairo_pin_translate;
}

static void
schgui_cairo_pin_draw(SchGUICairoDrawItem *shape, cairo_t *cairo)
{
  if (cairo != NULL) {

    SchGUICairoPinPrivate *privat = SCHGUI_CAIRO_PIN_GET_PRIVATE(shape);

    if (privat != NULL) {

      cairo_set_source_rgba(cairo, privat->red, privat->green, privat->blue, privat->alpha);

      cairo_set_line_width(cairo, privat->line_width);

      cairo_move_to(cairo, privat->x[0], privat->y[0]);
      cairo_line_to(cairo, privat->x[1], privat->y[1]);

      cairo_stroke(cairo);
    }
  }
}

GType
schgui_cairo_pin_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(SchGUICairoPinClass),    /* class_size */
            NULL,                           /* base_init */
            NULL,                           /* base_finalize */
            schgui_cairo_pin_class_init,    /* class_init */
            NULL,                           /* class_finalize */
            NULL,                           /* class_data */
            sizeof(SchGUICairoPin),         /* instance_size */
            0,                              /* n_preallocs */
            NULL,                           /* instance_init */
            NULL                            /* value_table */
            };

        type = g_type_register_static(
            SCHGUI_TYPE_CAIRO_DRAW_ITEM,
            "SchGUICairoPin",
            &tinfo,
            0
            );
    }

    return type;
}

static void
schgui_cairo_pin_mirror_y(SchGUICairoDrawItem *item)
{
    SchGUICairoPinPrivate *privat = SCHGUI_CAIRO_PIN_GET_PRIVATE(item);

    if (privat != NULL) {
        privat->x[0] = -privat->x[0];
        privat->x[1] = -privat->x[1];
    }
}


SchGUICairoPin*
schgui_cairo_pin_new(const SchPin *shape, SchGUIDrawingCfg *config)
{
    SchGUICairoPin *item = SCHGUI_CAIRO_PIN(g_object_new(SCHGUI_TYPE_CAIRO_PIN, NULL));

    SchGUICairoPinPrivate *privat = SCHGUI_CAIRO_PIN_GET_PRIVATE(item);

    if (privat != NULL) {

        GeomLine              line;
        MiscGUIColor          color;
        //SchFillStyle          fill_style;
        int                   index;

        sch_pin_get_color(shape, &index);

        privat->visible = schgui_drawing_cfg_get_color(config, index, &color);

        privat->red   = color.red;
        privat->green = color.green;
        privat->blue  = color.blue;
        privat->alpha = privat->visible ? 1.0 : 0.0;

        privat->line_width = schgui_drawing_cfg_get_line_width_pin(config);

        sch_pin_get_line(shape, &line);

        privat->x[0] = line.x[0];
        privat->y[0] = line.y[0];
        privat->x[1] = line.x[1];
        privat->y[1] = line.y[1];
    }

    return item;
}

static void
schgui_cairo_pin_rotate(SchGUICairoDrawItem *item, double dt)
{
    SchGUICairoPinPrivate *privat = SCHGUI_CAIRO_PIN_GET_PRIVATE(item);

    if (privat != NULL) {

        cairo_matrix_t transform;

        cairo_matrix_init_rotate(&transform, dt);

        cairo_matrix_transform_point(&transform, &(privat->x[0]), &(privat->y[0]));
        cairo_matrix_transform_point(&transform, &(privat->x[1]), &(privat->y[1]));
    }
}

static void
schgui_cairo_pin_translate(SchGUICairoDrawItem *item, double dx, double dy)
{
    SchGUICairoPinPrivate *privat = SCHGUI_CAIRO_PIN_GET_PRIVATE(item);

    if (privat != NULL) {

        privat->x[0] += dx;
        privat->y[0] += dy;
        privat->x[1] += dx;
        privat->y[1] += dy;
    }
}

