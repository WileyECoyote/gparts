/* gEDA - GPL Electronic Design Automation
 * gparts - gEDA Parts Manager
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

/*! \file gpform-ui-dialog.c
 */

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "misc-object.h"

#include "gpform.h"

#define GPFORM_UI_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),GPFORM_TYPE_UI_DIALOG,GPFormUIDialogPrivate))

enum
{
    GPFORM_UI_DIALOG_HELP_SPEC = 1,
    GPFORM_UI_DIALOG_MODEL,
    GPFORM_UI_DIALOG_MODEL_NAME
};

typedef struct _GPFormUIDialogPrivate GPFormUIDialogPrivate;

struct _GPFormUIDialogPrivate
{
    gchar          *help_spec;
    GPFormPMDialog *model;
    gchar          *model_name;
};



static void
gpform_ui_dialog_class_init(gpointer g_class, gpointer g_class_data);

static void
gpform_ui_dialog_dispose(GObject *object);

static void
gpform_ui_dialog_finalize(GObject *object);

static void
gpform_ui_dialog_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
gpform_ui_dialog_init(GTypeInstance *instance, gpointer g_class);

static void
gpform_ui_dialog_response_cb(GPFormUIDialog *dialog, gint response_id, gpointer user_data);

static void
gpform_ui_dialog_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void
gpform_ui_dialog_update_model_cb(GtkWidget *widget, GPFormPMDialog *model);



static void
gpform_ui_dialog_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *klasse = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(klasse, sizeof(GPFormUIDialogPrivate));

    klasse->dispose  = gpform_ui_dialog_dispose;
    klasse->finalize = gpform_ui_dialog_finalize;

    klasse->get_property = gpform_ui_dialog_get_property;
    klasse->set_property = gpform_ui_dialog_set_property;

    g_object_class_install_property(
        klasse,
        GPFORM_UI_DIALOG_HELP_SPEC,
        g_param_spec_string(
            "help-spec",
            "Help Spec",
            "Help Spec",
            NULL,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPFORM_UI_DIALOG_MODEL,
        g_param_spec_object(
            "model",
            "Model",
            "Model",
            GPFORM_TYPE_PM_DIALOG,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPFORM_UI_DIALOG_MODEL_NAME,
        g_param_spec_string(
            "model-name",
            "Model Name",
            "Model Name",
            NULL,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );
}

static void
gpform_ui_dialog_dispose(GObject *object)
{
    GPFormUIDialogPrivate *privat = GPFORM_UI_DIALOG_GET_PRIVATE(object);

    if (privat != NULL)
    {
        if (privat->model != NULL)
        {
            g_object_unref(privat->model);
            privat->model = NULL;
        }
    }

    misc_object_chain_dispose(object);
}

static void
gpform_ui_dialog_finalize(GObject *object)
{
    GPFormUIDialogPrivate *privat = GPFORM_UI_DIALOG_GET_PRIVATE(object);

    if (privat != NULL)
    {
        g_free(privat->help_spec);
        privat->help_spec = NULL;

        g_free(privat->model_name);
        privat->model_name = NULL;
    }

    misc_object_chain_finalize(object);
}

gchar*
gpform_ui_dialog_get_help_spec(const GPFormUIDialog *dialog)
{
    gchar *help_spec = NULL;
    GPFormUIDialogPrivate *privat = GPFORM_UI_DIALOG_GET_PRIVATE(dialog);

    if (privat != NULL)
    {
        help_spec = g_strdup(privat->help_spec);
    }

    return help_spec;
}

GPFormPMDialog*
gpform_ui_dialog_get_model(const GPFormUIDialog *dialog)
{
    GPFormPMDialog *model = NULL;
    GPFormUIDialogPrivate *privat = GPFORM_UI_DIALOG_GET_PRIVATE(dialog);

    if (privat != NULL)
    {
        model = privat->model;

        if (model != NULL)
        {
            g_object_ref(model);
        }
    }

    return model;
}

gchar*
gpform_ui_dialog_get_model_name(const GPFormUIDialog *dialog)
{
    gchar *model_name = NULL;
    GPFormUIDialogPrivate *privat = GPFORM_UI_DIALOG_GET_PRIVATE(dialog);

    if (privat != NULL)
    {
        model_name = g_strdup(privat->model_name);
    }

    return model_name;
}

static void
gpform_ui_dialog_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    GPFormUIDialog *dialog = GPFORM_UI_DIALOG(object);

    if (dialog != NULL)
    {
        switch (property_id)
        {
            case GPFORM_UI_DIALOG_HELP_SPEC:
                g_value_take_string(value, gpform_ui_dialog_get_help_spec(dialog));
                break;

            case GPFORM_UI_DIALOG_MODEL:
                g_value_take_object(value, gpform_ui_dialog_get_model(dialog));
                break;

            case GPFORM_UI_DIALOG_MODEL_NAME:
                g_value_take_string(value, gpform_ui_dialog_get_model_name(dialog));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GdkPixbuf*
gpform_ui_dialog_get_snapshot(const GPFormUIDialog* dialog)
{
    GdkPixbuf *pixbuf = NULL;

    if (dialog != NULL)
    {
        pixbuf = gtk_widget_get_snapshot(
            GTK_WIDGET(dialog),
            NULL
            );
    }

    return pixbuf;
}

GType
gpform_ui_dialog_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(GPFormUIDialogClass),    /* class_size */
            NULL,                           /* base_init */
            NULL,                           /* base_finalize */
            gpform_ui_dialog_class_init,    /* class_init */
            NULL,                           /* class_finalize */
            NULL,                           /* class_data */
            sizeof(GPFormUIDialog),         /* instance_size */
            0,                              /* n_preallocs */
            gpform_ui_dialog_init,          /* instance_init */
            NULL                            /* value_table */
            };

        type = g_type_register_static(
            GTK_TYPE_DIALOG,
            "GPFormUIDialog",
            &tinfo,
            0
            );
    }

    return type;
}

static void
gpform_ui_dialog_init(GTypeInstance *instance, gpointer g_class)
{
    if (instance != NULL)
    {
        GPFormUIDialogPrivate *privat = GPFORM_UI_DIALOG_GET_PRIVATE(instance);

        if (privat != NULL)
        {
            /* Initialize private members */
        }

        g_signal_connect(
            G_OBJECT(instance),
            "response",
            G_CALLBACK(gpform_ui_dialog_response_cb),
            NULL
            );
    }
}

GPFormUIDialog*
gpform_ui_dialog_new(void)
{
    return GPFORM_UI_DIALOG(g_object_new(GPFORM_TYPE_UI_DIALOG, NULL));
}

static void
gpform_ui_dialog_response_cb(GPFormUIDialog *dialog, gint response_id, gpointer user_data)
{
    switch (response_id)
    {
        case GTK_RESPONSE_CANCEL:
            g_debug("GTK_RESPONSE_CANCEL");
            break;

        case GTK_RESPONSE_HELP:
            g_debug("GTK_RESPONSE_HELP");
            break;

        case GTK_RESPONSE_OK:
            g_debug("GTK_RESPONSE_OK");
            break;

        default:
            g_debug("unhandled response");
    }
}

static void
gpform_ui_dialog_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GPFormUIDialog *dialog = GPFORM_UI_DIALOG(object);

    if (dialog != NULL)
    {
        switch (property_id)
        {
            case GPFORM_UI_DIALOG_HELP_SPEC:
                gpform_ui_dialog_set_help_spec(dialog, g_value_get_string(value));
                break;

            case GPFORM_UI_DIALOG_MODEL:
                gpform_ui_dialog_set_model(dialog, g_value_get_object(value));
                break;

            case GPFORM_UI_DIALOG_MODEL_NAME:
                gpform_ui_dialog_set_model_name(dialog, g_value_get_string(value));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

void
gpform_ui_dialog_set_help_spec(GPFormUIDialog *dialog, const gchar *help_spec)
{
    GPFormUIDialogPrivate *privat = GPFORM_UI_DIALOG_GET_PRIVATE(dialog);

    if (privat != NULL)
    {
        g_free(privat->help_spec);
        privat->help_spec = g_strdup(help_spec);

        g_object_notify(G_OBJECT(dialog), "help-spec");
    }
}

void
gpform_ui_dialog_set_model(GPFormUIDialog *dialog, GPFormPMDialog *model)
{
    GPFormUIDialogPrivate *privat = GPFORM_UI_DIALOG_GET_PRIVATE(dialog);

    if (privat != NULL)
    {
        if (privat->model != NULL)
        {
            g_object_unref(privat->model);
        }

        privat->model = model;

        if (privat->model != NULL)
        {
            g_object_ref(privat->model);
        }

        gtk_container_forall(
            GTK_CONTAINER(dialog),
            (GtkCallback) gpform_ui_dialog_update_model_cb,
            privat->model
            );

        g_object_notify(G_OBJECT(dialog), "model");
    }
}

void
gpform_ui_dialog_set_model_name(GPFormUIDialog *dialog, const gchar *model_name)
{
    GPFormUIDialogPrivate *privat = GPFORM_UI_DIALOG_GET_PRIVATE(dialog);

    if (privat != NULL)
    {
        g_free(privat->model_name);
        privat->model_name = g_strdup(model_name);

        g_object_notify(G_OBJECT(dialog), "model-name");
    }
}

static void
gpform_ui_dialog_update_model_cb(GtkWidget *widget, GPFormPMDialog *model)
{
    g_debug("Dialog child type: %s", G_OBJECT_TYPE_NAME(widget));

    if (GPFORM_IS_UI_WIDGET(widget))
    {
        gpform_ui_widget_set_model(
            GPFORM_UI_WIDGET(widget),
            model
            );
    }

    if (GTK_IS_CONTAINER(widget))
    {
        gtk_container_forall(
            GTK_CONTAINER(widget),
            (GtkCallback) gpform_ui_dialog_update_model_cb,
            model
            );
    }
}


