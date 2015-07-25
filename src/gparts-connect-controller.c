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

/*! \file gparts-connect-controller.c
 */

#include "gpartsui.h"


#define GPARTS_CONNECT_CONTROLLER_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE(object,GPARTS_TYPE_CONNECT_CONTROLLER,GPartsConnectControllerPrivate)

enum
{
    GPARTS_CONNECT_CONTROLLER_PROPID_CONNECT_DATA = 1,
    GPARTS_CONNECT_CONTROLLER_PROPID_DATABASE_TYPE,
    GPARTS_CONNECT_CONTROLLER_PROPID_DATABASE_TYPES,
    GPARTS_CONNECT_CONTROLLER_PROPID_FLAGS,
    GPARTS_CONNECT_CONTROLLER_PROPID_VALID
};

typedef struct _GPartsConnectControllerPrivate GPartsConnectControllerPrivate;

struct _GPartsConnectControllerPrivate
{
    GtkFileChooser  *file_dialog;
    GtkDialog       *login_dialog;

    GtkComboBoxText *engine_combo;

    GtkEntry        *user_name_entry;
    GtkEntry        *password_entry;
    GtkEntry        *server_entry;
    GtkEntry        *database_entry;
    GtkEntry        *filename_entry;
    GtkButton       *button;
};



static void
gparts_connect_controller_class_init(gpointer g_class, gpointer g_class_data);

static void
gparts_connect_controller_dispose(GObject *object);

static void
gparts_connect_controller_finalize(GObject *object);

static void
gparts_connect_controller_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
gparts_connect_controller_instance_init(GTypeInstance *instance, gpointer g_class);

static void
gparts_connect_controller_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

/**** Signal handlers ****/

static void
gparts_connect_controller_changed_cb(GtkTreeSelection *selection, GPartsConnectController *connect_controller);

static void
gparts_connect_controller_connect_data_changed_cb(GObject *widget, GParamSpec *pspec, GPartsConnectController *controller);

static void
gparts_connect_controller_clicked_cb(GtkTreeSelection *selection, GPartsConnectController *connect_controller);

static void
gparts_connect_controller_show_cb(GtkWidget *widget, GPartsConnectController *connect_controller);

static void
gparts_connect_controller_select_file_cb(GtkTreeSelection *selection, GPartsConnectController *controller);



static void
gparts_connect_controller_changed_cb(GtkTreeSelection *selection, GPartsConnectController *controller)
{
    g_object_notify(G_OBJECT(controller), "database-type");
}

/*! \brief Initialize class data.
 *
 *  \param [in] g_class
 *  \param [in] g_class_data
 */
static void
gparts_connect_controller_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass* object_class = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(g_class, sizeof(GPartsConnectControllerPrivate));

    object_class->dispose  = gparts_connect_controller_dispose;
    object_class->finalize = gparts_connect_controller_finalize;

    object_class->get_property = gparts_connect_controller_get_property;
    object_class->set_property = gparts_connect_controller_set_property;

    g_object_class_install_property(
        object_class,
        GPARTS_CONNECT_CONTROLLER_PROPID_CONNECT_DATA,
        g_param_spec_boxed(
            "connect-data",
            "Connect Data",
            "Connect Data",
            GPARTS_TYPE_CONNECT_DATA,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        GPARTS_CONNECT_CONTROLLER_PROPID_DATABASE_TYPE,
        g_param_spec_string(
            "database-type",
            "Database Type",
            "Database Type",
            NULL,
            G_PARAM_LAX_VALIDATION | G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        GPARTS_CONNECT_CONTROLLER_PROPID_DATABASE_TYPES,
        g_param_spec_boxed(
            "database-types",
            "Database Types",
            "Database Types",
            G_TYPE_STRV,
            G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        GPARTS_CONNECT_CONTROLLER_PROPID_FLAGS,
        g_param_spec_int(
            "flags",
            "Flags",
            "Flags",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        GPARTS_CONNECT_CONTROLLER_PROPID_VALID,
        g_param_spec_boolean(
            "valid",
            "Valid",
            "Valid",
            FALSE,
            G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS
            )
        );
}

void
gparts_connect_controller_clear_password(GPartsConnectController *controller)
{
    GPartsConnectControllerPrivate *privat = GPARTS_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        gtk_entry_set_text(privat->password_entry, "");
    }
}

static void
gparts_connect_controller_connect_data_changed_cb(GObject *widget, GParamSpec *pspec, GPartsConnectController *controller)
{
    g_object_notify(G_OBJECT(controller), "connect-data");
}

static void
gparts_connect_controller_dispose(GObject *object)
{
    GPartsConnectControllerPrivate *privat = GPARTS_CONNECT_CONTROLLER_GET_PRIVATE(object);

    if (privat != NULL)
    {
        if (privat->file_dialog != NULL)
        {
            gtk_widget_destroy(GTK_WIDGET(privat->file_dialog));
            privat->file_dialog = NULL;
        }

        if (privat->login_dialog != NULL)
        {
            gtk_widget_destroy(GTK_WIDGET(privat->login_dialog));
            privat->login_dialog = NULL;
        }
    }

    misc_object_chain_dispose(object);
}

/*! \brief Deallocate all resources.
 *
 *  This function performs the second and last stage of object destruction.
 *
 *  \param [in] object The object to finalize.
 */
static void
gparts_connect_controller_finalize(GObject *object)
{
    GPartsConnectControllerPrivate *privat = GPARTS_CONNECT_CONTROLLER_GET_PRIVATE(object);

    misc_object_chain_finalize(object);
}

/*! \brief Gets the GType of the GPartsConnectController class.
 *
 *  \return The GType of the GPartsConnectController class.
 */
GType
gparts_connect_controller_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(GPartsConnectControllerClass),
            NULL,
            NULL,
            gparts_connect_controller_class_init,
            NULL,
            NULL,
            sizeof(GPartsConnectController),
            0,
            gparts_connect_controller_instance_init,
            NULL
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "GPartsConnectController",
            &tinfo,
            0
            );
    }
    return type;
}

GPartsConnectData*
gparts_connect_controller_get_connect_data(GPartsConnectController *controller)
{
    GPartsConnectData *data = NULL;
    GPartsConnectControllerPrivate *privat = GPARTS_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        data = gparts_connect_data_new();

        data->username = g_strdup(gtk_entry_get_text(privat->user_name_entry));
        data->password = g_strdup(gtk_entry_get_text(privat->password_entry));
        data->database = g_strdup(gtk_entry_get_text(privat->database_entry));
        data->hostname = g_strdup(gtk_entry_get_text(privat->server_entry));
        data->filename = g_strdup(gtk_entry_get_text(privat->filename_entry));
    }

    return data;
}

char*
gparts_connect_controller_get_database_type(GPartsConnectController *controller)
{
  char *type = NULL;
  GPartsConnectControllerPrivate *privat = GPARTS_CONNECT_CONTROLLER_GET_PRIVATE(controller);

  if (privat != NULL) {

    type = g_strdup(gtk_combo_box_text_get_active_text(privat->engine_combo));

  }

  return type;

}

static void
gparts_connect_controller_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    switch (property_id) {

        case GPARTS_CONNECT_CONTROLLER_PROPID_CONNECT_DATA:
            g_value_take_boxed(value, gparts_connect_controller_get_connect_data(GPARTS_CONNECT_CONTROLLER(object)));
            break;

        case GPARTS_CONNECT_CONTROLLER_PROPID_DATABASE_TYPE:
            g_value_take_string(value, gparts_connect_controller_get_database_type(GPARTS_CONNECT_CONTROLLER(object)));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static void
gparts_connect_controller_instance_init(GTypeInstance *instance, gpointer g_class)
{
    GPartsConnectControllerPrivate *privat = GPARTS_CONNECT_CONTROLLER_GET_PRIVATE(instance);

    if (privat != NULL) {

        GtkWidget *button;
        GtkWidget *content;
        GtkWidget *entry;
        GtkWidget *image;
        GtkWidget *label;
        GtkTable  *table;

        privat->file_dialog = GTK_FILE_CHOOSER(gtk_file_chooser_dialog_new(
            "Select a Database File",
            NULL,
            GTK_FILE_CHOOSER_ACTION_OPEN,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_OK,     GTK_RESPONSE_OK,
            NULL
            ));

        privat->login_dialog = GTK_DIALOG(gtk_dialog_new_with_buttons(
            "Connect to a Database",
            NULL,
            GTK_DIALOG_MODAL,
            GTK_STOCK_HELP,    GTK_RESPONSE_HELP,
            GTK_STOCK_CANCEL,  GTK_RESPONSE_CANCEL,
            GTK_STOCK_CONNECT, GTK_RESPONSE_OK,
            NULL
            ));

        gtk_dialog_set_alternative_button_order(
            privat->login_dialog,
            GTK_RESPONSE_HELP,
            GTK_RESPONSE_OK,
            GTK_RESPONSE_CANCEL,
            -1
            );

        gtk_dialog_set_default_response(privat->login_dialog, GTK_RESPONSE_OK);

        gtk_container_set_border_width(GTK_CONTAINER(privat->login_dialog), 5);

        content = gtk_dialog_get_content_area(privat->login_dialog);

        gtk_box_set_spacing(GTK_BOX(content), 8);

        table = GTK_TABLE(gtk_table_new(6, 4, FALSE));
        gtk_table_set_row_spacings(table, 8);
        gtk_table_set_col_spacings(table, 10);
        gtk_widget_set_visible(GTK_WIDGET(table), TRUE);
        gtk_container_add(GTK_CONTAINER(content), GTK_WIDGET(table));

        image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_AUTHENTICATION, GTK_ICON_SIZE_DIALOG);
        gtk_widget_set_visible(GTK_WIDGET(image), TRUE);
        gtk_misc_set_alignment(GTK_MISC(image), 0, 0);
        gtk_table_attach(table, GTK_WIDGET(image), 0, 1, 0, 6, GTK_FILL, GTK_FILL, 0, 0);

        label = gtk_label_new("Type");
        gtk_widget_set_visible(GTK_WIDGET(label), TRUE);
        gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
        gtk_table_attach(table, GTK_WIDGET(label), 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

        privat->engine_combo = GTK_COMBO_BOX_TEXT(gtk_combo_box_text_new());
        gtk_widget_set_visible(GTK_WIDGET(privat->engine_combo), TRUE);
        gtk_table_attach(table, GTK_WIDGET(privat->engine_combo), 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

        label = gtk_label_new("Username");
        gtk_widget_set_visible(GTK_WIDGET(label), TRUE);
        gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
        gtk_table_attach(table, GTK_WIDGET(label), 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);

        privat->user_name_entry = GTK_ENTRY(gtk_entry_new());
        gtk_widget_set_visible(GTK_WIDGET(privat->user_name_entry), TRUE);
        gtk_table_attach(table, GTK_WIDGET(privat->user_name_entry), 2, 3, 1, 2, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);

        label = gtk_label_new("Password");
        gtk_widget_set_visible(GTK_WIDGET(label), TRUE);
        gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
        gtk_table_attach(table, label, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);

        privat->password_entry = GTK_ENTRY(gtk_entry_new());
        g_object_set(privat->password_entry, "visibility", FALSE,NULL);
        gtk_widget_set_visible(GTK_WIDGET(privat->password_entry), TRUE);
        gtk_table_attach(table, GTK_WIDGET(privat->password_entry), 2, 3, 2, 3, GTK_FILL, GTK_FILL, 0, 0);

        label = gtk_label_new("Server");
        gtk_widget_set_visible(GTK_WIDGET(label), TRUE);
        gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
        gtk_table_attach(table, label, 1, 2, 3, 4, GTK_FILL, GTK_FILL, 0, 0);

        privat->server_entry = GTK_ENTRY(gtk_entry_new());
        gtk_widget_set_visible(GTK_WIDGET(privat->server_entry), TRUE);
        gtk_table_attach(table, GTK_WIDGET(privat->server_entry), 2, 3, 3, 4, GTK_FILL, GTK_FILL, 0, 0);

        label = gtk_label_new("Database Name");
        gtk_widget_set_visible(GTK_WIDGET(label), TRUE);
        gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
        gtk_table_attach(table, label, 1, 2, 4, 5, GTK_FILL, GTK_FILL, 0, 0);

        privat->database_entry = GTK_ENTRY(gtk_entry_new());
        gtk_widget_set_visible(GTK_WIDGET(privat->database_entry), TRUE);
        gtk_table_attach(table, GTK_WIDGET(privat->database_entry), 2, 3, 4, 5, GTK_FILL, GTK_FILL, 0, 0);

        label = gtk_label_new("Filename");
        gtk_widget_set_visible(GTK_WIDGET(label), TRUE);
        gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
        gtk_table_attach(table, label, 1, 2, 5, 6, GTK_FILL, GTK_FILL, 0, 0);

        privat->filename_entry = GTK_ENTRY(gtk_entry_new());
        gtk_widget_set_visible(GTK_WIDGET(privat->filename_entry), TRUE);
        gtk_table_attach(table, GTK_WIDGET(privat->filename_entry), 2, 3, 5, 6, GTK_FILL, GTK_FILL, 0, 0);

        privat->button = GTK_BUTTON(gtk_button_new_with_label("..."));
        gtk_widget_set_visible(GTK_WIDGET(privat->button), TRUE);
        gtk_table_attach(table, GTK_WIDGET(privat->button), 3, 4, 5, 6, GTK_FILL, GTK_FILL, 0, 0);

        g_signal_connect(
            privat->engine_combo,
            "changed",
            G_CALLBACK(gparts_connect_controller_changed_cb),
            instance
            );

        g_signal_connect(
            privat->user_name_entry,
            "notify::text",
            G_CALLBACK(gparts_connect_controller_connect_data_changed_cb),
            instance
            );

        g_signal_connect(
            privat->password_entry,
            "notify::text",
            G_CALLBACK(gparts_connect_controller_connect_data_changed_cb),
            instance
            );

        g_signal_connect(
            privat->server_entry,
            "notify::text",
            G_CALLBACK(gparts_connect_controller_connect_data_changed_cb),
            instance
            );

        g_signal_connect(
            privat->database_entry,
            "notify::text",
            G_CALLBACK(gparts_connect_controller_connect_data_changed_cb),
            instance
            );

        g_signal_connect(
            privat->filename_entry,
            "notify::text",
            G_CALLBACK(gparts_connect_controller_connect_data_changed_cb),
            instance
            );

        g_signal_connect(
            privat->button,
            "clicked",
            G_CALLBACK(gparts_connect_controller_select_file_cb),
            instance
            );
    }
}

GPartsConnectController*
gparts_connect_controller_new(void)
{
    return GPARTS_CONNECT_CONTROLLER(g_object_new(GPARTS_TYPE_CONNECT_CONTROLLER, NULL));
}

gint
gparts_connect_controller_run(GPartsConnectController* controller)
{
    GPartsConnectControllerPrivate *privat = GPARTS_CONNECT_CONTROLLER_GET_PRIVATE(controller);
    gint result = GTK_RESPONSE_NONE;

    if (privat != NULL)
    {
        result = gtk_dialog_run(privat->login_dialog);

        gtk_widget_hide(GTK_WIDGET(privat->login_dialog));
    }

    return result;
}

static void
gparts_connect_controller_select_file_cb(GtkTreeSelection *selection, GPartsConnectController *controller)
{
    GPartsConnectControllerPrivate *privat = GPARTS_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if ((privat != NULL) && (privat->file_dialog != NULL))
    {
        gint result = gtk_dialog_run(GTK_DIALOG(privat->file_dialog));

        if (result == GTK_RESPONSE_OK)
        {
            char *filename = gtk_file_chooser_get_filename(privat->file_dialog);

            gtk_entry_set_text(privat->filename_entry, filename);

            g_free(filename);
        }

        gtk_widget_hide(GTK_WIDGET(privat->file_dialog));
    }
}

void
gparts_connect_controller_set_connect_data(GPartsConnectController *controller, const GPartsConnectData *data)
{
    if (data != NULL)
    {
        GPartsConnectControllerPrivate *privat = GPARTS_CONNECT_CONTROLLER_GET_PRIVATE(controller);

        if (privat != NULL)
        {
            gtk_entry_set_text(privat->user_name_entry, data->username);
            gtk_entry_set_text(privat->password_entry, data->password);
            gtk_entry_set_text(privat->server_entry, data->hostname);
            gtk_entry_set_text(privat->database_entry, data->database);
            gtk_entry_set_text(privat->filename_entry, data->filename);

            g_object_notify(G_OBJECT(controller), "connect-data");
        }
    }
}

void
gparts_connect_controller_set_database_types(GPartsConnectController *controller, const GStrv types)
{
    GPartsConnectControllerPrivate *privat = GPARTS_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        gtk_list_store_clear(
            GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(privat->engine_combo)))
            );

        /* gtk_combo_box_get_model() does not increment the reference count */

        if (types != NULL)
        {
            GStrv temp = types;

            while (*temp != NULL)
            {
                gtk_combo_box_text_append_text(privat->engine_combo, *temp);

                temp++;
            }

            if (gtk_combo_box_get_active(GTK_COMBO_BOX(privat->engine_combo)) < 0)
            {
                gtk_combo_box_set_active(GTK_COMBO_BOX(privat->engine_combo), 0);
            }
        }
    }
}

static void
gparts_connect_controller_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GPartsConnectController *controller = GPARTS_CONNECT_CONTROLLER(object);

    switch (property_id)
    {
        case GPARTS_CONNECT_CONTROLLER_PROPID_CONNECT_DATA:
            gparts_connect_controller_set_connect_data(controller, GPARTS_CONNECT_DATA(g_value_get_boxed(value)));
            break;

        case GPARTS_CONNECT_CONTROLLER_PROPID_DATABASE_TYPES:
            gparts_connect_controller_set_database_types(controller, g_value_get_boxed(value));
            break;

        case GPARTS_CONNECT_CONTROLLER_PROPID_FLAGS:
            gparts_connect_controller_set_flags(controller, g_value_get_int(value));
            break;

        case GPARTS_CONNECT_CONTROLLER_PROPID_VALID:
            gparts_connect_controller_set_valid(controller, g_value_get_boolean(value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

void
gparts_connect_controller_set_flags(GPartsConnectController *controller, gint flags)
{
    GPartsConnectControllerPrivate *privat = GPARTS_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        gtk_widget_set_sensitive(
            GTK_WIDGET(privat->user_name_entry),
            (flags & GPARTS_DATABASE_TYPE_FLAGS_USES_USERNAME)
            );

        gtk_widget_set_sensitive(
            GTK_WIDGET(privat->password_entry),
            (flags & GPARTS_DATABASE_TYPE_FLAGS_USES_PASSWORD)
            );

        gtk_widget_set_sensitive(
            GTK_WIDGET(privat->server_entry),
            (flags & GPARTS_DATABASE_TYPE_FLAGS_USES_HOSTNAME)
            );

        gtk_widget_set_sensitive(
            GTK_WIDGET(privat->database_entry),
            (flags & GPARTS_DATABASE_TYPE_FLAGS_USES_DATABASE)
            );

        gtk_widget_set_sensitive(
            GTK_WIDGET(privat->filename_entry),
            (flags & GPARTS_DATABASE_TYPE_FLAGS_USES_FILENAME)
            );

        gtk_widget_set_sensitive(
            GTK_WIDGET(privat->button),
            (flags & GPARTS_DATABASE_TYPE_FLAGS_USES_FILENAME)
            );

        g_object_notify(G_OBJECT(controller), "flags");
    }
}

void
gparts_connect_controller_set_valid(GPartsConnectController *controller, gboolean valid)
{
    GPartsConnectControllerPrivate *privat = GPARTS_CONNECT_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL)
    {
        gtk_widget_set_sensitive(
            gtk_dialog_get_widget_for_response(privat->login_dialog, GTK_RESPONSE_OK),
            valid
            );

        g_object_notify(G_OBJECT(controller), "valid");
    }
}

