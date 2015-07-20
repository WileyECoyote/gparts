/* gEDA - GPL Electronic Design Automation
 * gparts - gEDA Parts Manager
 * Copyright (C) 2010-2015 Edward C. Hennessy
 * Copyright (C) 2010-2015 gEDA Contributors (see ChangeLog for details)
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

/*! \file schgui-cairo-path.c
 */

#include <math.h>

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "schgui.h"
#include "misc-object.h"

#define SCHGUI_CAIRO_PATH_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCHGUI_TYPE_CAIRO_PATH,SchGUICairoPathPrivate))

typedef struct _SchGUICairoPathPrivate SchGUICairoPathPrivate;

struct _SchGUICairoPathPrivate
{
    double red;
    double green;
    double blue;
    double alpha;

    GSList *commands;

    double line_width;

    int    solid;
    int    visible;
};

static void
schgui_cairo_path_bounds(SchGUICairoDrawItem *item, cairo_t *cairo, GeomBounds *bounds);

static void
schgui_cairo_path_class_init(void *g_class, void *g_class_data);

static void
schgui_cairo_path_draw(SchGUICairoDrawItem *item, cairo_t *cairo);

static void
schgui_cairo_path_finalize(GObject *object);

static void
schgui_cairo_path_mirror_y(SchGUICairoDrawItem *item);

static void
schgui_cairo_path_rotate(SchGUICairoDrawItem *item, double dt);

static void
schgui_cairo_path_translate(SchGUICairoDrawItem *item, double dx, double dy);


void
schgui_cairo_path_append_rev(SchPathCommand *command, SchGUICairoPath *path)
{
    SchGUICairoPathPrivate *privat = SCHGUI_CAIRO_PATH_GET_PRIVATE(path);

    if (privat != NULL) {

        privat->commands = g_slist_append(privat->commands, sch_path_command_copy(command));
    }
}

static void
schgui_cairo_path_bounds(SchGUICairoDrawItem *item, cairo_t *cairo, GeomBounds *bounds)
{

#if 0
    if (bounds != NULL)
    {
        SchGUICairoPathPrivate *privat = SCHGUI_CAIRO_PATH_GET_PRIVATE(item);

        if (privat != NULL)
        {
            GeomBounds temp;

            if (privat->x[0] < privat->x[1])
            {
                temp.min_x = privat->x[0];
                temp.max_x = privat->x[1];
            }
            else
            {
                temp.min_x = privat->x[1];
                temp.max_x = privat->x[0];
            }

            if (privat->y[0] < privat->y[1])
            {
                temp.min_y = privat->y[0];
                temp.max_y = privat->y[1];
            }
            else
            {
                temp.min_y = privat->y[1];
                temp.max_y = privat->y[0];
            }

            geom_bounds_expand(&temp, &temp, privat->width);

            geom_bounds_union(bounds, bounds, &temp);
        }
    }
#endif
}

static void
schgui_cairo_path_class_init(void *g_class, void *g_class_data)
{
    SchGUICairoDrawItemClass *i_klasse = SCHGUI_CAIRO_DRAW_ITEM_CLASS(g_class);
    GObjectClass             *o_klasse = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(o_klasse, sizeof(SchGUICairoPathPrivate));

    o_klasse->finalize  = schgui_cairo_path_finalize;

    i_klasse->bounds    = schgui_cairo_path_bounds;
    i_klasse->draw      = schgui_cairo_path_draw;
    i_klasse->mirror_y  = schgui_cairo_path_mirror_y;
    i_klasse->rotate    = schgui_cairo_path_rotate;
    i_klasse->translate = schgui_cairo_path_translate;
}

static void
schgui_cairo_path_draw(SchGUICairoDrawItem *item, cairo_t *cairo)
{
    if (cairo != NULL) {

        SchGUICairoPathPrivate *privat = SCHGUI_CAIRO_PATH_GET_PRIVATE(item);

        if (privat != NULL) {

            GSList *node;
            SchPathCommand *command;

            cairo_set_source_rgba(cairo, privat->red, privat->green, privat->blue, privat->alpha);

            cairo_set_line_width(cairo, privat->line_width);

            node = privat->commands;

            while (node != NULL) {

                command = SCH_PATH_COMMAND(node->data);

                switch (command->type) {

                    case SCH_PATH_COMMAND_INVALID:
                        g_debug("SchGUICairoPath: invalid path command");
                        break;

                    case SCH_PATH_COMMAND_CLOSEPATH:
                        cairo_close_path(cairo);
                        break;

                    case SCH_PATH_COMMAND_CURVETO_ABS:
                        cairo_curve_to(
                            cairo,
                            command->curveto.x[0], command->curveto.y[0],
                            command->curveto.x[1], command->curveto.y[1],
                            command->curveto.x[2], command->curveto.y[2]
                            );
                        break;

                    case SCH_PATH_COMMAND_CURVETO_REL:
                        cairo_rel_curve_to(
                            cairo,
                            command->curveto.x[0], command->curveto.y[0],
                            command->curveto.x[1], command->curveto.y[1],
                            command->curveto.x[2], command->curveto.y[2]
                            );
                        break;

                    case SCH_PATH_COMMAND_LINETO_ABS:
                        cairo_line_to(cairo, command->lineto.x, command->lineto.y);
                        break;

                    case SCH_PATH_COMMAND_LINETO_REL:
                        cairo_rel_line_to(cairo, command->lineto.x, command->lineto.y);
                        break;

                    case SCH_PATH_COMMAND_MOVETO_ABS:
                        cairo_move_to(cairo, command->moveto.x, command->moveto.y);
                        break;

                    case SCH_PATH_COMMAND_MOVETO_REL:
                        cairo_rel_move_to(cairo, command->moveto.x, command->moveto.y);
                        break;

                    default:
                        g_debug("SchGUICairoPath: unknown path command");
                }

                node = g_slist_next(node);
            }

            if (privat->solid) {
                cairo_fill_preserve(cairo);
            }

            cairo_stroke(cairo);
        }
    }
}

static void
schgui_cairo_path_finalize(GObject *object)
{
    SchGUICairoPathPrivate *privat = SCHGUI_CAIRO_PATH_GET_PRIVATE(object);

    if (privat != NULL) {

        GSList *node = privat->commands;

        while (node != NULL) {

            sch_path_command_free(SCH_PATH_COMMAND(node->data));

            node = g_slist_next(node);
        }

        g_slist_free(privat->commands);

        privat->commands = NULL;
    }

    misc_object_chain_finalize(object);
}

GType
schgui_cairo_path_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(SchGUICairoPathClass),    /* class_size */
            NULL,                            /* base_init */
            NULL,                            /* base_finalize */
            schgui_cairo_path_class_init,    /* class_init */
            NULL,                            /* class_finalize */
            NULL,                            /* class_data */
            sizeof(SchGUICairoPath),         /* instance_size */
            0,                               /* n_preallocs */
            NULL,                            /* instance_init */
            NULL                             /* value_table */
            };

        type = g_type_register_static(
            SCHGUI_TYPE_CAIRO_DRAW_ITEM,
            "SchGUICairoPath",
            &tinfo,
            0
            );
    }

    return type;
}

static void
schgui_cairo_path_mirror_y(SchGUICairoDrawItem *item)
{
    SchGUICairoPathPrivate *privat = SCHGUI_CAIRO_PATH_GET_PRIVATE(item);

    if (privat != NULL) {

        GSList *node = privat->commands;

        while (node != NULL) {

            //sch_path_command_mirror_y(SCH_PATH_COMMAND(node->data));

            node = g_slist_next(node);
        }
    }
}

SchGUICairoPath*
schgui_cairo_path_new(const SchPath *shape, SchGUIDrawingCfg *config)
{
    SchGUICairoPath *item = SCHGUI_CAIRO_PATH(g_object_new(SCHGUI_TYPE_CAIRO_PATH, NULL));

    SchGUICairoPathPrivate *privat = SCHGUI_CAIRO_PATH_GET_PRIVATE(item);

    if (privat != NULL) {

        MiscGUIColor          color;
        SchFillStyle          fill_style;
        int                   index;
        double                item_width;
        int                   shape_width;

        index = sch_path_get_color(shape);

        privat->visible = schgui_drawing_cfg_get_color(config, index, &color);

        privat->red   = color.red;
        privat->green = color.green;
        privat->blue  = color.blue;
        privat->alpha = privat->visible ? 1.0 : 0.0;

        shape_width = sch_path_get_line_width(shape);

        schgui_drawing_cfg_get_output_line_width(config, shape_width, &item_width);

        privat->line_width = item_width;

        sch_path_get_fill_style(shape, &fill_style);

        privat->solid = (fill_style.type == SCH_FILL_STYLE_FILL_TYPE_SOLID);

        sch_path_foreach(shape, (GFunc)schgui_cairo_path_append_rev, item);
    }

    return item;
}

static void
schgui_cairo_path_rotate(SchGUICairoDrawItem *item, double dt)
{
    SchGUICairoPathPrivate *privat = SCHGUI_CAIRO_PATH_GET_PRIVATE(item);

    if (privat != NULL) {

        GSList *node = privat->commands;

        while (node != NULL) {

            sch_path_command_rotate(SCH_PATH_COMMAND(node->data), dt);

            node = g_slist_next(node);
        }
    }
}

static void
schgui_cairo_path_translate(SchGUICairoDrawItem *item, double dx, double dy)
{
    SchGUICairoPathPrivate *privat = SCHGUI_CAIRO_PATH_GET_PRIVATE(item);

    if (privat != NULL) {

        GSList *node = privat->commands;

        while (node != NULL) {

            sch_path_command_translate(SCH_PATH_COMMAND(node->data), dx, dy);

            node = g_slist_next(node);
        }
    }
}

