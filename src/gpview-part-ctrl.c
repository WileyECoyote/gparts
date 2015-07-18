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

/*! \file gpview-part-ctrl.c
 */

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "schgui.h"
#include "gparts.h"
#include "gpform.h"
#include "gpview.h"


#include "schgui-clipboard.h"

#define GPVIEW_PART_CTRL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),GPVIEW_TYPE_PART_CTRL,GPViewPartCtrlPrivate))

enum
{
    GPVIEW_PART_CTRL_CURRENT_VIEW = 1,
    GPVIEW_PART_CTRL_DATABASE,
    GPVIEW_PART_CTRL_UI_MANAGER
};

typedef struct _GPViewPartCtrlPrivate GPViewPartCtrlPrivate;

struct _GPViewPartCtrlPrivate
{
    GtkActionGroup *action_group[3];
    GPartsDatabase *database;
    GPViewPartView *current_view;
    GtkUIManager   *ui_manager;

    SchGUIClipboard *clipboard;

    GPFormFactory  *form_factory;
};

static void
gpview_part_ctrl_class_init(gpointer g_class, gpointer g_class_data);

static void
gpview_part_ctrl_create_cb(GtkAction *action, GPViewPartCtrl *ctrl);

static void
gpview_part_ctrl_delete_cb(GtkAction *action, GPViewPartCtrl *ctrl);

static void
gpview_part_ctrl_dispose(GObject *object);

static void
gpview_part_ctrl_edit_cb(GtkAction *action, GPViewPartCtrl *ctrl);

static void
gpview_part_ctrl_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
gpview_part_ctrl_init(GTypeInstance *instance, gpointer g_class);

static void
gpview_part_ctrl_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void
gpview_part_ctrl_update_0_cb(GObject *unused, GParamSpec *pspec, GPViewPartCtrl *ctrl);

static void
gpview_part_ctrl_update_1_cb(GObject *unused, GParamSpec *pspec, GPViewPartCtrl *ctrl);

static void
gpview_part_ctrl_update_2_cb(GObject *unused, GParamSpec *pspec, GPViewPartCtrl *ctrl);

static void
gpview_part_ctrl_copy_cb(GtkAction *action, GPViewPartCtrl *ctrl);



static const GtkActionEntry gpview_part_ctrl_actions_0[] =
{
    {
        "part-create",
        GTK_STOCK_ADD,
        "New Part",
        NULL,
        "New Part",
        G_CALLBACK(gpview_part_ctrl_create_cb)
    },
};


static const GtkActionEntry gpview_part_ctrl_actions_1[] =
{
    {
        "part-edit",
        GTK_STOCK_EDIT,
        "Edit Part",
        NULL,
        "Edit Part",
        G_CALLBACK(gpview_part_ctrl_edit_cb)
    },
    {
        "part-delete",
        GTK_STOCK_REMOVE,
        "Delete Part",
        NULL,
        "Delete Part",
        G_CALLBACK(gpview_part_ctrl_delete_cb)
    }
};

static const GtkActionEntry gpview_part_ctrl_actions_2[] =
{
    {
        "part-copy",
        GTK_STOCK_COPY,
        "Copy Part",
        NULL,
        "Copy Part",
        G_CALLBACK(gpview_part_ctrl_copy_cb)
    }
};



static void
gpview_part_ctrl_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *klasse = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(g_class, sizeof(GPViewPartCtrlPrivate));

    klasse->dispose = gpview_part_ctrl_dispose;

    klasse->get_property = gpview_part_ctrl_get_property;
    klasse->set_property = gpview_part_ctrl_set_property;

    g_object_class_install_property(
        klasse,
        GPVIEW_PART_CTRL_CURRENT_VIEW,
        g_param_spec_object(
            "current-view",
            "Current View",
            "Current View",
            GPVIEW_TYPE_PART_VIEW,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPVIEW_PART_CTRL_DATABASE,
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
        GPVIEW_PART_CTRL_UI_MANAGER,
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
gpview_part_ctrl_create_cb(GtkAction *action, GPViewPartCtrl *ctrl)
{
    GPViewPartCtrlPrivate *privat = GPVIEW_PART_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL) {

        GPFormUIDialog *dialog = NULL;
        gchar          *form;

            g_debug("gpview_part_ctrl_create_cb");
        form = gpview_part_view_get_create_form(privat->current_view);

        if (form != NULL) {

            g_debug("Form name: %s", form);
            dialog = gpform_factory_create_form(privat->form_factory, form);
            g_free(form);
        }

        if (dialog != NULL) {

            gtk_window_set_transient_for(GTK_WINDOW(dialog),
                                         GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(privat->current_view))));

            gtk_widget_show_all(GTK_WIDGET(dialog));

            /*gint result =*/ gtk_dialog_run(GTK_DIALOG(dialog));

            gtk_widget_hide(GTK_WIDGET(dialog));
        }
    }
}

static void
gpview_part_ctrl_dispose(GObject *object)
{
    GPViewPartCtrlPrivate *privat = GPVIEW_PART_CTRL_GET_PRIVATE(object);

    if (privat != NULL) {

        misc_object_unref(privat->clipboard);
        privat->clipboard = NULL;

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
gpview_part_ctrl_delete_cb(GtkAction *action, GPViewPartCtrl *ctrl)
{
    GPViewPartCtrlPrivate *privat = GPVIEW_PART_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL) {

        GPFormUIDialog *dialog;

        dialog = gpform_factory_create_form(privat->form_factory, "company-delete.xml");

        if (dialog != NULL) {

            gtk_widget_show_all(GTK_WIDGET(dialog));

            /*gint result =*/ gtk_dialog_run(GTK_DIALOG(dialog));

            gtk_widget_hide(GTK_WIDGET(dialog));
        }
    }
}

static void
gpview_part_ctrl_edit_cb(GtkAction *action, GPViewPartCtrl *ctrl)
{
    g_debug("Edit Company");
}


static void
gpview_part_ctrl_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    GPViewPartCtrlPrivate *privat = GPVIEW_PART_CTRL_GET_PRIVATE(object);

    if (privat != NULL) {

        switch (property_id) {

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}


unsigned int
gpview_part_ctrl_get_type(void)
{
    static unsigned int type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(GPViewPartCtrlClass),    /* class_size */
            NULL,                              /* base_init */
            NULL,                              /* base_finalize */
            gpview_part_ctrl_class_init,    /* class_init */
            NULL,                              /* class_finalize */
            NULL,                              /* class_data */
            sizeof(GPViewPartCtrl),         /* instance_size */
            0,                                 /* n_preallocs */
            gpview_part_ctrl_init,          /* instance_init */
            NULL                               /* value_table */
            };

        static const GInterfaceInfo iinfo = {
            NULL,                              /* interface_init */
            NULL,                              /* interface_finalize */
            NULL                               /* interface_data */
            };

        type = g_type_register_static(G_TYPE_OBJECT,"GPViewPartCtrl",&tinfo, 0);

        g_type_add_interface_static(type, GTK_TYPE_BUILDABLE, &iinfo);
    }

    return type;
}


static void
gpview_part_ctrl_init(GTypeInstance *instance, gpointer g_class)
{
    GPViewPartCtrlPrivate *privat = GPVIEW_PART_CTRL_GET_PRIVATE(instance);

    if (privat != NULL) {

        privat->action_group[0] = gtk_action_group_new("company-action-group-0");

        gtk_action_group_add_actions(
            privat->action_group[0],
            gpview_part_ctrl_actions_0,
            G_N_ELEMENTS(gpview_part_ctrl_actions_0),
            instance
            );

        g_signal_connect(
            GPVIEW_PART_CTRL(instance),
            "notify::database",
            G_CALLBACK(gpview_part_ctrl_update_0_cb),
            GPVIEW_PART_CTRL(instance)
            );

        privat->action_group[1] = gtk_action_group_new("company-action-group-1");

        gtk_action_group_add_actions(
            privat->action_group[1],
            gpview_part_ctrl_actions_1,
            G_N_ELEMENTS(gpview_part_ctrl_actions_1),
            instance
            );

        g_signal_connect(
            GPVIEW_PART_CTRL(instance),
            "notify::current-view",
            G_CALLBACK(gpview_part_ctrl_update_1_cb),
            GPVIEW_PART_CTRL(instance)
            );

        g_signal_connect(
            GPVIEW_PART_CTRL(instance),
            "notify::database",
            G_CALLBACK(gpview_part_ctrl_update_1_cb),
            GPVIEW_PART_CTRL(instance)
            );

        privat->action_group[2] = gtk_action_group_new("company-action-group-2");

        gtk_action_group_add_actions(
            privat->action_group[2],
            gpview_part_ctrl_actions_2,
            G_N_ELEMENTS(gpview_part_ctrl_actions_2),
            instance
            );

        g_signal_connect(
            GPVIEW_PART_CTRL(instance),
            "notify::current-view",
            G_CALLBACK(gpview_part_ctrl_update_2_cb),
            GPVIEW_PART_CTRL(instance)
            );

        g_object_notify(G_OBJECT(instance), "current-view");
        g_object_notify(G_OBJECT(instance), "database");

        privat->form_factory = gpform_factory_new();

        privat->clipboard = schgui_clipboard_new();
    }
}


GPViewPartCtrl*
gpview_part_ctrl_new(void)
{
    return GPVIEW_PART_CTRL(g_object_new(GPVIEW_TYPE_PART_CTRL, NULL));
}

GPViewPartCtrl*
gpview_part_ctrl_new_with_manager(GtkUIManager *manager)
{
    return GPVIEW_PART_CTRL(g_object_new(GPVIEW_TYPE_PART_CTRL,
                                         "ui-manager", manager,
                                         NULL));
}

void
gpview_part_ctrl_set_current_view(GPViewPartCtrl *ctrl, GPViewPartView *view)
{
    GPViewPartCtrlPrivate *privat = GPVIEW_PART_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL) {

        if (privat->current_view != NULL) {

            g_signal_handlers_disconnect_by_func(privat->current_view,
                                                 G_CALLBACK(gpview_part_ctrl_update_2_cb),
                                                 ctrl);

            g_object_unref(privat->current_view);
        }

        privat->current_view = view;

        if (privat->current_view != NULL) {

            g_object_ref(privat->current_view);

            g_signal_connect(privat->current_view, "notify::drawing",
                             G_CALLBACK(gpview_part_ctrl_update_2_cb), ctrl);
        }

        g_object_notify(G_OBJECT(ctrl), "current-view");
    }
}


void
gpview_part_ctrl_set_database(GPViewPartCtrl *ctrl, GPartsDatabase *database)
{
    GPViewPartCtrlPrivate *privat = GPVIEW_PART_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL)
    {
        if (privat->database != NULL)
        {
            g_signal_handlers_disconnect_by_func(
                privat->database,
                G_CALLBACK(gpview_part_ctrl_update_1_cb),
                ctrl
                );

            g_signal_handlers_disconnect_by_func(
                privat->database,
                G_CALLBACK(gpview_part_ctrl_update_0_cb),
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
                G_CALLBACK(gpview_part_ctrl_update_0_cb),
                ctrl
                );

            g_signal_connect(
                privat->database,
                "notify::connected",
                G_CALLBACK(gpview_part_ctrl_update_1_cb),
                ctrl
                );
        }

        g_object_notify(G_OBJECT(ctrl), "database");
    }
}


void
gpview_part_ctrl_set_form_factory(GPViewPartCtrl *ctrl, GPFormFactory *factory)
{
    GPViewPartCtrlPrivate *privat = GPVIEW_PART_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL) {

        if (privat->form_factory != NULL) {

            g_object_unref(privat->form_factory);
        }

        privat->form_factory = factory;

        if (privat->form_factory != NULL) {

            g_object_ref(privat->form_factory);
        }

        g_object_notify(G_OBJECT(ctrl), "form-factory");
    }
}



static void
gpview_part_ctrl_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GPViewPartCtrl *view = GPVIEW_PART_CTRL(object);

    if (view != NULL) {

        switch (property_id) {

            case GPVIEW_PART_CTRL_CURRENT_VIEW:
                gpview_part_ctrl_set_current_view(view, g_value_get_object(value));
                break;

            case GPVIEW_PART_CTRL_DATABASE:
                gpview_part_ctrl_set_database(view, g_value_get_object(value));
                break;

            case GPVIEW_PART_CTRL_UI_MANAGER:
                gpview_part_ctrl_set_ui_manager(view, g_value_get_object(value));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}


void
gpview_part_ctrl_set_ui_manager(GPViewPartCtrl *ctrl, GtkUIManager *ui_manager)
{
    GPViewPartCtrlPrivate *privat = GPVIEW_PART_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL) {

        if (privat->ui_manager != NULL) {

            gtk_ui_manager_remove_action_group(
                privat->ui_manager,
                privat->action_group[2]
                );

            gtk_ui_manager_remove_action_group(
                privat->ui_manager,
                privat->action_group[1]
                );

            gtk_ui_manager_remove_action_group(
                privat->ui_manager,
                privat->action_group[0]
                );

            g_object_unref(privat->ui_manager);
        }

        privat->ui_manager = ui_manager;

        if (privat->ui_manager != NULL) {

            g_object_ref(privat->ui_manager);

            gtk_ui_manager_insert_action_group(
                privat->ui_manager,
                privat->action_group[0],
                0
                );

            gtk_ui_manager_insert_action_group(
                privat->ui_manager,
                privat->action_group[1],
                0
                );

            gtk_ui_manager_insert_action_group(
                privat->ui_manager,
                privat->action_group[2],
                0
                );
        }

        g_object_notify(G_OBJECT(ctrl), "ui-manager");
    }
}


static void
gpview_part_ctrl_update_0_cb(GObject *unused, GParamSpec *pspec, GPViewPartCtrl *ctrl)
{
    GPViewPartCtrlPrivate *privat = GPVIEW_PART_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL) {

        gtk_action_group_set_sensitive(
            privat->action_group[0],
            gparts_database_connected(privat->database)
            );
    }
}


static void
gpview_part_ctrl_update_1_cb(GObject *unused, GParamSpec *pspec, GPViewPartCtrl *ctrl)
{
    GPViewPartCtrlPrivate *privat = GPVIEW_PART_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL) {

        int visible   = (privat->current_view != NULL);
        int sensitive = visible && gparts_database_connected(privat->database);

        gtk_action_group_set_sensitive(
            privat->action_group[1],
            sensitive
            );

        gtk_action_group_set_visible(
            privat->action_group[1],
            visible
            );
    }
}


static void
gpview_part_ctrl_update_2_cb(GObject *unused, GParamSpec *pspec, GPViewPartCtrl *ctrl)
{
    GPViewPartCtrlPrivate *privat = GPVIEW_PART_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL) {

        gboolean visible = (privat->current_view != NULL);
        SchDrawing *drawing = gpview_part_view_get_drawing(privat->current_view);
        gboolean sensitive = visible && (drawing != NULL);

        misc_object_unref(drawing);

        gtk_action_group_set_sensitive(
            privat->action_group[2],
            sensitive
            );

        gtk_action_group_set_visible(
            privat->action_group[2],
            visible
            );
    }
}

/* This function sends "failed to create drawable" to the terminal. It
 * is caused by gnome-open.
 */
static void
gpview_part_ctrl_copy_cb(GtkAction *action, GPViewPartCtrl *ctrl)
{
    GPViewPartCtrlPrivate *privat = GPVIEW_PART_CTRL_GET_PRIVATE(ctrl);

    if (privat != NULL) {

        SchDrawing *drawing = gpview_part_view_get_drawing(privat->current_view);

        schgui_clipboard_copy_drawing(privat->clipboard, drawing);

        misc_object_unref(drawing);
    }
}

