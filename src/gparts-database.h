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

/*! \file gparts-database.h
 *
 *! \brief An abstract base class for database implementations.
 *
 *  This base class abstracts database implementations, allowing the application
 *  to use different databases such as MySQL, PostreSQL, and SQLite.
 */

#define GPARTS_TYPE_DATABASE (gparts_database_get_type())
#define GPARTS_DATABASE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GPARTS_TYPE_DATABASE,GPartsDatabase))
#define GPARTS_DATABASE_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),GPARTS_TYPE_DATABASE,GPartsDatabaseClass))
#define GPARTS_IS_DATABASE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GPARTS_TYPE_DATABASE))
#define GPARTS_IS_DATABASE_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),GPARTS_TYPE_DATABASE))
#define GPARTS_DATABASE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),GPARTS_TYPE_DATABASE,GPartsDatabaseClass))

/*! \todo Move to a boxed type in another file */
typedef struct _connect_data connect_data;

typedef struct _GPartsDatabase GPartsDatabase;
typedef struct _GPartsDatabaseClass GPartsDatabaseClass;

struct _GPartsDatabase
{
    GObject parent;
};

/*! \private */
struct _GPartsDatabaseClass
{
    GObjectClass parent;

    void (*connect)(GPartsDatabase *database, connect_data* data, GError **error);
    void (*disconnect)(GPartsDatabase *database, GError **error);
    GPartsDatabaseResult* (*query)(GPartsDatabase *database, const gchar *query, GError **error);
    const gchar* (*get_name)(GPartsDatabase *database);
};

struct _connect_data
{
    gchar *host_name;
    gchar *user_name;
    gchar *password;
    gchar *database_name;
    gchar *filename;
};

/*! \private */
GType
gparts_database_get_type(void);

/*! \brief Establishes a connection to the database.
 *
 *  \param [in] database The database to connect to.
 *  \param [in] data Data needed to establish the connection.
 *  \param [out] error An error, if any, using the GError protocol.
 */
void
gparts_database_connect(GPartsDatabase *database, connect_data* data, GError **error);

/*! \brief Establishes a connection to the database.
 *
 *  \param [in] database The database to disconnect from.
 *  \param [out] error An error, if any, using the GError protocol.
 */
void
gparts_database_disconnect(GPartsDatabase *database, GError **error);

/*! \brief Query the database
 *
 *  \param [in] database The database to query.
 *  \param [in] query The SQL query to make.
 *  \param [out] error An error, if any, using the GError protocol.
 *  \return The result from the query.  If unsuccessful, this function returns
 *  NULL.
 */
GPartsDatabaseResult*
gparts_database_query(GPartsDatabase *database, const gchar *query, GError **error);

