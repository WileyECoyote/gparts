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

/*! \file sch-circle.c
 */

#include <glib.h>
#include <glib-object.h>

#include "sch.h"


#define SCH_CIRCLE_DEFAULT_COLOR      (SCH_CONFIG_DEFAULT_GRAPHIC_COLOR)
#define SCH_CIRCLE_DEFAULT_LINE_WIDTH (SCH_CONFIG_DEFAULT_LINE_WIDTH)

#define SCH_CIRCLE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCH_TYPE_CIRCLE,SchCirclePrivate))

enum
{
    SCH_CIRCLE_X = 1,
    SCH_CIRCLE_Y,
    SCH_CIRCLE_RADIUS,
    SCH_CIRCLE_COLOR,
    SCH_CIRCLE_LINE_WIDTH,
    SCH_CIRCLE_CAP_STYLE,
    SCH_CIRCLE_DASH_STYLE,
    SCH_CIRCLE_DASH_LENGTH,
    SCH_CIRCLE_DASH_SPACE,
    SCH_CIRCLE_FILL_TYPE,
    SCH_CIRCLE_FILL_WIDTH,
    SCH_CIRCLE_FILL_ANGLE1,
    SCH_CIRCLE_FILL_PITCH1,
    SCH_CIRCLE_FILL_ANGLE2,
    SCH_CIRCLE_FILL_PITCH2
};

typedef struct _SchCirclePrivate SchCirclePrivate;

struct _SchCirclePrivate
{
    GeomCircle   circle;
    int          color;
    int          line_width;
    SchLineStyle line_style;
    SchFillStyle fill_style;
};

static void
sch_circle_class_init(void *g_class, void *g_class_data);

static void
sch_circle_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec);

static void
sch_circle_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec);

static void
sch_circle_rotate(SchShape *shape, int angle);

static void
sch_circle_transform(SchShape *shape, const GeomTransform *transform);

static void
sch_circle_translate(SchShape *shape, int dx, int dy);

static void
sch_circle_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error);


static void
sch_circle_class_init(void *g_class, void *g_class_data)
{
    GObjectClass *object_class = G_OBJECT_CLASS(g_class);
    SchCircleClass *klasse = SCH_CIRCLE_CLASS(g_class);

    g_type_class_add_private(object_class, sizeof(SchCirclePrivate));

    object_class->get_property = sch_circle_get_property;
    object_class->set_property = sch_circle_set_property;

    klasse->parent.rotate    = sch_circle_rotate;
    klasse->parent.transform = sch_circle_transform;
    klasse->parent.translate = sch_circle_translate;
    klasse->parent.write     = sch_circle_write;

    g_object_class_install_property(object_class,
                                    SCH_CIRCLE_X,
                                    g_param_spec_int("center-x",
                                                     "Center X",
                                                     "Center X",
                                                     G_MININT,
                                                     G_MAXINT,
                                                     0,
                                                     G_PARAM_LAX_VALIDATION |
                                                     G_PARAM_READWRITE |
                                                     G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(object_class,
                                    SCH_CIRCLE_Y,
                                    g_param_spec_int("center-y",
                                                     "Center Y",
                                                     "Center Y",
                                                     G_MININT,
                                                     G_MAXINT,
                                                     0,
                                                     G_PARAM_LAX_VALIDATION |
                                                     G_PARAM_READWRITE |
                                                     G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(object_class,
                                    SCH_CIRCLE_RADIUS,
                                    g_param_spec_int("radius",
                                                     "Radius",
                                                     "Radius",
                                                     0,
                                                     G_MAXINT,
                                                     0,
                                                     G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(
        object_class,
        SCH_CIRCLE_COLOR,
        g_param_spec_int(
            "color",
            "Color",
            "Color",
            0,
            G_MAXINT,
            SCH_CIRCLE_DEFAULT_COLOR,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(
        object_class,
        SCH_CIRCLE_LINE_WIDTH,
        g_param_spec_int(
            "line-width",
            "Line Width",
            "Line Width",
            0,
            G_MAXINT,
            SCH_CIRCLE_DEFAULT_LINE_WIDTH,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(
        object_class,
        SCH_CIRCLE_CAP_STYLE,
        g_param_spec_int(
            "line-cap-style",
            "Line Cap Style",
            "Line Cap Style",
            0,
            2,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(
        object_class,
        SCH_CIRCLE_DASH_STYLE,
        g_param_spec_int(
            "line-dash-style",
            "Line dash Style",
            "Line dash Style",
            0,
            4,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(
        object_class,
        SCH_CIRCLE_DASH_LENGTH,
        g_param_spec_int(
            "line-dash-length",
            "Line Dash Length",
            "Line Dash Length",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(
        object_class,
        SCH_CIRCLE_DASH_SPACE,
        g_param_spec_int(
            "line-dash-space",
            "Line Dash Space",
            "Line Dash Space",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(
        object_class,
        SCH_CIRCLE_FILL_TYPE,
        g_param_spec_int(
            "fill-type",
            "Fill Type",
            "Fill Type",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(
        object_class,
        SCH_CIRCLE_FILL_WIDTH,
        g_param_spec_int(
            "fill-width",
            "Fill Width",
            "Fill Width",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(
        object_class,
        SCH_CIRCLE_FILL_ANGLE1,
        g_param_spec_int(
            "fill-angle1",
            "Fill Angle 1",
            "Fill Angle 1",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(
        object_class,
        SCH_CIRCLE_FILL_PITCH1,
        g_param_spec_int(
            "fill-pitch1",
            "Fill Pitch 1",
            "Fill Pitch 1",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(
        object_class,
        SCH_CIRCLE_FILL_ANGLE2,
        g_param_spec_int(
            "fill-angle2",
            "Fill Angle 2",
            "Fill Angle 2",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(
        object_class,
        SCH_CIRCLE_FILL_PITCH2,
        g_param_spec_int(
            "fill-pitch2",
            "Fill Pitch 2",
            "Fill Pitch 2",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

}

static void
sch_circle_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec)
{
    SchCirclePrivate *privat = SCH_CIRCLE_GET_PRIVATE(object);

    if (privat != NULL)
    {
        switch (property_id)
        {
            case SCH_CIRCLE_X:
                g_value_set_int(value, privat->circle.center_x);
                break;

            case SCH_CIRCLE_Y:
                g_value_set_int(value, privat->circle.center_y);
                break;

            case SCH_CIRCLE_RADIUS:
                g_value_set_int(value, privat->circle.radius);
                break;

            case SCH_CIRCLE_COLOR:
                g_value_set_int(value, privat->color);
                break;

            case SCH_CIRCLE_LINE_WIDTH:
                g_value_set_int(value, privat->line_width);
                break;

            case SCH_CIRCLE_CAP_STYLE:
                g_value_set_int(value, privat->line_style.cap_style);
                break;

            case SCH_CIRCLE_DASH_STYLE:
                g_value_set_int(value, privat->line_style.dash_style);
                break;

            case SCH_CIRCLE_DASH_LENGTH:
                g_value_set_int(value, privat->line_style.dash_length);
                break;

            case SCH_CIRCLE_DASH_SPACE:
                g_value_set_int(value, privat->line_style.dash_space);
                break;

            case SCH_CIRCLE_FILL_TYPE:
                g_value_set_int(value, privat->fill_style.type);
                break;

            case SCH_CIRCLE_FILL_WIDTH:
                g_value_set_int(value, privat->fill_style.width);
                break;

            case SCH_CIRCLE_FILL_ANGLE1:
                g_value_set_int(value, privat->fill_style.angle1);
                break;

            case SCH_CIRCLE_FILL_PITCH1:
                g_value_set_int(value, privat->fill_style.pitch1);
                break;

            case SCH_CIRCLE_FILL_ANGLE2:
                g_value_set_int(value, privat->fill_style.angle2);
                break;

            case SCH_CIRCLE_FILL_PITCH2:
                g_value_set_int(value, privat->fill_style.pitch2);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
sch_circle_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(SchCircleClass),    /* class_size */
            NULL,                      /* base_init */
            NULL,                      /* base_finalize */
            sch_circle_class_init,     /* class_init */
            NULL,                      /* class_finalize */
            NULL,                      /* class_data */
            sizeof(SchCircle),         /* instance_size */
            0,                         /* n_preallocs */
            NULL,                      /* instance_init */
            NULL                       /* value_table */
            };

        type = g_type_register_static(SCH_TYPE_SHAPE, "SchCircle",&tinfo, 0);
    }

    return type;
}

static void
sch_circle_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec)
{
    SchCirclePrivate *privat = SCH_CIRCLE_GET_PRIVATE(object);

    if (privat != NULL)
    {
        switch (property_id)
        {
            case SCH_CIRCLE_X:
                privat->circle.center_x = g_value_get_int(value);
                break;

            case SCH_CIRCLE_Y:
                privat->circle.center_y = g_value_get_int(value);
                break;

            case SCH_CIRCLE_RADIUS:
                privat->circle.radius = g_value_get_int(value);
                break;

            case SCH_CIRCLE_COLOR:
                privat->color = g_value_get_int(value);
                break;

            case SCH_CIRCLE_LINE_WIDTH:
                privat->line_width = g_value_get_int(value);
                break;

            case SCH_CIRCLE_CAP_STYLE:
                privat->line_style.cap_style = g_value_get_int(value);
                break;

            case SCH_CIRCLE_DASH_STYLE:
                privat->line_style.dash_style = g_value_get_int(value);
                break;

           case SCH_CIRCLE_DASH_LENGTH:
                privat->line_style.dash_length = g_value_get_int(value);
                break;

            case SCH_CIRCLE_DASH_SPACE:
                privat->line_style.dash_space = g_value_get_int(value);
                break;

            case SCH_CIRCLE_FILL_TYPE:
                privat->fill_style.type = g_value_get_int(value);
                break;

            case SCH_CIRCLE_FILL_WIDTH:
                privat->fill_style.width = g_value_get_int(value);
                break;

            case SCH_CIRCLE_FILL_ANGLE1:
                privat->fill_style.angle1 = g_value_get_int(value);
                break;

            case SCH_CIRCLE_FILL_PITCH1:
                privat->fill_style.pitch1 = g_value_get_int(value);
                break;

            case SCH_CIRCLE_FILL_ANGLE2:
                privat->fill_style.angle2 = g_value_get_int(value);
                break;

            case SCH_CIRCLE_FILL_PITCH2:
                privat->fill_style.pitch2 = g_value_get_int(value);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

void
sch_circle_get_color(const SchCircle *shape, int *index)
{
    if (index != NULL)
    {
        SchCirclePrivate *privat = SCH_CIRCLE_GET_PRIVATE(shape);

        *index = SCH_CIRCLE_DEFAULT_COLOR;

        if (privat != NULL)
        {
            *index = privat->color;
        }
    }
}

void
sch_circle_get_circle(const SchCircle *shape, GeomCircle *circle)
{
    if (circle != NULL)
    {
        SchCirclePrivate *privat = SCH_CIRCLE_GET_PRIVATE(shape);

        if (privat != NULL)
        {
            *circle = privat->circle;
        }
        else
        {
            geom_circle_init(circle);
        }
    }
}

void
sch_circle_get_line_width(const SchCircle *shape, int *width)
{
    if (width != NULL)
    {
        SchCirclePrivate *privat = SCH_CIRCLE_GET_PRIVATE(shape);

        *width = SCH_CIRCLE_DEFAULT_LINE_WIDTH;

        if (privat != NULL)
        {
            *width = privat->line_width;
        }
    }
}

void
sch_circle_get_fill_style(const SchCircle *shape, struct _SchFillStyle *style)
{
    if (style != NULL)
    {
        SchCirclePrivate *privat = SCH_CIRCLE_GET_PRIVATE(shape);

        if (privat != NULL)
        {
            *style = privat->fill_style;
        }
        else
        {
            sch_fill_style_init(style);
        }
    }
}

void
sch_circle_get_line_style(const SchCircle *shape, struct _SchLineStyle *style)
{
    if (style != NULL)
    {
        SchCirclePrivate *privat = SCH_CIRCLE_GET_PRIVATE(shape);

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

SchCircle*
sch_circle_new(const SchConfig *config)
{
    SchFillStyle fill_style;
    SchLineStyle line_style;

    sch_config_get_fill_style(config, &fill_style);
    sch_config_get_line_style(config, &line_style);

    return SCH_CIRCLE(g_object_new(
        SCH_TYPE_CIRCLE,
        "color",            sch_config_get_graphic_color(config),
        "line-width",       sch_config_get_line_width(config),
        "line-cap-style",   line_style.cap_style,
        "line-dash-style",  line_style.dash_style,
        "line-dash-length", line_style.dash_length,
        "line-dash-space",  line_style.dash_space,
        "fill-type",        fill_style.type,
        "fill-width",       fill_style.width,
        "fill-angle1",      fill_style.angle1,
        "fill-pitch1",      fill_style.pitch1,
        "fill-angle2",      fill_style.angle2,
        "fill-pitch2",      fill_style.pitch2,
        NULL
        ));
}

static void
sch_circle_rotate(SchShape *shape, int angle)
{
    SchCirclePrivate *privat = SCH_CIRCLE_GET_PRIVATE(shape);

    if (privat != NULL) {
        geom_circle_rotate(&(privat->circle), angle);
    }
}

static void
sch_circle_transform(SchShape *shape, const GeomTransform *transform)
{
    SchCirclePrivate *privat = SCH_CIRCLE_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        geom_transform_point(transform, &(privat->circle.center_x), &(privat->circle.center_y));
    }
}

void
sch_circle_translate(SchShape *shape, int dx, int dy)
{
    SchCirclePrivate *privat = SCH_CIRCLE_GET_PRIVATE(shape);

    if (privat != NULL) {
        geom_circle_translate(&(privat->circle), dx, dy);
    }
}

static void
sch_circle_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error)
{
    sch_file_format_2_write_circle(format, stream, SCH_CIRCLE(shape), error);
}

