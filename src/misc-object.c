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

/*! \file misc-object.c
 */

#include <glib.h>
#include <glib-object.h>

void
misc_object_chain_dispose(GObject *object)
{
    GObjectClass *class = g_type_class_peek((unsigned)object);

    if (class != NULL) {

        GObjectClass *parent_class = g_type_class_peek_parent(class);

        if (parent_class != NULL) {

            parent_class->dispose(object);
        }
    }
}

void
misc_object_chain_finalize(GObject *object)
{
    GObjectClass *class = g_type_class_peek((unsigned)object);

    if (class != NULL) {

        GObjectClass *parent_class = g_type_class_peek_parent(class);

        if (parent_class != NULL) {

            parent_class->finalize(object);
        }
    }

}

void
misc_object_unref(void *object)
{
    if (object != NULL) {
        g_object_unref(object);
    }
}

