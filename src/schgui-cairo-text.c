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

/*! \file schgui-cairo-text.c
 */

#include <string.h>

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "schgui.h"

#define SCHGUI_CAIRO_TEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCHGUI_TYPE_CAIRO_TEXT,SchGUICairoTextPrivate))

typedef struct _SchGUICairoTextPrivate SchGUICairoTextPrivate;

struct _SchGUICairoTextPrivate
{
    double red;
    double green;
    double blue;
    double alpha;

    double x;
    double y;

    double angle;

    char   *markup;

    PangoFontDescription *font_desc;

        int alignment;

    int    visible;
};

static void
schgui_cairo_text_bounds(SchGUICairoDrawItem *item, cairo_t *cairo, GeomBounds *bounds);

static void
schgui_cairo_text_class_init(void *g_class, void *g_class_data);

static void
schgui_cairo_text_draw(SchGUICairoDrawItem *item, cairo_t *cairo);

static void
schgui_cairo_text_mirror_y(SchGUICairoDrawItem *item);

static void
schgui_cairo_text_rotate(SchGUICairoDrawItem *item, double dt);

static void
schgui_cairo_text_translate(SchGUICairoDrawItem *item, double dx, double dy);


static void
schgui_cairo_text_bounds(SchGUICairoDrawItem *item, cairo_t *cairo, GeomBounds *bounds)
{
  if (bounds != NULL) {

    SchGUICairoTextPrivate *privat = SCHGUI_CAIRO_TEXT_GET_PRIVATE(item);

    if (privat != NULL) {
      ;
    }
  }
}

static void
schgui_cairo_text_class_init (void *g_class, void *g_class_data)
{
    SchGUICairoDrawItemClass *klasse = SCHGUI_CAIRO_DRAW_ITEM_CLASS(g_class);

    g_type_class_add_private(G_OBJECT_CLASS(g_class), sizeof(SchGUICairoTextPrivate));

    klasse->bounds    = schgui_cairo_text_bounds;
    klasse->draw      = schgui_cairo_text_draw;
    klasse->mirror_y  = schgui_cairo_text_mirror_y;
    klasse->rotate    = schgui_cairo_text_rotate;
    klasse->translate = schgui_cairo_text_translate;
}

static void
schgui_cairo_text_draw(SchGUICairoDrawItem *item, cairo_t *cairo)
{
  if (cairo != NULL) {

    SchGUICairoTextPrivate *privat = SCHGUI_CAIRO_TEXT_GET_PRIVATE(item);

    if (privat != NULL) {

      PangoLayout *layout = pango_cairo_create_layout(cairo);

      if (layout != NULL) {

        PangoLayoutIter *iter;
        int width;

        cairo_save(cairo);

        cairo_set_source_rgba(cairo, privat->red, privat->green, privat->blue, privat->alpha);

        pango_cairo_context_set_resolution(pango_layout_get_context(layout), 936);
        pango_layout_set_spacing(layout, 40000);
        pango_layout_set_font_description(layout, privat->font_desc);

        pango_layout_set_markup(layout, privat->markup, -1);

        cairo_move_to(cairo, privat->x, privat->y);
        cairo_rotate(cairo, privat->angle);
        cairo_scale(cairo, 1, -1);

        switch (privat->alignment)
        {
          case 2:
          case 5:
          case 8:
            /* upper */
            break;

          case 1:
          case 4:
          case 7:
            /* center */
            iter = pango_layout_get_iter(layout);
            while (!pango_layout_iter_at_last_line(iter))
            {
              pango_layout_iter_next_line(iter);
            }
            cairo_rel_move_to(cairo, 0, -pango_layout_iter_get_baseline(iter) / PANGO_SCALE / 2);
            pango_layout_iter_free(iter);
            break;

          case 0:
          case 3:
          case 6:
          default:
            /* lower */
            iter = pango_layout_get_iter(layout);
            while (!pango_layout_iter_at_last_line(iter))
            {
              pango_layout_iter_next_line(iter);
            }
            cairo_rel_move_to(cairo, 0, -pango_layout_iter_get_baseline(iter) / PANGO_SCALE);
            pango_layout_iter_free(iter);
        }

        switch (privat->alignment)
        {

          case 3:
          case 4:
          case 5:
            /* center */
            pango_layout_get_size(layout, &width, NULL);
            cairo_rel_move_to(cairo, -width / PANGO_SCALE / 2, 0);
            break;

          case 6:
          case 7:
          case 8:
            pango_layout_get_size(layout, &width, NULL);
            cairo_rel_move_to(cairo, -width / PANGO_SCALE, 0);
            /* right */
            break;

          case 0:
          case 1:
          case 2:
          default:
            /* left */
            ;
        }

        pango_cairo_show_layout(cairo, layout);

        cairo_restore(cairo);

        g_object_unref(layout);
      }
    }
  }
}

GType
schgui_cairo_text_get_type(void)
{
  static GType type = G_TYPE_INVALID;

  if (type == G_TYPE_INVALID) {

    static const GTypeInfo tinfo = {
      sizeof(SchGUICairoTextClass),    /* class_size */
      NULL,                            /* base_init */
      NULL,                            /* base_finalize */
      schgui_cairo_text_class_init,    /* class_init */
      NULL,                            /* class_finalize */
      NULL,                            /* class_data */
      sizeof(SchGUICairoText),         /* instance_size */
      0,                               /* n_preallocs */
      NULL,                            /* instance_init */
      NULL                             /* value_table */
    };

    type = g_type_register_static(SCHGUI_TYPE_CAIRO_DRAW_ITEM,
                                  "SchGUICairoText", &tinfo, 0);
  }

  return type;
}

static void
schgui_cairo_text_mirror_y(SchGUICairoDrawItem *item)
{
  SchGUICairoTextPrivate *privat = SCHGUI_CAIRO_TEXT_GET_PRIVATE(item);

  if (privat != NULL) {

    /*! \todo Implement mirror for text */

    privat->x = -privat->x;

    switch (privat->alignment) {

      case 0:
        privat->alignment = 6;
        break;

      case 1:
        privat->alignment = 7;
        break;

      case 2:
        privat->alignment = 8;
        break;

      case 6:
        privat->alignment = 0;
        break;

      case 7:
        privat->alignment = 1;
        break;

      case 8:
        privat->alignment = 2;
    }
  }
}

SchGUICairoText*
schgui_cairo_text_new(const SchText *shape, SchGUIDrawingCfg *config)
{
  SchGUICairoText *item = NULL;

  if (sch_text_get_visible(shape)) {

    item = SCHGUI_CAIRO_TEXT(g_object_new(SCHGUI_TYPE_CAIRO_TEXT, NULL));

    SchGUICairoTextPrivate *privat = SCHGUI_CAIRO_TEXT_GET_PRIVATE(item);

    if (privat != NULL) {

      MiscGUIColor          color;
      int                   index;
//    int                   show;

      index = sch_text_get_color(shape);

      privat->visible = schgui_drawing_cfg_get_color(config, index, &color);

      privat->red   = color.red;
      privat->green = color.green;
      privat->blue  = color.blue;
      privat->alpha = privat->visible ? 1.0 : 0.0;

      privat->alignment = sch_text_get_alignment(shape);
      privat->angle = geom_angle_radians(sch_text_get_angle(shape));
      privat->x = sch_text_get_x(shape);
      privat->y = sch_text_get_y(shape);

      privat->font_desc = pango_font_description_from_string("Sans");

      pango_font_description_set_size(privat->font_desc, PANGO_SCALE * sch_text_get_size(shape));

      {
        char *temp = sch_text_get_shown_string(shape);

        if (temp == NULL) {

          temp = strdup("ERROR");
        }

        privat->markup = temp;
      }
    }
  }

  return item;
}

#if 0
static void
schgui_cairo_drafter_draw_text(SchGUICairoDrafter *drafter, const struct _SchText *text)
{
    if (text != NULL)
    {
        int visible;

        sch_text_get_visible(text, &visible);

        if (visible)
        {

    SchGUICairoDrafterPrivate *privat = SCHGUI_CAIRO_DRAFTER_GET_PRIVATE(drafter);

    if (privat->cairo != NULL) {

        PangoLayout *layout;
        SchMultiline *multiline = sch_text_get_multiline(text);
        int point_size = sch_text_get_size(text);
        float height;
        int alignment;
        cairo_font_options_t *options;
        PangoContext *context;
        int baseline;
        PangoLayoutIter *iter;
        int index;
        int show;
        SchGUIDrawingCfgColor color;
        int          enabled;

        sch_text_get_color(text, &index);

        enabled = schgui_drawing_cfg_get_color(privat->config, index, &color);

        if (enabled) {

            if (0) { /* show ink rect */

                GeomBounds bounds;
                int        success;

                success = schgui_cairo_drafter_text_bounds(drafter, text, &bounds);

                if (success) {

                    cairo_set_source_rgb(privat->cairo, 1.0, 0, 0);

                    cairo_move_to(privat->cairo, bounds.min_x, bounds.min_y);
                    cairo_line_to(privat->cairo, bounds.max_x, bounds.min_y);

                    cairo_stroke(privat->cairo);

                    cairo_set_source_rgb(privat->cairo, 0.75, 0, 0);

                    cairo_move_to(privat->cairo, bounds.max_x, bounds.min_y);
                    cairo_line_to(privat->cairo, bounds.max_x, bounds.max_y);
                    cairo_line_to(privat->cairo, bounds.min_x, bounds.max_y);
                    cairo_line_to(privat->cairo, bounds.min_x, bounds.min_y);
                    //cairo_close_path(privat->cairo);

                    cairo_stroke(privat->cairo);

                    cairo_set_source_rgb(privat->cairo, 0, 0, 0);
                }

                cairo_set_source_rgb(privat->cairo, 0, 0, 1.0);
                cairo_new_sub_path(privat->cairo);
                cairo_arc(privat->cairo, sch_text_get_x(text), sch_text_get_y(text), 10, 0, 2 * M_PI);
                cairo_stroke(privat->cairo);
                cairo_set_source_rgb(privat->cairo, 0, 0, 0);
            }

            cairo_save(privat->cairo);

            height = 1000 * point_size / 72;

            layout = pango_cairo_create_layout(privat->cairo);
            pango_cairo_context_set_resolution(pango_layout_get_context(layout), 936);

//  context = pango_layout_get_context(layout);
//  options = cairo_font_options_create ();
//  cairo_font_options_set_hint_metrics (options, CAIRO_HINT_METRICS_OFF);
//  cairo_font_options_set_hint_style (options, CAIRO_HINT_STYLE_MEDIUM);
//  pango_cairo_context_set_font_options (context, options);
//  cairo_font_options_destroy (options);


            cairo_set_source_rgb(privat->cairo, color.red, color.green, color.blue);


            pango_font_description_set_size(privat->desc, point_size * PANGO_SCALE );
            pango_layout_set_spacing(layout, 40000);

            pango_layout_set_font_description(layout, privat->desc);

            sch_text_get_show(text, &show);
            pango_layout_set_markup(layout, sch_multiline_peek_markup(multiline, show), -1);


            PangoFontMetrics *metrics = pango_context_get_metrics(
                pango_layout_get_context(layout),
                privat->desc,
                NULL
                );


            cairo_move_to(privat->cairo, sch_text_get_x(text), sch_text_get_y(text));

            cairo_rotate(privat->cairo, M_PI * sch_text_get_angle(text) / 180);

            cairo_scale(privat->cairo, 1, -1);

            baseline = pango_layout_get_baseline(layout);

            alignment = sch_text_get_alignment(text);
#if 1
            switch (alignment)
            {
                case 2:
                case 5:
                case 8:
                /* upper */
                //cairo_rel_move_to(privat->cairo, 0, -pango_font_metrics_get_ascent(metrics)/(privat->zoom * PANGO_SCALE));
                //cairo_rel_move_to(privat->cairo, 0, height);
                break;

                case 1:
                case 4:
                case 7:
                    /* center */
                    cairo_rel_move_to(privat->cairo, 0, -pango_font_metrics_get_ascent(metrics)/(privat->zoom * PANGO_SCALE));
                    cairo_rel_move_to(privat->cairo, 0, height);
                    cairo_rel_move_to(privat->cairo, 0, -pango_font_metrics_get_ascent(metrics) * sch_multiline_lines(multiline)/(2 * privat->zoom * PANGO_SCALE));
                    cairo_rel_move_to(privat->cairo, 0, -pango_font_metrics_get_descent(metrics) * (sch_multiline_lines(multiline) - 1)/(2 * privat->zoom * PANGO_SCALE));
                    break;

                case 0:
                case 3:
                case 6:
                default:
                    /* lower */
                    //cairo_rel_move_to(privat->cairo, 0, -pango_font_metrics_get_ascent(metrics) * sch_multiline_lines(multiline)/(privat->zoom * PANGO_SCALE));
                    //cairo_rel_move_to(privat->cairo, 0, -pango_font_metrics_get_descent(metrics) * (sch_multiline_lines(multiline)-1)/(privat->zoom * PANGO_SCALE));
                    //cairo_rel_move_to(privat->cairo, 0, -pango_layout_get_spacing(layout) * (sch_multiline_lines(multiline)-1)/ PANGO_SCALE);
                    iter = pango_layout_get_iter(layout);
                    while (!pango_layout_iter_at_last_line(iter))
                    {
                        pango_layout_iter_next_line(iter);
                    }
                    cairo_rel_move_to(privat->cairo, 0, -pango_layout_iter_get_baseline(iter) / PANGO_SCALE);
                    pango_layout_iter_free(iter);
            }
#endif

            //g_debug("Ascent:    %d", pango_font_metrics_get_ascent(metrics));
            //g_debug("Descent:   %d", pango_font_metrics_get_descent(metrics));
            //g_debug("Spacing:   %d", pango_layout_get_spacing(layout));
            //g_debug("Font size: %d", pango_font_description_get_size(privat->desc));
            //g_debug("Baseline   %d", pango_layout_get_baseline(layout));

            pango_font_metrics_unref(metrics);

            pango_cairo_show_layout(privat->cairo, layout);

            cairo_restore(privat->cairo);

            g_object_unref(layout);
        }
    }
}
}

}
#endif

static void
schgui_cairo_text_rotate(SchGUICairoDrawItem *item, double dt)
{
  SchGUICairoTextPrivate *privat = SCHGUI_CAIRO_TEXT_GET_PRIVATE(item);

  if (privat != NULL) {

    cairo_matrix_t transform;

    cairo_matrix_init_rotate(&transform, dt);

    cairo_matrix_transform_point(&transform, &(privat->x), &(privat->y));

    privat->angle += dt;
  }
}

static void
schgui_cairo_text_translate(SchGUICairoDrawItem *item, double dx, double dy)
{
    SchGUICairoTextPrivate *privat = SCHGUI_CAIRO_TEXT_GET_PRIVATE(item);

    if (privat != NULL) {
        privat->x += dx;
        privat->y += dy;
    }
}

