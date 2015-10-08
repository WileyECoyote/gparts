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

/*! \file schgui-config.c
 */

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "misc-object.h"

#include "schgui.h"

#define SCHGUI_CONFIG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCHGUI_TYPE_CONFIG,SchGUIConfigPrivate))

enum
{
    SCHGUI_CONFIG_DRAWING_DISPLAY = 1,
    SCHGUI_CONFIG_DRAWING_PRINT
};

typedef struct _SchGUIConfigPrivate SchGUIConfigPrivate;

struct _SchGUIConfigPrivate
{
    SchGUIDrawingCfg *config_drawing_display;
    SchGUIDrawingCfg *config_drawing_print;
};



static void
schgui_config_class_init(gpointer g_class, gpointer g_class_data);

static void
schgui_config_dispose(GObject *object);

static void
schgui_config_finalize(GObject *object);

static void
schgui_config_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
schgui_config_init(GTypeInstance *instance, gpointer g_class);

static void
schgui_config_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);



static void
schgui_config_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *klasse = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(klasse, sizeof(SchGUIConfigPrivate));

    klasse->dispose  = schgui_config_dispose;
    klasse->finalize = schgui_config_finalize;

    klasse->get_property = schgui_config_get_property;
    klasse->set_property = schgui_config_set_property;

    g_object_class_install_property(
        klasse,
        SCHGUI_CONFIG_DRAWING_DISPLAY,
        g_param_spec_object(
            "config-drawing-display",
            "Configuration Drawing Display",
            "Configuration Drawing Display",
            SCHGUI_TYPE_DRAWING_CFG,
            G_PARAM_LAX_VALIDATION | G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCHGUI_CONFIG_DRAWING_PRINT,
        g_param_spec_object(
            "config-drawing-print",
            "Configuration Drawing Print",
            "Configuration Drawing Print",
            SCHGUI_TYPE_DRAWING_CFG,
            G_PARAM_LAX_VALIDATION | G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );
}

static void
schgui_config_dispose(GObject *object)
{
    SchGUIConfigPrivate *privat = SCHGUI_CONFIG_GET_PRIVATE(object);

    if (privat != NULL)
    {
        if (privat->config_drawing_display != NULL)
        {
            g_object_unref(privat->config_drawing_display);

            privat->config_drawing_display = NULL;
        }

        if (privat->config_drawing_print != NULL)
        {
            g_object_unref(privat->config_drawing_print);

            privat->config_drawing_print = NULL;
        }
    }
    misc_object_chain_dispose(object);
}

static void
schgui_config_finalize(GObject *object)
{
    misc_object_chain_finalize(object);
}

static void
schgui_config_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    SchGUIConfig *config = SCHGUI_CONFIG(object);

    if (config != NULL)
    {
        switch (property_id)
        {
            case SCHGUI_CONFIG_DRAWING_DISPLAY:
                g_value_set_object(value, schgui_config_get_config_drawing_display(config));
                break;

            case SCHGUI_CONFIG_DRAWING_PRINT:
                g_value_set_object(value, schgui_config_get_config_drawing_print(config));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
schgui_config_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(SchGUIConfigClass),    /* class_size */
            NULL,                         /* base_init */
            NULL,                         /* base_finalize */
            schgui_config_class_init,     /* class_init */
            NULL,                         /* class_finalize */
            NULL,                         /* class_data */
            sizeof(SchGUIConfig),         /* instance_size */
            0,                            /* n_preallocs */
            schgui_config_init,           /* instance_init */
            NULL                          /* value_table */
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "SchGUIConfig",
            &tinfo,
            0
            );
    }

    return type;
}

static void
schgui_config_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    SchGUIConfig *config = SCHGUI_CONFIG(object);

    if (config != NULL)
    {
        switch (property_id)
        {
            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

SchGUIDrawingCfg*
schgui_config_get_config_drawing_display(const SchGUIConfig *config)
{
  SchGUIDrawingCfg    *config_drawing = NULL;
  SchGUIConfigPrivate *privat         = SCHGUI_CONFIG_GET_PRIVATE(config);

  if (privat != NULL) {

    config_drawing = privat->config_drawing_display;

    if (config_drawing != NULL) {

      g_object_ref(config_drawing);
    }
  }

  return config_drawing;
}

SchGUIDrawingCfg*
schgui_config_get_config_drawing_print(const SchGUIConfig *config)
{
    SchGUIDrawingCfg *config_drawing = NULL;
    SchGUIConfigPrivate *privat = SCHGUI_CONFIG_GET_PRIVATE(config);

    if (privat != NULL) {

        config_drawing = privat->config_drawing_print;

        if (config_drawing != NULL) {
            g_object_ref(config_drawing);
        }
    }

    return config_drawing;
}


static void
schgui_config_init(GTypeInstance *instance, gpointer g_class)
{
    SchGUIConfigPrivate *privat = SCHGUI_CONFIG_GET_PRIVATE(instance);

    if (privat != NULL)
    {
        privat->config_drawing_display = schgui_drawing_cfg_get_default_display();
        privat->config_drawing_print   = schgui_drawing_cfg_get_default_print();
    }
}

SchGUIConfig*
schgui_config_new(void)
{
    static SchGUIConfig *config = NULL;

    if (config == NULL)
    {
        config = SCHGUI_CONFIG(g_object_new(SCHGUI_TYPE_CONFIG, NULL));
    }

    if (config != NULL)
    {
        g_object_ref(config);
    }

    return config;
}

