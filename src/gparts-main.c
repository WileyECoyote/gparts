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

/*! \file gparts-main.c
 */

#include <dlfcn.h>

#include "gpartsui.h"
#include "gpform.h"
#include "gpview.h"

#include "scmcfg-config.h"
#include "scmcfg-dirs.h"

#define GPARTS_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE(object, GPARTS_TYPE, GPartsPrivate)

typedef struct _GPartsPrivate GPartsPrivate;

struct _GPartsPrivate
{
    GtkBuilder             *builder;

    GPViewFactory          *view_factory;

    GPartsUIConnectModel   *connect_model;
    GPartsUIDatabaseModel  *database_model;

    /* These view objects are inserted into the UI. They will be disposed
     * of when the toplevel widget is destroyed.
     */

    GtkNotebook         *notebook;

    GPViewCompanyView   *company_view;
    GPViewDeviceView    *device_view;
    GPViewDocumentView  *document_view;
    GPViewFootprintView *footprint_view;
    GPViewPackageView   *package_view;
    GPViewPartView      *part_view;
    GPViewSymbolView    *symbol_view;

    GtkWidget           *current_view;

};

/**** Static methods ****/

static void
gparts_instance_init(GTypeInstance *instance, gpointer g_class);

static void
gparts_dispose(GObject *object);

static void
gparts_finalize(GObject *object);

static void
gparts_set_database(GParts *gparts, GPartsDatabase *database);

static void
gparts_set_notebook(GParts *gparts, GtkNotebook *notebook);

/**** Signal handlers ****/

static void
gparts_action_edit_preferences_cb(GtkWidget* widget, gpointer data);

static void
gparts_set_current_controller(GParts *gparts, GPartsController *controller);

static void
gparts_destroy_cb(GtkWidget* widget, gpointer data);

static void
gparts_switch_page_cb(GtkNotebook *notebook, gpointer unused, gint page_no, gpointer user_data);



static void
gparts_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass* object_class = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(g_class, sizeof(GPartsPrivate));

    object_class->dispose      = gparts_dispose;
    object_class->finalize     = gparts_finalize;
}

static void
gparts_destroy_cb(GtkWidget* widget, gpointer data)
{
    g_object_unref(G_OBJECT(data));
    gtk_main_quit();
}

static void
gparts_dispose(GObject *object)
{
    GPartsPrivate *privat = GPARTS_GET_PRIVATE(object);

    if (privat != NULL)
    {
        misc_object_unref(privat->builder);
        privat->builder = NULL;

        misc_object_unref(privat->database_model);
        privat->database_model = NULL;

        misc_object_unref(privat->view_factory);
        privat->view_factory = NULL;

#if 0
        misc_object_unref(privat->company_view);
        privat->company_view = NULL;

        misc_object_unref(privat->device_view);
        privat->device_view = NULL;

        misc_object_unref(privat->document_view);
        privat->document_view = NULL;

        misc_object_unref(privat->footprint_view);
        privat->footprint_view = NULL;

        misc_object_unref(privat->package_view);
        privat->package_view = NULL;

        misc_object_unref(privat->part_view);
        privat->part_view = NULL;

        misc_object_unref(privat->symbol_view);
        privat->symbol_view = NULL;
#endif
    }

    misc_object_chain_dispose(object);
}

static void
gparts_finalize(GObject *object)
{
    misc_object_chain_finalize(object);
}

GType
gparts_get_type( void )
{
    static GType type = 0;

    if ( type == 0 )
    {
        static const GTypeInfo info = {
            sizeof(GPartsClass),
            NULL,
            NULL,
            gparts_class_init,
            NULL,
            NULL,
            sizeof(GParts),
            0,
            gparts_instance_init,
            NULL
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "gparts",
            &info,
            0
            );
    }

    return type;
}

GParts *gparts_new(void)
{
    return g_object_new(GPARTS_TYPE, NULL);
}

static void
gparts_instance_init(GTypeInstance* instance, gpointer g_class)
{
    GtkAction *action;
    GError* error = NULL;
    guint status;
    GPartsPrivate *privat = GPARTS_GET_PRIVATE(instance);
    char *datadir = scmcfg_dirs_find_gparts_data();
    char *path;

    GPARTS_TYPE_PREVIEW;
    //GPARTS_TYPE_RESULT_TREE;
    GPARTS_TYPE_OBJECT_LIST;
    GPARTS_TYPE_RESULT_VIEW;

    SCHGUI_TYPE_DRAWING_VIEW;



    privat->builder = gtk_builder_new();

    path = g_build_filename(datadir, "xml", "gparts.xml", NULL);

    status = gtk_builder_add_from_file(
        privat->builder,
        path,
        &error
        );

    g_free(path);

    if ( status == 0 )
    {
       g_error("%s", error->message) ;
    }

    GtkUIManager *ui_manager = GTK_UI_MANAGER(gtk_builder_get_object(privat->builder, "uimanager"));

    privat->view_factory = gpview_factory_new_with_ui_manager(ui_manager);

    path = g_build_filename(datadir, "xml", "gparts-ui.xml", NULL);
    g_free(datadir);

    gtk_ui_manager_add_ui_from_file(ui_manager, path, NULL);
    g_free(path);

    gtk_ui_manager_ensure_update(ui_manager);

    gtk_builder_connect_signals(privat->builder, NULL);

    GtkWidget *widget = GTK_WIDGET(gtk_builder_get_object(privat->builder, "main"));

    g_signal_connect(
        G_OBJECT(widget),
        "destroy",
        G_CALLBACK(gparts_destroy_cb),
        instance
        );

    /**** Controllers ****/

    //GObject *customize_ctrl = g_object_new(
    //    GPARTS_TYPE_CUSTOMIZE_CTRL,
    //    "customize-action", GTK_ACTION(gtk_builder_get_object(privat->builder, "view-customize")),
    //    NULL
    //    );

    /**** Presentation Models ****/

    privat->database_model = g_object_new(
        GPARTSUI_TYPE_DATABASE_MODEL,
        NULL
        );

    privat->connect_model = g_object_new(
        GPARTSUI_TYPE_CONNECT_MODEL,
        "database-model",    privat->database_model,
        "database-name",     "GParts",
        "hostname",          "localhost",
        "username",          g_get_user_name(),
        NULL
        );

#if 0
    privat->company_model = g_object_new(
        GPARTSUI_TYPE_COMPANY_MODEL,
        "database-model",    privat->database_model,
        NULL
        );

    privat->document_model = g_object_new(
        GPARTSUI_TYPE_DOCUMENT_MODEL,
        "database-model",    privat->database_model,
        NULL
        );
#endif

    /**** Presentation Controllers ****/

    GObject *connect_controller = g_object_new(
        GPARTSUI_TYPE_CONNECT_CONTROLLER,
        "connect-action",    gtk_builder_get_object(privat->builder, "database-connect"),
        "connect-model",     privat->connect_model,
        NULL
        );

    GObject *login_ctrl = g_object_new(
        GPARTSUI_TYPE_DATABASE_CONTROLLER,
        "database-model",    privat->database_model,
        "disconnect-action", gtk_builder_get_object(privat->builder, "database-disconnect"),
        "drop-action",       gtk_builder_get_object(privat->builder, "database-drop"),
        "refresh-action",    gtk_builder_get_object(privat->builder, "view-refresh"),
        NULL
        );

    privat->company_view = gpview_factory_create_company_view(privat->view_factory);
    privat->device_view = gpview_factory_create_device_view(privat->view_factory);
    privat->document_view = gpview_factory_create_document_view(privat->view_factory);
    privat->footprint_view = gpview_factory_create_footprint_view(privat->view_factory);
    privat->package_view = gpview_factory_create_package_view(privat->view_factory);
    privat->part_view = gpview_factory_create_part_view(privat->view_factory);
    privat->symbol_view = gpview_factory_create_symbol_view(privat->view_factory);

    gparts_set_notebook(
        GPARTS(instance),
        GTK_NOTEBOOK(gtk_builder_get_object(privat->builder, "notebook"))
        );

    gtk_notebook_append_page(
        privat->notebook,
        GTK_WIDGET(privat->company_view),
        gtk_label_new("Companies")
        );

    gtk_notebook_append_page(
        privat->notebook,
        GTK_WIDGET(privat->device_view),
        gtk_label_new("Devices")
        );

    gtk_notebook_append_page(
        privat->notebook,
        GTK_WIDGET(privat->document_view),
        gtk_label_new("Documents")
        );

    gtk_notebook_append_page(
        privat->notebook,
        GTK_WIDGET(privat->footprint_view),
        gtk_label_new("Footprints")
        );

    gtk_notebook_append_page(
        privat->notebook,
        GTK_WIDGET(privat->package_view),
        gtk_label_new("Packages")
        );

    gtk_notebook_append_page(
        privat->notebook,
        GTK_WIDGET(privat->part_view),
        gtk_label_new("Parts")
        );

    gtk_notebook_append_page(
        privat->notebook,
        GTK_WIDGET(privat->symbol_view),
        gtk_label_new("Symbols")
        );

    g_object_bind_property(
        privat->database_model,
        "database",
        privat->view_factory,
        "database",
        G_BINDING_DEFAULT
        );

    gtk_window_set_default_size(GTK_WINDOW(widget), 900, 600);

    gtk_widget_show_all(widget);
}

static void
gparts_page_added_cb(GtkNotebook *notebook, GtkWidget *child, gint page_no, gpointer user_data)
{
    GPartsPrivate *privat = GPARTS_GET_PRIVATE(user_data);

    g_debug("Page added %d", page_no);

    if (privat != NULL)
    {
        if (GPVIEW_IS_VIEW_INTERFACE(child))
        {
            g_object_bind_property(
                privat->database_model,
                "database",
                child,
                "database",
                G_BINDING_DEFAULT
                );
        }
    }
}

static void
gparts_page_removed_cb(GtkNotebook *notebook, GtkWidget *child, gint page_no, gpointer user_data)
{
    g_debug("Page removed %d", page_no);
}

static void
gparts_set_notebook(GParts *gparts, GtkNotebook *notebook)
{
    GPartsPrivate *privat = GPARTS_GET_PRIVATE(gparts);

    if (privat != NULL)
    {
        if (privat->notebook != NULL)
        {
            g_signal_handlers_disconnect_by_func(
                privat->notebook,
                G_CALLBACK(gparts_page_added_cb),
                gparts
                );

            g_signal_handlers_disconnect_by_func(
                privat->notebook,
                G_CALLBACK(gparts_page_removed_cb),
                gparts
                );

            g_signal_handlers_disconnect_by_func(
                privat->notebook,
                G_CALLBACK(gparts_switch_page_cb),
                gparts
                );

            g_object_unref(privat->notebook);
        }

        privat->notebook = notebook;

        if (privat->notebook != NULL)
        {
            g_object_ref(privat->notebook);

            g_signal_connect(
                privat->notebook,
                "page-added",
                G_CALLBACK(gparts_page_added_cb),
                gparts
                );

            g_signal_connect(
                privat->notebook,
                "page-removed",
                G_CALLBACK(gparts_page_removed_cb),
                gparts
                );

            g_signal_connect(
                privat->notebook,
                "switch-page",
                G_CALLBACK(gparts_switch_page_cb),
                gparts
                );
        }

        /* g_object_notify(G_OBJECT(gparts), "notebook"); */
    }
}


static void
gparts_switch_page_cb(GtkNotebook *notebook, gpointer unused, gint page_no, gpointer user_data)
{
    GPartsPrivate *privat = GPARTS_GET_PRIVATE(user_data);

    if (privat != NULL)
    {
        if (GPVIEW_IS_VIEW_INTERFACE(privat->current_view))
        {
            gpview_view_interface_deactivate(GPVIEW_VIEW_INTERFACE(privat->current_view));
        }

        privat->current_view = NULL;

        if (page_no >= 0)
        {
            GtkWidget *widget = gtk_notebook_get_nth_page(notebook, page_no);

            if (GPVIEW_IS_VIEW_INTERFACE(widget))
            {
                gpview_view_interface_activate(GPVIEW_VIEW_INTERFACE(widget));
            }

            privat->current_view = widget;
        }
    }
}


int main(int argc, char* argv[])
{
  GParts *gparts;

  g_type_init();

  if (gtk_init_check(&argc, &argv)) {



    gtk_init(&argc, &argv);

    dlopen(NULL, RTLD_NOW | RTLD_GLOBAL);

    scmcfg_config_init();
    scmcfg_config_load();

    gparts = gparts_new();

    gtk_main();
  }
  else {
    fprintf(stderr, "Failed to initialize GTK");
    return 1;
  }
  return 0;
}

