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

/*! \file sch-attributes.c
 */

#include <glib-object.h>

#include "sch.h"

#define SCH_ATTRIBUTES_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCH_TYPE_ATTRIBUTES,SchAttributesPrivate))

typedef struct _SchAttributesPrivate SchAttributesPrivate;

struct _SchAttributesPrivate
{
    GSList *shapes;
};



void
sch_attributes_append(SchAttributes *attributes, SchShape *shape)
{
    if (shape != NULL)
    {
        SchAttributesPrivate *privat = SCH_ATTRIBUTES_GET_PRIVATE(attributes);

        if (privat != NULL)
        {
            g_object_ref(shape);

            privat->shapes = g_slist_append(privat->shapes, shape);
        }
    }
}

sch_attributes_bounds(SchAttributes *attributes, SchDrafter *drafter, GeomBounds *bounds)
{
}

static void
sch_attributes_class_init(gpointer g_class, gpointer g_class_data)
{
    SchAttributesClass *klasse = SCH_ATTRIBUTES_CLASS(g_class);

    g_type_class_add_private(klasse, sizeof(SchAttributesPrivate));
}

sch_attributes_draw(SchAttributes *attributes, SchDrafter *drafter)
{
}

GType
sch_attributes_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(SchAttributesClass),    /* class_size */
            NULL,                          /* base_init */
            NULL,                          /* base_finalize */
            sch_attributes_class_init,     /* class_init */
            NULL,                          /* class_finalize */
            NULL,                          /* class_data */
            sizeof(SchAttributes),         /* instance_size */
            0,                             /* n_preallocs */
            NULL,                          /* instance_init */
            NULL                           /* value_table */
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "SchAttributes",
            &tinfo,
            0
            );
    }

    return type;
}

void
sch_attributes_rotate(SchAttributes *attributes, int angle)
{
}

void
sch_attributes_transform(SchAttributes *attributes, const GeomTransform *transform)
{
}

void
sch_attributes_translate(SchAttributes *attributes, int dx, int dy)
{
}

void
sch_attributes_write(SchAttributes *attributes, SchFileFormat2 *format, SchOutputStream *stream, GError **error)
{
    SchAttributesPrivate *privat = SCH_ATTRIBUTES_GET_PRIVATE(attributes);

    if ((privat != NULL) && (privat->shapes != NULL))
    {
        GSList *node = privat->shapes;

        sch_file_format_2_write_attributes_begin(format, stream, error);

        while (node != NULL)
        {
           sch_shape_write(SCH_SHAPE(node->data), format, stream, error);

            node = g_slist_next(node);
        }

        sch_file_format_2_write_attributes_end(format, stream, error);
   }
}
