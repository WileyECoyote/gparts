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

/*! \file gparts-mysql-database.h
 */

#define GPARTS_TYPE_MYSQL_DATABASE (gparts_mysql_database_get_type())
#define GPARTS_MYSQL_DATABASE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GPARTS_TYPE_MYSQL_DATABASE,GPartsMySQLDatabase))
#define GPARTS_MYSQL_DATABASE_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),GPARTS_TYPE_MYSQL_DATABASE,GPartsMySQLDatabaseClass))
#define GPARTS_IS_MYSQL_DATABASE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GPARTS_TYPE_MYSQL_DATABASE))
#define GPARTS_IS_MYSQL_DATABASE_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),GPARTS_TYPE_MYSQL_DATABASE))
#define GPARTS_MYSQL_DATABASE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),GPARTS_TYPE_MYSQL_DATABASE,GPartsMySQLDatabaseClass))

typedef struct _GPartsMySQLDatabase      GPartsMySQLDatabase;
typedef struct _GPartsMySQLDatabaseClass GPartsMySQLDatabaseClass;

/*! \extends GPartsDatabase */
struct _GPartsMySQLDatabase
{
    GPartsDatabase parent;
};

/*! \private */
struct _GPartsMySQLDatabaseClass
{
    GPartsDatabaseClass parent;
};

/*! \private */
unsigned int
gparts_mysql_database_get_type(void);

GPartsMySQLDatabase*
gparts_mysql_database_new();

