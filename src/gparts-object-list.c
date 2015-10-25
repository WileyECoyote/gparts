/* gEDA - GPL Electronic Design Automation
 * gparts - gEDA Parts Manager
 * Copyright (C) 2009 Edward C. Hennessy
 * Copyright (C) 2009 gEDA Contributors (see ChangeLog for details)
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

/*! \file gparts-object-list.c
 */

#include <gtk/gtk.h>

#include "misc-object.h"

#include "gparts-object-list.h"

#define GPARTS_OBJECT_LIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),GPARTS_TYPE_OBJECT_LIST,GPartsObjectListPrivate))

typedef struct _GPartsObjectListPrivate GPartsObjectListPrivate;

struct _GPartsObjectListPrivate
{
    GSList *objects;
};

static void
gparts_object_list_add_child(GtkBuildable *buildable, GtkBuilder *builder, GObject *object, const gchar *type);

static void
gparts_object_list_buildable_init(gpointer g_iface, gpointer g_iface_data);

static void
gparts_object_list_class_init(gpointer g_class, gpointer g_class_data);

static void
gparts_object_list_dispose(GObject *object);

static void
gparts_object_list_add_child(GtkBuildable *buildable, GtkBuilder *builder, GObject *object, const gchar *type)
{
    GPartsObjectListPrivate *privat = GPARTS_OBJECT_LIST_GET_PRIVATE(buildable);

    privat->objects = g_slist_append(privat->objects, object);
}

static void
gparts_object_list_buildable_init(gpointer g_iface, gpointer g_iface_data)
{
    GtkBuildableIface *iface = (GtkBuildableIface*) g_iface;

    iface->add_child = gparts_object_list_add_child;
}

static void
gparts_object_list_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *object_class = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(object_class, sizeof(GPartsObjectListPrivate));

    object_class->dispose = gparts_object_list_dispose;
}

static void
gparts_object_list_dispose(GObject *object)
{
    GPartsObjectListPrivate *privat = GPARTS_OBJECT_LIST_GET_PRIVATE(object);

    g_slist_foreach(privat->objects, (GFunc)g_object_unref, NULL);

    g_slist_free(privat->objects);

    privat->objects = NULL;

    misc_object_chain_dispose(object);
}

GType
gparts_object_list_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(GPartsObjectListClass),    /* class_size */
            NULL,                             /* base_init */
            NULL,                             /* base_finalize */
            gparts_object_list_class_init,    /* class_init */
            NULL,                             /* class_finalize */
            NULL,                             /* class_data */
            sizeof(GPartsObjectList),         /* instance_size */
            0,                                /* n_preallocs */
            NULL,                             /* instance_init */
            NULL                              /* value_table */
            };

        static const GInterfaceInfo iinfo = {
            gparts_object_list_buildable_init,    /* interface_init */
            NULL,                                 /* interface_finalize */
            NULL                                  /* interface_data */
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "GPartsObjectList",
            &tinfo,
            0
            );

        g_type_add_interface_static(type, GTK_TYPE_BUILDABLE, &iinfo);
    }

    return type;
}
