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

/*! \file gpform-ui-label.c
 */

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "misc-object.h"

#include "gpform.h"

#define GPFORM_UI_LABEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),GPFORM_TYPE_UI_LABEL,GPFormUILabelPrivate))

enum
{
    GPFORM_UI_LABEL_DEFAULT_LABEL = 1,
    GPFORM_UI_LABEL_MODEL_NAME
};

typedef struct _GPFormUILabelPrivate GPFormUILabelPrivate;

struct _GPFormUILabelPrivate
{
    gchar *default_label;
    gchar *model_name;
};



static void
gpform_ui_label_class_init(gpointer g_class, gpointer g_class_data);

static void
gpform_ui_label_dispose(GObject *object);

static void
gpform_ui_label_finalize(GObject *object);

static void
gpform_ui_label_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
gpform_ui_label_init(GTypeInstance *instance, gpointer g_class);

static void
gpform_ui_label_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);



static void
gpform_ui_label_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *klasse = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(klasse, sizeof(GPFormUILabelPrivate));

    klasse->dispose  = gpform_ui_label_dispose;
    klasse->finalize = gpform_ui_label_finalize;

    klasse->get_property = gpform_ui_label_get_property;
    klasse->set_property = gpform_ui_label_set_property;

    g_object_class_install_property(
        klasse,
        GPFORM_UI_LABEL_DEFAULT_LABEL,
        g_param_spec_string(
            "default-label",
            "Default Label",
            "Default Label",
            NULL,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPFORM_UI_LABEL_MODEL_NAME,
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
gpform_ui_label_dispose(GObject *object)
{
    misc_object_chain_dispose(object);
}

static void
gpform_ui_label_finalize(GObject *object)
{
    GPFormUILabelPrivate *privat = GPFORM_UI_LABEL_GET_PRIVATE(object);

    if (privat != NULL)
    {
        g_free(privat->default_label);
        privat->default_label = NULL;

        g_free(privat->model_name);
        privat->model_name = NULL;
    }

    misc_object_chain_finalize(object);
}

gchar*
gpform_ui_label_get_default_label(const GPFormUILabel *label)
{
    char *default_label = NULL;
    GPFormUILabelPrivate *privat = GPFORM_UI_LABEL_GET_PRIVATE(label);

    if (privat != NULL)
    {
        default_label = g_strdup(privat->default_label);
    }

    return default_label;
}

gchar*
gpform_ui_label_get_model_name(const GPFormUILabel *label)
{
    char *model_name = NULL;
    GPFormUILabelPrivate *privat = GPFORM_UI_LABEL_GET_PRIVATE(label);

    if (privat != NULL)
    {
        model_name = g_strdup(privat->model_name);
    }

    return model_name;
}

static void
gpform_ui_label_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    GPFormUILabel *label = GPFORM_UI_LABEL(object);

    if (label != NULL)
    {
        switch (property_id)
        {
            case GPFORM_UI_LABEL_DEFAULT_LABEL:
                g_value_take_string(value, gpform_ui_label_get_default_label(label));
                break;

            case GPFORM_UI_LABEL_MODEL_NAME:
                g_value_take_string(value, gpform_ui_label_get_model_name(label));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
gpform_ui_label_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(GPFormUILabelClass),    /* class_size */
            NULL,                          /* base_init */
            NULL,                          /* base_finalize */
            gpform_ui_label_class_init,    /* class_init */
            NULL,                          /* class_finalize */
            NULL,                          /* class_data */
            sizeof(GPFormUILabel),         /* instance_size */
            0,                             /* n_preallocs */
            gpform_ui_label_init,          /* instance_init */
            NULL                           /* value_table */
            };

        type = g_type_register_static(
            GTK_TYPE_LABEL,
            "GPFormUILabel",
            &tinfo,
            0
            );
    }

    return type;
}

static void
gpform_ui_label_init(GTypeInstance *instance, gpointer g_class)
{
    GPFormUILabelPrivate *privat = GPFORM_UI_LABEL_GET_PRIVATE(instance);

    if (privat != NULL)
    {
        privat->default_label = g_strdup("");
    }
}

GPFormUILabel*
gpform_ui_label_new(void)
{
    return GPFORM_UI_LABEL(g_object_new(GPFORM_TYPE_UI_LABEL, NULL));
}

void
gpform_ui_label_set_default_label(GPFormUILabel *label, const gchar *default_label)
{
    GPFormUILabelPrivate *privat = GPFORM_UI_LABEL_GET_PRIVATE(label);

    if (privat != NULL)
    {
        g_free(privat->default_label);

        privat->default_label = g_strdup(default_label);

        g_object_notify(G_OBJECT(label), "default-label");
    }
}

void
gpform_ui_label_set_model_name(GPFormUILabel *label, const gchar *model_name)
{
    GPFormUILabelPrivate *privat = GPFORM_UI_LABEL_GET_PRIVATE(label);

    if (privat != NULL)
    {
        g_free(privat->model_name);

        privat->model_name = g_strdup(model_name);

        g_object_notify(G_OBJECT(label), "model-name");
    }
}


static void
gpform_ui_label_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GPFormUILabel *label = GPFORM_UI_LABEL(object);

    if (label != NULL)
    {
        switch (property_id)
        {
            case GPFORM_UI_LABEL_DEFAULT_LABEL:
                gpform_ui_label_set_default_label(label, g_value_get_string(value));
                break;

            case GPFORM_UI_LABEL_MODEL_NAME:
                gpform_ui_label_set_model_name(label, g_value_get_string(value));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

