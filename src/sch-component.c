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

/*! \file sch-component.c
 */

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

#include "sch.h"

#define SCH_COMPONENT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCH_TYPE_COMPONENT,SchComponentPrivate))

enum
{
    SCH_COMPONENT_X = 1,
    SCH_COMPONENT_Y,
    SCH_COMPONENT_SELECTABLE,
    SCH_COMPONENT_ANGLE,
    SCH_COMPONENT_MIRROR,
    SCH_COMPONENT_BASENAME,
    SCH_COMPONENT_DRAWING
};

typedef struct _SchComponentInstantiateProcData SchComponentInstantiateProcData;
typedef struct _SchComponentPrivate SchComponentPrivate;

struct _SchComponentPrivate
{
    int        x;
    int        y;
    int        selectable;
    int        angle;
    int        mirror;
    SchDrawing *drawing;
};

struct _SchComponentInstantiateProcData
{
    SchAttributes   *attributes;
    SchComponent    *component;
    const SchConfig *config;
};

static void
sch_component_class_init(gpointer g_class, gpointer g_class_data);

static void
sch_component_expand_macros(SchShape *shape, const GRegex *regex, const GHashTable *table);

static void
sch_component_find_macros(const SchShape *shape, const GRegex *regex, GHashTable *table);

static void
sch_component_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
sch_component_instantiate_proc(SchShape *shape, gpointer user_data);

static void
sch_component_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void
sch_component_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error);


static void
sch_component_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *object_class = G_OBJECT_CLASS(g_class);
    SchComponentClass *klasse = SCH_COMPONENT_CLASS(g_class);

    g_type_class_add_private(object_class, sizeof(SchComponentPrivate));

    object_class->get_property = sch_component_get_property;
    object_class->set_property = sch_component_set_property;

    klasse->parent.find_macros   = sch_component_find_macros;
    klasse->parent.expand_macros = sch_component_expand_macros;
    //klasse->parent.rotate    = sch_line_rotate;
    //klasse->parent.transform = sch_line_transform;
    //klasse->parent.translate = sch_line_translate;
    klasse->parent.write         = sch_component_write;


    g_object_class_install_property(
        object_class,
        SCH_COMPONENT_X,
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
        SCH_COMPONENT_Y,
        g_param_spec_int(
            "y",
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
        SCH_COMPONENT_SELECTABLE,
        g_param_spec_int(
            "selectable",
            "Selectable",
            "",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_COMPONENT_ANGLE,
        g_param_spec_int(
            "angle",
            "Angle",
            "",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_COMPONENT_MIRROR,
        g_param_spec_int(
            "mirror",
            "Mirror",
            "",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_COMPONENT_BASENAME,
        g_param_spec_string(
            "basename",
            "Basename",
            "",
            NULL,
            G_PARAM_LAX_VALIDATION | G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_COMPONENT_DRAWING,
        g_param_spec_object(
            "drawing",
            "Drawing",
            "",
            SCH_TYPE_DRAWING,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );
}

static void
sch_component_expand_macros(SchShape *shape, const GRegex *regex, const GHashTable *table)
{
    SchShapeClass *klasse = SCH_SHAPE_GET_CLASS(shape);

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
sch_component_find_macros(const SchShape *shape, const GRegex *regex, GHashTable *table)
{
    SchShapeClass *klasse = SCH_SHAPE_GET_CLASS(shape);

    if (klasse != NULL) {

        SchShapeClass *parent_klasse = SCH_SHAPE_CLASS(g_type_class_peek_parent(klasse));

        if ((parent_klasse != NULL) && (parent_klasse->expand_macros != NULL))
        {
            parent_klasse->find_macros(shape, regex, table);
        }
    }
}

int
sch_component_get_angle(const SchComponent *shape)
{
    int angle = 0;
    SchComponentPrivate *privat = SCH_COMPONENT_GET_PRIVATE(shape);

    if (privat != NULL) {
        angle = privat->angle;
    }

    return angle;
}

char*
sch_component_get_basename(const SchComponent *component)
{
    char *basename = NULL;
    SchComponentPrivate *privat = SCH_COMPONENT_GET_PRIVATE(component);

    if (privat != NULL) {

        if (privat->drawing != NULL) {

            basename = sch_drawing_get_basename(privat->drawing);
        }
    }

    return basename;
}

SchDrawing*
sch_component_get_drawing(const SchComponent *shape)
{
    SchDrawing *drawing = NULL;
    SchComponentPrivate *privat = SCH_COMPONENT_GET_PRIVATE(shape);

    if (privat != NULL) {

        drawing = privat->drawing;

        if (drawing != NULL) {

            g_object_ref(drawing);
        }
    }

    return drawing;
}

void
sch_component_get_insertion_point(const SchComponent *shape, int *x, int *y)
{
    SchComponentPrivate *privat = SCH_COMPONENT_GET_PRIVATE(shape);

    if (privat != NULL) {

        if (x != NULL) {

            *x = privat->x;
        }

        if (y != NULL) {

            *y = privat->y;
        }
    }
    else {

        if (x != NULL) {

            *x = 0;
        }

        if (y != NULL)
        {
            *y = 0;
        }
    }
}

int
sch_component_get_mirror(const SchComponent *shape)
{
    int mirror = 0;
    SchComponentPrivate *privat = SCH_COMPONENT_GET_PRIVATE(shape);

    if (privat != NULL) {

        mirror = privat->mirror;
    }

    return mirror;
}


void
sch_component_get_orientation(const SchComponent *shape, int *angle, int *mirror)
{
    SchComponentPrivate *privat = SCH_COMPONENT_GET_PRIVATE(shape);

    if (privat != NULL) {

        *angle  = privat->angle;
        *mirror = privat->mirror;
    }
    else {

        *angle = 0;
        *mirror = 0;
    }
}

static void
sch_component_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    SchComponentPrivate *privat = SCH_COMPONENT_GET_PRIVATE(object);

    switch (property_id)
    {
        case SCH_COMPONENT_X:
            g_value_set_int(value, privat->x);
            break;

        case SCH_COMPONENT_Y:
            g_value_set_int(value, privat->y);
            break;

        case SCH_COMPONENT_SELECTABLE:
            g_value_set_int(value, privat->selectable);
            break;

        case SCH_COMPONENT_ANGLE:
            g_value_set_int(value, privat->angle);
            break;

        case SCH_COMPONENT_MIRROR:
            g_value_set_int(value, privat->mirror);
            break;

        case SCH_COMPONENT_BASENAME:
            g_value_take_string(value, sch_component_get_basename(SCH_COMPONENT(object)));
            break;

        case SCH_COMPONENT_DRAWING:
            g_value_set_object(value, privat->drawing);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

GType
sch_component_get_type(void)
{
  static GType type = G_TYPE_INVALID;

  if (type == G_TYPE_INVALID) {

    static const GTypeInfo tinfo = {
      sizeof(SchComponentClass),    /* class_size */
      NULL,                         /* base_init */
      NULL,                         /* base_finalize */
      sch_component_class_init,     /* class_init */
      NULL,                         /* class_finalize */
      NULL,                         /* class_data */
      sizeof(SchComponent),         /* instance_size */
      0,                            /* n_preallocs */
      NULL,                         /* instance_init */
      NULL                          /* value_table */
    };

    type = g_type_register_static(SCH_TYPE_SHAPE, "SchComponent", &tinfo, 0);
  }

  return type;
}

int
sch_component_get_x(const SchComponent *shape)
{
    int x = 0;
    SchComponentPrivate *privat = SCH_COMPONENT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        x = privat->x;
    }

    return x;
}

int
sch_component_get_y(const SchComponent *shape)
{
    int y = 0;
    SchComponentPrivate *privat = SCH_COMPONENT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        y = privat->y;
    }

    return y;
}

SchComponent*
sch_component_instantiate(const SchConfig *config, SchDrawing *drawing)
{
    SchComponent *component = NULL;

    if (drawing != NULL)
    {
        SchComponentPrivate *privat;

        component = sch_component_new(config);
        privat = SCH_COMPONENT_GET_PRIVATE(component);

        if (privat != NULL)
        {
            SchComponentInstantiateProcData data;

            data.config     = config;
            data.component  = component;
            data.attributes = sch_shape_get_attributes(SCH_SHAPE(component));

            privat->drawing = drawing;
            g_object_ref(privat->drawing);

            sch_drawing_foreach(drawing, (GFunc) sch_component_instantiate_proc, &data);

            if (data.attributes != NULL)
            {
                g_object_unref(data.attributes);
            }
        }
    }

    return component;
}

static void
sch_component_instantiate_proc(SchShape *shape, gpointer user_data)
{
    if (SCH_IS_TEXT(shape))
    {
        SchComponentInstantiateProcData *data = (SchComponentInstantiateProcData*) user_data;

        SchText *text = SCH_TEXT(shape);

        SchText *text2 = sch_text_clone(text);

        sch_text_set_visible(text, 0);

        sch_text_set_color(text2, 5);

        sch_attributes_append(data->attributes, SCH_SHAPE(text2));

        if (text2 != NULL)
        {
            g_object_unref(text2);
        }
    }
}

SchComponent*
sch_component_new(const SchConfig *config)
{
    return SCH_COMPONENT(g_object_new(SCH_TYPE_COMPONENT, NULL));
}

void
sch_component_set_angle(SchComponent *shape, int angle)
{
    SchComponentPrivate *privat = SCH_COMPONENT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->angle = angle;

        g_object_notify(G_OBJECT(shape), "angle");
    }
}

void
sch_component_set_drawing(SchComponent *shape, SchDrawing *drawing)
{
    SchComponentPrivate *privat = SCH_COMPONENT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        if (privat->drawing != NULL)
        {
            g_object_unref(privat->drawing);
        }

        privat->drawing = drawing;

        if (privat->drawing != NULL)
        {
            g_object_ref(privat->drawing);
        }
    }
}

void
sch_component_set_mirror(SchComponent *shape, int mirror)
{
    SchComponentPrivate *privat = SCH_COMPONENT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->mirror = mirror;

        g_object_notify(G_OBJECT(shape), "mirror");
    }
}

static void
sch_component_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    SchComponent *component = SCH_COMPONENT(object);

    switch (property_id)
    {
        case SCH_COMPONENT_X:
            sch_component_set_x(component, g_value_get_int(value));
            break;

        case SCH_COMPONENT_Y:
            sch_component_set_y(component, g_value_get_int(value));
            break;

        case SCH_COMPONENT_SELECTABLE:
            sch_component_set_selectable(component, g_value_get_int(value));
            break;

        case SCH_COMPONENT_ANGLE:
            sch_component_set_angle(component, g_value_get_int(value));
            break;

        case SCH_COMPONENT_MIRROR:
            sch_component_set_mirror(component, g_value_get_int(value));
            break;

        case SCH_COMPONENT_DRAWING:
            sch_component_set_drawing(component, SCH_DRAWING(g_value_get_object(value)));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

void
sch_component_set_selectable(SchComponent *shape, int selectable)
{
    SchComponentPrivate *privat = SCH_COMPONENT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->selectable = selectable;

        g_object_notify(G_OBJECT(shape), "selectable");
    }
}

void
sch_component_set_x(SchComponent *shape, int x)
{
    SchComponentPrivate *privat = SCH_COMPONENT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->x = x;

        g_object_notify(G_OBJECT(shape), "x");
    }
}

void
sch_component_set_y(SchComponent *shape, int y)
{
    SchComponentPrivate *privat = SCH_COMPONENT_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        privat->y = y;

        g_object_notify(G_OBJECT(shape), "y");
    }
}


static void
sch_component_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error)
{
    sch_file_format_2_write_component(format, stream, SCH_COMPONENT(shape), error);
}

