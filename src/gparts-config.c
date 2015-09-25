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

/*! \file sch-config.c
 */

#include "gparts.h"

#define GPARTS_CONFIG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),GPARTS_TYPE_CONFIG,GPartsConfigPrivate))

enum
{
    GPARTS_CONFIG_DATABASE_TYPES = 1
};

typedef struct _GPartsConfigPrivate GPartsConfigPrivate;

struct _GPartsConfigPrivate
{
    GPartsDatabaseType *database_types;
};



static void
gparts_config_class_init(gpointer g_class, gpointer g_class_data);

static void
gparts_config_dispose(GObject *object);

static void
gparts_config_finalize(GObject *object);

static void
gparts_config_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
gparts_config_init(GTypeInstance *instance, gpointer g_class);

static void
gparts_config_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

void
gparts_config_set_database_types(GPartsConfig *config, GPartsDatabaseType *types)
{
    GPartsConfigPrivate *privat = GPARTS_CONFIG_GET_PRIVATE(config);

    if (privat != NULL) {

        if (privat->database_types != NULL) {

            g_object_unref(privat->database_types);
        }

        privat->database_types = types;

        if (privat->database_types != NULL) {

            g_object_ref(privat->database_types);
        }

        g_object_notify(G_OBJECT(config), "database-types");
    }
}

static void
gparts_config_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *klasse = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(klasse, sizeof(GPartsConfigPrivate));

    klasse->dispose  = gparts_config_dispose;
    klasse->finalize = gparts_config_finalize;

    klasse->get_property = gparts_config_get_property;
    klasse->set_property = gparts_config_set_property;

    g_object_class_install_property(
        klasse,
        GPARTS_CONFIG_DATABASE_TYPES,
        g_param_spec_object(
            "datbase-types",
            "Database Types",
            "Database Types",
            GPARTS_TYPE_DATABASE_TYPE,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );
}

static void
gparts_config_dispose(GObject *object)
{
    GPartsConfigPrivate *privat = GPARTS_CONFIG_GET_PRIVATE(object);

    if (privat != NULL) {
        gparts_config_set_database_types(GPARTS_CONFIG(object), NULL);
    }

    misc_object_chain_dispose(object);
}

static void
gparts_config_finalize(GObject *object)
{
    GPartsConfigPrivate *privat = GPARTS_CONFIG_GET_PRIVATE(object);

    if (privat != NULL)
    {
    }

    misc_object_chain_finalize(object);
}

GPartsDatabaseType*
gparts_config_get_database_types(const GPartsConfig *config)
{
  GPartsDatabaseType *types = NULL;

  if (config != NULL) {

    GPartsConfigPrivate *privat = GPARTS_CONFIG_GET_PRIVATE(config);

    if (privat != NULL) {

      types = privat->database_types;

      if (types != NULL) {

        g_object_ref(types);
      }
    }
  }

  return types;
}

static void
gparts_config_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    GPartsConfig *config = GPARTS_CONFIG(object);

    if (config != NULL)
    {
        switch (property_id)
        {
            case GPARTS_CONFIG_DATABASE_TYPES:
                g_value_set_object(value, gparts_config_get_database_types(config));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

static void
gparts_config_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GPartsConfig *config = GPARTS_CONFIG(object);

    switch (property_id)
    {
        case GPARTS_CONFIG_DATABASE_TYPES:
            gparts_config_set_database_types(config, GPARTS_DATABASE_TYPE(g_value_get_object(value)));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

GType
gparts_config_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(GPartsConfigClass),    /* class_size */
            NULL,                         /* base_init */
            NULL,                         /* base_finalize */
            gparts_config_class_init,     /* class_init */
            NULL,                         /* class_finalize */
            NULL,                         /* class_data */
            sizeof(GPartsConfig),         /* instance_size */
            0,                            /* n_preallocs */
            gparts_config_init,           /* instance_init */
            NULL                          /* value_table */
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "GPartsConfig",
            &tinfo,
            0
            );
    }

    return type;
}

static void
gparts_config_init(GTypeInstance *instance, gpointer g_class)
{
    GPartsConfigPrivate *privat = GPARTS_CONFIG_GET_PRIVATE(instance);

    if (privat != NULL)
    {
        privat->database_types = gparts_database_type_new();
    }
}

GPartsConfig*
gparts_config_new(void)
{
    static GPartsConfig *config = NULL;

    if (config == NULL)
    {
        config = GPARTS_CONFIG(g_object_new(GPARTS_TYPE_CONFIG, NULL));
    }

    return config;
}
