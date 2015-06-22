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

/*! \file gparts-sqlite-factory.c
 */

#include <sqlite3.h>
#include <string.h>

#include "gparts.h"

#include "gparts-sqlite-result.h"
#include "gparts-sqlite-database.h"
#include "gparts-sqlite-factory.h"

static GPartsDatabase*
gparts_sqlite_factory_create_database(GPartsDatabaseFactory *factory, GError **error);

static int
gparts_sqlite_factory_get_flags(const GPartsDatabaseFactory *factory);

static char*
gparts_sqlite_factory_get_name(const GPartsDatabaseFactory *factory);

static gboolean
gparts_sqlite_factory_validate_connect_data(const GPartsDatabaseFactory *factory, const GPartsConnectData *data);

static void
gparts_sqlite_factory_base_init(gpointer g_class)
{
}

static void
gparts_sqlite_factory_class_init(gpointer g_class, gpointer g_class_data)
{
    GPartsDatabaseFactoryClass *class = GPARTS_DATABASE_FACTORY_CLASS(g_class);

    class->create_database       = gparts_sqlite_factory_create_database;
    class->get_flags             = gparts_sqlite_factory_get_flags;
    class->get_name              = gparts_sqlite_factory_get_name;
    class->validate_connect_data = gparts_sqlite_factory_validate_connect_data;
}

static GPartsDatabase*
gparts_sqlite_factory_create_database(GPartsDatabaseFactory *factory, GError **error)
{
    return (GPartsDatabase*)gparts_sqlite_database_new();
}

static int
gparts_sqlite_factory_get_flags(const GPartsDatabaseFactory *factory)
{
    return GPARTS_DATABASE_TYPE_FLAGS_USES_FILENAME;
}

static char*
gparts_sqlite_factory_get_name(const GPartsDatabaseFactory *factory)
{
    GString *name   = g_string_new(NULL);

    g_string_printf(name, "SQLite %s", sqlite3_version);

    return g_string_free(name, FALSE);
}

unsigned int
gparts_sqlite_factory_get_type(void)
{
    static unsigned int type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(GPartsSQLiteFactoryClass),    /* class_size */
            gparts_sqlite_factory_base_init,     /* base_init */
            NULL,                                /* base_finalize */
            gparts_sqlite_factory_class_init,    /* class_init */
            NULL,                                /* class_finalize */
            NULL,                                /* class_data */
            sizeof(GPartsSQLiteFactory),         /* instance_size */
            0,                                   /* n_preallocs */
            NULL,                                /* instance_init */
            NULL                                 /* value_table */
            };

        type = g_type_register_static(GPARTS_TYPE_DATABASE_FACTORY,
                                      "gparts-sqlite-factory",
                                      &tinfo, 0);
    }

    return type;
}

GPartsSQLiteFactory*
gparts_sqlite_factory_new(void)
{
    return GPARTS_SQLITE_FACTORY(g_object_new(GPARTS_TYPE_SQLITE_FACTORY, NULL));
}

static gboolean
gparts_sqlite_factory_validate_connect_data(const GPartsDatabaseFactory *factory, const GPartsConnectData *data)
{
    return ((data != NULL) && (data->filename != NULL) && (strlen(data->filename) > 0));
}

