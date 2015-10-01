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

/*! \file sch-box.c
 */

#include <glib.h>
#include <glib-object.h>

#include "sch.h"


#define SCH_BOX_DEFAULT_COLOR      (SCH_CONFIG_DEFAULT_GRAPHIC_COLOR)
#define SCH_BOX_DEFAULT_LINE_WIDTH (SCH_CONFIG_DEFAULT_LINE_WIDTH)

#define SCH_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCH_TYPE_BOX,SchBoxPrivate))

enum
{
    SCH_BOX_X = 1,
    SCH_BOX_Y,
    SCH_BOX_WIDTH,
    SCH_BOX_HEIGHT,
    SCH_BOX_COLOR,
    SCH_BOX_LINE_WIDTH,
    SCH_BOX_CAP_STYLE,
    SCH_BOX_DASH_STYLE,
    SCH_BOX_DASH_LENGTH,
    SCH_BOX_DASH_SPACE,
    SCH_BOX_FILL_TYPE,
    SCH_BOX_FILL_WIDTH,
    SCH_BOX_FILL_ANGLE1,
    SCH_BOX_FILL_PITCH1,
    SCH_BOX_FILL_ANGLE2,
    SCH_BOX_FILL_PITCH2
};

typedef struct _SchBoxPrivate SchBoxPrivate;

struct _SchBoxPrivate
{
    GeomBox      box;
    gint         color;
    gint         line_width;
    SchLineStyle line_style;
    SchFillStyle fill_style;
};

static void
sch_box_class_init(gpointer g_class, gpointer g_class_data);

static void
sch_box_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

/*
static void
sch_box_schematic_shape_init(gpointer g_iface, gpointer g_iface_data);
*/

static void
sch_box_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void
sch_box_rotate(SchShape *shape, int rotate);

static void
sch_box_transform(SchShape *shape, const GeomTransform *transform);

static void
sch_box_translate(SchShape *shape, int dx, int dy);

static void
sch_box_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error);



static void
sch_box_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *object_class = G_OBJECT_CLASS(g_class);
    SchBoxClass *klasse = SCH_BOX_CLASS(g_class);

    g_type_class_add_private(object_class, sizeof(SchBoxPrivate));

    object_class->get_property = sch_box_get_property;
    object_class->set_property = sch_box_set_property;

    klasse->parent.rotate    = sch_box_rotate;
    klasse->parent.transform = sch_box_transform;
    klasse->parent.translate = sch_box_translate;
    klasse->parent.write     = sch_box_write;

    g_object_class_install_property(
        object_class,
        SCH_BOX_X,
        g_param_spec_int(
            "corner-x",
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
        SCH_BOX_Y,
        g_param_spec_int(
            "corner-y",
            "Y",
            "",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_BOX_WIDTH,
        g_param_spec_int(
            "width",
            "Width",
            "Width",
            0,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_BOX_HEIGHT,
        g_param_spec_int(
            "height",
            "Height",
            "Height",
            0,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_BOX_COLOR,
        g_param_spec_int(
            "color",
            "Color",
            "Color",
            0,
            G_MAXINT,
            SCH_BOX_DEFAULT_COLOR,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_BOX_LINE_WIDTH,
        g_param_spec_int(
            "line-width",
            "Line Width",
            "Line Width",
            0,
            G_MAXINT,
            SCH_BOX_DEFAULT_LINE_WIDTH,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_BOX_CAP_STYLE,
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
        object_class,
        SCH_BOX_DASH_STYLE,
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
        object_class,
        SCH_BOX_DASH_LENGTH,
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
        object_class,
        SCH_BOX_DASH_SPACE,
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

    g_object_class_install_property(
        object_class,
        SCH_BOX_FILL_TYPE,
        g_param_spec_int(
            "fill-type",
            "Fill Type",
            "Fill Type",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_BOX_FILL_WIDTH,
        g_param_spec_int(
            "fill-width",
            "Fill Width",
            "Fill Width",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_BOX_FILL_ANGLE1,
        g_param_spec_int(
            "fill-angle1",
            "Fill Angle 1",
            "Fill Angle 1",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_BOX_FILL_PITCH1,
        g_param_spec_int(
            "fill-pitch1",
            "Fill Pitch 1",
            "Fill Pitch 1",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_BOX_FILL_ANGLE2,
        g_param_spec_int(
            "fill-angle2",
            "Fill Angle 2",
            "Fill Angle 2",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_BOX_FILL_PITCH2,
        g_param_spec_int(
            "fill-pitch2",
            "Fill Pitch 2",
            "Fill Pitch 2",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );
}

static void
sch_box_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(object);

    if (privat != NULL)
    {
        switch (property_id)
        {
            case SCH_BOX_X:
                g_value_set_int(value, sch_box_get_corner_x(SCH_BOX(object)));
                break;

            case SCH_BOX_Y:
                g_value_set_int(value, sch_box_get_corner_y(SCH_BOX(object)));
                break;

            case SCH_BOX_WIDTH:
                g_value_set_int(value, sch_box_get_width(SCH_BOX(object)));
                break;

            case SCH_BOX_HEIGHT:
                g_value_set_int(value, sch_box_get_height(SCH_BOX(object)));
                break;

            case SCH_BOX_COLOR:
                g_value_set_int(value, sch_box_get_color(SCH_BOX(object)));
                break;

            case SCH_BOX_LINE_WIDTH:
                g_value_set_int(value, sch_box_get_line_width(SCH_BOX(object)));
                break;

            case SCH_BOX_CAP_STYLE:
                g_value_set_int(value, privat->line_style.cap_style);
                break;

            case SCH_BOX_DASH_STYLE:
                g_value_set_int(value, privat->line_style.dash_style);
                break;

            case SCH_BOX_DASH_LENGTH:
                g_value_set_int(value, privat->line_style.dash_length);
                break;

            case SCH_BOX_DASH_SPACE:
                g_value_set_int(value, privat->line_style.dash_space);
                break;

            case SCH_BOX_FILL_TYPE:
                g_value_set_int(value, privat->fill_style.type);
                break;

            case SCH_BOX_FILL_WIDTH:
                g_value_set_int(value, privat->fill_style.width);
                break;

            case SCH_BOX_FILL_ANGLE1:
                g_value_set_int(value, privat->fill_style.angle1);
                break;

            case SCH_BOX_FILL_PITCH1:
                g_value_set_int(value, privat->fill_style.pitch1);
                break;

            case SCH_BOX_FILL_ANGLE2:
                g_value_set_int(value, privat->fill_style.angle2);
                break;

            case SCH_BOX_FILL_PITCH2:
                g_value_set_int(value, privat->fill_style.pitch2);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
sch_box_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(SchBoxClass),    /* class_size */
            NULL,                   /* base_init */
            NULL,                   /* base_finalize */
            sch_box_class_init,     /* class_init */
            NULL,                   /* class_finalize */
            NULL,                   /* class_data */
            sizeof(SchBox),         /* instance_size */
            0,                      /* n_preallocs */
            NULL,                   /* instance_init */
            NULL                    /* value_table */
            };

        type = g_type_register_static(
            SCH_TYPE_SHAPE,
            "SchBox",
            &tinfo,
            0
            );
    }

    return type;
}

static void
sch_box_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(object);

    if (privat != NULL)
    {
        switch (property_id)
        {
            case SCH_BOX_X:
                sch_box_set_corner_x(SCH_BOX(object), g_value_get_int(value));
                break;

            case SCH_BOX_Y:
                sch_box_set_corner_y(SCH_BOX(object), g_value_get_int(value));
                break;

            case SCH_BOX_WIDTH:
                sch_box_set_width(SCH_BOX(object), g_value_get_int(value));
                break;

            case SCH_BOX_HEIGHT:
                sch_box_set_height(SCH_BOX(object), g_value_get_int(value));
                break;

            case SCH_BOX_COLOR:
                sch_box_set_color(SCH_BOX(object), g_value_get_int(value));
                break;

            case SCH_BOX_LINE_WIDTH:
                sch_box_set_line_width(SCH_BOX(object), g_value_get_int(value));
                break;

            case SCH_BOX_CAP_STYLE:
                privat->line_style.cap_style = g_value_get_int(value);
                break;

            case SCH_BOX_DASH_STYLE:
                privat->line_style.dash_style = g_value_get_int(value);
                break;

            case SCH_BOX_DASH_LENGTH:
                privat->line_style.dash_length = g_value_get_int(value);
                break;

            case SCH_BOX_DASH_SPACE:
                privat->line_style.dash_space = g_value_get_int(value);
                break;

            case SCH_BOX_FILL_TYPE:
                privat->fill_style.type = g_value_get_int(value);
                break;

            case SCH_BOX_FILL_WIDTH:
                privat->fill_style.width = g_value_get_int(value);
                break;

            case SCH_BOX_FILL_ANGLE1:
                privat->fill_style.angle1 = g_value_get_int(value);
                break;

            case SCH_BOX_FILL_PITCH1:
                privat->fill_style.pitch1 = g_value_get_int(value);
                break;

            case SCH_BOX_FILL_ANGLE2:
                privat->fill_style.angle2 = g_value_get_int(value);
                break;

            case SCH_BOX_FILL_PITCH2:
                privat->fill_style.pitch2 = g_value_get_int(value);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

gint
sch_box_get_corner_x(const SchBox *shape)
{
    gint corner_x = 0;
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        corner_x = privat->box.corner_x;
    }

    return corner_x;
}

gint
sch_box_get_corner_y(const SchBox *shape)
{
    gint corner_y = 0;
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        corner_y = privat->box.corner_y;
    }

    return corner_y;
}

gint
sch_box_get_width(const SchBox *shape)
{
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);
    gint width = 0;

    if (privat != NULL)
    {
        width = privat->box.width;
    }

    return width;
}

gint
sch_box_get_height(const SchBox *shape)
{
    gint height = 0;
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        height = privat->box.height;
    }

    return height;
}

gint
sch_box_get_color(const SchBox *shape)
{
    gint color = SCH_BOX_DEFAULT_COLOR;
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        color = privat->color;
    }

    return color;
}

void
sch_box_get_box(const SchBox *shape, GeomBox *box)
{
    if (box != NULL)
    {
        SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);

        if (privat != NULL)
        {
            *box = privat->box;
        }
        else
        {
            geom_box_init(box);
        }
    }
}

gint
sch_box_get_line_width(const SchBox *shape)
{
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);
    gint line_width = SCH_BOX_DEFAULT_LINE_WIDTH;

    if (privat != NULL)
    {
        line_width = privat->line_width;
    }

    return line_width;
}

SchFillStyle*
sch_box_get_fill_style(const SchBox *shape)
{
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);
    SchFillStyle *style = NULL;

    if (privat != NULL)
    {
        style = sch_fill_style_copy(&(privat->fill_style));
    }
    else
    {
        style = sch_fill_style_new();
    }

    return style;
}

SchLineStyle*
sch_box_get_line_style(const SchBox *shape)
{
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);
    SchLineStyle *style = NULL;

    if (privat != NULL)
    {
        style = sch_line_style_copy(&(privat->line_style));
    }
    else
    {
        style = sch_line_style_new();
    }

    return style;
}

SchBox*
sch_box_new(const SchConfig *config)
{
    SchFillStyle fill_style;
    SchLineStyle line_style;

    sch_config_get_fill_style(config, &fill_style);
    sch_config_get_line_style(config, &line_style);

    return SCH_BOX(g_object_new(
        SCH_TYPE_BOX,
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
sch_box_translate(SchShape *shape, int dx, int dy)
{
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        geom_box_translate(&(privat->box), dx, dy);
    }
}

static void
sch_box_transform(SchShape *shape, const GeomTransform *transform)
{
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        geom_transform_point(transform, &(privat->box.corner_x), &(privat->box.corner_y));
    }
}

static void
sch_box_rotate(SchShape *shape, int angle)
{
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        angle = geom_angle_normalize(angle);

        if (angle == 90)
        {
            GeomBox temp = privat->box;

            privat->box.corner_x = temp.corner_x - temp.height;
            privat->box.corner_y = temp.corner_y;
            privat->box.width    = temp.height;
            privat->box.height   = temp.width;
        }
        else if (angle == 270)
        {
            GeomBox temp = privat->box;

            privat->box.corner_x =  temp.corner_y;
            privat->box.corner_y = -temp.corner_x - temp.width;
            privat->box.width    =  temp.height;
            privat->box.height   =  temp.width;
        }

    }
}

static void
sch_box_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error)
{
    sch_file_format_2_write_box(format, stream, SCH_BOX(shape), error);
}

void
sch_box_set_corner_x(SchBox *shape, gint corner_x)
{
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->box.corner_x = corner_x;

        g_object_notify(G_OBJECT(shape), "corner-x");
    }
}

void
sch_box_set_corner_y(SchBox *shape, gint corner_y)
{
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->box.corner_y = corner_y;

        g_object_notify(G_OBJECT(shape), "corner-y");
    }
}

void
sch_box_set_width(SchBox *shape, gint width)
{
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->box.width = width;

        g_object_notify(G_OBJECT(shape), "width");
    }
}

void
sch_box_set_height(SchBox *shape, gint height)
{
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->box.height = height;

        g_object_notify(G_OBJECT(shape), "height");
    }
}

void
sch_box_set_color(SchBox *shape, gint color)
{
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->color = color;

        g_object_notify(G_OBJECT(shape), "color");
    }
}

void
sch_box_set_line_width(SchBox *shape, gint line_width)
{
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->line_width = line_width;

        g_object_notify(G_OBJECT(shape), "line-width");
    }
}

void
sch_box_set_line_style(SchBox *shape, const SchLineStyle *style)
{
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->line_style = *style;

        g_object_notify(G_OBJECT(shape), "line-style");
    }
}

void
sch_box_set_fill_style(SchBox *shape, const SchFillStyle *style)
{
    SchBoxPrivate *privat = SCH_BOX_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->fill_style = *style;

        g_object_notify(G_OBJECT(shape), "fill-style");
    }
}

