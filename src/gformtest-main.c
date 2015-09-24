/* gEDA - GPL Electronic Design Automation
 * gsymview - gEDA Parts Manager
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

/*! \file gformtest-main.c
 */

#include <gtk/gtk.h>

#include "misc-object.h"

#include "gpdata.h"
#include "gpform.h"

GPFormUIDialog *dialog = NULL;

static void
destroy_cb(GtkWidget* widget, gpointer data)
{
    gpform_ui_dialog_get_snapshot(dialog);
    gtk_main_quit();
}


int main(int argc, char* argv[])
{
    GPFormFactory  *factory = NULL;
    GdkPixbuf *pixbuf = NULL;

    g_type_init();
    gtk_init(&argc, &argv);

    factory = gpform_factory_new();

    if (factory != NULL) {

        dialog = gpform_factory_create_form(factory, "../xml/forms/inductor-add.xml");
    }

    if (dialog != NULL) {

        g_signal_connect(G_OBJECT(dialog), "response", G_CALLBACK(destroy_cb), NULL);

        gtk_widget_show_all(GTK_WIDGET(dialog));


        gpform_ui_dialog_set_model(dialog, NULL);
    }

    {
        GPDataFactory *factory = gpdata_factory_new();

        gpdata_factory_create_model(factory, "data-model.xml");
    }



    gtk_main();

    g_object_unref(factory);

    return 0;
}
