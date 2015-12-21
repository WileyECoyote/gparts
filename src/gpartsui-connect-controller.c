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

/*! \file gparts-connect-controller.c
 *
 *  This module contains a large number of signal handlers. Keep the signal
 *  handlers generic enough to allow a generic binding system to be
 *  implemented.
 */

#include "gpartsui.h"


#define GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE(object,GPARTSUI_TYPE_CONNECT_CONTROLLER,GPartsUIConnectControllerPrivate)

enum
{
    GPARTSUI_CONNECT_CONTROLLER_CONNECT_ACTION = 1,
    GPARTSUI_CONNECT_CONTROLLER_CONNECT_MODEL
};

typedef struct _GPartsUIConnectControllerPrivate GPartsUIConnectControllerPrivate;

struct _GPartsUIConnectControllerPrivate
{
    GtkAction             *connect_action;
    GPartsUIConnectModel  *connect_model;

    /* private */

    GtkDialog             *connect_dialog;
    GtkDialog             *file_dialog;

    GtkWidget             *table;
    GtkWidget             *image;

    GtkWidget             *connect_button;
    GtkWidget             *database_entry;
    GtkWidget             *database_label;
    GtkWidget             *database_type_combo;
    GtkWidget             *database_type_label;
    GtkWidget             *filename_button;
    GtkWidget             *filename_entry;
    GtkWidget             *filename_label;
    GtkWidget             *hostname_entry;
    GtkWidget             *hostname_label;
    GtkWidget             *password_entry;
    GtkWidget             *password_label;
    GtkWidget             *username_entry;
    GtkWidget             *username_label;
};



static void
gpartsui_connect_controller_class_init(gpointer g_class, gpointer g_class_data);

static void
gpartsui_connect_controller_dispose(GObject *object);

static void
gpartsui_connect_controller_finalize(GObject *object);

static void
gpartsui_connect_controller_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
gpartsui_connect_controller_instance_init(GTypeInstance *instance, gpointer g_class);

static void
gpartsui_connect_controller_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

/**** Signal handlers ****/

static void
gpartsui_connect_controller_connect_action_cb(GtkAction *action, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_database_notify_cb(GtkEntry *entry, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_database_sensitive_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_database_name_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_database_name_sensitive_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_database_name_text_notify_cb(GtkEntry* entry, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_database_name_valid_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_database_type_changed_cb(GtkComboBox *combo, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_database_types_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_database_valid_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);

static void
gparts_connect_controller_filename_button_clicked_cb(GtkButton *button, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_filename_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_filename_sensitive_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_filename_text_notify_cb(GtkEntry* entry, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_filename_valid_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_hostname_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_hostname_sensitive_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_hostname_text_notify_cb(GtkEntry* entry, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_hostname_valid_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_password_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_password_sensitive_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_password_text_notify_cb(GtkEntry *entry, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_password_valid_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_username_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_username_sensitive_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_username_text_notify_cb(GtkEntry* entry, GParamSpec *param, GPartsUIConnectController *controller);

static void
gpartsui_connect_controller_username_valid_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller);



static void
gpartsui_connect_controller_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass* klasse = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(g_class, sizeof(GPartsUIConnectControllerPrivate));

    klasse->dispose  = gpartsui_connect_controller_dispose;
    klasse->finalize = gpartsui_connect_controller_finalize;

    klasse->get_property = gpartsui_connect_controller_get_property;
    klasse->set_property = gpartsui_connect_controller_set_property;

    g_object_class_install_property(klasse,
                                    GPARTSUI_CONNECT_CONTROLLER_CONNECT_ACTION,
                                    g_param_spec_object("connect-action",
                                                        "Connect Action",
                                                        "Connect Action",
                                                        GTK_TYPE_ACTION,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(klasse,
                                    GPARTSUI_CONNECT_CONTROLLER_CONNECT_MODEL,
                                    g_param_spec_object("connect-model",
                                                        "Connect Model",
                                                        "Connect Model",
                                                        GPARTSUI_TYPE_CONNECT_MODEL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));
}

void
gpartsui_connect_controller_connect(GPartsUIConnectController *controller)
{
  GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

  if (privat != NULL) {

    int result = gtk_dialog_run(privat->connect_dialog);

    if (result == GTK_RESPONSE_OK) {

      GError *error = NULL;

      gpartsui_connect_model_connect(privat->connect_model, &error);

      if (error != NULL) {

        GtkWidget *error_dialog = gtk_message_dialog_new(NULL,
                                                         GTK_DIALOG_MODAL,
                                                         GTK_MESSAGE_ERROR,
                                                         GTK_BUTTONS_CLOSE,
                                                         "%s",
                                                         error->message);

        gtk_dialog_run(GTK_DIALOG(error_dialog));

        gtk_widget_destroy(error_dialog);

        g_clear_error(&error);
      }
    }

    gtk_widget_hide(GTK_WIDGET(privat->connect_dialog));
  }
}

static void
gpartsui_connect_controller_connect_action_cb(GtkAction *action, GPartsUIConnectController *controller)
{
    gpartsui_connect_controller_connect(controller);
}

static void
gpartsui_connect_controller_connect_sensitive_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        gtk_widget_set_sensitive(
            privat->connect_button,
            gpartsui_connect_model_get_connect_sensitive(privat->connect_model)
            );
    }
}

static void
gpartsui_connect_controller_database_name_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        char *database = gpartsui_connect_model_get_database_name(privat->connect_model);

        g_signal_handlers_block_by_func(
            privat->database_entry,
            G_CALLBACK(gpartsui_connect_controller_database_name_text_notify_cb),
            controller
            );

        gtk_entry_set_text(
            GTK_ENTRY(privat->database_entry),
            database
            );

        g_signal_handlers_unblock_by_func(
            privat->database_entry,
            G_CALLBACK(gpartsui_connect_controller_filename_text_notify_cb),
            controller
            );

        g_free(database);
    }
}

static void
gpartsui_connect_controller_database_name_sensitive_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        gtk_widget_set_sensitive(
            privat->database_entry,
            gpartsui_connect_model_get_database_name_sensitive(privat->connect_model)
            );
    }
}

static void
gpartsui_connect_controller_database_name_text_notify_cb(GtkEntry *entry, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        g_signal_handlers_block_by_func(
            privat->connect_model,
            G_CALLBACK(gpartsui_connect_controller_database_name_notify_cb),
            controller
            );

        gpartsui_connect_model_set_filename(
            privat->connect_model,
            gtk_entry_get_text(GTK_ENTRY(privat->database_entry))
            );

        g_signal_handlers_unblock_by_func(
            privat->connect_model,
            G_CALLBACK(gpartsui_connect_controller_database_name_notify_cb),
            controller
            );
    }
}

static void
gpartsui_connect_controller_database_name_valid_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
    }
}


static void
gpartsui_connect_controller_database_notify_cb(GtkEntry *entry, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        gpartsui_connect_model_set_database_name(
            privat->connect_model,
            gtk_entry_get_text(GTK_ENTRY(privat->database_entry))
            );
    }
}

static void
gpartsui_connect_controller_database_sensitive_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        gtk_widget_set_sensitive(
            privat->database_entry,
            gpartsui_connect_model_get_database_sensitive(privat->connect_model)
            );
    }
}

static void
gpartsui_connect_controller_database_type_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
    }
}

static void
gpartsui_connect_controller_database_type_changed_cb(GtkComboBox *combo, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        g_signal_handlers_block_by_func(
            privat->connect_model,
            G_CALLBACK(gpartsui_connect_controller_database_type_notify_cb),
            controller
            );

        gpartsui_connect_model_set_database_type(
            privat->connect_model,
            gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(privat->database_type_combo))
            );

        g_signal_handlers_block_by_func(
            privat->connect_model,
            G_CALLBACK(gpartsui_connect_controller_database_type_notify_cb),
            controller
            );
    }
}

static void
gpartsui_connect_controller_database_types_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        GStrv types = gpartsui_connect_model_get_database_types(privat->connect_model);

        gtk_list_store_clear(
            GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(privat->database_type_combo)))
            );

        /* gtk_combo_box_get_model() does not increment the reference count */

        if (types != NULL)
        {
            GStrv temp = types;

            while (*temp != NULL)
            {
                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(privat->database_type_combo), *temp);

                temp++;
            }

            if (gtk_combo_box_get_active(GTK_COMBO_BOX(privat->database_type_combo)) < 0)
            {
                gtk_combo_box_set_active(GTK_COMBO_BOX(privat->database_type_combo), 0);
            }

            g_strfreev(types);
        }
    }
}

static void
gpartsui_connect_controller_database_valid_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
    }
}


static void
gpartsui_connect_controller_dispose(GObject *object)
{
  GPartsUIConnectControllerPrivate *privat =
  GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(object);

  if (privat != NULL) {

    misc_object_unref(privat->connect_dialog);
    privat->connect_dialog = NULL;

    misc_object_unref(privat->file_dialog);
    privat->file_dialog = NULL;
  }

  misc_object_chain_dispose(object);
}

static void
gpartsui_connect_controller_finalize(GObject *object)
{
    GPartsUIConnectControllerPrivate *privat =
    GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(object);

    misc_object_chain_finalize(object);
}

static void
gpartsui_connect_controller_filename_button_clicked_cb(GtkButton *button, GPartsUIConnectController *controller)
{
  GPartsUIConnectControllerPrivate *privat =
  GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

  if ((privat != NULL) && (privat->file_dialog != NULL)) {

    int result = gtk_dialog_run(GTK_DIALOG(privat->file_dialog));

    if (result == GTK_RESPONSE_OK) {

      GtkFileChooser *dialog;
      GtkEntry *entry;

      char *filename;

      dialog   = (GtkFileChooser*)privat->file_dialog;
      entry    = (GtkEntry*)privat->filename_entry;
      filename = gtk_file_chooser_get_filename(dialog);

      gtk_entry_set_text(entry, filename);

      g_free(filename);
    }

    gtk_widget_hide(GTK_WIDGET(privat->file_dialog));
  }
}


static void
gpartsui_connect_controller_filename_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
  GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

  if (privat != NULL) {

    char *filename = gpartsui_connect_model_get_filename(privat->connect_model);

    g_signal_handlers_block_by_func(privat->filename_entry,
    G_CALLBACK(gpartsui_connect_controller_filename_text_notify_cb),
                                    controller);

    gtk_entry_set_text(GTK_ENTRY(privat->filename_entry),
                       filename);

    g_signal_handlers_unblock_by_func(privat->filename_entry,
    G_CALLBACK(gpartsui_connect_controller_filename_text_notify_cb),
                                      controller);

    g_free(filename);
  }
}

static void
gpartsui_connect_controller_filename_sensitive_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
  GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

  if (privat != NULL) {

    int sensitive = gpartsui_connect_model_get_filename_sensitive(privat->connect_model);

    gtk_widget_set_sensitive(privat->filename_button, sensitive);

    gtk_widget_set_sensitive(privat->filename_entry, sensitive);
  }
}

static void
gpartsui_connect_controller_filename_text_notify_cb(GtkEntry *entry, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        g_signal_handlers_block_by_func(privat->connect_model,
            G_CALLBACK(gpartsui_connect_controller_filename_notify_cb),
            controller);

        gpartsui_connect_model_set_filename(privat->connect_model,
            gtk_entry_get_text(GTK_ENTRY(privat->filename_entry)));

        g_signal_handlers_unblock_by_func(privat->connect_model,
            G_CALLBACK(gpartsui_connect_controller_filename_notify_cb),
            controller);
    }
}

static void
gpartsui_connect_controller_filename_valid_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
/*
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
    }
*/
}

GType
gpartsui_connect_controller_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(GPartsUIConnectControllerClass),
            NULL,
            NULL,
            gpartsui_connect_controller_class_init,
            NULL,
            NULL,
            sizeof(GPartsUIConnectController),
            0,
            gpartsui_connect_controller_instance_init,
            NULL
            };

        type = g_type_register_static( G_TYPE_OBJECT,
                                       "GPartsUIConnectController",
                                       &tinfo,
                                       0);
    }
    return type;
}

GtkAction*
gpartsui_connect_controller_get_connect_action(const GPartsUIConnectController *controller)
{
    GtkAction *action = NULL;
    GPartsUIConnectControllerPrivate *privat =
    GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        action = privat->connect_action;

        if (action != NULL) {

            g_object_ref(action);
        }
    }

    return action;
}

GPartsUIConnectModel*
gpartsui_connect_controller_get_connect_model(const GPartsUIConnectController *controller)
{
    GPartsUIConnectModel *model = NULL;
    GPartsUIConnectControllerPrivate *privat =
    GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        model = privat->connect_model;

        if (model != NULL) {

            g_object_ref(model);
        }
    }

    return model;
}

static void
gpartsui_connect_controller_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
  switch (property_id) {

    case GPARTSUI_CONNECT_CONTROLLER_CONNECT_ACTION:
      g_value_take_object(value, gpartsui_connect_controller_get_connect_action(GPARTSUI_CONNECT_CONTROLLER(object)));
      break;

    case GPARTSUI_CONNECT_CONTROLLER_CONNECT_MODEL:
      g_value_take_object(value, gpartsui_connect_controller_get_connect_model(GPARTSUI_CONNECT_CONTROLLER(object)));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
  }
}

static void
gpartsui_connect_controller_hostname_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
  GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

  if (privat != NULL) {

    char *hostname = gpartsui_connect_model_get_hostname(privat->connect_model);

    g_signal_handlers_block_by_func(privat->hostname_entry,
    G_CALLBACK(gpartsui_connect_controller_hostname_text_notify_cb),
                                    controller);

    gtk_entry_set_text(GTK_ENTRY(privat->hostname_entry),
                       hostname);

    g_signal_handlers_unblock_by_func(privat->hostname_entry,
    G_CALLBACK(gpartsui_connect_controller_hostname_text_notify_cb),
                                      controller);

    g_free(hostname);
  }
}

static void
gpartsui_connect_controller_hostname_sensitive_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
  GPartsUIConnectControllerPrivate *privat =
  GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

  if (privat != NULL) {

    gtk_widget_set_sensitive(privat->hostname_entry,
    gpartsui_connect_model_get_hostname_sensitive(privat->connect_model));
  }
}

static void
gpartsui_connect_controller_hostname_text_notify_cb(GtkEntry *entry, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        g_signal_handlers_block_by_func(privat->connect_model,
            G_CALLBACK(gpartsui_connect_controller_hostname_notify_cb),
            controller);

        gpartsui_connect_model_set_hostname(privat->connect_model,
            gtk_entry_get_text(GTK_ENTRY(privat->hostname_entry)));

        g_signal_handlers_unblock_by_func(privat->connect_model,
            G_CALLBACK(gpartsui_connect_controller_hostname_notify_cb),
            controller);
    }
}

static void
gpartsui_connect_controller_hostname_valid_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
/*
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
    }
*/
}

static void
gpartsui_connect_controller_instance_init(GTypeInstance *instance, gpointer g_class)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(instance);

    if (privat != NULL) {

        GtkWidget *content;

        privat->connect_dialog =
        GTK_DIALOG(gtk_dialog_new_with_buttons("Connect to a Database",
                                               NULL,
                                               GTK_DIALOG_MODAL,
                                               GTK_STOCK_HELP,    GTK_RESPONSE_HELP,
                                               GTK_STOCK_CANCEL,  GTK_RESPONSE_CANCEL,
                                               GTK_STOCK_CONNECT, GTK_RESPONSE_OK,
                                               NULL));

        privat->file_dialog =
        GTK_DIALOG(gtk_file_chooser_dialog_new("Select a Database File",
                                               NULL,
                                               GTK_FILE_CHOOSER_ACTION_OPEN,
                                               GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                               GTK_STOCK_OK,     GTK_RESPONSE_OK,
                                               NULL));

        gtk_dialog_set_alternative_button_order(privat->connect_dialog,
                                                GTK_RESPONSE_HELP,
                                                GTK_RESPONSE_OK,
                                                GTK_RESPONSE_CANCEL,
                                                -1);

        gtk_dialog_set_default_response(privat->connect_dialog, GTK_RESPONSE_OK);

        gtk_container_set_border_width(GTK_CONTAINER(privat->connect_dialog), 5);

        content = gtk_dialog_get_content_area(privat->connect_dialog);

        gtk_box_set_spacing(GTK_BOX(content), 8);

        privat->table = gtk_table_new(6, 4, FALSE);
        gtk_table_set_row_spacings(GTK_TABLE(privat->table), 8);
        gtk_table_set_col_spacings(GTK_TABLE(privat->table), 10);
        gtk_widget_set_visible(GTK_WIDGET(privat->table), TRUE);
        gtk_container_add(GTK_CONTAINER(content), GTK_WIDGET(privat->table));

        /******** an image for decoration ********/

        privat->image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_AUTHENTICATION, GTK_ICON_SIZE_DIALOG);

        gtk_misc_set_alignment(
            GTK_MISC(privat->image),
            0,
            0
            );

        gtk_table_attach(
            GTK_TABLE(privat->table),                    /* table */
            privat->image,                               /* child */
            0,                                           /* left_attach */
            1,                                           /* right_attach */
            0,                                           /* top_attach */
            6,                                           /* bottom_attach */
            GTK_FILL,                                    /* xoptions */
            GTK_FILL,                                    /* yoptions */
            0,                                           /* xpadding */
            0                                            /* ypadding */
            );

        gtk_widget_set_visible(GTK_WIDGET(privat->image), TRUE);

        /******** database type ********/

        privat->database_type_label = gtk_label_new("Database Type");

        gtk_misc_set_alignment(
            GTK_MISC(privat->database_type_label),
            0,
            0.5
            );

        gtk_table_attach(
            GTK_TABLE(privat->table),                    /* table */
            privat->database_type_label,                 /* child */
            1,                                           /* left_attach */
            2,                                           /* right_attach */
            0,                                           /* top_attach */
            1,                                           /* bottom_attach */
            GTK_FILL,                                    /* xoptions */
            GTK_FILL,                                    /* yoptions */
            0,                                           /* xpadding */
            0                                            /* ypadding */
            );

        gtk_widget_set_visible(GTK_WIDGET(privat->database_type_label), TRUE);

        privat->database_type_combo = gtk_combo_box_text_new();

        gtk_table_attach(
            GTK_TABLE(privat->table),                    /* table */
            privat->database_type_combo,                 /* child */
            2,                                           /* left_attach */
            3,                                           /* right_attach */
            0,                                           /* top_attach */
            1,                                           /* bottom_attach */
            GTK_FILL,                                    /* xoptions */
            GTK_FILL,                                    /* yoptions */
            0,                                           /* xpadding */
            0                                            /* ypadding */
            );

        gtk_widget_set_visible(GTK_WIDGET(privat->database_type_combo), TRUE);

        g_signal_connect(
            privat->database_type_combo,
            "changed",
            G_CALLBACK(gpartsui_connect_controller_database_type_changed_cb),
            instance
            );

        /******** username ********/

        privat->username_label = gtk_label_new("Username");

        gtk_misc_set_alignment(
            GTK_MISC(privat->username_label),
            0,
            0.5
            );

        gtk_table_attach(
            GTK_TABLE(privat->table),                    /* table */
            privat->username_label,                      /* child */
            1,                                           /* left_attach */
            2,                                           /* right_attach */
            1,                                           /* top_attach */
            2,                                           /* bottom_attach */
            GTK_FILL,                                    /* xoptions */
            GTK_FILL,                                    /* yoptions */
            0,                                           /* xpadding */
            0                                            /* ypadding */
            );

        gtk_widget_set_visible(GTK_WIDGET(privat->username_label), TRUE);

        gtk_table_attach(
            GTK_TABLE(privat->table),                    /* table */
            privat->username_entry = gtk_entry_new(),    /* child */
            2,                                           /* left_attach */
            3,                                           /* right_attach */
            1,                                           /* top_attach */
            2,                                           /* bottom_attach */
            GTK_EXPAND | GTK_FILL,                       /* xoptions */
            GTK_FILL,                                    /* yoptions */
            0,                                           /* xpadding */
            0                                            /* ypadding */
            );

        gtk_widget_set_visible(GTK_WIDGET(privat->username_entry), TRUE);

        g_signal_connect(
            privat->username_entry,
            "notify::text",
            G_CALLBACK(gpartsui_connect_controller_username_text_notify_cb),
            instance
            );

        /******** password ********/

        privat->password_label = gtk_label_new("Password");

        gtk_misc_set_alignment(
            GTK_MISC(privat->password_label),
            0,
            0.5
            );

        gtk_table_attach(
            GTK_TABLE(privat->table),                    /* table */
            privat->password_label,                      /* child */
            1,                                           /* left_attach */
            2,                                           /* right_attach */
            2,                                           /* top_attach */
            3,                                           /* bottom_attach */
            GTK_FILL,                                    /* xoptions */
            GTK_FILL,                                    /* yoptions */
            0,                                           /* xpadding */
            0                                            /* ypadding */
            );

        gtk_widget_set_visible(GTK_WIDGET(privat->password_label), TRUE);

        gtk_table_attach(
            GTK_TABLE(privat->table),                    /* table */
            privat->password_entry = gtk_entry_new(),    /* child */
            2,                                           /* left_attach */
            3,                                           /* right_attach */
            2,                                           /* top_attach */
            3,                                           /* bottom_attach */
            GTK_FILL,                                    /* xoptions */
            GTK_FILL,                                    /* yoptions */
            0,                                           /* xpadding */
            0                                            /* ypadding */
            );

        g_object_set(privat->password_entry, "visibility", FALSE,NULL);
        gtk_widget_set_visible(GTK_WIDGET(privat->password_entry), TRUE);

        g_signal_connect(
            privat->password_entry,
            "notify::text",
            G_CALLBACK(gpartsui_connect_controller_password_text_notify_cb),
            instance
            );

        /******** hostname ********/

        privat->hostname_label = gtk_label_new("Server");

        gtk_misc_set_alignment(
            GTK_MISC(privat->hostname_label),
            0,
            0.5
            );

        gtk_table_attach(
            GTK_TABLE(privat->table),                    /* table */
            privat->hostname_label,                      /* child */
            1,                                           /* left_attach */
            2,                                           /* right_attach */
            3,                                           /* top_attach */
            4,                                           /* bottom_attach */
            GTK_FILL,                                    /* xoptions */
            GTK_FILL,                                    /* yoptions */
            0,                                           /* xpadding */
            0                                            /* ypadding */
            );

        gtk_widget_set_visible(GTK_WIDGET(privat->hostname_label), TRUE);

        gtk_table_attach(
            GTK_TABLE(privat->table),                    /* table */
            privat->hostname_entry = gtk_entry_new(),    /* child */
            2,                                           /* left_attach */
            3,                                           /* right_attach */
            3,                                           /* top_attach */
            4,                                           /* bottom_attach */
            GTK_FILL,                                    /* xoptions */
            GTK_FILL,                                    /* yoptions */
            0,                                           /* xpadding */
            0                                            /* ypadding */
            );

        gtk_widget_set_visible(GTK_WIDGET(privat->hostname_entry), TRUE);

        g_signal_connect(
            privat->hostname_entry,
            "notify::text",
            G_CALLBACK(gpartsui_connect_controller_hostname_text_notify_cb),
            instance
            );

        /******** database ********/

        privat->database_label = gtk_label_new("Database Name");

        gtk_misc_set_alignment(
            GTK_MISC(privat->database_label),
            0,
            0.5
            );

        gtk_table_attach(
            GTK_TABLE(privat->table),                    /* table */
            privat->database_label,                      /* child */
            1,                                           /* left_attach */
            2,                                           /* right_attach */
            4,                                           /* top_attach */
            5,                                           /* bottom_attach */
            GTK_FILL,                                    /* xoptions */
            GTK_FILL,                                    /* yoptions */
            0,                                           /* xpadding */
            0                                            /* ypadding */
            );

        gtk_widget_set_visible(GTK_WIDGET(privat->database_label), TRUE);

        gtk_table_attach(
            GTK_TABLE(privat->table),                    /* table */
            privat->database_entry = gtk_entry_new(),    /* child */
            2,                                           /* left_attach */
            3,                                           /* right_attach */
            4,                                           /* top_attach */
            5,                                           /* bottom_attach */
            GTK_FILL,                                    /* xoptions */
            GTK_FILL,                                    /* yoptions */
            0,                                           /* xpadding */
            0                                            /* ypadding */
            );

        gtk_widget_set_visible(GTK_WIDGET(privat->database_entry), TRUE);

        g_signal_connect(
            privat->database_entry,
            "notify::text",
            G_CALLBACK(gpartsui_connect_controller_database_notify_cb),
            instance
            );

        /******** filename ********/

        privat->filename_label = gtk_label_new("Filename");

        gtk_misc_set_alignment(
            GTK_MISC(privat->filename_label),
            0,
            0.5
            );

        gtk_table_attach(
            GTK_TABLE(privat->table),                    /* table */
            privat->filename_label,                      /* child */
            1,                                           /* left_attach */
            2,                                           /* right_attach */
            5,                                           /* top_attach */
            6,                                           /* bottom_attach */
            GTK_FILL,                                    /* xoptions */
            GTK_FILL,                                    /* yoptions */
            0,                                           /* xpadding */
            0                                            /* ypadding */
            );

        gtk_widget_set_visible(GTK_WIDGET(privat->filename_label), TRUE);

        gtk_table_attach(
            GTK_TABLE(privat->table),                    /* table */
            privat->filename_entry = gtk_entry_new(),    /* child */
            2,                                           /* left_attach */
            3,                                           /* right_attach */
            5,                                           /* top_attach */
            6,                                           /* bottom_attach */
            GTK_FILL,                                    /* xoptions */
            GTK_FILL,                                    /* yoptions */
            0,                                           /* xpadding */
            0                                            /* ypadding */
            );

        gtk_widget_set_visible(GTK_WIDGET(privat->filename_entry), TRUE);

        g_signal_connect(
            privat->filename_entry,
            "notify::text",
            G_CALLBACK(gpartsui_connect_controller_filename_text_notify_cb),
            instance
            );

        privat->filename_button = gtk_button_new_with_label("...");

        gtk_table_attach(
            GTK_TABLE(privat->table),                    /* table */
            privat->filename_button,                     /* child */
            3,                                           /* left_attach */
            4,                                           /* right_attach */
            5,                                           /* top_attach */
            6,                                           /* bottom_attach */
            GTK_FILL,                                    /* xoptions */
            GTK_FILL,                                    /* yoptions */
            0,                                           /* xpadding */
            0                                            /* ypadding */
            );

        gtk_widget_set_visible(GTK_WIDGET(privat->filename_button), TRUE);

        g_signal_connect(
            privat->filename_button,
            "clicked",
            G_CALLBACK(gpartsui_connect_controller_filename_button_clicked_cb),
            instance
            );

    }
}

GPartsUIConnectController*
gpartsui_connect_controller_new(void)
{
    return GPARTSUI_CONNECT_CONTROLLER(g_object_new(GPARTSUI_TYPE_CONNECT_CONTROLLER, NULL));
}

static void
gpartsui_connect_controller_password_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        char *password = gpartsui_connect_model_get_password(privat->connect_model);

        g_signal_handlers_block_by_func(
            privat->password_entry,
            G_CALLBACK(gpartsui_connect_controller_password_text_notify_cb),
            controller
            );

        gtk_entry_set_text(
            GTK_ENTRY(privat->password_entry),
            password
            );

        g_signal_handlers_unblock_by_func(
            privat->password_entry,
            G_CALLBACK(gpartsui_connect_controller_password_text_notify_cb),
            controller
            );

        g_free(password);
    }
}

static void
gpartsui_connect_controller_password_sensitive_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        gtk_widget_set_sensitive(
            privat->password_entry,
            gpartsui_connect_model_get_password_sensitive(privat->connect_model)
            );
    }
}

static void
gpartsui_connect_controller_password_text_notify_cb(GtkEntry *entry, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        g_signal_handlers_block_by_func(
            privat->connect_model,
            G_CALLBACK(gpartsui_connect_controller_password_notify_cb),
            controller
            );

        gpartsui_connect_model_set_password(
            privat->connect_model,
            gtk_entry_get_text(GTK_ENTRY(privat->password_entry))
            );

        g_signal_handlers_unblock_by_func(
            privat->connect_model,
            G_CALLBACK(gpartsui_connect_controller_password_notify_cb),
            controller
            );
    }
}

static void
gpartsui_connect_controller_password_valid_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
    }
}

void
gpartsui_connect_controller_set_connect_action(GPartsUIConnectController *controller, GtkAction *action)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        if (privat->connect_action != NULL)
        {
            g_signal_handlers_disconnect_by_func(
                privat->connect_action,
                G_CALLBACK(gpartsui_connect_controller_connect_action_cb),
                controller
                );

            g_object_unref(privat->connect_action);
        }

        privat->connect_action = action;

        if (privat->connect_action != NULL)
        {
            g_object_unref(privat->connect_action);

            g_signal_connect(
                privat->connect_action,
                "activate",
                G_CALLBACK(gpartsui_connect_controller_connect_action_cb),
                controller
                );
        }

        g_object_notify(G_OBJECT(controller), "connect-action");
    }
}

void
gpartsui_connect_controller_set_connect_model(GPartsUIConnectController *controller, GPartsUIConnectModel *model)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        if (privat->connect_model != NULL)
        {
            g_signal_handlers_disconnect_by_func(
                privat->connect_model,
                G_CALLBACK(gpartsui_connect_controller_connect_sensitive_notify_cb),
                controller
                );

            g_signal_handlers_disconnect_by_func(
                privat->connect_model,
                G_CALLBACK(gpartsui_connect_controller_database_name_notify_cb),
                controller
                );

            g_signal_handlers_disconnect_by_func(
                privat->connect_model,
                G_CALLBACK(gpartsui_connect_controller_database_name_sensitive_notify_cb),
                controller
                );

            g_signal_handlers_disconnect_by_func(
                privat->connect_model,
                G_CALLBACK(gpartsui_connect_controller_database_name_valid_notify_cb),
                controller
                );

            g_signal_handlers_disconnect_by_func(
                privat->connect_model,
                G_CALLBACK(gpartsui_connect_controller_database_types_notify_cb),
                controller
                );

            g_signal_handlers_disconnect_by_func(
                privat->connect_model,
                G_CALLBACK(gpartsui_connect_controller_filename_notify_cb),
                controller
                );

            g_signal_handlers_disconnect_by_func(
                privat->connect_model,
                G_CALLBACK(gpartsui_connect_controller_filename_sensitive_notify_cb),
                controller
                );

            g_signal_handlers_disconnect_by_func(
                privat->connect_model,
                G_CALLBACK(gpartsui_connect_controller_filename_valid_notify_cb),
                controller
                );

            g_signal_handlers_disconnect_by_func(
                privat->connect_model,
                G_CALLBACK(gpartsui_connect_controller_hostname_notify_cb),
                controller
                );

            g_signal_handlers_disconnect_by_func(
                privat->connect_model,
                G_CALLBACK(gpartsui_connect_controller_hostname_sensitive_notify_cb),
                controller
                );

            g_signal_handlers_disconnect_by_func(
                privat->connect_model,
                G_CALLBACK(gpartsui_connect_controller_hostname_valid_notify_cb),
                controller
                );

            g_signal_handlers_disconnect_by_func(
                privat->connect_model,
                G_CALLBACK(gpartsui_connect_controller_password_notify_cb),
                controller
                );

            g_signal_handlers_disconnect_by_func(
                privat->connect_model,
                G_CALLBACK(gpartsui_connect_controller_password_sensitive_notify_cb),
                controller
                );

            g_signal_handlers_disconnect_by_func(
                privat->connect_model,
                G_CALLBACK(gpartsui_connect_controller_password_valid_notify_cb),
                controller
                );

            g_signal_handlers_disconnect_by_func(
                privat->connect_model,
                G_CALLBACK(gpartsui_connect_controller_username_notify_cb),
                controller
                );

            g_signal_handlers_disconnect_by_func(
                privat->connect_model,
                G_CALLBACK(gpartsui_connect_controller_username_sensitive_notify_cb),
                controller
                );

            g_signal_handlers_disconnect_by_func(
                privat->connect_model,
                G_CALLBACK(gpartsui_connect_controller_username_valid_notify_cb),
                controller
                );

            g_object_unref(privat->connect_model);
        }

        privat->connect_model = model;

        gpartsui_connect_controller_database_types_notify_cb(model, NULL, controller);

        if (privat->connect_model != NULL)
        {
            char *temp;

            g_object_ref(privat->connect_model);

            gtk_widget_set_sensitive(
                privat->connect_button,
                gpartsui_connect_model_get_connect_sensitive(privat->connect_model)
                );

            g_signal_connect(
                privat->connect_model,
                "notify::connect-sensitive",
                G_CALLBACK(gpartsui_connect_controller_connect_sensitive_notify_cb),
                controller
                );

            g_signal_connect(
                privat->connect_model,
                "notify::database_types",
                G_CALLBACK(gpartsui_connect_controller_database_types_notify_cb),
                controller
                );

            temp = gpartsui_connect_model_get_database_name(privat->connect_model);

            gtk_entry_set_text(
                GTK_ENTRY(privat->database_entry),
                temp
                );

            g_free(temp);

            g_signal_connect(
                privat->connect_model,
                "notify::database-name",
                G_CALLBACK(gpartsui_connect_controller_database_name_notify_cb),
                controller
                );

            gtk_widget_set_sensitive(
                privat->database_entry,
                gpartsui_connect_model_get_database_name_sensitive(privat->connect_model)
                );

            g_signal_connect(
                privat->connect_model,
                "notify::database-name-sensitive",
                G_CALLBACK(gpartsui_connect_controller_database_name_sensitive_notify_cb),
                controller
                );

            g_signal_connect(
                privat->connect_model,
                "notify::database-name-valid",
                G_CALLBACK(gpartsui_connect_controller_database_name_valid_notify_cb),
                controller
                );

            temp = gpartsui_connect_model_get_filename(privat->connect_model);

            gtk_entry_set_text(
                GTK_ENTRY(privat->filename_entry),
                temp
                );

            g_free(temp);

            g_signal_connect(
                privat->connect_model,
                "notify::filename",
                G_CALLBACK(gpartsui_connect_controller_filename_notify_cb),
                controller
                );

            gtk_widget_set_sensitive(
                privat->filename_button,
                gpartsui_connect_model_get_filename_sensitive(privat->connect_model)
                );

            gtk_widget_set_sensitive(
                privat->filename_entry,
                gpartsui_connect_model_get_filename_sensitive(privat->connect_model)
                );

            g_signal_connect(
                privat->connect_model,
                "notify::filename-sensitive",
                G_CALLBACK(gpartsui_connect_controller_filename_sensitive_notify_cb),
                controller
                );

            g_signal_connect(
                privat->connect_model,
                "notify::filename-valid",
                G_CALLBACK(gpartsui_connect_controller_filename_valid_notify_cb),
                controller
                );

            temp = gpartsui_connect_model_get_hostname(privat->connect_model);

            gtk_entry_set_text(
                GTK_ENTRY(privat->hostname_entry),
                temp
                );

            g_free(temp);


            g_signal_connect(
                privat->connect_model,
                "notify::hostname",
                G_CALLBACK(gpartsui_connect_controller_hostname_notify_cb),
                controller
                );

            gtk_widget_set_sensitive(
                privat->hostname_entry,
                gpartsui_connect_model_get_hostname_sensitive(privat->connect_model)
                );

            g_signal_connect(
                privat->connect_model,
                "notify::hostname-sensitive",
                G_CALLBACK(gpartsui_connect_controller_hostname_sensitive_notify_cb),
                controller
                );

            g_signal_connect(
                privat->connect_model,
                "notify::hostname-valid",
                G_CALLBACK(gpartsui_connect_controller_hostname_valid_notify_cb),
                controller
                );

            temp = gpartsui_connect_model_get_password(privat->connect_model);

            gtk_entry_set_text(
                GTK_ENTRY(privat->password_entry),
                temp
                );

            g_free(temp);


            g_signal_connect(
                privat->connect_model,
                "notify::password",
                G_CALLBACK(gpartsui_connect_controller_password_notify_cb),
                controller
                );

            gtk_widget_set_sensitive(
                privat->password_entry,
                gpartsui_connect_model_get_password_sensitive(privat->connect_model)
                );

            g_signal_connect(
                privat->connect_model,
                "notify::password-sensitive",
                G_CALLBACK(gpartsui_connect_controller_password_sensitive_notify_cb),
                controller
                );

            g_signal_connect(
                privat->connect_model,
                "notify::password-valid",
                G_CALLBACK(gpartsui_connect_controller_password_valid_notify_cb),
                controller
                );

            temp = gpartsui_connect_model_get_username(privat->connect_model);

            gtk_entry_set_text(
                GTK_ENTRY(privat->username_entry),
                temp
                );

            g_free(temp);

            g_signal_connect(
                privat->connect_model,
                "notify::username",
                G_CALLBACK(gpartsui_connect_controller_username_notify_cb),
                controller
                );

            gtk_widget_set_sensitive(
                privat->username_entry,
                gpartsui_connect_model_get_username_sensitive(privat->connect_model)
                );

            g_signal_connect(
                privat->connect_model,
                "notify::username-sensitive",
                G_CALLBACK(gpartsui_connect_controller_username_sensitive_notify_cb),
                controller
                );

            g_signal_connect(
                privat->connect_model,
                "notify::username-valid",
                G_CALLBACK(gpartsui_connect_controller_username_valid_notify_cb),
                controller
                );
        }

        g_object_notify(G_OBJECT(controller), "connect-model");
    }
}

static void
gpartsui_connect_controller_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GPartsUIConnectController *controller = GPARTSUI_CONNECT_CONTROLLER(object);

    switch (property_id)
    {
        case GPARTSUI_CONNECT_CONTROLLER_CONNECT_ACTION:
            gpartsui_connect_controller_set_connect_action(controller, GTK_ACTION(g_value_get_object(value)));
            break;

        case GPARTSUI_CONNECT_CONTROLLER_CONNECT_MODEL:
            gpartsui_connect_controller_set_connect_model(controller, GPARTSUI_CONNECT_MODEL(g_value_get_object(value)));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static void
gpartsui_connect_controller_username_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        char *username = gpartsui_connect_model_get_username(privat->connect_model);

        g_signal_handlers_block_by_func(
            privat->username_entry,
            G_CALLBACK(gpartsui_connect_controller_username_text_notify_cb),
            controller
            );

        gtk_entry_set_text(
            GTK_ENTRY(privat->username_entry),
            username
            );

        g_signal_handlers_unblock_by_func(
            privat->username_entry,
            G_CALLBACK(gpartsui_connect_controller_username_text_notify_cb),
            controller
            );

        g_free(username);
    }
}

static void
gpartsui_connect_controller_username_sensitive_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        gtk_widget_set_sensitive(
            privat->username_entry,
            gpartsui_connect_model_get_username_sensitive(privat->connect_model)
            );
    }
}

static void
gpartsui_connect_controller_username_text_notify_cb(GtkEntry *entry, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        g_signal_handlers_block_by_func(
            privat->connect_model,
            G_CALLBACK(gpartsui_connect_controller_username_notify_cb),
            controller
            );

        gpartsui_connect_model_set_username(
            privat->connect_model,
            gtk_entry_get_text(GTK_ENTRY(privat->username_entry))
            );

        g_signal_handlers_unblock_by_func(
            privat->connect_model,
            G_CALLBACK(gpartsui_connect_controller_username_notify_cb),
            controller
            );
    }
}

static void
gpartsui_connect_controller_username_valid_notify_cb(GPartsUIConnectModel *model, GParamSpec *param, GPartsUIConnectController *controller)
{
    GPartsUIConnectControllerPrivate *privat = GPARTSUI_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
    }
}

