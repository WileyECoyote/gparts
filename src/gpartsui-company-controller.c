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

/*! \file gpartsui-company-controller.c
 */

#include "gpartsui.h"

#define GPARTSUI_COMPANY_CONTROLLER_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE(object,GPARTSUI_TYPE_COMPANY_CONTROLLER,GPartsUICompanyControllerPrivate)

enum
{
    GPARTSUI_COMPANY_CONTROLLER_COMPANY_MODEL = 1,
    GPARTSUI_COMPANY_CONTROLLER_COMPANY_VIEW
};

typedef struct _GPartsUICompanyControllerPrivate GPartsUICompanyControllerPrivate;

struct _GPartsUICompanyControllerPrivate
{
    MiscUIActionController   *delete_controller;
    MiscUIActionController   *edit_controller;
    MiscUIActionController   *insert_controller;
    GPartsUIResultController *result_controller;

    GPartsUICompanyModel     *company_model;

    GtkTreeSelection         *company_selection;
    GtkTreeView              *company_tree_view;
    GPartsUIResultAdapter    *company_tree_model;

    GtkAction                *open_website_action;

    GStrv                    websites;
};

static void
gpartsui_company_controller_init(GTypeInstance *instance, void *g_class);

static void
gpartsui_company_controller_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec);

static void
gpartsui_company_controller_set_delete_action(void *controller, GtkAction *action);

static void
gpartsui_company_controller_set_edit_action(void *controller, GtkAction *action);

static void
gpartsui_company_controller_set_insert_action(void *controller, GtkAction *action);

static void
gpartsui_company_controller_set_open_website_action(void *controller, GtkAction *action);

static void
gpartsui_company_controller_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec);

static void
gpartsui_company_controller_update_open_website_action(GPartsUICompanyController *controller);

static void
gpartsui_company_controller_update_tree_model(GPartsUICompanyController *controller);

static void
gpartsui_company_controller_update_tree_view(GPartsUICompanyController *controller);

static void
gpartsui_company_controller_update_websites(GPartsUICompanyController *controller);

/**** signal handlers ****/

static void
gpartsui_company_controller_notify_company_result_cb(GPartsUICompanyModel *model, GParamSpec *param, GPartsUICompanyController *controller);

static void
gpartsui_company_controller_open_website_cb(GtkAction *action, GPartsUICompanyController *controller);

static void
gpartsui_company_controller_changed_selection_cb(GtkTreeSelection *selection, GPartsUICompanyController *controller);


static void
gpartsui_company_controller_class_init(void *g_class, void *g_class_data)
{
    GPartsUIViewControllerClass *klasse = GPARTSUI_VIEW_CONTROLLER_CLASS(g_class);

    g_type_class_add_private(g_class, sizeof(GPartsUICompanyControllerPrivate));

    klasse->parent.get_property = gpartsui_company_controller_get_property;
    klasse->parent.set_property = gpartsui_company_controller_set_property;

    klasse->set_delete_action       = gpartsui_company_controller_set_delete_action;
    klasse->set_edit_action         = gpartsui_company_controller_set_edit_action;
    klasse->set_insert_action       = gpartsui_company_controller_set_insert_action;
    klasse->set_open_website_action = gpartsui_company_controller_set_open_website_action;

    g_object_class_install_property(G_OBJECT_CLASS(g_class),
                                    GPARTSUI_COMPANY_CONTROLLER_COMPANY_MODEL,
                                    g_param_spec_object("company-model",
                                                        "Company Model",
                                                        "Comapny Model",
                                                        GPARTSUI_TYPE_COMPANY_MODEL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(G_OBJECT_CLASS(g_class),
                                    GPARTSUI_COMPANY_CONTROLLER_COMPANY_VIEW,
                                    g_param_spec_object("company-view",
                                                        "Company View",
                                                        "Comapny View",
                                                        GTK_TYPE_TREE_VIEW,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));
}

GPartsUICompanyModel*
gpartsui_company_controller_get_company_model(const GPartsUICompanyController *controller)
{
    return NULL;
}

GtkTreeView*
gpartsui_company_controller_get_company_view(const GPartsUICompanyController *controller)
{
    return NULL;
}

static void
gpartsui_company_controller_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec)
{
  GPartsUICompanyController *controller = GPARTSUI_COMPANY_CONTROLLER(object);

  switch (property_id) {

    case GPARTSUI_COMPANY_CONTROLLER_COMPANY_MODEL:
      g_value_take_object(value, gpartsui_company_controller_get_company_model(controller));
      break;

    case GPARTSUI_COMPANY_CONTROLLER_COMPANY_VIEW:
      g_value_take_object(value, gpartsui_company_controller_get_company_view(controller));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
  }
}


GType
gpartsui_company_controller_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(GPartsUICompanyControllerClass),    /* class_size */
            NULL,                                      /* base_init */
            NULL,                                      /* base_finalize */
            gpartsui_company_controller_class_init,    /* class_init */
            NULL,                                      /* class_finalize */
            NULL,                                      /* class_data */
            sizeof(GPartsUICompanyController),         /* instance_size */
            0,                                         /* n_preallocs */
            gpartsui_company_controller_init,          /* instance_init */
            NULL                                       /* value_table */
            };

        type = g_type_register_static(GPARTSUI_TYPE_VIEW_CONTROLLER,
                                      "GPartsUICompanyController",
                                      &tinfo,
                                      0);
    }

    return type;
}

static void
gpartsui_company_controller_init(GTypeInstance *instance, void *g_class)
{
  GPartsUICompanyControllerPrivate *privat = GPARTSUI_COMPANY_CONTROLLER_GET_PRIVATE(instance);

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
gpartsui_company_controller_notify_company_result_cb(GPartsUICompanyModel *model, GParamSpec *param, GPartsUICompanyController *controller)
{
    gpartsui_company_controller_update_tree_model(controller);
}

void
gpartsui_company_controller_open_website(const GPartsUICompanyController *controller)
{
  GPartsUICompanyControllerPrivate *privat = GPARTSUI_COMPANY_CONTROLLER_GET_PRIVATE(controller);

  if (privat != NULL) {

    if (privat->websites != NULL) {

      char  **temp = privat->websites;

      while (*temp != NULL) {

        GString *command = g_string_new(NULL);

        g_string_printf(command, "gnome-open %s", *temp );

        g_spawn_command_line_async(command->str, NULL);

        g_string_free(command, TRUE);

        temp++;
      }
    }
  }
}

static void
gpartsui_company_controller_open_website_cb(GtkAction *action,
                                            GPartsUICompanyController *controller)
{
    gpartsui_company_controller_open_website(controller);
}

static void
gpartsui_company_controller_changed_selection_cb(GtkTreeSelection *selection,
                                                 GPartsUICompanyController *controller)
{
    gpartsui_company_controller_update_websites(controller);
}

void
gpartsui_company_controller_set_company_model(GPartsUICompanyController *controller,
                                              GPartsUICompanyModel *model)
{
    GPartsUICompanyControllerPrivate *privat =
    GPARTSUI_COMPANY_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        if (privat->company_model != NULL) {

            g_signal_handlers_disconnect_by_func( privat->company_model,
                G_CALLBACK(gpartsui_company_controller_notify_company_result_cb),
                controller);

            g_object_unref(privat->company_model);
        }

        privat->company_model = model;

        if (privat->company_model != NULL) {

            g_object_ref(privat->company_model);

            g_signal_connect(
                privat->company_model,
                "notify::company-result",
                G_CALLBACK(gpartsui_company_controller_notify_company_result_cb),
                controller
                );
        }

        gpartsui_company_controller_update_tree_model(controller);
    }
}

void
gpartsui_company_controller_set_company_view(GPartsUICompanyController *controller, GtkTreeView *view)
{
    GPartsUICompanyControllerPrivate *privat =
    GPARTSUI_COMPANY_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        if (privat->company_selection != NULL) {

            g_signal_handlers_disconnect_by_func (privat->company_selection,
                G_CALLBACK(gpartsui_company_controller_changed_selection_cb),
                controller);

            g_object_unref(privat->company_selection);

            privat->company_selection = NULL;
        }

        if (privat->company_tree_view != NULL) {

            g_object_unref(privat->company_tree_view);
        }

        privat->company_tree_view = view;

        if (privat->company_tree_view != NULL) {

            g_object_ref(privat->company_tree_view);

            privat->company_selection = gtk_tree_view_get_selection(privat->company_tree_view);
        }

        if (privat->company_selection != NULL) {

            g_object_ref(privat->company_selection);

            g_signal_connect(privat->company_selection, "changed",
                G_CALLBACK(gpartsui_company_controller_changed_selection_cb),
                controller);
        }
    }
}

static void
gpartsui_company_controller_set_delete_action(void *pcontroller, GtkAction *action)
{
    GPartsUICompanyController *controller = pcontroller;
    GPartsUICompanyControllerPrivate *privat =
    GPARTSUI_COMPANY_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        miscui_action_controller_set_action(privat->delete_controller, action);
    }
}

static void
gpartsui_company_controller_set_edit_action(void *pcontroller, GtkAction *action)
{
    GPartsUICompanyController *controller = pcontroller;
    GPartsUICompanyControllerPrivate *privat =
    GPARTSUI_COMPANY_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        miscui_action_controller_set_action(privat->edit_controller, action);
    }
}

static void
gpartsui_company_controller_set_insert_action(void *pcontroller, GtkAction *action)
{
    GPartsUICompanyController *controller = pcontroller;
    GPartsUICompanyControllerPrivate *privat =
    GPARTSUI_COMPANY_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        miscui_action_controller_set_action(privat->insert_controller, action);
    }
}

static void
gpartsui_company_controller_set_open_website_action(void *pcontroller, GtkAction *action)
{
    GPartsUICompanyController *controller = pcontroller;
    GPartsUICompanyControllerPrivate *privat =
    GPARTSUI_COMPANY_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        if (privat->open_website_action != NULL) {

            g_signal_handlers_disconnect_by_func (privat->open_website_action,
                G_CALLBACK(gpartsui_company_controller_open_website_cb),
                controller);

            g_object_unref(privat->open_website_action);
        }

        privat->open_website_action = action;

        if (privat->open_website_action != NULL) {

            g_object_ref(privat->open_website_action);

            g_signal_connect(privat->open_website_action, "activate",
                G_CALLBACK(gpartsui_company_controller_open_website_cb),
                controller);

            gpartsui_company_controller_update_open_website_action(controller);
        }
    }
}


static void
gpartsui_company_controller_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec)
{
    GPartsUICompanyController *controller = GPARTSUI_COMPANY_CONTROLLER(object);

    switch (property_id) {

        case GPARTSUI_COMPANY_CONTROLLER_COMPANY_MODEL:
            gpartsui_company_controller_set_company_model(controller,
                                                          GPARTSUI_COMPANY_MODEL(g_value_get_object(value)));
            break;

        case GPARTSUI_COMPANY_CONTROLLER_COMPANY_VIEW:
            gpartsui_company_controller_set_company_view(controller,
                                                         GTK_TREE_VIEW(g_value_get_object(value)));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static void
gpartsui_company_controller_update_open_website_action(GPartsUICompanyController *controller)
{
    GPartsUICompanyControllerPrivate *privat = GPARTSUI_COMPANY_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        unsigned int count = 0;

        if (privat->websites != NULL) {

            count = g_strv_length(privat->websites);
        }

        gtk_action_set_sensitive(privat->open_website_action, (count > 0));

        if (count > 1) {

            gtk_action_set_label(privat->open_website_action, "Open Company Websites");
        }
        else {

            gtk_action_set_label(privat->open_website_action, "Open Company Website");
        }
    }
}

static void
gpartsui_company_controller_update_tree_model(GPartsUICompanyController *controller)
{
    GPartsUICompanyControllerPrivate *privat = GPARTSUI_COMPANY_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        GPartsDatabaseResult *database_result;

        if (privat->company_tree_model != NULL) {

            g_object_unref(privat->company_tree_model);

            privat->company_tree_model = NULL;
        }

        database_result = gpartsui_company_model_get_company_result(privat->company_model);

        if (database_result != NULL) {

            privat->company_tree_model = gpartsui_result_adapter_new(database_result);

            g_object_unref(database_result);
        }

        gpartsui_company_controller_update_tree_view(controller);
    }
}

static void
gpartsui_company_controller_update_tree_view(GPartsUICompanyController *controller)
{
    GPartsUICompanyControllerPrivate *privat = GPARTSUI_COMPANY_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        if (privat->company_tree_view) {

            gpartsui_result_adapter_adjust_columns(privat->company_tree_model, privat->company_tree_view);

            gtk_tree_view_set_model(privat->company_tree_view, GTK_TREE_MODEL(privat->company_tree_model));
        }
    }
}

static void
gpartsui_company_controller_update_websites(GPartsUICompanyController *controller)
{
    GPartsUICompanyControllerPrivate *privat = GPARTSUI_COMPANY_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        g_strfreev(privat->websites);

        privat->websites = gpartsui_result_adapter_get_fields(privat->company_tree_model, privat->company_selection, 2);

        gpartsui_company_controller_update_open_website_action(controller);
    }
}
