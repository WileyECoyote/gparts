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

/*! \file schgui-cairo-factory.c
 */

#include <math.h>

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "schgui.h"


#define SCHGUI_CAIRO_FACTORY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCHGUI_TYPE_CAIRO_FACTORY,SchGUICairoFactoryPrivate))

enum
{
    SCHGUI_CAIRO_FACTORY_CONFIG = 1,
};

typedef struct _SchGUICairoFactoryPrivate SchGUICairoFactoryPrivate;

struct _SchGUICairoFactoryPrivate
{
    SchGUIDrawingCfg *config;
};

struct _temp_context
{
    SchGUICairoFactory  *factory;
    SchGUICairoDrawList *draw_list;
};

static void
schgui_cairo_factory_class_init(gpointer g_class, gpointer g_class_data);

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_arc(SchGUICairoFactory *factory, SchArc *arc);

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_attributes(SchGUICairoFactory *factory, SchShape *shape);

static void
schgui_cairo_factory_create_from_attributes_proc(SchShape *shape, gpointer user_data);

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_box(SchGUICairoFactory *factory, SchBox *box);

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_bus(SchGUICairoFactory *factory, SchBus *bus);

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_circle(SchGUICairoFactory *factory, SchCircle *circle);

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_component(SchGUICairoFactory *factory, SchComponent *component);

static void
schgui_cairo_factory_create_from_drawing_proc(SchShape *shape, gpointer user_data);

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_line(SchGUICairoFactory *factory, SchLine *line);

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_net(SchGUICairoFactory *factory, SchNet *net);

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_path(SchGUICairoFactory *factory, SchPath *path);

/*
static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_path_proc(SchPathCommand *command, gpointer user_data);
*/

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_pin(SchGUICairoFactory *factory, SchPin *pin);

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_text(SchGUICairoFactory *factory, SchText *text);

static void
schgui_cairo_factory_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
schgui_cairo_factory_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);



static void
schgui_cairo_factory_base_finalize(gpointer g_class)
{
    SchGUICairoFactoryClass *klasse = SCHGUI_CAIRO_FACTORY_CLASS(g_class);

    g_hash_table_destroy(klasse->table);
    klasse->table = NULL;
}

static void
schgui_cairo_factory_base_init(gpointer g_class)
{
    SchGUICairoFactoryClass *klasse = SCHGUI_CAIRO_FACTORY_CLASS(g_class);

    klasse->table = g_hash_table_new(
        g_direct_hash,
        g_direct_equal
        );

    g_hash_table_insert(
        klasse->table,
        GINT_TO_POINTER(SCH_TYPE_ARC),
        schgui_cairo_factory_create_from_arc
        );

    g_hash_table_insert(
        klasse->table,
        GINT_TO_POINTER(SCH_TYPE_BOX),
        schgui_cairo_factory_create_from_box
        );

    g_hash_table_insert(
        klasse->table,
        GINT_TO_POINTER(SCH_TYPE_BUS),
        schgui_cairo_factory_create_from_bus
        );

    g_hash_table_insert(
        klasse->table,
        GINT_TO_POINTER(SCH_TYPE_CIRCLE),
        schgui_cairo_factory_create_from_circle
        );

    g_hash_table_insert(
        klasse->table,
        GINT_TO_POINTER(SCH_TYPE_COMPONENT),
        schgui_cairo_factory_create_from_component
        );

    g_hash_table_insert(
        klasse->table,
        GINT_TO_POINTER(SCH_TYPE_LINE),
        schgui_cairo_factory_create_from_line
        );

    g_hash_table_insert(
        klasse->table,
        GINT_TO_POINTER(SCH_TYPE_NET),
        schgui_cairo_factory_create_from_net
        );

    g_hash_table_insert(
        klasse->table,
        GINT_TO_POINTER(SCH_TYPE_PATH),
        schgui_cairo_factory_create_from_path
        );

    g_hash_table_insert(
        klasse->table,
        GINT_TO_POINTER(SCH_TYPE_PIN),
        schgui_cairo_factory_create_from_pin
        );

    g_hash_table_insert(
        klasse->table,
        GINT_TO_POINTER(SCH_TYPE_TEXT),
        schgui_cairo_factory_create_from_text
        );
}

static void
schgui_cairo_factory_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *klasse  = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(klasse, sizeof(SchGUICairoFactoryPrivate));

    klasse->get_property = schgui_cairo_factory_get_property;
    klasse->set_property = schgui_cairo_factory_set_property;

    g_object_class_install_property(
        klasse,
        SCHGUI_CAIRO_FACTORY_CONFIG,
        g_param_spec_object(
            "config",
            "Configuration",
            "Configuration",
            SCHGUI_TYPE_DRAWING_CFG,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );
}

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_arc(SchGUICairoFactory *factory, SchArc *shape)
{
    SchGUICairoDrawItem *item = NULL;
    SchGUICairoFactoryPrivate *privat = SCHGUI_CAIRO_FACTORY_GET_PRIVATE(factory);

    if (privat != NULL)
    {
        item = SCHGUI_CAIRO_DRAW_ITEM(schgui_cairo_arc_new(SCH_ARC(shape), privat->config));
    }

    return item;
}

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_attributes(SchGUICairoFactory *factory, SchShape *shape)
{
    SchAttributes *attributes = sch_shape_get_attributes(shape);
    struct _temp_context user_data;

    user_data.factory   = factory;
    user_data.draw_list = NULL;

    sch_attributes_foreach(attributes, (GFunc) schgui_cairo_factory_create_from_attributes_proc, &user_data);

    if (attributes != NULL)
    {
        g_object_unref(attributes);
    }

    return SCHGUI_CAIRO_DRAW_ITEM(user_data.draw_list);
}

static void
schgui_cairo_factory_create_from_attributes_proc(SchShape *shape, gpointer user_data)
{
    struct _temp_context *data = (struct _temp_context*) user_data;
    SchGUICairoDrawItem  *item;

    item = schgui_cairo_factory_create_from_shape(data->factory, shape);

    if (item != NULL)
    {
        if (data->draw_list == NULL)
        {
            data->draw_list = SCHGUI_CAIRO_DRAW_LIST(g_object_new(SCHGUI_TYPE_CAIRO_DRAW_LIST, NULL));
        }

        schgui_cairo_draw_list_append(data->draw_list, item);
    }
}


static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_box(SchGUICairoFactory *factory, SchBox *shape)
{
    SchGUICairoDrawItem *item = NULL;
    SchGUICairoFactoryPrivate *privat = SCHGUI_CAIRO_FACTORY_GET_PRIVATE(factory);

    if (privat != NULL)
    {
        item = SCHGUI_CAIRO_DRAW_ITEM(schgui_cairo_box_new(SCH_BOX(shape), privat->config));
    }

    return item;
}

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_bus(SchGUICairoFactory *factory, SchBus *shape)
{
    SchGUICairoDrawItem *item = NULL;
    SchGUICairoFactoryPrivate *privat = SCHGUI_CAIRO_FACTORY_GET_PRIVATE(factory);

    if (privat != NULL)
    {
        item = SCHGUI_CAIRO_DRAW_ITEM(schgui_cairo_bus_new(SCH_BUS(shape), privat->config));
    }

    return item;
}

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_circle(SchGUICairoFactory *factory, SchCircle *shape)
{
    SchGUICairoDrawItem *item = NULL;
    SchGUICairoFactoryPrivate *privat = SCHGUI_CAIRO_FACTORY_GET_PRIVATE(factory);

    if (privat != NULL)
    {
        item = SCHGUI_CAIRO_DRAW_ITEM(schgui_cairo_circle_new(SCH_CIRCLE(shape), privat->config));
    }

    return item;
}

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_component(SchGUICairoFactory *factory, SchComponent *component)
{
    SchGUICairoDrawList *draw_list = NULL;
    SchGUICairoFactoryPrivate *privat = SCHGUI_CAIRO_FACTORY_GET_PRIVATE(factory);

    if (privat != NULL)
    {
        int                 angle;
        SchDrawing          *drawing = sch_component_get_drawing(component);
        SchGUICairoDrawItem *draw_item;
        int                 dx;
        int                 dy;
        int                 mirror;

        sch_component_get_orientation(component, &angle, &mirror);
        sch_component_get_insertion_point(component, &dx, &dy);

        draw_item = schgui_cairo_factory_create_from_drawing(factory, drawing);

        if (drawing != NULL)
        {
            g_object_unref(drawing);
        }

        if (draw_item != NULL)
        {
            draw_list = SCHGUI_CAIRO_DRAW_LIST(g_object_new(SCHGUI_TYPE_CAIRO_DRAW_LIST, NULL));

            schgui_cairo_draw_list_append(draw_list, draw_item);

            g_object_unref(draw_item);
        }

        draw_item = schgui_cairo_factory_create_from_attributes(factory, SCH_SHAPE(component));

        if (draw_item != NULL)
        {
            if (draw_list == NULL)
            {
                draw_list = SCHGUI_CAIRO_DRAW_LIST(g_object_new(SCHGUI_TYPE_CAIRO_DRAW_LIST, NULL));
            }

            schgui_cairo_draw_list_append(draw_list, draw_item);

            g_object_unref(draw_item);
        }

        if (mirror)
        {
            schgui_cairo_draw_item_mirror_y(SCHGUI_CAIRO_DRAW_ITEM(draw_list));
        }

        schgui_cairo_draw_item_rotate(SCHGUI_CAIRO_DRAW_ITEM(draw_list), geom_angle_radians(angle));
        schgui_cairo_draw_item_translate(SCHGUI_CAIRO_DRAW_ITEM(draw_list), dx, dy);
    }

    return SCHGUI_CAIRO_DRAW_ITEM(draw_list);
}

SchGUICairoDrawItem*
schgui_cairo_factory_create_from_drawing(SchGUICairoFactory *factory, SchDrawing *drawing)
{
    struct _temp_context user_data;

    user_data.factory   = factory;
    user_data.draw_list = NULL;

    sch_drawing_foreach(drawing, (GFunc) schgui_cairo_factory_create_from_drawing_proc, &user_data);

    return SCHGUI_CAIRO_DRAW_ITEM(user_data.draw_list);
}

static void
schgui_cairo_factory_create_from_drawing_proc(SchShape *shape, gpointer user_data)
{
    struct _temp_context *data = (struct _temp_context*) user_data;
    SchGUICairoDrawItem  *item;

    item = schgui_cairo_factory_create_from_shape(data->factory, shape);

    if (item != NULL)
    {
        if (data->draw_list == NULL)
        {
            data->draw_list = SCHGUI_CAIRO_DRAW_LIST(g_object_new(SCHGUI_TYPE_CAIRO_DRAW_LIST, NULL));
        }

        schgui_cairo_draw_list_append(data->draw_list, item);
    }
}

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_line(SchGUICairoFactory *factory, SchLine *shape)
{
    SchGUICairoDrawItem *item = NULL;
    SchGUICairoFactoryPrivate *privat = SCHGUI_CAIRO_FACTORY_GET_PRIVATE(factory);

    if (privat != NULL)
    {
        item = SCHGUI_CAIRO_DRAW_ITEM(schgui_cairo_line_new(SCH_LINE(shape), privat->config));
    }

    return item;
}

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_net(SchGUICairoFactory *factory, SchNet *shape)
{
    SchGUICairoDrawItem *item = NULL;
    SchGUICairoFactoryPrivate *privat = SCHGUI_CAIRO_FACTORY_GET_PRIVATE(factory);

    if (privat != NULL)
    {
        item = SCHGUI_CAIRO_DRAW_ITEM(schgui_cairo_net_new(SCH_NET(shape), privat->config));
    }

    return item;
}

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_path(SchGUICairoFactory *factory, SchPath *path)
{
    SchGUICairoDrawItem *item = NULL;
    SchGUICairoFactoryPrivate *privat = SCHGUI_CAIRO_FACTORY_GET_PRIVATE(factory);

    if (privat != NULL)
    {
        item = SCHGUI_CAIRO_DRAW_ITEM(schgui_cairo_path_new(SCH_PATH(path), privat->config));
    }

    return item;
}

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_pin(SchGUICairoFactory *factory, SchPin *shape)
{
    SchGUICairoDrawList *draw_list = NULL;
    SchGUICairoFactoryPrivate *privat = SCHGUI_CAIRO_FACTORY_GET_PRIVATE(factory);

    if (privat != NULL)
    {
        SchGUICairoDrawItem *draw_item;

        draw_item = SCHGUI_CAIRO_DRAW_ITEM(schgui_cairo_pin_new(SCH_PIN(shape), privat->config));

        if (draw_item != NULL)
        {
            draw_list = SCHGUI_CAIRO_DRAW_LIST(g_object_new(SCHGUI_TYPE_CAIRO_DRAW_LIST, NULL));

            schgui_cairo_draw_list_append(draw_list, draw_item);

            g_object_unref(draw_item);
        }

        draw_item = schgui_cairo_factory_create_from_attributes(factory, SCH_SHAPE(shape));

        if (draw_item != NULL)
        {
            if (draw_list == NULL)
            {
                draw_list = SCHGUI_CAIRO_DRAW_LIST(g_object_new(SCHGUI_TYPE_CAIRO_DRAW_LIST, NULL));
            }

            schgui_cairo_draw_list_append(draw_list, draw_item);

            g_object_unref(draw_item);
        }

    }

    return SCHGUI_CAIRO_DRAW_ITEM(draw_list);
}

SchGUICairoDrawItem*
schgui_cairo_factory_create_from_shape(SchGUICairoFactory *factory, SchShape *shape)
{
    SchGUICairoDrawItem *item = NULL;

    if (factory != NULL)
    {
        SchGUICairoFactoryClass *klasse = SCHGUI_CAIRO_FACTORY_GET_CLASS(factory);

        SchGUICairoDrawItem* (*func)(SchGUICairoFactory *factory, SchShape *shape);

        if (klasse != NULL)
        {
            func = g_hash_table_lookup(klasse->table, GINT_TO_POINTER(G_TYPE_FROM_INSTANCE(shape)));

            if (func != NULL)
            {
                item = func(factory, shape);
            }
        }
    }

    return item;
}

static SchGUICairoDrawItem*
schgui_cairo_factory_create_from_text(SchGUICairoFactory *factory, SchText *shape)
{
    SchGUICairoDrawItem *item = NULL;
    SchGUICairoFactoryPrivate *privat = SCHGUI_CAIRO_FACTORY_GET_PRIVATE(factory);

    if (privat != NULL)
    {
        item = SCHGUI_CAIRO_DRAW_ITEM(schgui_cairo_text_new(SCH_TEXT(shape), privat->config));
    }

    return item;
}

static void
schgui_cairo_factory_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    SchGUICairoFactoryPrivate *privat = SCHGUI_CAIRO_FACTORY_GET_PRIVATE(object);

    if (privat != NULL)
    {
        switch (property_id)
        {
            case SCHGUI_CAIRO_FACTORY_CONFIG:
                g_value_set_object(value, privat->config);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
schgui_cairo_factory_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(SchGUICairoFactoryClass),       /* class_size */
            schgui_cairo_factory_base_init,        /* base_init */
            schgui_cairo_factory_base_finalize,    /* base_finalize */
            schgui_cairo_factory_class_init,       /* class_init */
            NULL,                                  /* class_finalize */
            NULL,                                  /* class_data */
            sizeof(SchGUICairoFactory),            /* instance_size */
            0,                                     /* n_preallocs */
            NULL,                                  /* instance_init */
            NULL                                   /* value_table */
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "SchGUICairoFactory",
            &tinfo,
            0
            );
    }

    return type;
}

SchGUICairoFactory*
schgui_cairo_factory_new(SchGUIDrawingCfg *config)
{
    return SCHGUI_CAIRO_FACTORY(
        g_object_new(
            SCHGUI_TYPE_CAIRO_FACTORY,
            "config", config,
            NULL
            )
        );
}

void
schgui_cairo_factory_set_config(SchGUICairoFactory *factory, SchGUIDrawingCfg *config)
{
    SchGUICairoFactoryPrivate *privat = SCHGUI_CAIRO_FACTORY_GET_PRIVATE(factory);

    if (privat != NULL)
    {
        if (privat->config != NULL)
        {
            g_object_unref(privat->config);
        }

        privat->config = config;

        if (privat->config != NULL)
        {
            g_object_ref(privat->config);
        }
    }
}

static void
schgui_cairo_factory_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    switch (property_id)
    {
        case SCHGUI_CAIRO_FACTORY_CONFIG:
            schgui_cairo_factory_set_config(SCHGUI_CAIRO_FACTORY(object), g_value_get_object(value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

