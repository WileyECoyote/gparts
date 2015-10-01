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

/*! \file sch_shape.c
 *
 *  \brief An abstract base class for schematic shapes.
 */

#include <glib-object.h>

#include "misc-object.h"

#include "sch.h"


#define SCH_SHAPE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCH_TYPE_SHAPE,SchShapePrivate))

enum
{
    SCH_SHAPE_ATTRIBUTES = 1,
};

typedef struct _SchShapePrivate SchShapePrivate;

struct _SchShapePrivate
{
    SchAttributes *attributes;
};


static void
sch_shape_class_init(gpointer g_class, gpointer g_class_data);

static void
sch_shape_dispose(GObject *object);

static void
sch_shape_expand_macros_base(SchShape *shape, const GRegex *regex, const GHashTable *table);

static void
sch_shape_find_macros_base(const SchShape *shape, const GRegex *regex, GHashTable *table);

static void
sch_shape_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
sch_shape_init(GTypeInstance *instance, gpointer g_class);

static void
sch_shape_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);



static void
sch_shape_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *object_class = G_OBJECT_CLASS(g_class);
    SchShapeClass *klasse = SCH_SHAPE_CLASS(g_class);

    g_type_class_add_private(klasse, sizeof(SchShapePrivate));

    object_class->dispose = sch_shape_dispose;

    klasse->parent.get_property = sch_shape_get_property;
    klasse->parent.set_property = sch_shape_set_property;

    klasse->expand_macros = sch_shape_expand_macros_base;
    klasse->find_macros   = sch_shape_find_macros_base;

    g_object_class_install_property(
        G_OBJECT_CLASS(klasse),
        SCH_SHAPE_ATTRIBUTES,
        g_param_spec_object(
            "attributes",
            "Attributes",
            "Attributes",
            SCH_TYPE_ATTRIBUTES,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );
}


static void
sch_shape_dispose(GObject *object)
{
    sch_shape_set_attributes(SCH_SHAPE(object), NULL);

    misc_object_chain_dispose(object);
}

void
sch_shape_expand_macros(SchShape *shape, const GRegex *regex, const GHashTable *table)
{
    SchShapeClass *klasse = SCH_SHAPE_GET_CLASS(shape);

    if ((klasse != NULL) && (klasse->expand_macros != NULL))
    {
        klasse->expand_macros(shape, regex, table);
    }
}

static void
sch_shape_expand_macros_base(SchShape *shape, const GRegex *regex, const GHashTable *table)
{
    SchShapePrivate *privat = SCH_SHAPE_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        sch_attributes_expand_macros(privat->attributes, regex, table);
    }
}

void
sch_shape_find_macros(const SchShape *shape, const GRegex *regex, GHashTable *table)
{
    SchShapeClass *klasse = SCH_SHAPE_GET_CLASS(shape);

    if ((klasse != NULL) && (klasse->find_macros != NULL))
    {
        klasse->find_macros(shape, regex, table);
    }
}

static void
sch_shape_find_macros_base(const SchShape *shape, const GRegex *regex, GHashTable *table)
{
    SchShapePrivate *privat = SCH_SHAPE_GET_PRIVATE(shape);

    if (privat != NULL)
    {
        sch_attributes_find_macros(privat->attributes, regex, table);
    }
}

static void
sch_shape_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    SchShapePrivate *privat = SCH_SHAPE_GET_PRIVATE(object);

    if (privat != NULL)
    {
        switch (property_id)
        {
            case SCH_SHAPE_ATTRIBUTES:
                g_value_set_object(value, privat->attributes);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
sch_shape_get_type(void)
{
  static GType type = G_TYPE_INVALID;

  if (type == G_TYPE_INVALID) {

    static const GTypeInfo tinfo = {
      sizeof(SchShapeClass),    /* class_size */
      NULL,                     /* base_init */
      NULL,                     /* base_finalize */
      sch_shape_class_init,     /* class_init */
      NULL,                     /* class_finalize */
      NULL,                     /* class_data */
      sizeof(SchShape),         /* instance_size */
      0,                        /* n_preallocs */
      sch_shape_init,           /* instance_init */
      NULL                      /* value_table */
    };

    type = g_type_register_static( G_TYPE_OBJECT, "SchShape",
                                   &tinfo,
                                   G_TYPE_FLAG_ABSTRACT);
  }

  return type;
}

static void
sch_shape_init(GTypeInstance *instance, gpointer g_class)
{
  SchShapePrivate *privat;

  privat = SCH_SHAPE_GET_PRIVATE(instance);

  if (privat != NULL) {

    privat->attributes = g_object_new(SCH_TYPE_ATTRIBUTES, NULL);
  }
}

void
sch_shape_rotate(SchShape *shape, int angle)
{
  SchShapeClass *klasse = SCH_SHAPE_GET_CLASS(shape);

  if ((klasse != NULL) && (klasse->rotate != NULL)) {
    klasse->rotate(shape, angle);
  }
}

static void
sch_shape_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
  SchShapePrivate *privat = SCH_SHAPE_GET_PRIVATE(object);

  if (privat != NULL) {

    switch (property_id) {

      case SCH_SHAPE_ATTRIBUTES:
        sch_shape_set_attributes(SCH_SHAPE(object), g_value_get_object(value));
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
  }
}

void
sch_shape_transform(SchShape *shape, const GeomTransform *transform)
{
  SchShapeClass *klasse = SCH_SHAPE_GET_CLASS(shape);

  if ((klasse != NULL) && (klasse->transform != NULL)) {
    klasse->transform(shape, transform);
  }
}

void
sch_shape_translate(SchShape *shape, int dx, int dy)
{
  SchShapeClass *klasse = SCH_SHAPE_GET_CLASS(shape);

  if ((klasse != NULL) && (klasse->translate != NULL)) {
    klasse->translate(shape, dx, dy);
  }
}

SchAttributes*
sch_shape_get_attributes(SchShape *shape)
{
  SchAttributes   *attributes = NULL;
  SchShapePrivate *privat     = SCH_SHAPE_GET_PRIVATE(shape);

  if (privat != NULL) {

    if (privat->attributes != NULL) {
      g_object_ref(privat->attributes);
    }

    attributes = privat->attributes;
  }

  return attributes;
}

void
sch_shape_set_attributes(SchShape *shape, SchAttributes *attributes)
{
  SchShapePrivate *privat = SCH_SHAPE_GET_PRIVATE(shape);

  if (privat != NULL) {

    if (privat->attributes != NULL) {
      g_object_unref(privat->attributes);
    }

    privat->attributes = attributes;

    if (privat->attributes != NULL) {
      g_object_ref(privat->attributes);
    }

    g_object_notify(G_OBJECT(shape), "attributes");
  }
}

void
sch_shape_write(const SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error)
{
  SchShapeClass *klasse = SCH_SHAPE_GET_CLASS(shape);

  if ((klasse != NULL) && (klasse->write != NULL)) {
    klasse->write(shape, format, stream, error);
  }
}

