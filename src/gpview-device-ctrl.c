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

/*! \file gpview-device-ctrl.c
 */

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "sch.h"
#include "gparts.h"
#include "gpform.h"
#include "gpview.h"

#define GPVIEW_DEVICE_CTRL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),GPVIEW_TYPE_DEVICE_CTRL,GPViewDeviceCtrlPrivate))

enum
{
    GPVIEW_DEVICE_CTRL_CURRENT_VIEW = 1,
    GPVIEW_DEVICE_CTRL_DATABASE,
    GPVIEW_DEVICE_CTRL_FORM_FACTORY,
    GPVIEW_DEVICE_CTRL_UI_MANAGER
};

typedef struct _GPViewDeviceCtrlPrivate GPViewDeviceCtrlPrivate;

struct _GPViewDeviceCtrlPrivate
{
    GPartsDatabase    *database;
    GPFormFactory     *form_factory;
    GPViewDeviceView *current_view;
    GtkUIManager      *ui_manager;

    GtkActionGroup    *action_group_connected;
    GtkActionGroup    *action_group_multiple;
    GtkActionGroup    *action_group_single;
};

static void
gpview_device_ctrl_class_init(void *g_class, void *g_class_data);

static void
gpview_device_ctrl_create_cb(GtkAction *action, GPViewDeviceCtrl *ctrl);

static void
gpview_device_ctrl_delete_cb(GtkAction *action, GPViewDeviceCtrl *ctrl);

static void
gpview_device_ctrl_dispose(GObject *object);

static void
gpview_device_ctrl_edit_cb(GtkAction *action, GPViewDeviceCtrl *ctrl);

static void
gpview_device_ctrl_get_property(GObject *object, unsigned property_id, GValue *value, GParamSpec *pspec);

static void
gpview_device_ctrl_init(GTypeInstance *instance, void *g_class);

static void
gpview_device_ctrl_set_property(GObject *object, unsigned property_id, const GValue *value, GParamSpec *pspec);

static void
gpview_device_ctrl_update_connected_cb(GObject *unused, GParamSpec *pspec, GPViewDeviceCtrl *ctrl);

static void
gpview_device_ctrl_update_ids_cb(GObject *unused, GParamSpec *pspec, GPViewDeviceCtrl *ctrl);

//static void
//gpview_device_ctrl_visit_cb(GtkAction *action, GPViewDeviceCtrl *ctrl);


static const GtkActionEntry gpview_device_ctrl_actions_connected[] =
{
    {
        "device-create",
        GTK_STOCK_ADD,
        "New Device",
        NULL,
        "New Device",
        G_CALLBACK(gpview_device_ctrl_create_cb)
    },
};


static const GtkActionEntry gpview_device_ctrl_actions_multiple[] =
{
    {
        "device-delete",
        GTK_STOCK_REMOVE,
        "Delete Device",
        NULL,
        "Delete Device",
        G_CALLBACK(gpview_device_ctrl_delete_cb)
    }
};

static const GtkActionEntry gpview_device_ctrl_actions_single[] =
{
    {
        "device-edit",
        GTK_STOCK_EDIT,
        "Edit Device",
        NULL,
        "Edit Device",
        G_CALLBACK(gpview_device_ctrl_edit_cb)
    },
};



static void
gpview_device_ctrl_class_init(void *g_class, void *g_class_data)
{
    GObjectClass *klasse = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(g_class, sizeof(GPViewDeviceCtrlPrivate));

    klasse->dispose = gpview_device_ctrl_dispose;

    klasse->get_property = gpview_device_ctrl_get_property;
    klasse->set_property = gpview_device_ctrl_set_property;

    g_object_class_install_property(
        klasse,
        GPVIEW_DEVICE_CTRL_CURRENT_VIEW,
        g_param_spec_object(
            "current-view",
            "Current View",
            "Current View",
            GPVIEW_TYPE_DEVICE_VIEW,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(klasse,
        GPVIEW_DEVICE_CTRL_DATABASE,
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
        GPVIEW_DEVICE_CTRL_FORM_FACTORY,
        g_param_spec_object(
            "form-factory",
            "Form Factory",
            "Form Factory",
            GPFORM_TYPE_FACTORY,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPVIEW_DEVICE_CTRL_UI_MANAGER,
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
gpview_device_ctrl_create_cb(GtkAction *action, GPViewDeviceCtrl *ctrl)
{
    GPViewDeviceCtrlPrivate *privat = GPVIEW_DEVICE_CTRL_GET_PRIVATE(ctrl);

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


static void
gpview_device_ctrl_dispose(GObject *object)
{
    GPViewDeviceCtrlPrivate *privat = GPVIEW_DEVICE_CTRL_GET_PRIVATE(object);

    if (privat != NULL)
    {
        misc_object_unref(privat->action_group_connected);
        privat->action_group_connected = NULL;

        misc_object_unref(privat->action_group_multiple);
        privat->action_group_multiple = NULL;

        misc_object_unref(privat->action_group_single);
        privat->action_group_single = NULL;

        misc_object_unref(privat->current_view);
        privat->current_view = NULL;

        misc_object_unref(privat->database);
        privat->database = NULL;

        misc_object_unref(privat->form_factory);
        privat->form_factory = NULL;

        misc_object_unref(privat->ui_manager);
        privat->ui_manager = NULL;
    }

    misc_object_chain_dispose(object);
}


static void
gpview_device_ctrl_delete_cb(GtkAction *action, GPViewDeviceCtrl *ctrl)
{
    GPViewDeviceCtrlPrivate *privat = GPVIEW_DEVICE_CTRL_GET_PRIVATE(ctrl);

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


static void
gpview_device_ctrl_edit_cb(GtkAction *action, GPViewDeviceCtrl *ctrl)
{
    g_debug("Edit Company");
}


GPViewDeviceView*
gpview_device_ctrl_get_current_view(const GPViewDeviceCtrl *ctrl)
{
    GPViewDeviceView *current_view = NULL;

    if (ctrl != NULL)
    {
        GPViewDeviceCtrlPrivate *privat = GPVIEW_DEVICE_CTRL_GET_PRIVATE(ctrl);

        if (privat != NULL)
        {
            current_view = privat->current_view;

            if (current_view != NULL)
            {
                g_object_ref(G_OBJECT(current_view));
            }
        }
    }

    return current_view;
}

GPartsDatabase*
gpview_device_ctrl_get_database(const GPViewDeviceCtrl *ctrl)
{
    GPartsDatabase *database = NULL;
    GPViewDeviceCtrlPrivate *privat = GPVIEW_DEVICE_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        database = privat->database;

        if (database != NULL)
        {
            g_object_ref(G_OBJECT(database));
        }
    }

    return database;
}

GPFormFactory*
gpview_device_ctrl_get_form_factory(const GPViewDeviceCtrl *ctrl)
{
    GPFormFactory *form_factory = NULL;
    GPViewDeviceCtrlPrivate *privat = GPVIEW_DEVICE_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        form_factory = privat->form_factory;

        if (form_factory != NULL)
        {
            g_object_ref(G_OBJECT(form_factory));
        }
    }

    return form_factory;
}

static void
gpview_device_ctrl_get_property(GObject *object, unsigned property_id, GValue *value, GParamSpec *pspec)
{
    GPViewDeviceCtrl *view = GPVIEW_DEVICE_CTRL(object);

    if (view != NULL)
    {
        switch (property_id)
        {
            case GPVIEW_DEVICE_CTRL_CURRENT_VIEW:
                g_value_take_object(value, gpview_device_ctrl_get_current_view(view));
                break;

            case GPVIEW_DEVICE_CTRL_DATABASE:
                g_value_take_object(value, gpview_device_ctrl_get_database(view));
                break;

            case GPVIEW_DEVICE_CTRL_FORM_FACTORY:
                g_value_take_object(value, gpview_device_ctrl_get_form_factory(view));
                break;

            case GPVIEW_DEVICE_CTRL_UI_MANAGER:
                g_value_take_object(value, gpview_device_ctrl_get_ui_manager(view));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}


GType
gpview_device_ctrl_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(GPViewDeviceCtrlClass),    /* class_size */
            NULL,                              /* base_init */
            NULL,                              /* base_finalize */
            gpview_device_ctrl_class_init,    /* class_init */
            NULL,                              /* class_finalize */
            NULL,                              /* class_data */
            sizeof(GPViewDeviceCtrl),         /* instance_size */
            0,                                 /* n_preallocs */
            gpview_device_ctrl_init,          /* instance_init */
            NULL                               /* value_table */
            };

        static const GInterfaceInfo iinfo = {
            NULL,                              /* interface_init */
            NULL,                              /* interface_finalize */
            NULL                               /* interface_data */
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "GPViewDeviceCtrl",
            &tinfo,
            0
            );

        g_type_add_interface_static(type, GTK_TYPE_BUILDABLE, &iinfo);
    }

    return type;
}


GtkUIManager*
gpview_device_ctrl_get_ui_manager(const GPViewDeviceCtrl *ctrl)
{
    GtkUIManager *manager = NULL;
    GPViewDeviceCtrlPrivate *privat = GPVIEW_DEVICE_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        manager = privat->ui_manager;

        if (manager != NULL)
        {
            g_object_ref(G_OBJECT(manager));
        }
    }

    return manager;
}


static void
gpview_device_ctrl_init(GTypeInstance *instance, void *g_class)
{
    GPViewDeviceCtrlPrivate *privat = GPVIEW_DEVICE_CTRL_GET_PRIVATE(instance);

    if (privat != NULL)
    {
        privat->action_group_connected = gtk_action_group_new("device-action-group-connected");

        gtk_action_group_add_actions(
            privat->action_group_connected,
            gpview_device_ctrl_actions_connected,
            G_N_ELEMENTS(gpview_device_ctrl_actions_connected),
            instance
            );

        g_signal_connect(
            GPVIEW_DEVICE_CTRL(instance),
            "notify::database",
            G_CALLBACK(gpview_device_ctrl_update_connected_cb),
            GPVIEW_DEVICE_CTRL(instance)
            );

        privat->action_group_multiple = gtk_action_group_new("device-action-group-multiple");

        gtk_action_group_add_actions(
            privat->action_group_multiple,
            gpview_device_ctrl_actions_multiple,
            G_N_ELEMENTS(gpview_device_ctrl_actions_multiple),
            instance
            );

        privat->action_group_single = gtk_action_group_new("device-action-group-single");

        gtk_action_group_add_actions(
            privat->action_group_single,
            gpview_device_ctrl_actions_single,
            G_N_ELEMENTS(gpview_device_ctrl_actions_single),
            instance
            );

        g_signal_connect(
            GPVIEW_DEVICE_CTRL(instance),
            "notify::current-view",
            G_CALLBACK(gpview_device_ctrl_update_ids_cb),
            GPVIEW_DEVICE_CTRL(instance)
            );

        g_object_notify(G_OBJECT(instance), "current-view");
        g_object_notify(G_OBJECT(instance), "database");

        privat->form_factory = gpform_factory_new();
    }
}


GPViewDeviceCtrl*
gpview_device_ctrl_new(void)
{
    return GPVIEW_DEVICE_CTRL(g_object_new(GPVIEW_TYPE_DEVICE_CTRL, NULL));
}


GPViewDeviceCtrl*
gpview_device_ctrl_new_with_manager(GtkUIManager *manager)
{
    return GPVIEW_DEVICE_CTRL(g_object_new(
        GPVIEW_TYPE_DEVICE_CTRL,
        "ui-manager", manager,
        NULL
        ));
}


void
gpview_device_ctrl_set_current_view(GPViewDeviceCtrl *ctrl, GPViewDeviceView *view)
{
    GPViewDeviceCtrlPrivate *privat = GPVIEW_DEVICE_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        if (privat->current_view != NULL)
        {
            g_signal_handlers_disconnect_by_func(
                privat->current_view,
                G_CALLBACK(gpview_device_ctrl_update_ids_cb),
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
                "notify::company-id",
                G_CALLBACK(gpview_device_ctrl_update_ids_cb),
                ctrl
                );
        }

        g_object_notify(G_OBJECT(ctrl), "current-view");
    }
}


void
gpview_device_ctrl_set_database(GPViewDeviceCtrl *ctrl, GPartsDatabase *database)
{
    GPViewDeviceCtrlPrivate *privat = GPVIEW_DEVICE_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        if (privat->database != NULL)
        {
            g_signal_handlers_disconnect_by_func(
                privat->database,
                G_CALLBACK(gpview_device_ctrl_update_connected_cb),
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
                G_CALLBACK(gpview_device_ctrl_update_connected_cb),
                ctrl
                );
        }

        g_object_notify(G_OBJECT(ctrl), "database");
    }
}


void
gpview_device_ctrl_set_form_factory(GPViewDeviceCtrl *ctrl, GPFormFactory *factory)
{
    GPViewDeviceCtrlPrivate *privat = GPVIEW_DEVICE_CTRL_GET_PRIVATE(ctrl);

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
gpview_device_ctrl_set_property(GObject *object, unsigned property_id, const GValue *value, GParamSpec *pspec)
{
    GPViewDeviceCtrl *view = GPVIEW_DEVICE_CTRL(object);

    if (view != NULL)
    {
        switch (property_id)
        {
            case GPVIEW_DEVICE_CTRL_CURRENT_VIEW:
                gpview_device_ctrl_set_current_view(view, g_value_get_object(value));
                break;

            case GPVIEW_DEVICE_CTRL_DATABASE:
                gpview_device_ctrl_set_database(view, g_value_get_object(value));
                break;

            case GPVIEW_DEVICE_CTRL_FORM_FACTORY:
                gpview_device_ctrl_set_form_factory(view, g_value_get_object(value));
                break;

            case GPVIEW_DEVICE_CTRL_UI_MANAGER:
                gpview_device_ctrl_set_ui_manager(view, g_value_get_object(value));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}


void
gpview_device_ctrl_set_ui_manager(GPViewDeviceCtrl *ctrl, GtkUIManager *ui_manager)
{
    GPViewDeviceCtrlPrivate *privat = GPVIEW_DEVICE_CTRL_GET_PRIVATE(ctrl);

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
                privat->action_group_single,
                0
                );
        }

        g_object_notify(G_OBJECT(ctrl), "ui-manager");
    }
}


static void
gpview_device_ctrl_update_connected_cb(GObject *unused, GParamSpec *pspec, GPViewDeviceCtrl *ctrl)
{
    GPViewDeviceCtrlPrivate *privat = GPVIEW_DEVICE_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        gtk_action_group_set_sensitive(
            privat->action_group_connected,
            gparts_database_connected(privat->database)
            );
    }
}


static void
gpview_device_ctrl_update_ids_cb(GObject *unused, GParamSpec *pspec, GPViewDeviceCtrl *ctrl)
{
    GPViewDeviceCtrlPrivate *privat = GPVIEW_DEVICE_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        gboolean visible = (privat->current_view != NULL);
        GStrv ids = gpview_device_view_get_ids(privat->current_view);
        gboolean sensitive_1 = visible && (ids != NULL) && (g_strv_length(ids) == 1);
        gboolean sensitive_n = visible && (ids != NULL) && (g_strv_length(ids) > 0);

        g_strfreev(ids);

        gtk_action_group_set_sensitive(
            privat->action_group_multiple,
            sensitive_n
            );

        gtk_action_group_set_visible(
            privat->action_group_multiple,
            visible
            );

        gtk_action_group_set_sensitive(
            privat->action_group_single,
            sensitive_1
	    );

        gtk_action_group_set_visible(
            privat->action_group_single,
            visible
            );
    }
}

