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

/*! \file gparts-database-type.c
 */

#include <gmodule.h>

#include "gparts.h"

#define GPARTS_DATABASE_TYPE_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE(object, GPARTS_TYPE_DATABASE_TYPE, GPartsDatabaseTypePrivate)

typedef void (*GPartsDatabaseInitFunc)(GPartsDatabaseType *database_type);

typedef struct _GPartsDatabaseTypeNode GPartsDatabaseTypeNode;
typedef struct _GPartsDatabaseTypePrivate GPartsDatabaseTypePrivate;

struct _GPartsDatabaseTypeNode
{
    gchar                 *name;
    GPartsDatabaseFactory *factory;
};

struct _GPartsDatabaseTypePrivate
{
    GArray *modules;
};

static void
gparts_database_type_class_init(gpointer g_class, gpointer g_class_data);

static void
gparts_database_type_dispose(GObject *object);

static void
gparts_database_type_finalize(GObject *object);

static void
gparts_database_type_init(GTypeInstance *instance, gpointer g_class);

/**** ****/

void
gparts_database_type_add_factory(GPartsDatabaseType *database_type, GPartsDatabaseFactory *factory)
{
    if (factory != NULL)
    {
        GPartsDatabaseTypePrivate *privat = GPARTS_DATABASE_TYPE_GET_PRIVATE(database_type);

        if (privat != NULL)
        {
            GPartsDatabaseTypeNode node;

            node.name    = gparts_database_factory_get_name(factory);
            node.factory = factory;

            g_object_ref(factory);

            g_array_append_val(privat->modules, node);
        }
    }
}

static void
gparts_database_type_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *object_class = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(g_class, sizeof(GPartsDatabaseTypePrivate));

    object_class->dispose  = gparts_database_type_dispose;
    object_class->finalize = gparts_database_type_finalize;

    ((GPartsDatabaseTypeClass*)object_class)->add_factory = gparts_database_type_add_factory;
}

static void
gparts_database_type_dispose(GObject *object)
{
    misc_object_chain_dispose(object);
}

static void
gparts_database_type_finalize(GObject *object)
{
    GPartsDatabaseTypePrivate *privat = GPARTS_DATABASE_TYPE_GET_PRIVATE(object);

    if (privat != NULL)
    {
        g_array_free(privat->modules, TRUE);
    }

    misc_object_chain_finalize(object);
}

GPartsDatabaseFactory*
gparts_database_type_get_factory(GPartsDatabaseType *database_type, const gchar *name)
{
    GPartsDatabaseFactory *factory = NULL;

    if (name != NULL)
    {
        GPartsDatabaseTypePrivate *privat = GPARTS_DATABASE_TYPE_GET_PRIVATE(database_type);

        if (privat != NULL)
        {
            gint index;

            for (index=0; index<privat->modules->len; index++)
            {
                GPartsDatabaseTypeNode *node = &g_array_index(privat->modules, GPartsDatabaseTypeNode, index);

                if (g_strcmp0(name, node->name) == 0)
                {
                    factory = node->factory;

                    if (factory != NULL)
                    {
                        g_object_ref(factory);
                    }

                    break;
                }
            }
        }
    }

    return factory;
}

gchar**
gparts_database_type_get_type_names(GPartsDatabaseType *database_type)
{
    GPartsDatabaseTypePrivate *privat = GPARTS_DATABASE_TYPE_GET_PRIVATE(database_type);
    GPtrArray *result = NULL;

    if (privat != NULL)
    {
        gint index;

        result = g_ptr_array_new();

        for (index=0; index<privat->modules->len; index++)
        {
            GPartsDatabaseTypeNode *node = &g_array_index(privat->modules, GPartsDatabaseTypeNode, index);

            g_debug("Node name %s", node->name);

            g_ptr_array_add(result, g_strdup(node->name));
        }

        g_ptr_array_add(result, NULL);
    }

    return (gchar**) g_ptr_array_free(result, FALSE);
}

GType
gparts_database_type_get_type(void)
{
    static GType type = 0;

    if (type == 0)
    {
        static const GTypeInfo tinfo = {
            sizeof(GPartsDatabaseTypeClass),
            NULL,
            NULL,
            gparts_database_type_class_init,
            NULL,
            NULL,
            sizeof(GPartsDatabaseType),
            0,
            gparts_database_type_init,
            NULL
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "GPartsDatabaseType",
            &tinfo,
            0
            );
    }

    return type;
}

static void
gparts_database_type_init(GTypeInstance* instance, gpointer g_class)
{
    GPartsDatabaseTypePrivate *privat = GPARTS_DATABASE_TYPE_GET_PRIVATE(instance);

    if (privat != NULL)
    {
        privat->modules = g_array_new(FALSE, FALSE, sizeof(GPartsDatabaseTypeNode));
    }
}


void
gparts_database_type_load_module(GPartsDatabaseType *database_type, const gchar *name, GError **error)
{
    GModule *module;
    GPartsDatabaseTypePrivate *privat = GPARTS_DATABASE_TYPE_GET_PRIVATE(database_type);

    /* find portable way to get the library directories */
    const gchar *dirs[] =
    {
        "/usr/local",
        NULL
    };

    const gchar *const *temp = dirs;

    while (*temp != NULL)
    {
        gchar *path1;
        gchar *path2;

        path1 = g_build_filename(*temp, "lib", NULL);
        path2 = g_module_build_path(path1, name);
        g_free(path1);

        module = g_module_open(path2, G_MODULE_BIND_LOCAL);
        g_free(path2);

        if (module != NULL)
        {
            break;
        }

        g_warning("%s", g_module_error());
        temp++;
    }

    if (module != NULL)
    {
        GPartsDatabaseInitFunc init_func;
        gboolean success;

        success = g_module_symbol(module, "gparts_database_register", (gpointer*) &init_func);

        if (success == FALSE)
        {
            g_warning("%s", g_module_error());
        }
        else if (init_func == NULL)
        {
            g_warning("%s", g_module_error());
        }
        else
        {
            init_func(database_type);
        }
    }
}

GPartsDatabaseType*
gparts_database_type_new(void)
{
    return GPARTS_DATABASE_TYPE(g_object_new(GPARTS_TYPE_DATABASE_TYPE, NULL));
}

