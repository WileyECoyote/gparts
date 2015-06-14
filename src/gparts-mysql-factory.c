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
#include <string.h>
#include <glib-object.h>
#include <mysql.h>

#include "gparts.h"

#include "gparts-mysql-result.h"
#include "gparts-mysql-database.h"
#include "gparts-mysql-factory.h"

static GPartsDatabase*
gparts_mysql_factory_create_database(GPartsDatabaseFactory *factory, GError **error);

static int
gparts_mysql_factory_get_flags(const GPartsDatabaseFactory *factory);

static char*
gparts_mysql_factory_get_name(const GPartsDatabaseFactory *factory);

static gboolean
gparts_mysql_factory_validate_connect_data(const GPartsDatabaseFactory *factory, const GPartsConnectData *data);


static void
gparts_mysql_factory_base_init(gpointer g_class)
{
}

static void
gparts_mysql_factory_class_init(gpointer g_class, gpointer g_class_data)
{
    GPartsDatabaseFactoryClass *klasse = GPARTS_DATABASE_FACTORY_CLASS(g_class);

    klasse->create_database       = gparts_mysql_factory_create_database;
    klasse->get_flags             = gparts_mysql_factory_get_flags;
    klasse->get_name              = gparts_mysql_factory_get_name;
    klasse->validate_connect_data = gparts_mysql_factory_validate_connect_data;
}

static GPartsDatabase*
gparts_mysql_factory_create_database(GPartsDatabaseFactory *factory, GError **error)
{
    return (GPartsDatabase*)gparts_mysql_database_new();
}

static int
gparts_mysql_factory_get_flags(const GPartsDatabaseFactory *factory)
{
   return
        GPARTS_DATABASE_TYPE_FLAGS_USES_USERNAME |
        GPARTS_DATABASE_TYPE_FLAGS_USES_PASSWORD |
        GPARTS_DATABASE_TYPE_FLAGS_USES_HOSTNAME |
        GPARTS_DATABASE_TYPE_FLAGS_USES_DATABASE ;
}

static char*
gparts_mysql_factory_get_name(const GPartsDatabaseFactory *factory)
{
    GString *name   = g_string_new(NULL);
    long    version = mysql_get_client_version();

    g_string_printf(name,
                    "MySQL %d.%d.%d",
                    version / 10000,(version / 100) % 100, version % 100);

    return g_string_free(name, FALSE);
}

GType
gparts_mysql_factory_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

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

        type = g_type_register_static(GPARTS_TYPE_DATABASE_FACTORY,
                                      "gparts-mysql-factory",
                                      &tinfo, 0);
    }

    return type;
}

GPartsMySQLFactory*
gparts_mysql_factory_new(void)
{
    return GPARTS_MYSQL_FACTORY(g_object_new(GPARTS_TYPE_MYSQL_FACTORY, NULL));
}

static int
gparts_mysql_factory_validate_connect_data(const GPartsDatabaseFactory *factory,
                                           const GPartsConnectData     *data)
{
  int result;

  result = (
    (data != NULL) &&
    (data->username != NULL) &&
    (strlen(data->username) > 0) &&
    (data->password != NULL) &&
    (data->hostname != NULL ) &&
    (strlen(data->hostname) > 0) &&
    (data->database != NULL) &&
    (strlen(data->database) > 0)
  );
  return result;
}

