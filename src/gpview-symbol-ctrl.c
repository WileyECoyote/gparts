/* gEDA - GPL Electronic Design Automation
 * gparts - gEDA Parts Manager
 * Copyright (C) 2012 Edward C. Hennessy
 * Copyright (C) 2012 gEDA Contributors (see ChangeLog for details)
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

/*! \file gpview-symbol-ctrl.c
 */

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "sch.h"
#include "gparts.h"
#include "gpform.h"
#include "gpview.h"

#define GPVIEW_SYMBOL_CTRL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),GPVIEW_TYPE_SYMBOL_CTRL,GPViewSymbolCtrlPrivate))

enum
{
    GPVIEW_SYMBOL_CTRL_CURRENT_VIEW = 1,
    GPVIEW_SYMBOL_CTRL_DATABASE,
    GPVIEW_SYMBOL_CTRL_UI_MANAGER
};

typedef struct _GPViewSymbolCtrlPrivate GPViewSymbolCtrlPrivate;

struct _GPViewSymbolCtrlPrivate
{
    GPartsDatabase    *database;
    GPViewSymbolView  *current_view;
    GtkUIManager      *ui_manager;

    GPFormFactory     *form_factory;

    GtkActionGroup    *action_group_connected;
    GtkActionGroup    *action_group_multiple;
    GtkActionGroup    *action_group_open;
    GtkActionGroup    *action_group_single;
};


static void
gpview_symbol_ctrl_action_create_cb(GtkAction *action, GPViewSymbolCtrl *ctrl);

static void
gpview_symbol_ctrl_action_delete_cb(GtkAction *action, GPViewSymbolCtrl *ctrl);

static void
gpview_symbol_ctrl_action_edit_cb(GtkAction *action, GPViewSymbolCtrl *ctrl);

static void
gpview_symbol_ctrl_action_open_cb(GtkAction *action, GPViewSymbolCtrl *ctrl);

static void
gpview_symbol_ctrl_action_scan_cb(GtkAction *action, GPViewSymbolCtrl *ctrl);

static void
gpview_symbol_ctrl_class_init(gpointer g_class, gpointer g_class_data);

static void
gpview_symbol_ctrl_dispose(GObject *object);

/*
static void
gpview_symbol_ctrl_edit_cb(GtkAction *action, GPViewSymbolCtrl *ctrl);
*/

static void
gpview_symbol_ctrl_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
gpview_symbol_ctrl_init(GTypeInstance *instance, gpointer g_class);

static void
gpview_symbol_ctrl_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void
gpview_symbol_ctrl_update_connected_cb(GObject *unused, GParamSpec *pspec, GPViewSymbolCtrl *ctrl);

static void
gpview_symbol_ctrl_update_ids_cb(GObject *unused, GParamSpec *pspec, GPViewSymbolCtrl *ctrl);

static void
gpview_symbol_ctrl_update_open_cb(GObject *unused, GParamSpec *pspec, GPViewSymbolCtrl *ctrl);



/* These actions can occur when the database is connected.
 */
static const GtkActionEntry gpview_symbol_ctrl_actions_connected[] =
{
    {
        "symbol-create",
        GTK_STOCK_ADD,
        "New Symbol",
        NULL,
        "New Symbol",
        G_CALLBACK(gpview_symbol_ctrl_action_create_cb)
    },
    {
        "symbol-scan",
        GTK_STOCK_REFRESH,
        "Scan Symbol Library",
        NULL,
        "Scan Symbol Library",
        G_CALLBACK(gpview_symbol_ctrl_action_scan_cb)
    }
};


/* These actions can occur when multiple items are selected.
 */
static const GtkActionEntry gpview_symbol_ctrl_actions_multiple[] =
{
    {
        "symbol-delete",
        GTK_STOCK_REMOVE,
        "Delete Symbol",
        NULL,
        "Delete Symbol",
        G_CALLBACK(gpview_symbol_ctrl_action_delete_cb)
    }
};


/* These actions can occur when only one item is selected.
 */
static const GtkActionEntry gpview_symbol_ctrl_actions_single[] =
{
    {
        "symbol-edit",
        GTK_STOCK_EDIT,
        "Edit Symbol",
        NULL,
        "Edit Symbol",
        G_CALLBACK(gpview_symbol_ctrl_action_edit_cb)
    }
};


/* These actions can occur when one or more symbol names exist.
 */
static const GtkActionEntry gpview_symbol_ctrl_actions_open[] =
{
    {
        "symbol-open",
        GTK_STOCK_OPEN,
        "Open Symbol File",
        NULL,
        "Open Symbol File",
        G_CALLBACK(gpview_symbol_ctrl_action_open_cb)
    }
};


/*! \brief Create a symbol
 *
 *  Not sure what to do with this one.
 */
static void
gpview_symbol_ctrl_action_create_cb(GtkAction *action, GPViewSymbolCtrl *ctrl)
{
    GPViewSymbolCtrlPrivate *privat = GPVIEW_SYMBOL_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        GPFormUIDialog *dialog;

        dialog = gpform_factory_create_form(privat->form_factory, "company-add.xml");

        if (dialog != NULL)
        {
            gtk_widget_show_all(GTK_WIDGET(dialog));

            /*gint result =*/ gtk_dialog_run(GTK_DIALOG(dialog));

            gtk_widget_hide(GTK_WIDGET(dialog));
        }
    }
}


/*! \brief Delete the selected symbols
 *
 *  Deleted the selected symbols from the database.
 */
static void
gpview_symbol_ctrl_action_delete_cb(GtkAction *action, GPViewSymbolCtrl *ctrl)
{
    GPViewSymbolCtrlPrivate *privat = GPVIEW_SYMBOL_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        GPFormUIDialog *dialog;

        dialog = gpform_factory_create_form(privat->form_factory, "company-delete.xml");

        if (dialog != NULL)
        {
            gtk_widget_show_all(GTK_WIDGET(dialog));

            /*gint result =*/ gtk_dialog_run(GTK_DIALOG(dialog));

            gtk_widget_hide(GTK_WIDGET(dialog));
        }
    }
}


/*! \brief Edit the selected symbol
 *
 *  Edits the selected symbol.
 */
static void
gpview_symbol_ctrl_action_edit_cb(GtkAction *action, GPViewSymbolCtrl *ctrl)
{
    g_debug("gpview_symbol_ctrl_action_edit_cb: Not implemented");
}


/*! \brief Open the selected symbols
 *
 *  Opens the selected symbols with the program associated with .sym files.
 */
static void
gpview_symbol_ctrl_action_open_cb(GtkAction *action, GPViewSymbolCtrl *ctrl)
{
    GPViewSymbolCtrlPrivate *privat = GPVIEW_SYMBOL_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        SchLoader *loader = sch_loader_get_default();
        GStrv symbols = gpview_symbol_view_get_symbol_names(privat->current_view);

        if (loader == NULL)
        {
            g_critical("Unable to get a loader in gpview_symbol_ctrl_open_cb");
        }
        else if (symbols != NULL)
        {
            GString *command = g_string_new(NULL);
            gchar **temp = symbols;

            while (*temp != NULL)
            {
                gchar *path = sch_loader_find_symbol_file(loader, *temp++);

                if (path == NULL)
                {
                    continue;
                }

                g_string_printf(
                    command,
                    "gnome-open %s",
                    path
                    );

                g_free(path);

                g_debug("Command line: %s", command->str);

                g_spawn_command_line_async(command->str, NULL);
            }

            g_string_free(command, TRUE);
        }

        g_strfreev(symbols);
    }
}


/*! \brief Scan the component library
 *
 *  Scans the component library for new symbols and places them in the
 *  database.
 */
static void
gpview_symbol_ctrl_action_scan_cb(GtkAction *action, GPViewSymbolCtrl *ctrl)
{
    g_debug("gpview_symbol_ctrl_action_scan_cb: Not implemented");
}


static void
gpview_symbol_ctrl_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *klasse = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(g_class, sizeof(GPViewSymbolCtrlPrivate));

    klasse->dispose = gpview_symbol_ctrl_dispose;

    klasse->get_property = gpview_symbol_ctrl_get_property;
    klasse->set_property = gpview_symbol_ctrl_set_property;

    g_object_class_install_property(
        klasse,
        GPVIEW_SYMBOL_CTRL_CURRENT_VIEW,
        g_param_spec_object(
            "current-view",
            "Current View",
            "Current View",
            GPVIEW_TYPE_SYMBOL_VIEW,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPVIEW_SYMBOL_CTRL_DATABASE,
        g_param_spec_object(
            "database",
            "Database",
            "Database",
            GPARTS_TYPE_DATABASE,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPVIEW_SYMBOL_CTRL_UI_MANAGER,
        g_param_spec_object(
            "ui-manager",
            "UI Manager",
            "UI Manager",
            GTK_TYPE_UI_MANAGER,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );
}



static void
gpview_symbol_ctrl_dispose(GObject *object)
{
    GPViewSymbolCtrlPrivate *privat = GPVIEW_SYMBOL_CTRL_GET_PRIVATE(object);

    if (privat != NULL)
    {
        misc_object_unref(privat->action_group_connected);
        privat->action_group_connected = NULL;

        misc_object_unref(privat->action_group_multiple);
        privat->action_group_multiple = NULL;

        misc_object_unref(privat->action_group_open);
        privat->action_group_open = NULL;

        misc_object_unref(privat->action_group_single);
        privat->action_group_single = NULL;

        misc_object_unref(privat->current_view);
        privat->database = NULL;

        misc_object_unref(privat->database);
        privat->database = NULL;

        misc_object_unref(privat->form_factory);
        privat->database = NULL;

        misc_object_unref(privat->ui_manager);
        privat->ui_manager = NULL;
    }

    misc_object_chain_dispose(object);
}


GPViewSymbolView*
gpview_symbol_ctrl_get_current_view(const GPViewSymbolCtrl *ctrl)
{
    GPViewSymbolView *view = NULL;

    if (ctrl != NULL)
    {
        GPViewSymbolCtrlPrivate *privat = GPVIEW_SYMBOL_CTRL_GET_PRIVATE(ctrl);

        if (privat != NULL)
        {
            view = privat->current_view;

            if (view != NULL)
            {
                g_object_ref(view);
            }
        }
    }

    return view;
}


GPartsDatabase*
gpview_symbol_ctrl_get_database(const GPViewSymbolCtrl *ctrl)
{
    GPartsDatabase *database = NULL;

    if (ctrl != NULL)
    {
        GPViewSymbolCtrlPrivate *privat = GPVIEW_SYMBOL_CTRL_GET_PRIVATE(ctrl);

        if (privat != NULL)
        {
            database = privat->database;

            if (database != NULL)
            {
                g_object_ref(database);
            }
        }
    }

    return database;
}


static void
gpview_symbol_ctrl_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    GPViewSymbolCtrl *ctrl = GPVIEW_SYMBOL_CTRL(object);

    if (ctrl != NULL)
    {
        switch (property_id)
        {
            case GPVIEW_SYMBOL_CTRL_CURRENT_VIEW:
                g_value_take_object(value, gpview_symbol_ctrl_get_current_view(ctrl));
                break;

            case GPVIEW_SYMBOL_CTRL_DATABASE:
                g_value_take_object(value, gpview_symbol_ctrl_get_database(ctrl));
                break;

            case GPVIEW_SYMBOL_CTRL_UI_MANAGER:
                g_value_take_object(value, gpview_symbol_ctrl_get_ui_manager(ctrl));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}


GtkUIManager*
gpview_symbol_ctrl_get_ui_manager(const GPViewSymbolCtrl *ctrl)
{
    GtkUIManager *manager = NULL;

    if (ctrl != NULL)
    {
        GPViewSymbolCtrlPrivate *privat = GPVIEW_SYMBOL_CTRL_GET_PRIVATE(ctrl);

        if (privat != NULL)
        {
            manager = privat->ui_manager;

            if (manager != NULL)
            {
                g_object_ref(manager);
            }
        }
    }

    return manager;
}


GType
gpview_symbol_ctrl_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(GPViewSymbolCtrlClass),    /* class_size */
            NULL,                             /* base_init */
            NULL,                             /* base_finalize */
            gpview_symbol_ctrl_class_init,    /* class_init */
            NULL,                             /* class_finalize */
            NULL,                             /* class_data */
            sizeof(GPViewSymbolCtrl),         /* instance_size */
            0,                                /* n_preallocs */
            gpview_symbol_ctrl_init,          /* instance_init */
            NULL                              /* value_table */
            };

        static const GInterfaceInfo iinfo = {
            NULL,                              /* interface_init */
            NULL,                              /* interface_finalize */
            NULL                               /* interface_data */
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "GPViewSymbolCtrl",
            &tinfo,
            0
            );

        g_type_add_interface_static(type, GTK_TYPE_BUILDABLE, &iinfo);
    }

    return type;
}


static void
gpview_symbol_ctrl_init(GTypeInstance *instance, gpointer g_class)
{
    GPViewSymbolCtrlPrivate *privat = GPVIEW_SYMBOL_CTRL_GET_PRIVATE(instance);

    if (privat != NULL)
    {
        privat->action_group_connected = gtk_action_group_new("symbol-action-group-connected");

        gtk_action_group_add_actions(
            privat->action_group_connected,
            gpview_symbol_ctrl_actions_connected,
            G_N_ELEMENTS(gpview_symbol_ctrl_actions_connected),
            instance
            );

        g_signal_connect(
            GPVIEW_SYMBOL_CTRL(instance),
            "notify::database",
            G_CALLBACK(gpview_symbol_ctrl_update_connected_cb),
            GPVIEW_SYMBOL_CTRL(instance)
            );

        privat->action_group_multiple = gtk_action_group_new("symbol-action-group-multiple");

        gtk_action_group_add_actions(
            privat->action_group_multiple,
            gpview_symbol_ctrl_actions_multiple,
            G_N_ELEMENTS(gpview_symbol_ctrl_actions_multiple),
            instance
            );

        privat->action_group_single = gtk_action_group_new("symbol-action-group-single");

        gtk_action_group_add_actions(
            privat->action_group_single,
            gpview_symbol_ctrl_actions_single,
            G_N_ELEMENTS(gpview_symbol_ctrl_actions_single),
            instance
            );

        g_signal_connect(
            GPVIEW_SYMBOL_CTRL(instance),
            "notify::current-view",
            G_CALLBACK(gpview_symbol_ctrl_update_ids_cb),
            GPVIEW_SYMBOL_CTRL(instance)
            );

        privat->action_group_open = gtk_action_group_new("symbol-action-group-open");

        gtk_action_group_add_actions(
            privat->action_group_open,
            gpview_symbol_ctrl_actions_open,
            G_N_ELEMENTS(gpview_symbol_ctrl_actions_open),
            instance
            );

        g_signal_connect(
            GPVIEW_SYMBOL_CTRL(instance),
            "notify::current-view",
            G_CALLBACK(gpview_symbol_ctrl_update_open_cb),
            GPVIEW_SYMBOL_CTRL(instance)
            );

        g_object_notify(G_OBJECT(instance), "current-view");
        g_object_notify(G_OBJECT(instance), "database");

        privat->form_factory = gpform_factory_new();
    }
}


GPViewSymbolCtrl*
gpview_symbol_ctrl_new(void)
{
    return GPVIEW_SYMBOL_CTRL(g_object_new(GPVIEW_TYPE_SYMBOL_CTRL, NULL));
}


GPViewSymbolCtrl*
gpview_symbol_ctrl_new_with_manager(GtkUIManager *manager)
{
    return GPVIEW_SYMBOL_CTRL(g_object_new(
        GPVIEW_TYPE_SYMBOL_CTRL,
        "ui-manager", manager,
        NULL
        ));
}


void
gpview_symbol_ctrl_set_current_view(GPViewSymbolCtrl *ctrl, GPViewSymbolView *view)
{
    GPViewSymbolCtrlPrivate *privat = GPVIEW_SYMBOL_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        if (privat->current_view != NULL)
        {
            g_signal_handlers_disconnect_by_func(
                privat->current_view,
                G_CALLBACK(gpview_symbol_ctrl_update_open_cb),
                ctrl
                );

            g_signal_handlers_disconnect_by_func(
                privat->current_view,
                G_CALLBACK(gpview_symbol_ctrl_update_ids_cb),
                ctrl
                );

            g_object_unref(privat->current_view);
        }

        privat->current_view = view;

        if (privat->current_view != NULL)
        {
            g_object_ref(privat->current_view);

            g_signal_connect(
                privat->current_view,
                "notify::symbol-ids",
                G_CALLBACK(gpview_symbol_ctrl_update_ids_cb),
                ctrl
                );

            g_signal_connect(
                privat->current_view,
                "notify::symbol-names",
                G_CALLBACK(gpview_symbol_ctrl_update_open_cb),
                ctrl
                );
        }

        g_object_notify(G_OBJECT(ctrl), "current-view");
    }
}


void
gpview_symbol_ctrl_set_database(GPViewSymbolCtrl *ctrl, GPartsDatabase *database)
{
    GPViewSymbolCtrlPrivate *privat = GPVIEW_SYMBOL_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        if (privat->database != NULL)
        {
            g_signal_handlers_disconnect_by_func(
                privat->database,
                G_CALLBACK(gpview_symbol_ctrl_update_connected_cb),
                ctrl
                );

            g_object_unref(privat->database);
        }

        privat->database = database;

        if (privat->database != NULL)
        {
            g_object_ref(privat->database);

            g_signal_connect(
                privat->database,
                "notify::connected",
                G_CALLBACK(gpview_symbol_ctrl_update_connected_cb),
                ctrl
                );
        }

        g_object_notify(G_OBJECT(ctrl), "database");
    }
}

void
gpview_symbol_ctrl_set_form_factory(GPViewSymbolCtrl *ctrl, GPFormFactory *factory)
{
    GPViewSymbolCtrlPrivate *privat = GPVIEW_SYMBOL_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        if (privat->form_factory != NULL)
        {
            g_object_unref(privat->form_factory);
        }

        privat->form_factory = factory;

        if (privat->form_factory != NULL)
        {
            g_object_ref(privat->form_factory);
        }

        g_object_notify(G_OBJECT(ctrl), "form-factory");
    }
}


static void
gpview_symbol_ctrl_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GPViewSymbolCtrl *view = GPVIEW_SYMBOL_CTRL(object);

    if (view != NULL)
    {
        switch (property_id)
        {
            case GPVIEW_SYMBOL_CTRL_CURRENT_VIEW:
                gpview_symbol_ctrl_set_current_view(view, g_value_get_object(value));
                break;

            case GPVIEW_SYMBOL_CTRL_DATABASE:
                gpview_symbol_ctrl_set_database(view, g_value_get_object(value));
                break;

            case GPVIEW_SYMBOL_CTRL_UI_MANAGER:
                gpview_symbol_ctrl_set_ui_manager(view, g_value_get_object(value));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}


void
gpview_symbol_ctrl_set_ui_manager(GPViewSymbolCtrl *ctrl, GtkUIManager *ui_manager)
{
    GPViewSymbolCtrlPrivate *privat = GPVIEW_SYMBOL_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        if (privat->ui_manager != NULL)
        {
            gtk_ui_manager_remove_action_group(
                privat->ui_manager,
                privat->action_group_single
                );

            gtk_ui_manager_remove_action_group(
                privat->ui_manager,
                privat->action_group_open
                );

            gtk_ui_manager_remove_action_group(
                privat->ui_manager,
                privat->action_group_multiple
                );

            gtk_ui_manager_remove_action_group(
                privat->ui_manager,
                privat->action_group_connected
                );

            g_object_unref(privat->ui_manager);
        }

        privat->ui_manager = ui_manager;

        if (privat->ui_manager != NULL)
        {
            g_object_ref(privat->ui_manager);

            gtk_ui_manager_insert_action_group(
                privat->ui_manager,
                privat->action_group_connected,
                0
                );

            gtk_ui_manager_insert_action_group(
                privat->ui_manager,
                privat->action_group_multiple,
                0
                );

            gtk_ui_manager_insert_action_group(
                privat->ui_manager,
                privat->action_group_open,
                0
                );

            gtk_ui_manager_insert_action_group(
                privat->ui_manager,
                privat->action_group_single,
                0
                );
        }

        g_object_notify(G_OBJECT(ctrl), "ui-manager");
    }
}


static void
gpview_symbol_ctrl_update_connected_cb(GObject *unused, GParamSpec *pspec, GPViewSymbolCtrl *ctrl)
{
    GPViewSymbolCtrlPrivate *privat = GPVIEW_SYMBOL_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        gtk_action_group_set_sensitive(
            privat->action_group_connected,
            gparts_database_connected(privat->database)
            );
    }
}


static void
gpview_symbol_ctrl_update_ids_cb(GObject *unused, GParamSpec *pspec, GPViewSymbolCtrl *ctrl)
{
    GPViewSymbolCtrlPrivate *privat = GPVIEW_SYMBOL_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        gboolean visible = (privat->current_view != NULL);
        GStrv ids = gpview_symbol_view_get_symbol_ids(privat->current_view);
        gboolean sensitive_1 = visible && (ids != NULL) && (g_strv_length(ids) == 1);
        gboolean sensitive_m = visible && (ids != NULL) && (g_strv_length(ids) > 0);

        g_strfreev(ids);

        gtk_action_group_set_sensitive(
            privat->action_group_single,
            sensitive_1
            );

        gtk_action_group_set_visible(
            privat->action_group_single,
            visible
            );

        gtk_action_group_set_sensitive(
            privat->action_group_multiple,
            sensitive_m
            );

        gtk_action_group_set_visible(
            privat->action_group_multiple,
            visible
            );
    }
}


static void
gpview_symbol_ctrl_update_open_cb(GObject *unused, GParamSpec *pspec, GPViewSymbolCtrl *ctrl)
{
    GPViewSymbolCtrlPrivate *privat = GPVIEW_SYMBOL_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        gboolean visible = (privat->current_view != NULL);
        GStrv symbols = gpview_symbol_view_get_symbol_names(privat->current_view);
        gboolean sensitive = visible && (symbols != NULL) && (g_strv_length(symbols) > 0);

        g_strfreev(symbols);

        gtk_action_group_set_sensitive(
            privat->action_group_open,
            sensitive
            );

        gtk_action_group_set_visible(
            privat->action_group_open,
            visible
            );
    }
}

