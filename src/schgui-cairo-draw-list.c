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

/*! \file schgui-cairo-draw-list.c
 */

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "schgui.h"
#include "misc-object.h"

#define SCHGUI_CAIRO_DRAW_LIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCHGUI_TYPE_CAIRO_DRAW_LIST,SchGUICairoDrawListPrivate))

typedef struct _SchGUICairoDrawListPrivate SchGUICairoDrawListPrivate;

struct _SchGUICairoDrawListPrivate
{
    GSList *items;
};

static void
schgui_cairo_draw_list_bounds(SchGUICairoDrawItem *item, cairo_t *cairo, GeomBounds *bounds);

/*
static void
schgui_cairo_draw_list_dispose(GObject *object);
*/

static void
schgui_cairo_draw_list_draw(SchGUICairoDrawItem *item, cairo_t *cairo);

static void
schgui_cairo_draw_list_mirror_y(SchGUICairoDrawItem *item);

static void
schgui_cairo_draw_list_rotate(SchGUICairoDrawItem *item, double dt);

static void
schgui_cairo_draw_list_translate(SchGUICairoDrawItem *item, double dx, double dy);


void
schgui_cairo_draw_list_append(SchGUICairoDrawList *list, SchGUICairoDrawItem* item)
{
    if (item != NULL) {

        SchGUICairoDrawListPrivate *privat = SCHGUI_CAIRO_DRAW_LIST_GET_PRIVATE(list);

        if (privat != NULL) {

            privat->items = g_slist_append(privat->items, item);

            g_object_ref(item);
        }
    }
}

static void
schgui_cairo_draw_list_bounds(SchGUICairoDrawItem *item, cairo_t *cairo, GeomBounds *bounds)
{
    SchGUICairoDrawListPrivate *privat = SCHGUI_CAIRO_DRAW_LIST_GET_PRIVATE(item);

    if (privat != NULL) {

        GSList *node = privat->items;

        while (node != NULL) {

            schgui_cairo_draw_item_bounds(SCHGUI_CAIRO_DRAW_ITEM(node->data), cairo, bounds);

            node = g_slist_next(node);
        }
    }
}

static void
schgui_cairo_draw_list_class_init(gpointer g_class, gpointer g_class_data)
{
    SchGUICairoDrawItemClass *item_class = SCHGUI_CAIRO_DRAW_ITEM_CLASS(g_class);

    g_type_class_add_private(G_OBJECT_CLASS(g_class), sizeof(SchGUICairoDrawListPrivate));

    item_class->bounds    = schgui_cairo_draw_list_bounds;
    item_class->draw      = schgui_cairo_draw_list_draw;
    item_class->mirror_y  = schgui_cairo_draw_list_mirror_y;
    item_class->rotate    = schgui_cairo_draw_list_rotate;
    item_class->translate = schgui_cairo_draw_list_translate;
}

/*
static void
schgui_cairo_draw_list_dispose(GObject *object)
{
    SchGUICairoDrawListPrivate *privat = SCHGUI_CAIRO_DRAW_LIST_GET_PRIVATE(object);

    if (privat != NULL) {

        GSList *node = privat->items;

        while (node != NULL) {

            g_object_unref(node->data);

            node = g_slist_next(node);
        }

        g_slist_free(privat->items);

        privat->items = NULL;
    }

    misc_object_chain_dispose(object);
}
*/

static void
schgui_cairo_draw_list_draw(SchGUICairoDrawItem *item, cairo_t *cairo)
{
    SchGUICairoDrawListPrivate *privat = SCHGUI_CAIRO_DRAW_LIST_GET_PRIVATE(item);

    if (privat != NULL) {

        GSList *node = privat->items;

        while (node != NULL) {

            schgui_cairo_draw_item_draw(SCHGUI_CAIRO_DRAW_ITEM(node->data), cairo);

            node = g_slist_next(node);
        }
    }
}

unsigned int
schgui_cairo_draw_list_get_type(void)
{
    static unsigned int type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)  {

        static const GTypeInfo tinfo = {
            sizeof(SchGUICairoDrawListClass),     /* class_size */
            NULL,                                 /* base_init */
            NULL,                                 /* base_finalize */
            schgui_cairo_draw_list_class_init,    /* class_init */
            NULL,                                 /* class_finalize */
            NULL,                                 /* class_data */
            sizeof(SchGUICairoDrawList),          /* instance_size */
            0,                                    /* n_preallocs */
            NULL,                                 /* instance_init */
            NULL                                  /* value_table */
            };

        type = g_type_register_static(SCHGUI_TYPE_CAIRO_DRAW_ITEM,
                                      "SchGUICairoDrawList", &tinfo, 0);
    }

    return type;
}

static void
schgui_cairo_draw_list_mirror_y(SchGUICairoDrawItem *item)
{
    SchGUICairoDrawListPrivate *privat = SCHGUI_CAIRO_DRAW_LIST_GET_PRIVATE(item);

    if (privat != NULL) {

        GSList *node = privat->items;

        while (node != NULL) {

            schgui_cairo_draw_item_mirror_y(SCHGUI_CAIRO_DRAW_ITEM(node->data));

            node = g_slist_next(node);
        }
    }
}

void
schgui_cairo_draw_list_prepend(SchGUICairoDrawList *list, SchGUICairoDrawItem* item)
{
    if (item != NULL) {

        SchGUICairoDrawListPrivate *privat = SCHGUI_CAIRO_DRAW_LIST_GET_PRIVATE(list);

        if (privat != NULL) {

            privat->items = g_slist_prepend(privat->items, item);

            g_object_ref(item);
        }
    }
}

static void
schgui_cairo_draw_list_rotate(SchGUICairoDrawItem *item, double dt)
{
    SchGUICairoDrawListPrivate *privat = SCHGUI_CAIRO_DRAW_LIST_GET_PRIVATE(item);

    if (privat != NULL) {

        GSList *node = privat->items;

        while (node != NULL) {

            schgui_cairo_draw_item_rotate(SCHGUI_CAIRO_DRAW_ITEM(node->data), dt);

            node = g_slist_next(node);
        }
    }
}

static void
schgui_cairo_draw_list_translate(SchGUICairoDrawItem *item, double dx, double dy)
{
    SchGUICairoDrawListPrivate *privat = SCHGUI_CAIRO_DRAW_LIST_GET_PRIVATE(item);

    if (privat != NULL) {

        GSList *node = privat->items;

        while (node != NULL) {

            schgui_cairo_draw_item_translate(SCHGUI_CAIRO_DRAW_ITEM(node->data), dx, dy);

            node = g_slist_next(node);
        }
    }
}
