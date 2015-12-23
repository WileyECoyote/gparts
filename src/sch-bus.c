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

/*! \file sch-bus.c
 */

#include <glib.h>
#include <glib-object.h>

#include "sch.h"

#define SCH_BUS_DEFAULT_COLOR (SCH_CONFIG_DEFAULT_BUS_COLOR)

#define SCH_BUS_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCH_TYPE_BUS,SchBusPrivate))

enum
{
    SCH_BUS_X1 = 1,
    SCH_BUS_Y1,
    SCH_BUS_X2,
    SCH_BUS_Y2,
    SCH_BUS_COLOR,
    SCH_BUS_RIPPER_DIR
};

typedef struct _SchBusPrivate SchBusPrivate;

struct _SchBusPrivate
{
    GeomLine line;
    int     color;
    int     ripper_dir;
};

static void
sch_bus_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec);

static void
sch_bus_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec);

static void
sch_bus_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error);



static void
sch_bus_class_init(void *g_class, void *g_class_data)
{
    GObjectClass *object_class = G_OBJECT_CLASS(g_class);
    SchBusClass *klasse = SCH_BUS_CLASS(g_class);

    g_type_class_add_private(object_class, sizeof(SchBusPrivate));

    object_class->get_property = sch_bus_get_property;
    object_class->set_property = sch_bus_set_property;

    klasse->parent.write  = sch_bus_write;

    g_object_class_install_property(object_class, SCH_BUS_X1,
        g_param_spec_int(
            "x1",
            "X1",
            "X1",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(object_class, SCH_BUS_Y1,
        g_param_spec_int(
            "y1",
            "Y1",
            "Y1",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(object_class, SCH_BUS_X2,
        g_param_spec_int(
            "x2",
            "X2",
            "X2",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(object_class, SCH_BUS_Y2,
        g_param_spec_int(
            "y2",
            "Y2",
            "Y2",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(object_class, SCH_BUS_COLOR,
        g_param_spec_int(
            "color",
            "Color",
            "Color",
            0,
            G_MAXINT,
            SCH_BUS_DEFAULT_COLOR,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(object_class, SCH_BUS_RIPPER_DIR,
        g_param_spec_int(
            "ripper-dir",
            "Ripper Direction",
            "Ripper Direction",
            0,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
sch_bus_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec)
{
    SchBusPrivate *privat = SCH_BUS_GET_PRIVATE(object);

    if (privat != NULL) {

        switch (property_id) {

            case SCH_BUS_X1:
                g_value_set_int(value, privat->line.x[0]);
                break;

            case SCH_BUS_Y1:
                g_value_set_int(value, privat->line.y[0]);
                break;

            case SCH_BUS_X2:
                g_value_set_int(value, privat->line.x[1]);
                break;

            case SCH_BUS_Y2:
                g_value_set_int(value, privat->line.y[1]);
                break;

            case SCH_BUS_COLOR:
                g_value_set_int(value, privat->color);
                break;

            case SCH_BUS_RIPPER_DIR:
                g_value_set_int(value, privat->ripper_dir);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
sch_bus_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(SchBusClass),    /* class_size */
            NULL,                   /* base_init */
            NULL,                   /* base_finalize */
            sch_bus_class_init,     /* class_init */
            NULL,                   /* class_finalize */
            NULL,                   /* class_data */
            sizeof(SchBus),         /* instance_size */
            0,                      /* n_preallocs */
            NULL,                   /* instance_init */
            NULL                    /* value_table */
            };

        type = g_type_register_static(
            SCH_TYPE_SHAPE,
            "SchBus",
            &tinfo,
            0);
    }

    return type;
}

static void
sch_bus_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec)
{
    SchBusPrivate *privat = SCH_BUS_GET_PRIVATE(object);

    if (privat != NULL) {

        switch (property_id) {

            case SCH_BUS_X1:
                privat->line.x[0] = g_value_get_int(value);
                break;

            case SCH_BUS_Y1:
                privat->line.y[0] = g_value_get_int(value);
                break;

            case SCH_BUS_X2:
                privat->line.x[1] = g_value_get_int(value);
                break;

            case SCH_BUS_Y2:
                privat->line.y[1] = g_value_get_int(value);
                break;

            case SCH_BUS_COLOR:
                privat->color = g_value_get_int(value);
                break;

            case SCH_BUS_RIPPER_DIR:
                privat->ripper_dir = g_value_get_int(value);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

int
sch_bus_get_color(const SchBus *shape)
{
    int index = SCH_BUS_DEFAULT_COLOR;
    SchBusPrivate *privat = SCH_BUS_GET_PRIVATE(shape);

    if (privat != NULL) {

        index = privat->color;
    }

    return index;
}

void
sch_bus_get_line(const SchBus *shape, GeomLine *line)
{
    if (line != NULL) {

        SchBusPrivate *privat = SCH_BUS_GET_PRIVATE(shape);

        if (privat != NULL) {

            *line = privat->line;
        }
        else {

            geom_line_init(line);
        }
    }
}

SchBus*
sch_bus_new(const SchConfig *config)
{
    return SCH_BUS(g_object_new(
        SCH_TYPE_BUS,
        "color", sch_config_get_bus_color(config),
        NULL
        ));
}

void
sch_bus_set_line(SchBus *shape, const GeomLine *line)
{
    SchBusPrivate *privat = SCH_BUS_GET_PRIVATE(shape);

    if (privat != NULL) {

        privat->line = *line;

        g_object_notify(G_OBJECT(shape), "x0");
        g_object_notify(G_OBJECT(shape), "x1");
        g_object_notify(G_OBJECT(shape), "y0");
        g_object_notify(G_OBJECT(shape), "y1");
    }
}

void
sch_bus_set_x0(SchBus *shape, int x0)
{
    SchBusPrivate *privat = SCH_BUS_GET_PRIVATE(shape);

    if (privat != NULL) {

        privat->line.x[0] = x0;

        g_object_notify(G_OBJECT(shape), "x0");
    }
}

void
sch_bus_set_x1(SchBus *shape, int x1)
{
    SchBusPrivate *privat = SCH_BUS_GET_PRIVATE(shape);

    if (privat != NULL) {

        privat->line.x[1] = x1;

        g_object_notify(G_OBJECT(shape), "x1");
    }
}

void
sch_bus_set_y0(SchBus *shape, int y0)
{
    SchBusPrivate *privat = SCH_BUS_GET_PRIVATE(shape);

    if (privat != NULL) {

        privat->line.y[0] = y0;

        g_object_notify(G_OBJECT(shape), "y0");
    }
}

void
sch_bus_set_y1(SchBus *shape, int y1)
{
    SchBusPrivate *privat = SCH_BUS_GET_PRIVATE(shape);

    if (privat != NULL)  {

        privat->line.y[1] = y1;

        g_object_notify(G_OBJECT(shape), "y1");
    }
}

static void
sch_bus_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error)
{
    sch_file_format_2_write_bus(format, stream, SCH_BUS(shape), error);
}
