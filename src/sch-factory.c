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

/*! \file sch-factory.c
 */

#include <glib.h>
#include <glib-object.h>

#include "misc-object.h"

#include "sch.h"

#define SCH_FACTORY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCH_TYPE_FACTORY,SchFactoryPrivate))

enum
{
    SCH_FACTORY_CONFIG = 1,
};

typedef struct _SchFactoryPrivate SchFactoryPrivate;

struct _SchFactoryPrivate
{
    SchConfig *config;
};


static void
sch_factory_class_init(gpointer g_class, gpointer g_class_data);

static void
sch_factory_dispose(GObject *object);

static void
sch_factory_finalize(GObject *object);

static void
sch_factory_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
sch_factory_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);


static void
sch_factory_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *klasse = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(klasse, sizeof(SchFactoryPrivate));

    klasse->dispose  = sch_factory_dispose;
    klasse->finalize = sch_factory_finalize;

    klasse->get_property = sch_factory_get_property;
    klasse->set_property = sch_factory_set_property;

    g_object_class_install_property(
        klasse,
        SCH_FACTORY_CONFIG,
        g_param_spec_object(
            "config",
            "Configuration",
            "Configuration",
            SCH_TYPE_CONFIG,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );
}

static void
sch_factory_dispose(GObject *object)
{
    sch_factory_set_config(SCH_FACTORY(object), NULL);

    misc_object_chain_dispose(object);
}

static void
sch_factory_finalize(GObject *object)
{
    misc_object_chain_finalize(object);
}


SchConfig*
sch_factory_get_config(SchFactory *factory)
{
    SchConfig *config = NULL;
    SchFactoryPrivate *privat = SCH_FACTORY_GET_PRIVATE(factory);

    if (privat != NULL) {
        config = privat->config;
    }

    return config;
}

static void
sch_factory_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    SchFactoryPrivate *privat = SCH_FACTORY_GET_PRIVATE(object);

    if (privat != NULL) {

        switch (property_id) {

            case SCH_FACTORY_CONFIG:
                g_value_set_object(value, privat->config);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
sch_factory_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(SchFactoryClass),    /* class_size */
            NULL,                       /* base_init */
            NULL,                       /* base_finalize */
            sch_factory_class_init,     /* class_init */
            NULL,                       /* class_finalize */
            NULL,                       /* class_data */
            sizeof(SchFactory),         /* instance_size */
            0,                          /* n_preallocs */
            NULL,                       /* instance_init */
            NULL                        /* value_table */
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "SchFactory",
            &tinfo,
            0
            );
    }

    return type;
}

SchFactory*
sch_factory_new(SchConfig *config)
{
  return SCH_FACTORY(g_object_new(SCH_TYPE_FACTORY, "config", config, NULL));
}

void
sch_factory_set_config(SchFactory *factory, SchConfig *config)
{
    SchFactoryPrivate *privat = SCH_FACTORY_GET_PRIVATE(factory);

    if (privat != NULL) {

        if (privat->config != NULL) {
            /* g_object_unref(privat->config) */
        }

        privat->config = config;

        if (privat->config != NULL) {
            /* g_object_ref(privat->config) */
        }
    }
}

static void
sch_factory_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    SchFactory *factory = SCH_FACTORY(object);

    if (factory != NULL) {

        switch (property_id) {

            case SCH_FACTORY_CONFIG:
                sch_factory_set_config(factory, SCH_CONFIG(g_value_get_object(value)));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}
