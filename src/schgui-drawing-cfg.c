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

/*! \file schgui-drawing-cfg.c
 */

#include <glib.h>
#include <glib-object.h>
#include <gdk/gdk.h>

#include "schgui.h"
#include "misc-object.h"

#define SCHGUI_DRAWING_CFG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCHGUI_TYPE_DRAWING_CFG,SchGUIDrawingCfgPrivate))

enum
{
    SCHGUI_DRAWING_CFG_BACKGROUND = 1,
    SCHGUI_DRAWING_CFG_COLOR_BACKGROUND,
    SCHGUI_DRAWING_CFG_FONT_STRING,
    SCHGUI_DRAWING_CFG_LINE_WIDTH_BUS,
    SCHGUI_DRAWING_CFG_LINE_WIDTH_GRAPHIC_MIN,
    SCHGUI_DRAWING_CFG_LINE_WIDTH_NET,
    SCHGUI_DRAWING_CFG_LINE_WIDTH_PIN
};

typedef struct _SchGUIDrawingCfgPrivate SchGUIDrawingCfgPrivate;

struct _SchGUIDrawingCfgPrivate
{
    GHashTable *colors;

    gchar      *font_string;

    gdouble    line_width_bus;
    gdouble    line_width_graphic_min;
    gdouble    line_width_net;
    gdouble    line_width_pin;
};

static void
schgui_drawing_cfg_class_init(gpointer g_class, gpointer g_class_data);

static void
schgui_drawing_cfg_dispose(GObject *object);

static void
schgui_drawing_cfg_finalize(GObject *object);

static void
schgui_drawing_cfg_get_background(SchGUIDrawingCfg *object, GValue *value);

static void
schgui_drawing_cfg_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
schgui_drawing_cfg_init(GTypeInstance *instance, gpointer g_class);

static void
schgui_drawing_cfg_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);




static void
schgui_drawing_cfg_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *object_class = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(object_class, sizeof(SchGUIDrawingCfgPrivate));

    object_class->dispose  = schgui_drawing_cfg_dispose;
    object_class->finalize = schgui_drawing_cfg_finalize;

    object_class->get_property = schgui_drawing_cfg_get_property;
    object_class->set_property = schgui_drawing_cfg_set_property;

    g_object_class_install_property(
        object_class,
        SCHGUI_DRAWING_CFG_BACKGROUND,
        g_param_spec_boxed(
            "background",
            "Background",
            "Background",
            GDK_TYPE_COLOR,
            G_PARAM_LAX_VALIDATION | G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCHGUI_DRAWING_CFG_COLOR_BACKGROUND,
        g_param_spec_boxed(
            "color-background",
            "Color Background",
            "Color Background",
            MISCGUI_TYPE_COLOR,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCHGUI_DRAWING_CFG_FONT_STRING,
        g_param_spec_string(
            "font-string",
            "Font String",
            "Font String",
            "Sans",
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCHGUI_DRAWING_CFG_LINE_WIDTH_BUS,
        g_param_spec_double(
            "line-width-bus",
            "Line Width Bus",
            "Line Width Bus",
            0,
            G_MAXDOUBLE,
            20.0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCHGUI_DRAWING_CFG_LINE_WIDTH_GRAPHIC_MIN,
        g_param_spec_double(
            "line-width-graphic-min",
            "Line Width Graphic Min",
            "Line Width Graphic Min",
            0,
            G_MAXDOUBLE,
            10.0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCHGUI_DRAWING_CFG_LINE_WIDTH_NET,
        g_param_spec_double(
            "line-width-net",
            "Line Width Net",
            "Line Width Net",
            0,
            G_MAXDOUBLE,
            10.0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCHGUI_DRAWING_CFG_LINE_WIDTH_PIN,
        g_param_spec_double(
            "line-width-pin",
            "Line Width Pin",
            "Line Width Pin",
            0,
            G_MAXDOUBLE,
            10.0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

}

static void
schgui_drawing_cfg_dispose(GObject *object)
{
    misc_object_chain_dispose(object);
}

static void
schgui_drawing_cfg_finalize(GObject *object)
{
    SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(object);

    if (privat != NULL)
    {
        if (privat->colors != NULL)
        {
            g_hash_table_destroy(privat->colors);
        }
    }

    misc_object_chain_finalize(object);
}

static void
schgui_drawing_cfg_get_background(SchGUIDrawingCfg *object, GValue *value)
{
    if (value != NULL)
    {
        GdkColor color;
        int      enabled = FALSE;

        if (object != NULL)
        {
            enabled = schgui_drawing_cfg_get_color_as_gdk_color(object, 0, &color);
        }

        if (!enabled)
        {
            color.red   = 0;
            color.green = 0;
            color.blue  = 0;
        }

        g_value_set_boxed(value, &color);
    }
}

void
schgui_drawing_cfg_get_background_as_gdk_color(SchGUIDrawingCfg *object, GdkColor *color)
{
    int enabled = FALSE;

    if (object != NULL)
    {
        enabled = schgui_drawing_cfg_get_color_as_gdk_color(object, 0, color);
    }

    if (!enabled)
    {
        color->red   = 0;
        color->green = 0;
        color->blue  = 0;
    }
}

static void
schgui_drawing_cfg_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    SchGUIDrawingCfg *config = SCHGUI_DRAWING_CFG(object);

    if (config != NULL)
    {
        switch (property_id)
        {
            case SCHGUI_DRAWING_CFG_BACKGROUND:
                schgui_drawing_cfg_get_background(config, value);
                break;

            case SCHGUI_DRAWING_CFG_COLOR_BACKGROUND:
                g_value_take_boxed(value, schgui_drawing_cfg_get_color_background(config));
                break;

            case SCHGUI_DRAWING_CFG_FONT_STRING:
                g_value_take_string(value, schgui_drawing_cfg_get_font_string(config));
                break;

            case SCHGUI_DRAWING_CFG_LINE_WIDTH_BUS:
                g_value_set_double(value, schgui_drawing_cfg_get_line_width_bus(config));
                break;

            case SCHGUI_DRAWING_CFG_LINE_WIDTH_GRAPHIC_MIN:
                g_value_set_double(value, schgui_drawing_cfg_get_line_width_graphic_min(config));
                break;

            case SCHGUI_DRAWING_CFG_LINE_WIDTH_NET:
                g_value_set_double(value, schgui_drawing_cfg_get_line_width_net(config));
                break;

            case SCHGUI_DRAWING_CFG_LINE_WIDTH_PIN:
                g_value_set_double(value, schgui_drawing_cfg_get_line_width_pin(config));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
schgui_drawing_cfg_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(SchGUIDrawingCfgClass),    /* class_size */
            NULL,                             /* base_init */
            NULL,                             /* base_finalize */
            schgui_drawing_cfg_class_init,    /* class_init */
            NULL,                             /* class_finalize */
            NULL,                             /* class_data */
            sizeof(SchGUIDrawingCfg),         /* instance_size */
            0,                                /* n_preallocs */
            schgui_drawing_cfg_init,          /* instance_init */
            NULL                              /* value_table */
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "SchGUIDrawingCfg",
            &tinfo,
            0
            );
    }

    return type;
}

static void
schgui_drawing_cfg_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    SchGUIDrawingCfg *config = SCHGUI_DRAWING_CFG(object);

    if (config != NULL)
    {
        switch (property_id)
        {
            case SCHGUI_DRAWING_CFG_BACKGROUND:
                schgui_drawing_cfg_set_color_by_gdk_color(config, 0, g_value_get_boxed(value));
                break;

            case SCHGUI_DRAWING_CFG_COLOR_BACKGROUND:
                schgui_drawing_cfg_set_color_background(config, MISCGUI_COLOR(g_value_get_boxed(value)));
                break;

            case SCHGUI_DRAWING_CFG_FONT_STRING:
                schgui_drawing_cfg_set_font_string(config, g_value_get_string(value));
                break;

            case SCHGUI_DRAWING_CFG_LINE_WIDTH_BUS:
                schgui_drawing_cfg_set_line_width_bus(config, g_value_get_double(value));
                break;

            case SCHGUI_DRAWING_CFG_LINE_WIDTH_GRAPHIC_MIN:
                schgui_drawing_cfg_set_line_width_graphic_min(config, g_value_get_double(value));
                break;

            case SCHGUI_DRAWING_CFG_LINE_WIDTH_NET:
                schgui_drawing_cfg_set_line_width_net(config, g_value_get_double(value));
                break;

            case SCHGUI_DRAWING_CFG_LINE_WIDTH_PIN:
                schgui_drawing_cfg_set_line_width_pin(config, g_value_get_double(value));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

void
schgui_drawing_cfg_get_display_color(SchGUIDrawingCfg *cfg, int index, double *red, double *green, double *blue)
{
    //sch_color_get_default(index, red, green, blue);
}

void
schgui_drawing_cfg_get_print_color(SchGUIDrawingCfg *cfg, int index, double *red, double *green, double *blue)
{
    //sch_color_get_default(index, red, green, blue);
}

void
schgui_drawing_cfg_get_output_line_width(SchGUIDrawingCfg *cfg, int shape_width, double *output_width)
{
    *output_width = shape_width;

    if (*output_width < 10)
    {
         *output_width = 10;
    }
}

void
schgui_drawing_cfg_disable_color(SchGUIDrawingCfg *config, int index)
{
    SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

    if ((privat != NULL) && (privat->colors != NULL))
    {
        g_hash_table_remove(privat->colors, GINT_TO_POINTER(index));

        if (index == 0)
        {
            g_object_notify(G_OBJECT(config), "background");
        }
    }
}

int
schgui_drawing_cfg_get_color(SchGUIDrawingCfg *config, int index, MiscGUIColor *color)
{
    int enabled = FALSE;

    if (index >= 0)
    {
        SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

        if ((privat != NULL) && (privat->colors != NULL))
        {
            MiscGUIColor *value = g_hash_table_lookup(privat->colors, GINT_TO_POINTER(index));

            if (value != NULL)
            {
                *color = *value;
                enabled = TRUE;
            }
        }

    }

    return enabled;
}

int
schgui_drawing_cfg_get_color_as_gdk_color(SchGUIDrawingCfg *config, int index, GdkColor *color)
{
    int enabled = FALSE;

    if (index >= 0) {

        SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

        if ((privat != NULL) && (privat->colors != NULL)) {

            MiscGUIColor *value = g_hash_table_lookup(privat->colors, GINT_TO_POINTER(index));

            if (value != NULL) {

                if (color != NULL) {

                    color->red   = (guint16) (65535.0 * value->red);
                    color->green = (guint16) (65535.0 * value->green);
                    color->blue  = (guint16) (65535.0 * value->blue);
                }

                enabled = TRUE;
            }
        }
    }

    return enabled;
}

SchGUIDrawingCfg*
schgui_drawing_cfg_get_default_display(void)
{
    static SchGUIDrawingCfg *config = NULL;

    if (config == NULL) {

        config = (SchGUIDrawingCfg*) g_object_new(SCHGUI_TYPE_DRAWING_CFG, NULL);
    }

    return config;
}

SchGUIDrawingCfg*
schgui_drawing_cfg_get_default_print(void)
{
    static SchGUIDrawingCfg *config = NULL;

    if (config == NULL) {
        config = (SchGUIDrawingCfg*) g_object_new(SCHGUI_TYPE_DRAWING_CFG, NULL);
    }

    return config;
}


static void
schgui_drawing_cfg_init(GTypeInstance *instance, gpointer g_class)
{
    SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(instance);

    if (privat != NULL) {

        privat->colors = g_hash_table_new_full(
          g_direct_hash,        /* hash_func          */
          g_direct_equal,       /* key_equal_func     */
          NULL,                 /* key_destroy_func   */
          g_free);  /* value_destroy_func */

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            0,           /* background */
            "#000000"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            1,           /* pin */
            "#ffffff"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            2,           /* net-endpoint */
            "#ff0000"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            3,           /* graphic */
            "#00ff00"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            4,           /* net */
            "#0000ff"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            5,           /* attribute */
            "#ffff00"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            6,           /* logic-bubble */
            "#00ffff"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            7,           /* dots-grid */
            "#bebebe"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            22,          /* mesh-grid-major */
            "#1e1e1e"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            23,          /* mesh-grid-minor */
            "#171717"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            8,           /* detached-attribute  */
            "#ff0000"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            9,           /* text */
            "#00ff00"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            10,          /* bus */
            "#00ff00"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            11,          /* select */
            "#ffa500"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            12,          /* bounding-box */
            "#ffa500"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            13,          /* zoom-box */
            "#00ffff"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            14,          /* stroke */
            "#e5e5e5"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            15,          /* lock */
            "#bebebe"
            );

        schgui_drawing_cfg_set_color_by_name(
            SCHGUI_DRAWING_CFG(instance),
            21,          /* junction */
            "#ffff00"
            );

        privat->line_width_bus         = 20.0;
        privat->line_width_graphic_min = 10.0;
        privat->line_width_net         = 10.0;
        privat->line_width_pin         = 10.0;
    }
}

void
schgui_drawing_cfg_set_color_by_gdk_color(SchGUIDrawingCfg *config, int index, const GdkColor *color)
{
    if ((index >= 0) && (color != NULL)) {

        SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

        if ((privat != NULL) && (privat->colors != NULL))  {

            MiscGUIColor *value = miscgui_color_new();

            value->red   = (double) color->red   / 65535.0;
            value->green = (double) color->green / 65535.0;
            value->blue  = (double) color->blue  / 65535.0;

            g_hash_table_replace(privat->colors, GINT_TO_POINTER(index), value);

            if (index == 0) {

                g_object_notify(G_OBJECT(config), "background");
            }
        }
    }
}

void
schgui_drawing_cfg_set_color_by_name(SchGUIDrawingCfg *config, int index, const char *color)
{
    if ((index >= 0) && (color != NULL)) {

        SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

        if ((privat != NULL) && (privat->colors != NULL)) {

            int      success;
            GdkColor value;

            success = gdk_color_parse(color, &value);

            if (success) {

                schgui_drawing_cfg_set_color_by_gdk_color(config, index, &value);
            }
        }
    }
}



gdouble
schgui_drawing_cfg_calcualte_line_width_graphic(const SchGUIDrawingCfg *config, gint width)
{
    gdouble width_out = width;

    if (config != NULL)
    {
        SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

        if (privat != NULL)
        {
            if (width_out < privat->line_width_graphic_min)
            {
                width_out = privat->line_width_graphic_min;
            }
        }
    }
    else
    {
        if (width_out < 10.0)
        {
            width_out = 10.0;
        }
    }

    return width_out;
}

MiscGUIColor*
schgui_drawing_cfg_get_color_background(const SchGUIDrawingCfg *config)
{
    MiscGUIColor *color = NULL;

    if (config != NULL)
    {
        SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

        if ((privat != NULL) && (privat->colors != NULL))
        {
            MiscGUIColor *value = g_hash_table_lookup(privat->colors, GINT_TO_POINTER(0));

            if (value != NULL)
            {
                color = miscgui_color_copy(value);
            }
        }
    }

    if (color == NULL)
    {
        color = miscgui_color_new();

        color->red   = 0.0;
        color->green = 0.0;
        color->blue  = 0.0;
        color->alpha = 1.0;
    }

    return color;
}

gchar*
schgui_drawing_cfg_get_font_string(const SchGUIDrawingCfg *config)
{
    gchar *string = NULL;

    if (config != NULL)
    {
        SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

        if (privat != NULL)
        {
            string = g_strdup(privat->font_string);
        }
    }

    if (string == NULL)
    {
        string = g_strdup("Sans");
    }

    return string;
}

gdouble
schgui_drawing_cfg_get_line_width_bus(const SchGUIDrawingCfg *config)
{
    gdouble width = 20.0;

    if (config != NULL)
    {
        SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

        if (privat != NULL)
        {
            width = privat->line_width_bus;
        }
    }

    return width;
}

gdouble
schgui_drawing_cfg_get_line_width_graphic_min(const SchGUIDrawingCfg *config)
{
    gdouble width = 10.0;

    if (config != NULL)
    {
        SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

        if (privat != NULL)
        {
            width = privat->line_width_graphic_min;
        }
    }

    return width;
}

gdouble
schgui_drawing_cfg_get_line_width_net(const SchGUIDrawingCfg *config)
{
    gdouble width = 10.0;

    if (config != NULL)
    {
        SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

        if (privat != NULL)
        {
            width = privat->line_width_net;
        }
    }

    return width;
}

gdouble
schgui_drawing_cfg_get_line_width_pin(const SchGUIDrawingCfg *config)
{
    gdouble width = 10.0;

    if (config != NULL)
    {
        SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

        if (privat != NULL)
        {
            width = privat->line_width_pin;
        }
    }

    return width;
}

void
schgui_drawing_cfg_set_color_background(SchGUIDrawingCfg *config, const MiscGUIColor *color)
{
    SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

    if ((privat != NULL) && (privat->colors != NULL))
    {
        MiscGUIColor *value;

        if (color != NULL)
        {
            value = miscgui_color_copy(color);

            value->alpha = 1.0;
        }
        else
        {
            value = miscgui_color_new();

            value->red   = 0.0;
            value->green = 0.0;
            value->blue  = 0.0;
            value->alpha = 1.0;
        }

        g_hash_table_replace(privat->colors, 0, value);

        g_object_notify(G_OBJECT(config), "color-background");
    }
}

void
schgui_drawing_cfg_set_font_string(SchGUIDrawingCfg *config, const gchar *string)
{
    SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

    if (privat != NULL)
    {
        if (string != NULL)
        {
            privat->font_string = g_strdup(string);
        }
        else
        {
            privat->font_string = g_strdup("Sans");
        }

        g_object_notify(G_OBJECT(config), "font-string");
    }
}

void
schgui_drawing_cfg_set_line_width_bus(SchGUIDrawingCfg *config, gdouble width)
{
    SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

    if (privat != NULL)
    {
        if (width > 0.0)
        {
            privat->line_width_bus = width;
        }
        else
        {
            privat->line_width_bus = 0.0;
        }

        g_object_notify(G_OBJECT(config), "line-width-bus");
    }
}

void
schgui_drawing_cfg_set_line_width_graphic_min(SchGUIDrawingCfg *config, gdouble width)
{
    SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

    if (privat != NULL)
    {
        if (width > 0.0)
        {
            privat->line_width_graphic_min = width;
        }
        else
        {
            privat->line_width_graphic_min = 0.0;
        }

        g_object_notify(G_OBJECT(config), "line-width-graphic-min");
    }
}

void
schgui_drawing_cfg_set_line_width_net(SchGUIDrawingCfg *config, gdouble width)
{
    SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

    if (privat != NULL)
    {
        if (width > 0.0)
        {
            privat->line_width_net = width;
        }
        else
        {
            privat->line_width_net = 0.0;
        }

        g_object_notify(G_OBJECT(config), "line-width-net");
    }
}

void
schgui_drawing_cfg_set_line_width_pin(SchGUIDrawingCfg *config, gdouble width)
{
    SchGUIDrawingCfgPrivate *privat = SCHGUI_DRAWING_CFG_GET_PRIVATE(config);

    if (privat != NULL)
    {
        if (width > 0.0)
        {
            privat->line_width_pin = width;
        }
        else
        {
            privat->line_width_pin = 0.0;
        }

        g_object_notify(G_OBJECT(config), "line-width-pin");
    }
}

