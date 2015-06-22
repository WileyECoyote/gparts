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

/*! \file gparts-sqlite-factory.h
 *
 *  \brief A class for creating SQLite database interface objects.
 */

#define GPARTS_TYPE_SQLITE_FACTORY (gparts_sqlite_factory_get_type())
#define GPARTS_SQLITE_FACTORY(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GPARTS_TYPE_SQLITE_FACTORY,GPartsSQLiteFactory))
#define GPARTS_SQLITE_FACTORY_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),GPARTS_TYPE_SQLITE_FACTORY,GPartsSQLiteFactoryClass))
#define GPARTS_IS_SQLITE_FACTORY(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GPARTS_TYPE_SQLITE_FACTORY))
#define GPARTS_IS_SQLITE_FACTORY_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),GPARTS_TYPE_SQLITE_FACTORY))
#define GPARTS_SQLITE_FACTORY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),GPARTS_TYPE_SQLITE_FACTORY,GPartsSQLiteFactoryClass))

typedef struct _GPartsSQLiteFactory GPartsSQLiteFactory;
typedef struct _GPartsSQLiteFactoryClass GPartsSQLiteFactoryClass;

/*! \extends GPartsDatabaseFactory */
struct _GPartsSQLiteFactory
{
    GPartsDatabaseFactory parent;
};

/*! \private */
struct _GPartsSQLiteFactoryClass
{
    GPartsDatabaseFactoryClass parent;
};

/*! \private */
unsigned int         gparts_sqlite_factory_get_type(void);

GPartsSQLiteFactory *gparts_sqlite_factory_new(void);

