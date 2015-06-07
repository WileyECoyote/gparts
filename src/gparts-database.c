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

/*! \file gparts-database.c
 */

#include "gparts.h"

enum
{
    GPARTS_DATABASE_CONNECTED = 1
};

gboolean
gparts_database_connected_base(const GPartsDatabase *database);

static void
gparts_database_drop_base(GPartsDatabase *database, GError **error);

static gboolean
gparts_database_droppable_base(const GPartsDatabase *database);

static void
gparts_database_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
gparts_database_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);


static void
gparts_database_base_init(gpointer g_class)
{
}

static void
gparts_database_class_init(gpointer g_class, gpointer g_class_data)
{
    GPartsDatabaseClass *klasse = GPARTS_DATABASE_CLASS(g_class);

    klasse->parent.get_property = gparts_database_get_property;
    klasse->parent.set_property = gparts_database_set_property;

    klasse->connected = gparts_database_connected;
    klasse->drop      = gparts_database_drop_base;
    klasse->droppable = gparts_database_droppable_base;

    g_object_class_install_property(
        G_OBJECT_CLASS(klasse),
        GPARTS_DATABASE_CONNECTED,
        g_param_spec_boolean(
            "connected",
            "Connected",
            "Connected",
            FALSE,
            G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );
}

void
gparts_database_connect(GPartsDatabase *database, GPartsConnectData* data, GError **error)
{
    if (database != NULL) {

        GPartsDatabaseClass *klasse = GPARTS_DATABASE_GET_CLASS(database);

        if (klasse == NULL) {

            g_critical("Unable to get GPartsDatabaseClass from parameter");
        }
        else if (klasse->connect == NULL) {

            g_critical("GPartsDatabaseClass contains NULL connect() function pointer");
        }
        else {

            klasse->connect(database, data, error);
        }
    }
}

gboolean
gparts_database_connected(const GPartsDatabase *database)
{
    gboolean connected = FALSE;

    if (database != NULL) {

        GPartsDatabaseClass *klasse = GPARTS_DATABASE_GET_CLASS(database);

        if (klasse == NULL) {

            g_critical("Unable to get GPartsDatabaseClass from parameter");
        }
        else if (klasse->connected == NULL) {

            g_critical("GPartsDatabaseClass contains NULL connected() function pointer");
        }
        else {

            connected = klasse->connected(database);
        }
    }

    return connected;
}

gboolean
gparts_database_connected_base(const GPartsDatabase *database)
{
    return FALSE;
}

void
gparts_database_disconnect(GPartsDatabase *database, GError **error)
{
    if (database != NULL) {

        GPartsDatabaseClass *klasse = GPARTS_DATABASE_GET_CLASS(database);

        if (klasse == NULL) {

            g_critical("Unable to get GPartsDatabaseClass from parameter");
        }
        else if (klasse->disconnect == NULL) {

            g_critical("GPartsDatabaseClass contains NULL disconnect() function pointer");
        }
        else {

            klasse->disconnect(database, error);
        }
    }
}

void
gparts_database_drop(GPartsDatabase *database, GError **error)
{
    if (database != NULL) {

        GPartsDatabaseClass *klasse = GPARTS_DATABASE_GET_CLASS(database);

        if (klasse == NULL) {

            g_critical("Unable to get GPartsDatabaseClass from parameter");
        }
        else if (klasse->drop == NULL) {

            g_critical("GPartsDatabaseClass contains NULL drop() function pointer");
        }
        else {

            klasse->drop(database, error);
        }
    }
}

static void
gparts_database_drop_base(GPartsDatabase *database, GError **error)
{
  g_set_error( error,
               g_quark_from_static_string( "gparts-database-error" ),
               0,
               "%s",
               "Drop database functionality not implemented");
}

gboolean
gparts_database_droppable(const GPartsDatabase *database)
{
    gboolean droppable = FALSE;

    if (database != NULL) {

        GPartsDatabaseClass *klasse = GPARTS_DATABASE_GET_CLASS(database);

        if (klasse == NULL) {

            g_critical("Unable to get GPartsDatabaseClass from parameter");
        }
        else if (klasse->droppable == NULL) {

            g_critical("GPartsDatabaseClass contains NULL droppable() function pointer");
        }
        else {

            droppable = klasse->droppable(database);
        }
    }

    return droppable;
}

static gboolean
gparts_database_droppable_base(const GPartsDatabase *database)
{
    return FALSE;
}

static void
gparts_database_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    GPartsDatabase *database = GPARTS_DATABASE(object);

    if (database != NULL) {

        switch ( property_id ) {

            case GPARTS_DATABASE_CONNECTED:
                g_value_set_boolean(value, gparts_database_connected(database));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}


GType
gparts_database_get_type(void)
{
    static unsigned int type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(GPartsDatabaseClass),    /* class_size */
            gparts_database_base_init,      /* base_init */
            NULL,                           /* base_finalize */
            gparts_database_class_init,     /* class_init */
            NULL,                           /* class_finalize */
            NULL,                           /* class_data */
            sizeof(GPartsDatabase),         /* instance_size */
            0,                              /* n_preallocs */
            NULL,                           /* instance_init */
            NULL                            /* value_table */
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "gparts-database",
            &tinfo,
            G_TYPE_FLAG_ABSTRACT
            );
    }

    return type;
}

GPartsDatabaseResult*
gparts_database_query(GPartsDatabase *database, const gchar *query, GError **error)
{
    GPartsDatabaseResult* result = NULL;

    if (database != NULL) {

        GPartsDatabaseClass *klasse = GPARTS_DATABASE_GET_CLASS(database);

        if (klasse == NULL) {

            g_critical("Unable to get GPartsDatabaseClass from parameter");
        }
        else if (klasse->query == NULL) {

            g_critical("GPartsDatabaseClass contains NULL query() function pointer");
        }
        else {

            result = klasse->query(database, query, error);
        }
    }

    return result;
}

static void
gparts_database_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GPartsDatabase *database = GPARTS_DATABASE(object);

    if (database != NULL) {

        switch (property_id) {

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}
