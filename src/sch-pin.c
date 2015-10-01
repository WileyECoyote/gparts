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

/*! \file sch-pin.c
 */

#include <glib.h>
#include <glib-object.h>

#include "sch.h"


#define SCH_PIN_DEFAULT_COLOR 1

#define SCH_PIN_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCH_TYPE_PIN,SchPinPrivate))

enum
{
    SCH_PIN_X1 = 1,
    SCH_PIN_Y1,
    SCH_PIN_X2,
    SCH_PIN_Y2,
    SCH_PIN_COLOR,
    SCH_PIN_PIN_TYPE,
    SCH_PIN_PIN_END
};

typedef struct _SchPinPrivate SchPinPrivate;

struct _SchPinPrivate
{
    GeomLine line;
    int      color;
    int      pin_type;
    int      pin_end;
};

static void
sch_pin_class_init(void *g_class, void *g_class_data);

static void
sch_pin_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec);

static void
sch_pin_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec);

static void
sch_pin_rotate(SchShape *shape, int angle);

static void
sch_pin_transform(SchShape *shape, const GeomTransform *transform);

static void
sch_pin_translate(SchShape *shape, int dx, int dy);

static void
sch_pin_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error);


static void
sch_pin_class_init(void *g_class, void *g_class_data)
{
    GObjectClass *object_class = G_OBJECT_CLASS(g_class);
    SchPinClass *klasse = SCH_PIN_CLASS(g_class);

    g_type_class_add_private(object_class, sizeof(SchPinPrivate));

    object_class->get_property = sch_pin_get_property;
    object_class->set_property = sch_pin_set_property;

    klasse->parent.rotate    = sch_pin_rotate;
    klasse->parent.transform = sch_pin_transform;
    klasse->parent.translate = sch_pin_translate;
    klasse->parent.write     = sch_pin_write;

    g_object_class_install_property(
        object_class,
        SCH_PIN_X1,
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
        object_class,
        SCH_PIN_Y1,
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
        object_class,
        SCH_PIN_X2,
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
        object_class,
        SCH_PIN_Y2,
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
        object_class,
        SCH_PIN_COLOR,
        g_param_spec_int(
            "color",
            "Color",
            "Color",
            0,
            G_MAXINT,
            SCH_PIN_DEFAULT_COLOR,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_PIN_PIN_TYPE,
        g_param_spec_int(
            "pin-type",
            "Pin Type",
            "Pin Type",
            0,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_PIN_PIN_END,
        g_param_spec_int(
            "pin-end",
            "Pin End",
            "Pin End",
            0,
            2,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );
}

static void
sch_pin_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec)
{
    SchPinPrivate *privat = SCH_PIN_GET_PRIVATE(object);

    if (privat != NULL)
    {
        switch (property_id)
        {
            case SCH_PIN_X1:
                g_value_set_int(value, privat->line.x[0]);
                break;

            case SCH_PIN_Y1:
                g_value_set_int(value, privat->line.y[0]);
                break;

            case SCH_PIN_X2:
                g_value_set_int(value, privat->line.x[1]);
                break;

            case SCH_PIN_Y2:
                g_value_set_int(value, privat->line.y[1]);
                break;

            case SCH_PIN_COLOR:
                g_value_set_int(value, privat->color);
                break;

            case SCH_PIN_PIN_TYPE:
                g_value_set_int(value, privat->pin_type);
                break;

            case SCH_PIN_PIN_END:
                g_value_set_int(value, privat->pin_end);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
sch_pin_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(SchPinClass),    /* class_size */
            NULL,                   /* base_init */
            NULL,                   /* base_finalize */
            sch_pin_class_init,     /* class_init */
            NULL,                   /* class_finalize */
            NULL,                   /* class_data */
            sizeof(SchPin),         /* instance_size */
            0,                      /* n_preallocs */
            NULL,                   /* instance_init */
            NULL                    /* value_table */
            };

        type = g_type_register_static(SCH_TYPE_SHAPE, "SchPin", &tinfo, 0);
    }

    return type;
}

static void
sch_pin_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec)
{
    SchPinPrivate *privat = SCH_PIN_GET_PRIVATE(object);

    if (privat != NULL) {

        switch (property_id) {

            case SCH_PIN_X1:
                privat->line.x[0] = g_value_get_int(value);
                break;

            case SCH_PIN_Y1:
                privat->line.y[0] = g_value_get_int(value);
                break;

            case SCH_PIN_X2:
                privat->line.x[1] = g_value_get_int(value);
                break;

            case SCH_PIN_Y2:
                privat->line.y[1] = g_value_get_int(value);
                break;

            case SCH_PIN_COLOR:
                privat->color = g_value_get_int(value);
                break;

            case SCH_PIN_PIN_TYPE:
                privat->pin_type = g_value_get_int(value);
                break;

            case SCH_PIN_PIN_END:
                privat->pin_end = g_value_get_int(value);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

void
sch_pin_get_color(const SchPin *shape, int *index)
{
    if (index != NULL) {

        SchPinPrivate *privat = SCH_PIN_GET_PRIVATE(shape);

        *index = SCH_PIN_DEFAULT_COLOR;

        if (privat != NULL) {
            *index = privat->color;
        }
    }
}

void
sch_pin_get_line(const SchPin *shape, GeomLine *line)
{
    if (line != NULL) {

        SchPinPrivate *privat = SCH_PIN_GET_PRIVATE(shape);

        if (privat != NULL) {
            *line = privat->line;
        }
        else  {
            geom_line_init(line);
        }
    }
}

SchPin*
sch_pin_new(const SchConfig *config)
{
    return SCH_PIN(g_object_new(SCH_TYPE_PIN, NULL));
}

static void
sch_pin_rotate(SchShape *shape, int angle)
{
    SchPinPrivate *privat = SCH_PIN_GET_PRIVATE(shape);

    if (privat != NULL) {
        geom_line_rotate(&(privat->line), angle);
    }
}

static void
sch_pin_transform(SchShape *shape, const GeomTransform *transform)
{
    if (transform != NULL) {

        SchPinPrivate *privat = SCH_PIN_GET_PRIVATE(shape);

        if (privat != NULL) {

            geom_line_transform(&(privat->line), transform);
        }
    }
}


static void
sch_pin_translate(SchShape *shape, int dx, int dy)
{
    SchPinPrivate *privat = SCH_PIN_GET_PRIVATE(shape);

    if (privat != NULL) {
        geom_line_translate(&(privat->line), dx, dy);
    }
}

static void
sch_pin_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error)
{
    sch_file_format_2_write_pin(format, stream, SCH_PIN(shape), error);
}

