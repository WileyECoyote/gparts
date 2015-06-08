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

/*! \file gparts-mysql-result.h
 */

#define GPARTS_TYPE_MYSQL_RESULT (gparts_mysql_result_get_type())
#define GPARTS_MYSQL_RESULT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GPARTS_TYPE_MYSQL_RESULT,GPARTSMySQLResult))
#define GPARTS_MYSQL_RESULT_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),GPARTS_TYPE_MYSQL_RESULT,GPARTSMySQLResultClass))
#define GPARTS_IS_MYSQL_RESULT(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GPARTS_TYPE_MYSQL_RESULT))
#define GPARTS_IS_MYSQL_RESULT_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),GPARTS_TYPE_MYSQL_RESULT))
#define GPARTS_MYSQL_RESULT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),GPARTS_TYPE_MYSQL_RESULT,GPARTSMySQLResultClass))

typedef struct _GPartsMySQLResult GPartsMySQLResult;
typedef struct _GPartsMySQLResultClass GPartsMySQLResultClass;

/*! \extends GPartsDatabaseResult */
struct _GPartsMySQLResult
{
    GPartsDatabaseResult parent;
};

/*! \private */
struct _GPartsMySQLResultClass
{
    GPartsDatabaseResultClass parent;
};

/*! \private */
unsigned int
gparts_mysql_result_get_type(void);

GPartsMySQLResult*
gparts_mysql_result_new(MYSQL_RES *result);
