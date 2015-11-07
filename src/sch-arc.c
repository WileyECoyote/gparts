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

/*! \file sch-arc.c
 */

#include <glib.h>
#include <glib-object.h>

#include "sch.h"

#define SCH_ARC_DEFAULT_COLOR       (SCH_CONFIG_DEFAULT_GRAPHIC_COLOR)
#define SCH_ARC_DEFAULT_LINE_WIDTH  (SCH_CONFIG_DEFAULT_LINE_WIDTH)

#define SCH_ARC_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCH_TYPE_ARC,SchArcPrivate))

enum
{
    SCH_ARC_ARC = 1,
    SCH_ARC_CENTER_X,
    SCH_ARC_CENTER_Y,
    SCH_ARC_RADIUS,
    SCH_ARC_START,
    SCH_ARC_SWEEP,
    SCH_ARC_COLOR,
    SCH_ARC_WIDTH,
    SCH_ARC_LINE_STYLE,
    SCH_ARC_CAP_STYLE,
    SCH_ARC_DASH_STYLE,
    SCH_ARC_DASH_LENGTH,
    SCH_ARC_DASH_SPACE
};

typedef struct _SchArcPrivate SchArcPrivate;

struct _SchArcPrivate
{
    GeomArc      arc;
    int          color;
    int          line_width;
    SchLineStyle line_style;
};

static void
sch_arc_class_init(void *g_class, void *g_class_data);

static void
sch_arc_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
sch_arc_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

void
sch_arc_rotate(SchShape *shape, int angle);

static void
sch_arc_transform(SchShape *shape, const GeomTransform *transform);

void
sch_arc_translate(SchShape *shape, int dx, int dy);

static void
sch_arc_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error);



static void
sch_arc_class_init(void *g_class, void *g_class_data)
{
  GObjectClass *object_class = G_OBJECT_CLASS(g_class);
  SchArcClass *klasse = SCH_ARC_CLASS(g_class);

  g_type_class_add_private(object_class, sizeof(SchArcPrivate));

  object_class->get_property = sch_arc_get_property;
  object_class->set_property = sch_arc_set_property;

  klasse->parent.rotate    = sch_arc_rotate;
  klasse->parent.transform = sch_arc_transform;
  klasse->parent.translate = sch_arc_translate;
  klasse->parent.write     = sch_arc_write;

  g_object_class_install_property(object_class, SCH_ARC_ARC,
                                  g_param_spec_boxed("arc",
                                                     "Arc",
                                                     "Arc",
                                                     GEOM_TYPE_ARC,
                                                     G_PARAM_LAX_VALIDATION |
                                                     G_PARAM_READWRITE |
                                                     G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(object_class, SCH_ARC_CENTER_X,
                                  g_param_spec_int("center-x",
                                                   "Center X",
                                                   "Center X",
                                                   G_MININT,
                                                   G_MAXINT,
                                                   0,
                                                   G_PARAM_LAX_VALIDATION |
                                                   G_PARAM_READWRITE |
                                                   G_PARAM_STATIC_STRINGS));


  g_object_class_install_property(object_class, SCH_ARC_CENTER_Y,
                                  g_param_spec_int("center-y",
                                                   "Center Y",
                                                   "Center Y",
                                                   G_MININT,
                                                   G_MAXINT,
                                                   0,
                                                   G_PARAM_LAX_VALIDATION |
                                                   G_PARAM_READWRITE |
                                                   G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(object_class, SCH_ARC_RADIUS,
                                  g_param_spec_int("radius",
                                                   "Radius",
                                                   "Radius",
                                                   G_MININT,
                                                   G_MAXINT,
                                                   0,
                                                   G_PARAM_LAX_VALIDATION |
                                                   G_PARAM_READWRITE |
                                                   G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(object_class, SCH_ARC_START,
                                  g_param_spec_int("start",
                                                   "Start",
                                                   "Start",
                                                   G_MININT,
                                                   G_MAXINT,
                                                   0,
                                                   G_PARAM_LAX_VALIDATION |
                                                   G_PARAM_READWRITE |
                                                   G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(object_class, SCH_ARC_SWEEP,
                                  g_param_spec_int("sweep",
                                                   "Sweep",
                                                   "Sweep",
                                                   G_MININT,
                                                   G_MAXINT,
                                                   0,
                                                   G_PARAM_LAX_VALIDATION |
                                                   G_PARAM_READWRITE |
                                                   G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(object_class, SCH_ARC_COLOR,
                                  g_param_spec_int("color",
                                                   "Color",
                                                   "Color",
                                                   0,
                                                   G_MAXINT,
                                                   SCH_ARC_DEFAULT_COLOR,
                                                   G_PARAM_LAX_VALIDATION |
                                                   G_PARAM_READWRITE |
                                                   G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(object_class, SCH_ARC_WIDTH,
                                  g_param_spec_int("line-width",
                                                   "Line Width",
                                                   "Line Width",
                                                   0,
                                                   G_MAXINT,
                                                   0,
                                                   G_PARAM_LAX_VALIDATION |
                                                   G_PARAM_READWRITE |
                                                   G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(object_class, SCH_ARC_LINE_STYLE,
                                  g_param_spec_boxed("line-style",
                                                     "Line Style",
                                                     "Line Style",
                                                     SCH_TYPE_LINE_STYLE,
                                                     G_PARAM_LAX_VALIDATION |
                                                     G_PARAM_READWRITE |
                                                     G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(object_class,
                                  SCH_ARC_CAP_STYLE,
                                  g_param_spec_int("line-cap-style",
                                                   "Line Cap Style",
                                                   "Line Cap Style",
                                                   0,
                                                   2,
                                                   0,
                                                   G_PARAM_LAX_VALIDATION |
                                                   G_PARAM_READWRITE |
                                                   G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(object_class,
                                  SCH_ARC_DASH_STYLE,
                                  g_param_spec_int("line-dash-style",
                                                   "Line dash Style",
                                                   "Line dash Style",
                                                   0,
                                                   4,
                                                   0,
                                                   G_PARAM_LAX_VALIDATION |
                                                   G_PARAM_READWRITE |
                                                   G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(object_class, SCH_ARC_DASH_LENGTH,
                                  g_param_spec_int("line-dash-length",
                                                   "Line Dash Length",
                                                   "Line Dash Length",
                                                   -1,
                                                   G_MAXINT,
                                                   -1,
                                                   G_PARAM_LAX_VALIDATION |
                                                   G_PARAM_READWRITE |
                                                   G_PARAM_STATIC_STRINGS));

  g_object_class_install_property(object_class, SCH_ARC_DASH_SPACE,
                                  g_param_spec_int("line-dash-space",
                                                   "Line Dash Space",
                                                   "Line Dash Space",
                                                   -1,
                                                   G_MAXINT,
                                                   -1,
                                                   G_PARAM_LAX_VALIDATION |
                                                   G_PARAM_READWRITE |
                                                   G_PARAM_STATIC_STRINGS));
}


int
sch_arc_get_center_x(const SchArc *shape)
{
    int center_x = 0;
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);

    if (privat != NULL) {

        center_x = privat->arc.center_x;
    }

    return center_x;
}


int
sch_arc_get_center_y(const SchArc *shape)
{
    int center_y = 0;
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);

    if (privat != NULL) {

        center_y = privat->arc.center_y;
    }

    return center_y;
}


int
sch_arc_get_radius(const SchArc *shape)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);
    int radius = 0;

    if (privat != NULL) {

        radius = privat->arc.radius;
    }

    return radius;
}


int
sch_arc_get_start(const SchArc *shape)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);
    int start = 0;

    if (privat != NULL) {

        start = privat->arc.start;
    }

    return start;
}


int
sch_arc_get_sweep(const SchArc *shape)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);
    int sweep = 0;

    if (privat != NULL) {

        sweep = privat->arc.sweep;
    }

    return sweep;
}


static void
sch_arc_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(object);

    if (privat != NULL) {

        switch (property_id) {

            case SCH_ARC_CENTER_X:
                g_value_set_int(value, privat->arc.center_x);
                break;

            case SCH_ARC_CENTER_Y:
                g_value_set_int(value, privat->arc.center_y);
                break;

            case SCH_ARC_RADIUS:
                g_value_set_int(value, privat->arc.radius);
                break;

            case SCH_ARC_START:
                g_value_set_int(value, privat->arc.start);
                break;

            case SCH_ARC_SWEEP:
                g_value_set_int(value, privat->arc.sweep);
                break;

            case SCH_ARC_COLOR:
                g_value_set_int(value, privat->color);
                break;

            case SCH_ARC_WIDTH:
                g_value_set_int(value, privat->line_width);
                break;

            case SCH_ARC_CAP_STYLE:
                g_value_set_int(value, privat->line_style.cap_style);
                break;

            case SCH_ARC_DASH_STYLE:
                g_value_set_int(value, privat->line_style.dash_style);
                break;

            case SCH_ARC_DASH_LENGTH:
                g_value_set_int(value, privat->line_style.dash_length);
                break;

            case SCH_ARC_DASH_SPACE:
                g_value_set_int(value, privat->line_style.dash_space);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}


GType
sch_arc_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(SchArcClass),    /* class_size */
            NULL,                   /* base_init */
            NULL,                   /* base_finalize */
            sch_arc_class_init,     /* class_init */
            NULL,                   /* class_finalize */
            NULL,                   /* class_data */
            sizeof(SchArc),         /* instance_size */
            0,                      /* n_preallocs */
            NULL,                   /* instance_init */
            NULL                    /* value_table */
            };

        type = g_type_register_static(SCH_TYPE_SHAPE,
                                      "SchArc",
                                      &tinfo, 0);
    }

    return type;
}


static void
sch_arc_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(object);

    if (privat != NULL) {

        switch (property_id) {

            case SCH_ARC_ARC:
                sch_arc_set_arc(SCH_ARC(object), g_value_get_boxed(value));
                break;

            case SCH_ARC_CENTER_X:
                sch_arc_set_center_x(SCH_ARC(object), g_value_get_int(value));
                break;

            case SCH_ARC_CENTER_Y:
                sch_arc_set_center_y(SCH_ARC(object), g_value_get_int(value));
                break;

            case SCH_ARC_RADIUS:
                sch_arc_set_radius(SCH_ARC(object), g_value_get_int(value));
                break;

            case SCH_ARC_START:
                sch_arc_set_start(SCH_ARC(object), g_value_get_int(value));
                break;

            case SCH_ARC_SWEEP:
                sch_arc_set_sweep(SCH_ARC(object), g_value_get_int(value));
                break;

            case SCH_ARC_COLOR:
                sch_arc_set_color(SCH_ARC(object), g_value_get_int(value));
                break;

            case SCH_ARC_WIDTH:
                sch_arc_set_line_width(SCH_ARC(object), g_value_get_int(value));
                break;

            case SCH_ARC_LINE_STYLE:
                sch_arc_set_line_style(SCH_ARC(object), g_value_get_boxed(value));
                break;

            case SCH_ARC_CAP_STYLE:
                privat->line_style.cap_style = g_value_get_int(value);
                break;

            case SCH_ARC_DASH_STYLE:
                privat->line_style.dash_style = g_value_get_int(value);
                break;

            case SCH_ARC_DASH_LENGTH:
                privat->line_style.dash_length = g_value_get_int(value);
                break;

            case SCH_ARC_DASH_SPACE:
                privat->line_style.dash_space = g_value_get_int(value);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}


int
sch_arc_get_color(const SchArc *shape)
{
    int color = SCH_ARC_DEFAULT_COLOR;
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);

    if (privat != NULL) {

        color = privat->color;
    }

    return color;
}


void
sch_arc_get_arc(const SchArc *shape, GeomArc *arc)
{
    if (arc != NULL) {

        SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);

        if (privat != NULL) {

            *arc = privat->arc;
        }
        else {

            geom_arc_init(arc);
        }
    }
}


int
sch_arc_get_line_width(const SchArc *shape)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);
    int width = SCH_ARC_DEFAULT_LINE_WIDTH;

    if (privat != NULL) {

        width = privat->line_width;
    }

    return width;
}


SchArc*
sch_arc_new(const SchConfig *config)
{
    SchLineStyle line_style;

    sch_config_get_line_style(config, &line_style);

    return SCH_ARC(g_object_new(SCH_TYPE_ARC,
        "color",            sch_config_get_graphic_color(config),
        "line-width",       sch_config_get_line_width(config),
        "line-cap-style",   line_style.cap_style,
        "line-dash-style",  line_style.dash_style,
        "line-dash-length", line_style.dash_length,
        "line-dash-space",  line_style.dash_space,
        NULL));
}


void
sch_arc_rotate(SchShape *shape, int angle)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);

    if (privat != NULL) {

        geom_arc_rotate(&(privat->arc), angle);

        g_object_notify(G_OBJECT(shape), "arc");
        g_object_notify(G_OBJECT(shape), "center-x");
        g_object_notify(G_OBJECT(shape), "center-y");
        g_object_notify(G_OBJECT(shape), "start");
    }
}


void
sch_arc_set_arc(SchArc *shape, const GeomArc *arc)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);

    if (privat != NULL) {

        privat->arc = *arc;

        g_object_notify(G_OBJECT(shape), "arc");
        g_object_notify(G_OBJECT(shape), "center-x");
        g_object_notify(G_OBJECT(shape), "center-y");
        g_object_notify(G_OBJECT(shape), "radius");
        g_object_notify(G_OBJECT(shape), "start");
        g_object_notify(G_OBJECT(shape), "sweep");
    }
}


void
sch_arc_set_center_x(SchArc *shape, int center_x)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);

    if (privat != NULL) {

        privat->arc.center_x = center_x;

        g_object_notify(G_OBJECT(shape), "arc");
        g_object_notify(G_OBJECT(shape), "center-x");
    }
}


void
sch_arc_set_center_y(SchArc *shape, int center_y)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);

    if (privat != NULL) {

        privat->arc.center_y = center_y;

        g_object_notify(G_OBJECT(shape), "arc");
        g_object_notify(G_OBJECT(shape), "center-y");
    }
}


void
sch_arc_set_color(SchArc *shape, int color)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);

    if (privat != NULL) {

        privat->color = color;

        g_object_notify(G_OBJECT(shape), "color");
    }
}


void
sch_arc_set_line_style(SchArc *shape, const SchLineStyle *style)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);

    if (privat != NULL) {

        privat->line_style = *style;

        g_object_notify(G_OBJECT(shape), "line-style");
        g_object_notify(G_OBJECT(shape), "cap-style");
        g_object_notify(G_OBJECT(shape), "dash-style");
        g_object_notify(G_OBJECT(shape), "dash-length");
        g_object_notify(G_OBJECT(shape), "dash-space");
    }
}


void
sch_arc_set_line_width(SchArc *shape, int line_width)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);

    if (privat != NULL) {

        privat->line_width = line_width;

        g_object_notify(G_OBJECT(shape), "line-width");
    }
}


void
sch_arc_set_radius(SchArc *shape, int radius)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);

    if (privat != NULL) {

        privat->arc.radius = radius;

        g_object_notify(G_OBJECT(shape), "arc");
        g_object_notify(G_OBJECT(shape), "radius");
    }
}


void
sch_arc_set_start(SchArc *shape, int start)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);

    if (privat != NULL) {

        privat->arc.start = start;

        g_object_notify(G_OBJECT(shape), "arc");
        g_object_notify(G_OBJECT(shape), "start");
    }

}


void
sch_arc_set_sweep(SchArc *shape, int sweep)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);

    if (privat != NULL) {

        privat->arc.sweep = sweep;

        g_object_notify(G_OBJECT(shape), "arc");
        g_object_notify(G_OBJECT(shape), "sweep");
    }
}


void
sch_arc_transform(SchShape *shape, const GeomTransform *transform)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);

    if (privat != NULL) {

        geom_arc_transform(&(privat->arc), transform);

        g_object_notify(G_OBJECT(shape), "arc");
        g_object_notify(G_OBJECT(shape), "center-x");
        g_object_notify(G_OBJECT(shape), "center-y");
        g_object_notify(G_OBJECT(shape), "radius");
        g_object_notify(G_OBJECT(shape), "start");
    }
}


void
sch_arc_translate(SchShape *shape, int dx, int dy)
{
    SchArcPrivate *privat = SCH_ARC_GET_PRIVATE(shape);

    if (privat != NULL) {

        geom_arc_translate(&(privat->arc), dx, dy);

        g_object_notify(G_OBJECT(shape), "arc");
        g_object_notify(G_OBJECT(shape), "center-x");
        g_object_notify(G_OBJECT(shape), "center-y");
    }
}


static void
sch_arc_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error)
{
    sch_file_format_2_write_arc(format, stream, SCH_ARC(shape), error);
}

