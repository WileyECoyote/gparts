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

/*! \file sch-text.c
 */

#include <math.h>

#include <glib.h>
#include <glib-object.h>

#include "misc-macro.h"

#include "sch.h"

#define SCH_TEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCH_TYPE_TEXT,SchTextPrivate))

enum
{
    SCH_TEXT_X = 1,
    SCH_TEXT_Y,
    SCH_TEXT_COLOR,
    SCH_TEXT_SIZE,
    SCH_TEXT_VISIBILITY,
    SCH_TEXT_SHOW_NAME_VALUE,
    SCH_TEXT_ANGLE,
    SCH_TEXT_ALIGNMENT,
    SCH_TEXT_NUM_LINES
};

typedef struct _SchTextPrivate SchTextPrivate;

struct _SchTextPrivate
{
    int  x;
    int  y;
    int  color;
    int  size;
    int  visibility;
    int  show_name_value;
    int  angle;
    int  alignment;

    SchMultiline *multiline;
};

static void
sch_text_class_init(void *g_class, void *g_class_data);

static void
sch_text_expand_macros(SchShape *shape, const GRegex *regex, const GHashTable *table);

static void
sch_text_find_macros(const SchShape *shape, const GRegex *regex, GHashTable *table);

static void
sch_text_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec);

/*
static void
sch_text_schematic_shape_init(void *g_iface, void *g_iface_data);
*/

static void
sch_text_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec);

void
sch_text_rotate(SchShape *shape, int angle);

static void
sch_text_transform(SchShape *shape, const GeomTransform *transform);

void
sch_text_translate(SchShape *shape, int dx, int dy);

static void
sch_text_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error);


static void
sch_text_class_init(void *g_class, void *g_class_data)
{
    GError *error = NULL;
    GObjectClass *object_class = G_OBJECT_CLASS(g_class);
    SchTextClass *klasse = SCH_TEXT_CLASS(g_class);

    g_type_class_add_private(object_class, sizeof(SchTextPrivate));

    object_class->get_property = sch_text_get_property;
    object_class->set_property = sch_text_set_property;

    klasse->parent.expand_macros = sch_text_expand_macros;
    klasse->parent.find_macros   = sch_text_find_macros;
    klasse->parent.rotate        = sch_text_rotate;
    klasse->parent.transform     = sch_text_transform;
    klasse->parent.translate     = sch_text_translate;
    klasse->parent.write         = sch_text_write;

    g_object_class_install_property(
        object_class,
        SCH_TEXT_X,
        g_param_spec_int(
            "x",
            "X",
            "X",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_TEXT_Y,
        g_param_spec_int(
            "y",
            "Y",
            "Y",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_TEXT_COLOR,
        g_param_spec_int(
            "color",
            "Color",
            "Color",
            0,
            G_MAXINT,
            SCH_CONFIG_DEFAULT_TEXT_COLOR,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_TEXT_SIZE,
        g_param_spec_int(
            "size",
            "Size",
            "Size",
            0,
            G_MAXINT,
            SCH_CONFIG_DEFAULT_TEXT_SIZE,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_TEXT_VISIBILITY,
        g_param_spec_int(
            "visibility",
            "Visibility",
            "Visibility",
            0,
            1,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_TEXT_SHOW_NAME_VALUE,
        g_param_spec_int(
            "show-name-value",
            "Show Name Value",
            "Show Name Value",
            0,
            2,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_TEXT_ANGLE,
        g_param_spec_int(
            "angle",
            "Angle",
            "Angle",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_TEXT_ALIGNMENT,
        g_param_spec_int(
            "alignment",
            "Alignment",
            "Alignment",
            0,
            8,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_TEXT_NUM_LINES,
        g_param_spec_int(
            "num-lines",
            "Num Lines",
            "Num Lines",
            0,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    klasse->regex = g_regex_new(
        "^((?:[ ]*[^= ]+)*)=([^ ].*)",
        G_REGEX_DOTALL,
        0,
        &error
        );

    if (error != NULL)
    {
        g_critical("g_regex_new: %s", error->message);

        g_error_free(error);
    }
}

SchText*
sch_text_clone(const SchText *text)
{
    SchText *temp = NULL;

    if (text != NULL)
    {
        SchMultiline *multiline1;
        SchMultiline *multiline2;

        temp = SCH_TEXT(g_object_new(
            SCH_TYPE_TEXT,
            "x",               sch_text_get_x(text),
            "y",               sch_text_get_y(text),
            "color",           sch_text_get_color(text),
            "size",            sch_text_get_size(text),
            "visibility",      sch_text_get_visible(text),
            "show-name-value", sch_text_get_show(text),
            "angle",           sch_text_get_angle(text),
            "alignment",       sch_text_get_alignment(text),
            NULL
            ));

        multiline1 = sch_text_get_multiline(text);

        multiline2 = sch_multiline_clone(multiline1);

        sch_text_set_multiline(temp, multiline2);

        if (multiline1 != NULL)
        {
            g_object_unref(multiline1);
        }

        if (multiline2 != NULL)
        {
            g_object_unref(multiline2);
        }
    }

    return temp;
}

static void
sch_text_expand_macros(SchShape *shape, const GRegex *regex, const GHashTable *table)
{
    SchShapeClass  *klasse = SCH_SHAPE_GET_CLASS(shape);
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        gchar *param = sch_text_get_string(SCH_TEXT(shape));
        gchar *result;

        result = misc_macro_expand(regex, param, table);

        sch_text_set_string(SCH_TEXT(shape), result);

        g_free(param);
        g_free(result);

    }

    if (klasse != NULL)
    {
        SchShapeClass *parent_klasse = SCH_SHAPE_CLASS(g_type_class_peek_parent(klasse));

        if ((parent_klasse != NULL) && (parent_klasse->expand_macros != NULL))
        {
            parent_klasse->expand_macros(shape, regex, table);
        }
    }
}

static void
sch_text_find_macros(const SchShape *shape, const GRegex *regex, GHashTable *table)
{
    SchShapeClass  *klasse = SCH_SHAPE_GET_CLASS(shape);
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        gchar *param = sch_text_get_string(SCH_TEXT(shape));

        misc_macro_find(regex, param, table);

        g_free(param);
    }

    if (klasse != NULL)
    {
        SchShapeClass *parent_klasse = SCH_SHAPE_CLASS(g_type_class_peek_parent(klasse));

        if ((parent_klasse != NULL) && (parent_klasse->expand_macros != NULL))
        {
            parent_klasse->find_macros(shape, regex, table);
        }
    }
}

int
sch_text_get_angle(const SchText *text)
{
    int angle = 0;
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(text);

    if (privat != NULL)
    {
        angle = privat->angle;
    }

    return angle;
}

int
sch_text_get_alignment(const SchText *text)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(text);

    if (privat != NULL)
    {
        return privat->alignment;
    }

    return 0;
}

gchar*
sch_text_get_attribute_name(const SchText *shape)
{
    gchar *name = NULL;

    if (shape != NULL)
    {
        SchTextClass *klasse = SCH_TEXT_GET_CLASS(shape);

        if ((klasse != NULL) && (klasse->regex != NULL))
        {
            GMatchInfo *match_info;
            gchar *string = sch_text_get_string(shape);

            g_regex_match(klasse->regex, string, 0, &match_info);

            if (g_match_info_matches(match_info))
            {
                name = g_match_info_fetch(match_info, 1);
            }

            g_match_info_free(match_info);
            g_free(string);
        }
    }

    return name;
}

gchar*
sch_text_get_attribute_value(const SchText *shape)
{
    gchar *value = NULL;

    if (shape != NULL)
    {
        SchTextClass *klasse = SCH_TEXT_GET_CLASS(shape);

        if ((klasse != NULL) && (klasse->regex != NULL))
        {
            GMatchInfo *match_info;
            gchar *string = sch_text_get_string(shape);

            g_regex_match(klasse->regex, string, 0, &match_info);

            if (g_match_info_matches(match_info))
            {
                value = g_match_info_fetch(match_info, 2);
            }

            g_match_info_free(match_info);
            g_free(string);
        }
    }

    return value;
}

int
sch_text_get_color(const SchText *shape)
{
    int color = SCH_CONFIG_DEFAULT_TEXT_COLOR;
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        color = privat->color;
    }

    return color;
}

SchMultiline*
sch_text_get_multiline(const SchText *text)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(text);
    SchMultiline *multiline = NULL;

    if (privat != NULL)
    {
        multiline = privat->multiline;

        if (multiline != NULL)
        {
            g_object_ref(multiline);
        }
    }

    return multiline;
}

static void
sch_text_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(object);

    if (privat != NULL)
    {
        switch (property_id)
        {
            case SCH_TEXT_X:
                g_value_set_int(value, privat->x);
                break;

            case SCH_TEXT_Y:
                g_value_set_int(value, privat->y);
                break;

            case SCH_TEXT_COLOR:
                g_value_set_int(value, privat->color);
                break;

            case SCH_TEXT_SIZE:
                g_value_set_int(value, privat->size);
                break;

            case SCH_TEXT_VISIBILITY:
                g_value_set_int(value, privat->visibility);
                break;

            case SCH_TEXT_SHOW_NAME_VALUE:
                g_value_set_int(value, privat->show_name_value);
                break;

            case SCH_TEXT_ANGLE:
                g_value_set_int(value, privat->angle);
                break;

            case SCH_TEXT_ALIGNMENT:
                g_value_set_int(value, privat->alignment);
                break;

            case SCH_TEXT_NUM_LINES:
                g_value_set_int(value, sch_multiline_lines(privat->multiline));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

int
sch_text_get_size(const SchText *text)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(text);

    if (privat != NULL)
    {
        return privat->size;
    }

    return 0;
}

int
sch_text_get_show(const SchText *shape)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(shape);
    int show = 1;

    if (privat != NULL)
    {
        show = privat->show_name_value;
    }

    return show;
}

gchar*
sch_text_get_shown_string(const SchText *text)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(text);
    gchar *string = NULL;

    if (privat != NULL)
    {
        switch (privat->show_name_value)
        {
            case SCH_TEXT_SHOW_VALUE:
                string = sch_text_get_attribute_value(text);
                break;

            case SCH_TEXT_SHOW_NAME:
                string = sch_text_get_attribute_name(text);
                break;

            case SCH_TEXT_SHOW_ALL:
            default:
                string = sch_text_get_string(text);
        }
    }

    return string;
}

char*
sch_text_get_string(const SchText *text)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(text);
    char *string = NULL;

    if ((privat != NULL) && (privat->multiline != NULL))
    {
        string = g_strdup(sch_multiline_peek_plain(privat->multiline, 0));
    }

    return string;
}

GType
sch_text_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(SchTextClass),    /* class_size */
            NULL,                    /* base_init */
            NULL,                    /* base_finalize */
            sch_text_class_init,     /* class_init */
            NULL,                    /* class_finalize */
            NULL,                    /* class_data */
            sizeof(SchText),         /* instance_size */
            0,                       /* n_preallocs */
            NULL,                    /* instance_init */
            NULL                     /* value_table */
            };

        type = g_type_register_static(
            SCH_TYPE_SHAPE,
            "SchText",
            &tinfo,
            0
            );
    }

    return type;
}

int
sch_text_get_visible(const SchText *shape)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(shape);
    int visible = 1;

    if (privat != NULL)
    {
        visible = privat->visibility;
    }

    return visible;
}

int
sch_text_get_x(const SchText *text)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(text);
    int x = 0;

    if (privat != NULL)
    {
        x = privat->x;
    }

    return x;
}

int
sch_text_get_y(const SchText *text)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(text);
    int y = 0;

    if (privat != NULL)
    {
        y = privat->y;
    }

    return y;
}

SchText*
sch_text_new(const SchConfig *config)
{
    return SCH_TEXT(g_object_new(
        SCH_TYPE_TEXT,
        "color", sch_config_get_text_color(config),
        "size",  sch_config_get_text_size(config),
        NULL
        ));
}

void
sch_text_rotate(SchShape *shape, int angle)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        double   radians = geom_angle_radians(angle);
        int tx = privat->x;
        int ty = privat->y;

        privat->x = round(tx * cos(radians) - ty * sin(radians));
        privat->y = round(tx * sin(radians) + ty * cos(radians));

        privat->angle += angle;
    }
}

void
sch_text_set_alignment(SchText *shape, int alignment)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->alignment = alignment;

        g_object_notify(G_OBJECT(shape), "alignment");
    }
}

void
sch_text_set_angle(SchText *shape, int angle)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->angle = angle;

        g_object_notify(G_OBJECT(shape), "angle");
    }
}

void
sch_text_set_color(SchText *shape, int color)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->color = color;

        g_object_notify(G_OBJECT(shape), "color");
    }
}

static void
sch_text_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec)
{
    switch (property_id)
    {
        case SCH_TEXT_X:
            sch_text_set_x(SCH_TEXT(object), g_value_get_int(value));
            break;

        case SCH_TEXT_Y:
            sch_text_set_y(SCH_TEXT(object), g_value_get_int(value));
            break;

        case SCH_TEXT_COLOR:
            sch_text_set_color(SCH_TEXT(object), g_value_get_int(value));
            break;

        case SCH_TEXT_SIZE:
            sch_text_set_size(SCH_TEXT(object), g_value_get_int(value));
            break;

        case SCH_TEXT_VISIBILITY:
            sch_text_set_visible(SCH_TEXT(object), g_value_get_int(value));
            break;

        case SCH_TEXT_SHOW_NAME_VALUE:
            sch_text_set_show(SCH_TEXT(object), g_value_get_int(value));
            break;

        case SCH_TEXT_ANGLE:
            sch_text_set_angle(SCH_TEXT(object), g_value_get_int(value));
            break;

        case SCH_TEXT_ALIGNMENT:
            sch_text_set_alignment(SCH_TEXT(object), g_value_get_int(value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

void
sch_text_set_multiline(SchText *text, SchMultiline *multiline)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(text);

    if (privat != NULL)
    {
        if (privat->multiline != NULL)
        {
            g_object_unref(privat->multiline);
        }

        privat->multiline = multiline;

        if (privat->multiline != NULL)
        {
            g_object_ref(privat->multiline);
        }
    }
}

void
sch_text_set_show(SchText *shape, int show)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->show_name_value = show;

        g_object_notify(G_OBJECT(shape), "show-name-value");
    }
}


void
sch_text_set_size(SchText *shape, int size)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->size = size;

        g_object_notify(G_OBJECT(shape), "size");
    }
}

void
sch_text_set_string(SchText *shape, const char *string)
{
    SchMultiline *multiline = g_object_new(SCH_TYPE_MULTILINE, NULL);

    gchar **lines = g_strsplit(string, "\n", 0);
    gchar **temp = lines;

    while (*temp != NULL)
    {
        sch_multiline_append(multiline, *temp);

        temp++;
    }

    g_strfreev(lines);

    sch_text_set_multiline(shape, multiline);
}

void
sch_text_set_visible(SchText *shape, int visible)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->visibility = visible;

        g_object_notify(G_OBJECT(shape), "visibility");
    }
}


void
sch_text_set_x(SchText *shape, int x)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->x = x;

        g_object_notify(G_OBJECT(shape), "x");
    }
}

void
sch_text_set_y(SchText *shape, int y)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->y = y;

        g_object_notify(G_OBJECT(shape), "y");
    }
}

static void
sch_text_transform(SchShape *shape, const GeomTransform *transform)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        geom_transform_point(transform, &(privat->x), &(privat->y));
    }
}

void
sch_text_translate(SchShape *shape, int dx, int dy)
{
    SchTextPrivate *privat = SCH_TEXT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->x += dx;
        privat->y += dy;
    }
}

static void
sch_text_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error)
{
    sch_file_format_2_write_text(format, stream, SCH_TEXT(shape), error);
}

