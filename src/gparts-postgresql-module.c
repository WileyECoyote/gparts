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

/*! \file gparts-postgresql-module.c 
 */

#include <glib-object.h>
#include <gmodule.h>
#include <libpq-fe.h>

#include "gparts-pg-result.h"
#include "gparts-pg-database.h"

const gchar*
g_module_check_init(GModule *module)
{
    g_module_make_resident(module);

    //GPARTS_TYPE_PG_DATABASE;
    //GPARTS_TYPE_PG_RESULT;

    return NULL;
}

void
gparts_database_register()
{
}

