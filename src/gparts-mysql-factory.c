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

#include <glib-object.h>
#include <mysql.h>

#include "gparts-connect-data.h"

#include "gparts-database-result.h"
#include "gparts-database.h"
#include "gparts-database-factory.h"

#include "gparts-mysql-result.h"
#include "gparts-mysql-database.h"
#include "gparts-mysql-factory.h"

static GPartsDatabase*
gparts_mysql_factory_create_database(GPartsDatabaseFactory *factory, GError **error);

static gchar*
gparts_mysql_factory_get_name(const GPartsDatabaseFactory *factory);



static void
gparts_mysql_factory_base_init(gpointer g_class)
{
}

static void
gparts_mysql_factory_class_init(gpointer g_class, gpointer g_class_data)
{
    GPartsDatabaseFactoryClass *klasse = GPARTS_DATABASE_FACTORY_CLASS(g_class);

    klasse->create_database = gparts_mysql_factory_create_database;
    klasse->get_name        = gparts_mysql_factory_get_name;
}

static GPartsDatabase*
gparts_mysql_factory_create_database(GPartsDatabaseFactory *factory, GError **error)
{
    g_set_error(
        error,
        g_quark_from_static_string( "gparts-database-error" ),
        0,
        "%s",
        "create database functionality not implemented"
        );

    return NULL;
}

static gchar*
gparts_mysql_factory_get_name(const GPartsDatabaseFactory *factory)
{
    return g_strdup("MySQL");
}

GType
gparts_mysql_factory_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(GPartsMySQLFactoryClass),    /* class_size */
            gparts_mysql_factory_base_init,     /* base_init */
            NULL,                               /* base_finalize */
            gparts_mysql_factory_class_init,    /* class_init */
            NULL,                               /* class_finalize */
            NULL,                               /* class_data */
            sizeof(GPartsMySQLFactory),         /* instance_size */
            0,                                  /* n_preallocs */
            NULL,                               /* instance_init */
            NULL                                /* value_table */
            };

        type = g_type_register_static(
            GPARTS_TYPE_DATABASE_FACTORY,
            "gparts-mysql-factory",
            &tinfo,
            0
            );
    }

    return type;
}
