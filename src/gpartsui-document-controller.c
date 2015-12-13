/* gEDA - GPL Electronic Design Automation
 * gparts - gEDA Parts Manager
 * Copyright (C) 2010 Edward C. Hennessy
 * Copyright (C) 2010 gEDA Contributors (see ChangeLog for details)
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

/*! \file gpartsui-document-controller.c
 */

#include "gpartsui.h"

#define GPARTSUI_DOCUMENT_CONTROLLER_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE(object,GPARTSUI_TYPE_DOCUMENT_CONTROLLER,GPartsUIDocumentControllerPrivate)

enum
{
    GPARTSUI_DOCUMENT_CONTROLLER_DOCUMENT_MODEL = 1,
    GPARTSUI_DOCUMENT_CONTROLLER_DOCUMENT_VIEW
};

typedef struct _GPartsUIDocumentControllerPrivate GPartsUIDocumentControllerPrivate;

struct _GPartsUIDocumentControllerPrivate
{
    MiscUIActionController   *delete_controller;
    MiscUIActionController   *edit_controller;
    MiscUIActionController   *insert_controller;
    GPartsUIResultController *result_controller;

    GPartsUIDocumentModel    *document_model;

    GtkTreeSelection         *document_selection;
    GtkTreeView              *document_tree_view;
    GPartsUIResultAdapter    *document_tree_model;

    GtkAction                *open_document_action;

    GStrv                    documents;
};

static void
gpartsui_document_controller_init(GTypeInstance *instance, void *g_class);

static void
gpartsui_document_controller_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec);

static void
gpartsui_document_controller_set_delete_action(void *controller, GtkAction *action);

static void
gpartsui_document_controller_set_edit_action(void *controller, GtkAction *action);

static void
gpartsui_document_controller_set_insert_action(void *controller, GtkAction *action);

static void
gpartsui_document_controller_set_open_document_action(void *controller, GtkAction *action);

static void
gpartsui_document_controller_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec);

static void
gpartsui_document_controller_update_open_document_action(GPartsUIDocumentController *controller);

static void
gpartsui_document_controller_update_tree_model(GPartsUIDocumentController *controller);

static void
gpartsui_document_controller_update_tree_view(GPartsUIDocumentController *controller);

static void
gpartsui_document_controller_update_documents(GPartsUIDocumentController *controller);

/**** signal handlers ****/

static void
gpartsui_document_controller_notify_document_result_cb(GPartsUICompanyModel *model, GParamSpec *param, GPartsUIDocumentController *controller);

static void
gpartsui_document_controller_open_document_cb(GtkAction *action, GPartsUIDocumentController *controller);

static void
gpartsui_document_controller_changed_selection_cb(GtkTreeSelection *selection, GPartsUIDocumentController *controller);


static void
gpartsui_document_controller_class_init(void *g_class, void *g_class_data)
{
    GPartsUIViewControllerClass *klasse = GPARTSUI_VIEW_CONTROLLER_CLASS(g_class);

    g_type_class_add_private(g_class, sizeof(GPartsUIDocumentControllerPrivate));

    klasse->parent.get_property = gpartsui_document_controller_get_property;
    klasse->parent.set_property = gpartsui_document_controller_set_property;

    klasse->set_delete_action        = gpartsui_document_controller_set_delete_action;
    klasse->set_edit_action          = gpartsui_document_controller_set_edit_action;
    klasse->set_insert_action        = gpartsui_document_controller_set_insert_action;
    klasse->set_open_document_action = gpartsui_document_controller_set_open_document_action;

    g_object_class_install_property(G_OBJECT_CLASS(g_class),
                                    GPARTSUI_DOCUMENT_CONTROLLER_DOCUMENT_MODEL,
                                    g_param_spec_object("document-model",
                                                        "Document Model",
                                                        "Document Model",
                                                        GPARTSUI_TYPE_DOCUMENT_MODEL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(G_OBJECT_CLASS(g_class),
                                    GPARTSUI_DOCUMENT_CONTROLLER_DOCUMENT_VIEW,
                                    g_param_spec_object("document-view",
                                                        "Document View",
                                                        "Document View",
                                                        GTK_TYPE_TREE_VIEW,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));
}

GPartsUIDocumentModel*
gpartsui_document_controller_get_document_model(const GPartsUIDocumentController *controller)
{
    return NULL;
}

GtkTreeView*
gpartsui_document_controller_get_document_view(const GPartsUIDocumentController *controller)
{
    return NULL;
}

static void
gpartsui_document_controller_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec)
{
    GPartsUIDocumentController *controller = GPARTSUI_DOCUMENT_CONTROLLER(object);

    switch (property_id)  {

        case GPARTSUI_DOCUMENT_CONTROLLER_DOCUMENT_MODEL:
            g_value_take_object(value, gpartsui_document_controller_get_document_model(controller));
            break;

        case GPARTSUI_DOCUMENT_CONTROLLER_DOCUMENT_VIEW:
            g_value_take_object(value, gpartsui_document_controller_get_document_view(controller));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}


GType
gpartsui_document_controller_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(GPartsUIDocumentControllerClass),    /* class_size */
            NULL,                                      /* base_init */
            NULL,                                      /* base_finalize */
            gpartsui_document_controller_class_init,    /* class_init */
            NULL,                                      /* class_finalize */
            NULL,                                      /* class_data */
            sizeof(GPartsUIDocumentController),         /* instance_size */
            0,                                         /* n_preallocs */
            gpartsui_document_controller_init,          /* instance_init */
            NULL                                       /* value_table */
            };

        type = g_type_register_static(GPARTSUI_TYPE_VIEW_CONTROLLER,
                                      "GPartsUIDocumentController",
                                      &tinfo,
                                      0);
    }

    return type;
}

static void
gpartsui_document_controller_init(GTypeInstance *instance, void *g_class)
{
    GPartsUIDocumentControllerPrivate *privat = GPARTSUI_DOCUMENT_CONTROLLER_GET_PRIVATE(instance);

    if (privat != NULL) {

        privat->delete_controller = MISCUI_ACTION_CONTROLLER(g_object_new(
            MISCUI_TYPE_ACTION_CONTROLLER,
            "default-label",  "Delete",
            "sensitive",      FALSE,
            NULL));

        privat->edit_controller = MISCUI_ACTION_CONTROLLER(g_object_new(
            MISCUI_TYPE_ACTION_CONTROLLER,
            "default-label",  "Edit",
            "sensitive",      FALSE,
            NULL));

        privat->insert_controller = MISCUI_ACTION_CONTROLLER(g_object_new(
            MISCUI_TYPE_ACTION_CONTROLLER,
            "default-label",  "Insert",
            "sensitive",      FALSE,
            NULL));

        privat->result_controller = GPARTSUI_RESULT_CONTROLLER(g_object_new(
            GPARTSUI_TYPE_RESULT_CONTROLLER,
            NULL));
    }
}

static void
gpartsui_document_controller_notify_document_result_cb(GPartsUICompanyModel *model, GParamSpec *param, GPartsUIDocumentController *controller)
{
    gpartsui_document_controller_update_tree_model(controller);
}

void
gpartsui_document_controller_open_document(const GPartsUIDocumentController *controller)
{
    GPartsUIDocumentControllerPrivate *privat = GPARTSUI_DOCUMENT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        if (privat->documents != NULL) {

            char **temp = privat->documents;

            while (*temp != NULL) {

                GString *command = g_string_new(NULL);

                g_string_printf(command, "gnome-open %s", *temp);

                g_spawn_command_line_async(command->str, NULL);

                g_string_free(command, TRUE);

                temp++;
            }
        }
    }
}

static void
gpartsui_document_controller_open_document_cb(GtkAction *action,
                                              GPartsUIDocumentController *controller)
{
    gpartsui_document_controller_open_document(controller);
}

static void
gpartsui_document_controller_changed_selection_cb(GtkTreeSelection *selection,
                                                  GPartsUIDocumentController *controller)
{
    gpartsui_document_controller_update_documents(controller);
}

void
gpartsui_document_controller_set_document_model(GPartsUIDocumentController *controller,
                                                GPartsUIDocumentModel      *model)
{
    GPartsUIDocumentControllerPrivate *privat =
    GPARTSUI_DOCUMENT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        if (privat->document_model != NULL) {

            g_signal_handlers_disconnect_by_func(privat->document_model,
                G_CALLBACK(gpartsui_document_controller_notify_document_result_cb),
                controller);

            g_object_unref(privat->document_model);
        }

        privat->document_model = model;

        if (privat->document_model != NULL) {

            g_object_ref(privat->document_model);

            g_signal_connect(privat->document_model,"notify::company-result",
                G_CALLBACK(gpartsui_document_controller_notify_document_result_cb),
                controller);
        }

        gpartsui_document_controller_update_tree_model(controller);
    }
}

void
gpartsui_document_controller_set_document_view(GPartsUIDocumentController *controller, GtkTreeView *view)
{
    GPartsUIDocumentControllerPrivate *privat =
    GPARTSUI_DOCUMENT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        if (privat->document_selection != NULL) {

            g_signal_handlers_disconnect_by_func(privat->document_selection,
                G_CALLBACK(gpartsui_document_controller_changed_selection_cb),
                controller);

            g_object_unref(privat->document_selection);

            privat->document_selection = NULL;
        }

        if (privat->document_tree_view != NULL) {

            g_object_unref(privat->document_tree_view);
        }

        privat->document_tree_view = view;

        if (privat->document_tree_view != NULL) {

            g_object_ref(privat->document_tree_view);

            privat->document_selection = gtk_tree_view_get_selection(privat->document_tree_view);
        }

        if (privat->document_selection != NULL)  {

            g_object_ref(privat->document_selection);

            g_signal_connect(privat->document_selection,"changed",
                G_CALLBACK(gpartsui_document_controller_changed_selection_cb),
                controller);
        }
    }
}

static void
gpartsui_document_controller_set_delete_action(void *pcontroller, GtkAction *action)
{
    GPartsUIDocumentController *controller = pcontroller;
    GPartsUIDocumentControllerPrivate *privat =
    GPARTSUI_DOCUMENT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)  {

        miscui_action_controller_set_action(privat->delete_controller, action);
    }
}

static void
gpartsui_document_controller_set_edit_action(void *pcontroller, GtkAction *action)
{
    GPartsUIDocumentController *controller = pcontroller;
    GPartsUIDocumentControllerPrivate *privat =
    GPARTSUI_DOCUMENT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)  {

        miscui_action_controller_set_action(privat->edit_controller, action);
    }
}

static void
gpartsui_document_controller_set_insert_action(void *pcontroller, GtkAction *action)
{
    GPartsUIDocumentController *controller = pcontroller;
    GPartsUIDocumentControllerPrivate *privat =
    GPARTSUI_DOCUMENT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)  {

        miscui_action_controller_set_action(privat->insert_controller, action);
    }
}

static void
gpartsui_document_controller_set_open_document_action(void *pcontroller, GtkAction *action)
{
    GPartsUIDocumentController *controller = pcontroller;
    GPartsUIDocumentControllerPrivate *privat =
    GPARTSUI_DOCUMENT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        if (privat->open_document_action != NULL) {

            g_signal_handlers_disconnect_by_func(privat->open_document_action,
                G_CALLBACK(gpartsui_document_controller_open_document_cb),
                controller);

            g_object_unref(privat->open_document_action);
        }

        privat->open_document_action = action;

        if (privat->open_document_action != NULL) {

            g_object_ref(privat->open_document_action);

            g_signal_connect(privat->open_document_action, "activate",
                G_CALLBACK(gpartsui_document_controller_open_document_cb),
                controller);

            gpartsui_document_controller_update_open_document_action(controller);
        }
    }
}


static void
gpartsui_document_controller_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec)
{
    GPartsUIDocumentController *controller = GPARTSUI_DOCUMENT_CONTROLLER(object);

    switch (property_id) {

        case GPARTSUI_DOCUMENT_CONTROLLER_DOCUMENT_MODEL:
            gpartsui_document_controller_set_document_model(controller, GPARTSUI_DOCUMENT_MODEL(g_value_get_object(value)));
            break;

        case GPARTSUI_DOCUMENT_CONTROLLER_DOCUMENT_VIEW:
            gpartsui_document_controller_set_document_view(controller, GTK_TREE_VIEW(g_value_get_object(value)));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static void
gpartsui_document_controller_update_open_document_action(GPartsUIDocumentController *controller)
{
    GPartsUIDocumentControllerPrivate *privat = GPARTSUI_DOCUMENT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        unsigned int count = 0;

        if (privat->documents != NULL) {

            count = g_strv_length(privat->documents);
        }

        gtk_action_set_sensitive(privat->open_document_action, (count > 0));

        if (count > 1) {

            gtk_action_set_label(privat->open_document_action, "Open Documents");
        }
        else {

            gtk_action_set_label(privat->open_document_action, "Open Document");
        }
    }
}

static void
gpartsui_document_controller_update_tree_model(GPartsUIDocumentController *controller)
{
    GPartsUIDocumentControllerPrivate *privat = GPARTSUI_DOCUMENT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        GPartsDatabaseResult *database_result;

        if (privat->document_tree_model != NULL) {

            g_object_unref(privat->document_tree_model);

            privat->document_tree_model = NULL;
        }

        database_result = gpartsui_document_model_get_document_result(privat->document_model);

        if (database_result != NULL)  {

            privat->document_tree_model = gpartsui_result_adapter_new(database_result);

            g_object_unref(database_result);
        }

        gpartsui_document_controller_update_tree_view(controller);
    }
}

static void
gpartsui_document_controller_update_tree_view(GPartsUIDocumentController *controller)
{
    GPartsUIDocumentControllerPrivate *privat = GPARTSUI_DOCUMENT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        if (privat->document_tree_view) {

            gpartsui_result_adapter_adjust_columns(privat->document_tree_model, privat->document_tree_view);

            gtk_tree_view_set_model(privat->document_tree_view, GTK_TREE_MODEL(privat->document_tree_model));
        }
    }
}

static void
gpartsui_document_controller_update_documents(GPartsUIDocumentController *controller)
{
    GPartsUIDocumentControllerPrivate *privat = GPARTSUI_DOCUMENT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        g_strfreev(privat->documents);

        privat->documents = gpartsui_result_adapter_get_fields(privat->document_tree_model, privat->document_selection, 2);

        gpartsui_document_controller_update_open_document_action(controller);
    }
}
