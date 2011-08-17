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

#include "gpform.h"

static void
destroy_cb(GtkWidget* widget, gpointer data)
{
    gtk_main_quit();
}


int main(int argc, char* argv[])
{
    GPFormUIDialog *dialog = NULL;
    GPFormFactory  *factory = NULL;
    GdkPixbuf *pixbuf = NULL;

    g_type_init();
    gtk_init(&argc, &argv);

    factory = gpform_factory_new();

    if (factory != NULL)
    {
        dialog = gpform_factory_create_form(factory, "../xml/forms/inductor-add.xml");
    }

    if (dialog != NULL)
    {

        g_signal_connect(
            G_OBJECT(dialog),
            "response",
            G_CALLBACK(destroy_cb),
            NULL
            );

        gtk_widget_show_all(GTK_WIDGET(dialog));

        pixbuf = gpform_ui_dialog_get_snapshot(dialog);
    }

    if (pixbuf != NULL)
    {
        g_debug("%s", G_OBJECT_TYPE_NAME(pixbuf));

        gdk_pixbuf_save(
            pixbuf,
            "screenshot",
            "png",
            NULL,
            NULL
            );

        g_object_unref(pixbuf);
    }

    gtk_main();

    g_object_unref(factory);

    return 0;
}

