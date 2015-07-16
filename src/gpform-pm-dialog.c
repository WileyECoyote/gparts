/* gEDA - GPL Electronic Design Automation
 * gparts - gEDA Parts Manager
 * Copyright (C) 2011 Edward C. Hennessy
 * Copyright (C) 2011 gEDA Contributors (see ChangeLog for details)
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

/*! \file gpform-pm-dialog.c
 */

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gpform.h"


static void
gpform_pm_dialog_class_init(void *g_class, void *g_class_data);


static void
gpform_pm_dialog_class_init(void *g_class, void *g_class_data)
{
   /*  GObjectClass *klasse = G_OBJECT_CLASS(g_class); */
}

unsigned int
gpform_pm_dialog_get_type(void)
{
    static unsigned int type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(GPFormPMDialogClass),    /* class_size */
            NULL,                           /* base_init */
            NULL,                           /* base_finalize */
            gpform_pm_dialog_class_init,    /* class_init */
            NULL,                           /* class_finalize */
            NULL,                           /* class_data */
            sizeof(GPFormPMDialog),         /* instance_size */
            0,                              /* n_preallocs */
            NULL,                           /* instance_init */
            NULL                            /* value_table */
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "GPFormPMDialog",
            &tinfo,
            G_TYPE_FLAG_ABSTRACT
            );
    }

    return type;
}
