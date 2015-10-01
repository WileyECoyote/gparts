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

/*! \file sch-net.c
 */

#include <glib.h>
#include <glib-object.h>

#include "sch.h"


#define SCH_NET_DEFAULT_COLOR (SCH_CONFIG_DEFAULT_NET_COLOR)

#define SCH_NET_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCH_TYPE_NET,SchNetPrivate))

enum
{
    SCH_NET_X1 = 1,
    SCH_NET_Y1,
    SCH_NET_X2,
    SCH_NET_Y2,
    SCH_NET_COLOR,
    SCH_NET_NET_TYPE,
    SCH_NET_NET_END
};

typedef struct _SchNetPrivate SchNetPrivate;

struct _SchNetPrivate
{
    GeomLine line;
    int      color;
    int      net_type;
    int      net_end;
};

static void
sch_net_class_init(void *g_class, void *g_class_data);

static void
sch_net_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec);

static void
sch_net_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec);

static void
sch_net_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error);


static void
sch_net_class_init(void *g_class, void *g_class_data)
{
    GObjectClass *object_class = G_OBJECT_CLASS(g_class);
    SchNetClass *klasse = SCH_NET_CLASS(g_class);

    g_type_class_add_private(object_class, sizeof(SchNetPrivate));

    object_class->get_property = sch_net_get_property;
    object_class->set_property = sch_net_set_property;

    klasse->parent.write  = sch_net_write;

    g_object_class_install_property(
        object_class,
        SCH_NET_X1,
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
        SCH_NET_Y1,
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
        SCH_NET_X2,
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
        SCH_NET_Y2,
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
        SCH_NET_COLOR,
        g_param_spec_int(
            "color",
            "Color",
            "Color",
            0,
            G_MAXINT,
            SCH_NET_DEFAULT_COLOR,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_NET_NET_TYPE,
        g_param_spec_int(
            "net-type",
            "Net Type",
            "Net Type",
            0,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_NET_NET_END,
        g_param_spec_int(
            "net-end",
            "Net End",
            "Net End",
            0,
            2,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );
}

static void
sch_net_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec)
{
    SchNetPrivate *privat = SCH_NET_GET_PRIVATE(object);

    if (privat != NULL) {

        switch (property_id) {

            case SCH_NET_X1:
                g_value_set_int(value, privat->line.x[0]);
                break;

            case SCH_NET_Y1:
                g_value_set_int(value, privat->line.y[0]);
                break;

            case SCH_NET_X2:
                g_value_set_int(value, privat->line.x[1]);
                break;

            case SCH_NET_Y2:
                g_value_set_int(value, privat->line.y[1]);
                break;

            case SCH_NET_COLOR:
                g_value_set_int(value, privat->color);
                break;

            case SCH_NET_NET_TYPE:
                g_value_set_int(value, privat->net_type);
                break;

            case SCH_NET_NET_END:
                g_value_set_int(value, privat->net_end);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
sch_net_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(SchNetClass),    /* class_size */
            NULL,                   /* base_init */
            NULL,                   /* base_finalize */
            sch_net_class_init,     /* class_init */
            NULL,                   /* class_finalize */
            NULL,                   /* class_data */
            sizeof(SchNet),         /* instance_size */
            0,                      /* n_preallocs */
            NULL,                   /* instance_init */
            NULL                    /* value_table */
            };

        type = g_type_register_static(
            SCH_TYPE_SHAPE,
            "SchNet",
            &tinfo,
            0
            );
    }

    return type;
}

static void
sch_net_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec)
{
    SchNetPrivate *privat = SCH_NET_GET_PRIVATE(object);

    if (privat != NULL) {

        switch (property_id) {

            case SCH_NET_X1:
                privat->line.x[0] = g_value_get_int(value);
                break;

            case SCH_NET_Y1:
                privat->line.y[0] = g_value_get_int(value);
                break;

            case SCH_NET_X2:
                privat->line.x[1] = g_value_get_int(value);
                break;

            case SCH_NET_Y2:
                privat->line.y[1] = g_value_get_int(value);
                break;

            case SCH_NET_COLOR:
                privat->color = g_value_get_int(value);
                break;

            case SCH_NET_NET_TYPE:
                privat->net_type = g_value_get_int(value);
                break;

            case SCH_NET_NET_END:
                privat->net_end = g_value_get_int(value);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

void
sch_net_get_color(const SchNet *shape, int *index)
{
    if (index != NULL) {

        SchNetPrivate *privat = SCH_NET_GET_PRIVATE(shape);

        *index = SCH_NET_DEFAULT_COLOR;

        if (privat != NULL)
        {
            *index = privat->color;
        }
    }
}

void
sch_net_get_line(const SchNet *shape, GeomLine *line)
{
    if (line != NULL)
    {
        SchNetPrivate *privat = SCH_NET_GET_PRIVATE(shape);

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

SchNet*
sch_net_new(const SchConfig *config)
{
    return SCH_NET(g_object_new(
        SCH_TYPE_NET,
        "color", sch_config_get_net_color(config),
        NULL
        ));
}

void
sch_net_set_x0(SchNet *shape, int  x0)
{
    SchNetPrivate *privat = SCH_NET_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->line.x[0] = x0;

        g_object_notify(G_OBJECT(shape), "x0");
    }
}

void
sch_net_set_x1(SchNet *shape, int  x1)
{
    SchNetPrivate *privat = SCH_NET_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->line.x[1] = x1;

        g_object_notify(G_OBJECT(shape), "x1");
    }
}

void
sch_net_set_y0(SchNet *shape, int  y0)
{
    SchNetPrivate *privat = SCH_NET_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->line.y[0] = y0;

        g_object_notify(G_OBJECT(shape), "y0");
    }
}

void
sch_net_set_y1(SchNet *shape, int  y1)
{
    SchNetPrivate *privat = SCH_NET_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->line.y[1] = y1;

        g_object_notify(G_OBJECT(shape), "y1");
    }
}

static void
sch_net_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error)
{
    sch_file_format_2_write_net(format, stream, SCH_NET(shape), error);
}

