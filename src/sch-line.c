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

/*! \file sch-line.c
 */

#include <glib.h>
#include <glib-object.h>

#include "sch.h"

#define SCH_LINE_DEFAULT_COLOR      (SCH_CONFIG_DEFAULT_GRAPHIC_COLOR)
#define SCH_LINE_DEFAULT_LINE_WIDTH (SCH_CONFIG_DEFAULT_LINE_WIDTH)

#define SCH_LINE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCH_TYPE_LINE,SchLinePrivate))

enum
{
    SCH_LINE_X1 = 1,
    SCH_LINE_Y1,
    SCH_LINE_X2,
    SCH_LINE_Y2,
    SCH_LINE_COLOR,
    SCH_LINE_WIDTH,
    SCH_LINE_CAP_STYLE,
    SCH_LINE_DASH_STYLE,
    SCH_LINE_DASH_LENGTH,
    SCH_LINE_DASH_SPACE
};

typedef struct _SchLinePrivate SchLinePrivate;

struct _SchLinePrivate
{
    GeomLine     line;
    int          color;
    int          line_width;
    SchLineStyle line_style;
};



static void
sch_line_class_init(void *g_class, void *g_class_data);

static void
sch_line_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec);

static void
sch_line_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec);

static void
sch_line_rotate(SchShape *shape, int angle);

static void
sch_line_transform(SchShape *shape, const GeomTransform *transform);

static void
sch_line_translate(SchShape *shape, int dx, int dy);

static void
sch_line_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error);


static void
sch_line_class_init(void *g_class, void *g_class_data)
{
    GObjectClass *klasse = G_OBJECT_CLASS(g_class);
    SchLineClass *klasse1 = SCH_LINE_CLASS(g_class);

    g_type_class_add_private(klasse, sizeof(SchLinePrivate));

    klasse->get_property = sch_line_get_property;
    klasse->set_property = sch_line_set_property;

    klasse1->parent.rotate    = sch_line_rotate;
    klasse1->parent.transform = sch_line_transform;
    klasse1->parent.translate = sch_line_translate;
    klasse1->parent.write     = sch_line_write;

    g_object_class_install_property(
        klasse,
        SCH_LINE_X1,
        g_param_spec_int(
            "x1",
            "X1",
            "X1",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCH_LINE_Y1,
        g_param_spec_int(
            "y1",
            "Y1",
            "Y1",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCH_LINE_X2,
        g_param_spec_int(
            "x2",
            "X2",
            "X2",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCH_LINE_Y2,
        g_param_spec_int(
            "y2",
            "Y2",
            "Y2",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCH_LINE_COLOR,
        g_param_spec_int(
            "color",
            "Color",
            "Color",
            0,
            G_MAXINT,
            SCH_LINE_DEFAULT_COLOR,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCH_LINE_WIDTH,
        g_param_spec_int(
            "line-width",
            "Line Width",
            "Line Width",
            0,
            G_MAXINT,
            SCH_LINE_DEFAULT_LINE_WIDTH,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCH_LINE_CAP_STYLE,
        g_param_spec_int(
            "line-cap-style",
            "Line Cap Style",
            "Line Cap Style",
            0,
            2,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCH_LINE_DASH_STYLE,
        g_param_spec_int(
            "line-dash-style",
            "Line dash Style",
            "Line dash Style",
            0,
            4,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCH_LINE_DASH_LENGTH,
        g_param_spec_int(
            "line-dash-length",
            "Line Dash Length",
            "Line Dash Length",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCH_LINE_DASH_SPACE,
        g_param_spec_int(
            "line-dash-space",
            "Line Dash Space",
            "Line Dash Space",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );
}

static void
sch_line_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec)
{
    SchLinePrivate *privat = SCH_LINE_GET_PRIVATE(object);

    if (privat != NULL)
    {
        switch (property_id)
        {
            case SCH_LINE_X1:
                g_value_set_int(value, privat->line.x[0]);
                break;

            case SCH_LINE_Y1:
                g_value_set_int(value, privat->line.y[0]);
                break;

            case SCH_LINE_X2:
                g_value_set_int(value, privat->line.x[1]);
                break;

            case SCH_LINE_Y2:
                g_value_set_int(value, privat->line.y[1]);
                break;

            case SCH_LINE_COLOR:
                g_value_set_int(value, privat->color);
                break;

            case SCH_LINE_WIDTH:
                g_value_set_int(value, privat->line_width);
                break;

            case SCH_LINE_CAP_STYLE:
                g_value_set_int(value, privat->line_style.cap_style);
                break;

            case SCH_LINE_DASH_STYLE:
                g_value_set_int(value, privat->line_style.dash_style);
                break;

            case SCH_LINE_DASH_LENGTH:
                g_value_set_int(value, privat->line_style.dash_length);
                break;

            case SCH_LINE_DASH_SPACE:
                g_value_set_int(value, privat->line_style.dash_space);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
sch_line_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(SchLineClass),    /* class_size */
            NULL,                    /* base_init */
            NULL,                    /* base_finalize */
            sch_line_class_init,     /* class_init */
            NULL,                    /* class_finalize */
            NULL,                    /* class_data */
            sizeof(SchLine),         /* instance_size */
            0,                       /* n_preallocs */
            NULL,                    /* instance_init */
            NULL                     /* value_table */
            };

        type = g_type_register_static(
            SCH_TYPE_SHAPE,
            "SchLine",
            &tinfo,
            0
            );
    }

    return type;
}

static void
sch_line_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec)
{
    SchLinePrivate *privat = SCH_LINE_GET_PRIVATE(object);

    if (privat != NULL)
    {
        switch (property_id)
        {
            case SCH_LINE_X1:
                privat->line.x[0] = g_value_get_int(value);
                break;

            case SCH_LINE_Y1:
                privat->line.y[0] = g_value_get_int(value);
                break;

            case SCH_LINE_X2:
                privat->line.x[1] = g_value_get_int(value);
                break;

            case SCH_LINE_Y2:
                privat->line.y[1] = g_value_get_int(value);
                break;

            case SCH_LINE_COLOR:
                privat->color = g_value_get_int(value);
                break;

            case SCH_LINE_WIDTH:
                privat->line_width = g_value_get_int(value);
                break;

            case SCH_LINE_CAP_STYLE:
                privat->line_style.cap_style = g_value_get_int(value);
                break;

            case SCH_LINE_DASH_STYLE:
                privat->line_style.dash_style = g_value_get_int(value);
                break;

            case SCH_LINE_DASH_LENGTH:
                privat->line_style.dash_length = g_value_get_int(value);
                break;

            case SCH_LINE_DASH_SPACE:
                privat->line_style.dash_space = g_value_get_int(value);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

void
sch_line_get_color(const SchLine *line, int *index)
{
    if (index != NULL)
    {
        SchLinePrivate *privat = SCH_LINE_GET_PRIVATE(line);

        *index = SCH_LINE_DEFAULT_COLOR;

        if (privat != NULL)
        {
            *index = privat->color;
        }
    }
}

void
sch_line_get_line(const SchLine *shape, GeomLine *line)
{
    if (line != NULL)
    {
        SchLinePrivate *privat = SCH_LINE_GET_PRIVATE(shape);

        if (privat != NULL)
        {
            *line = privat->line;
        }
        else
        {
            geom_line_init(line);
        }
    }
}

void
sch_line_get_line_style(const SchLine *shape, SchLineStyle *style)
{
    if (style != NULL)
    {
        SchLinePrivate *privat = SCH_LINE_GET_PRIVATE(shape);

        if (privat != NULL)
        {
            *style = privat->line_style;
        }
        else
        {
            sch_line_style_init(style);
        }
    }
}

void
sch_line_get_line_width(const SchLine *line, int *width)
{
    if (width != NULL)
    {
        SchLinePrivate *privat = SCH_LINE_GET_PRIVATE(line);

        *width = SCH_LINE_DEFAULT_LINE_WIDTH;

        if (privat != NULL)
        {
            *width = privat->line_width;
        }
    }
}

SchLine*
sch_line_new(const SchConfig *config)
{
    SchLineStyle line_style;

    sch_config_get_line_style(config, &line_style);

    return SCH_LINE(g_object_new(
        SCH_TYPE_LINE,
        "color",            sch_config_get_graphic_color(config),
        "line-width",       sch_config_get_line_width(config),
        "line-cap-style",   line_style.cap_style,
        "line-dash-style",  line_style.dash_style,
        "line-dash-length", line_style.dash_length,
        "line-dash-space",  line_style.dash_space,
        NULL
        ));
}

static void
sch_line_rotate(SchShape *shape, int angle)
{
    SchLinePrivate *privat = SCH_LINE_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        geom_line_rotate(&(privat->line), angle);
    }
}


static void
sch_line_transform(SchShape *shape, const GeomTransform *transform)
{
    if (transform != NULL)
    {
        SchLinePrivate *privat = SCH_LINE_GET_PRIVATE(shape);

        if (privat != NULL)
        {
            geom_line_transform(&(privat->line), transform);
        }
    }
}



static void
sch_line_translate(SchShape *shape, int dx, int dy)
{
    SchLinePrivate *privat = SCH_LINE_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        geom_line_translate(&(privat->line), dx, dy);
    }
}

static void
sch_line_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error)
{
    sch_file_format_2_write_line(format, stream, SCH_LINE(shape), error);
}

