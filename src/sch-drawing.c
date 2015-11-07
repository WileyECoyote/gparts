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

/*! \file sch-drawing.c
 */

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

#include "misc-object.h"

#include "sch.h"

#define SCH_DRAWING_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCH_TYPE_DRAWING,SchDrawingPrivate))

enum
{
    SCH_DRAWING_BASENAME = 1,
    SCH_DRAWING_FILE_FORMAT,
    SCH_DRAWING_FILENAME
};

typedef struct _SchDrawingPrivate SchDrawingPrivate;

struct _SchDrawingPrivate
{
    SchFileFormat2 *file_format;
    char           *filename;
    GSList         *shapes;
};

static void
sch_drawing_class_init(gpointer g_class, gpointer g_class_data);

static void
sch_drawing_dispose(GObject *object);

static void
sch_drawing_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
sch_drawing_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void
sch_drawing_init(GTypeInstance *instance, gpointer g_class);



void
sch_drawing_append_shape(SchDrawing *drawing, SchShape *shape)
{
    if (shape != NULL)
    {
        SchDrawingPrivate *privat = SCH_DRAWING_GET_PRIVATE(drawing);

        if (privat != NULL)
        {
            SchDrawingClass *klasse = SCH_DRAWING_GET_CLASS(drawing);

            g_object_ref(shape);

            privat->shapes = g_slist_append(privat->shapes, shape);

            if (klasse != NULL)
            {
                g_signal_emit(
                    drawing,
                    klasse->signal_id[SCH_DRAWING_SIGNAL_APPEND],
                    0,
                    shape
                    );
            }
        }
    }
}

static void
sch_drawing_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *object_class = G_OBJECT_CLASS(g_class);
    SchDrawingClass *klasse = SCH_DRAWING_CLASS(g_class);

    g_type_class_add_private(g_class, sizeof(SchDrawingPrivate));

    object_class->get_property = sch_drawing_get_property;
    object_class->set_property = sch_drawing_set_property;

    object_class->dispose = sch_drawing_dispose;

    g_object_class_install_property(
        object_class,
        SCH_DRAWING_BASENAME,
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
        SCH_DRAWING_FILE_FORMAT,
        g_param_spec_object(
            "file-format",
            "File Format",
            "File Format",
            SCH_TYPE_FILE_FORMAT_2,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        object_class,
        SCH_DRAWING_FILENAME,
        g_param_spec_string(
            "filename",
            "Filename",
            "",
            NULL,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );


    klasse->signal_id[SCH_DRAWING_SIGNAL_APPEND] = g_signal_new(
        "append",
        SCH_TYPE_DRAWING,
        G_SIGNAL_RUN_LAST,
        0,
        NULL,
        NULL,
        g_cclosure_marshal_VOID__POINTER,
        G_TYPE_NONE,
        1,
        SCH_TYPE_SHAPE
        );

    klasse->signal_id[SCH_DRAWING_SIGNAL_PREPEND] = g_signal_new(
        "prepend",
        SCH_TYPE_DRAWING,
        G_SIGNAL_RUN_LAST,
        0,
        NULL,
        NULL,
        g_cclosure_marshal_VOID__POINTER,
        G_TYPE_NONE,
        1,
        SCH_TYPE_SHAPE
        );

}

static void
sch_drawing_dispose(GObject *object)
{
    SchDrawingPrivate *privat = SCH_DRAWING_GET_PRIVATE(object);

    if (privat != NULL)
    {
        GSList *node = privat->shapes;

        while (node != NULL)
        {
            g_object_unref(node->data);
            node = g_slist_next(node);
        }

        g_slist_free(privat->shapes);
        privat->shapes = NULL;

        if (privat->file_format != NULL)
        {
            g_object_unref(privat->file_format);
            privat->file_format = NULL;
        }
    }

    misc_object_chain_dispose(object);
}

void
sch_drawing_foreach(SchDrawing *drawing, GFunc func, void *user_data)
{
    if (func != NULL) {

        SchDrawingPrivate *privat = SCH_DRAWING_GET_PRIVATE(drawing);

        if (privat != NULL) {
            g_slist_foreach(privat->shapes, func, user_data);
        }
    }
}

char*
sch_drawing_get_basename(const SchDrawing *drawing)
{
    char *basename = NULL;
    SchDrawingPrivate *privat = SCH_DRAWING_GET_PRIVATE(drawing);

    if (privat != NULL)
    {
        basename = g_path_get_basename(privat->filename);
    }

    return basename;
}

char*
sch_drawing_get_filename(const SchDrawing *drawing)
{
    char *filename = NULL;
    SchDrawingPrivate *privat = SCH_DRAWING_GET_PRIVATE(drawing);

    if (privat != NULL)
    {
        filename = g_strdup(privat->filename);
    }

    return filename;
}

void
sch_drawing_prepend_shape(SchDrawing *drawing, SchShape *shape)
{
    if (shape != NULL)
    {
        SchDrawingPrivate *privat = SCH_DRAWING_GET_PRIVATE(drawing);

        if (privat != NULL)
        {
            SchDrawingClass *klasse = SCH_DRAWING_GET_CLASS(drawing);

            g_object_ref(shape);

            privat->shapes = g_slist_prepend(privat->shapes, shape);

            if (klasse != NULL)
            {
                g_signal_emit(
                    drawing,
                    klasse->signal_id[SCH_DRAWING_SIGNAL_PREPEND],
                    0,
                    shape
                    );
            }
        }
    }
}

static void
sch_drawing_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    SchDrawingPrivate *privat = SCH_DRAWING_GET_PRIVATE(object);

    if (privat != NULL)
    {
        switch (property_id)
        {
            case SCH_DRAWING_BASENAME:
                g_value_take_string(value, sch_drawing_get_basename(SCH_DRAWING(object)));
                break;

            case SCH_DRAWING_FILE_FORMAT:
                g_value_set_object(value, privat->file_format);
                break;

            case SCH_DRAWING_FILENAME:
                g_value_set_string(value, privat->filename);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
sch_drawing_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(SchDrawingClass),    /* class_size */
            NULL,                       /* base_init */
            NULL,                       /* base_finalize */
            sch_drawing_class_init,     /* class_init */
            NULL,                       /* class_finalize */
            NULL,                       /* class_data */
            sizeof(SchDrawing),         /* instance_size */
            0,                          /* n_preallocs */
            sch_drawing_init,           /* instance_init */
            NULL                        /* value_table */
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "SchDrawing",
            &tinfo,
            0
            );
    }

    return type;
}

SchDrawing*
sch_drawing_new(void)
{
    return SCH_DRAWING(g_object_new(SCH_TYPE_DRAWING, NULL));
}

void
sch_drawing_set_file_format(SchDrawing *drawing, SchFileFormat2 *format)
{
    SchDrawingPrivate *privat = SCH_DRAWING_GET_PRIVATE(drawing);

    if (privat != NULL)
    {
        if (privat->file_format != NULL)
        {
            g_object_unref(privat->file_format);
        }

        privat->file_format = format;

        if (privat->file_format != NULL)
        {
            g_object_ref(privat->file_format);
        }
    }
}

void
sch_drawing_set_filename(const SchDrawing *drawing, const char *filename)
{
    SchDrawingPrivate *privat = SCH_DRAWING_GET_PRIVATE(drawing);

    if (privat != NULL)
    {
        g_free(privat->filename);

        privat->filename = g_strdup(filename);

        g_object_notify(G_OBJECT(drawing), "basename");
        g_object_notify(G_OBJECT(drawing), "filename");
    }
}

static void
sch_drawing_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    switch (property_id)
    {
        case SCH_DRAWING_FILE_FORMAT:
            sch_drawing_set_file_format(SCH_DRAWING(object), SCH_FILE_FORMAT_2(g_value_get_object(value)));
            break;

        case SCH_DRAWING_FILENAME:
            sch_drawing_set_filename(SCH_DRAWING(object), g_value_get_string(value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static void
sch_drawing_init(GTypeInstance *instance, gpointer g_class)
{
    SchDrawingPrivate *privat = SCH_DRAWING_GET_PRIVATE(instance);

    if (privat != NULL)
    {
        privat->file_format = (SchFileFormat2*) g_object_new(SCH_TYPE_FILE_FORMAT_2, NULL);
    }
}

void
sch_drawing_rotate(SchDrawing *drawing, int angle)
{
    SchDrawingPrivate *privat = SCH_DRAWING_GET_PRIVATE(drawing);

    if (privat != NULL)
    {
        GSList *node = privat->shapes;

        while (node != NULL)
        {
            sch_shape_rotate(SCH_SHAPE(node->data), angle);

            node = g_slist_next(node);
        }
    }
}

void
sch_drawing_transform(SchDrawing *drawing, const GeomTransform *transform)
{
    SchDrawingPrivate *privat = SCH_DRAWING_GET_PRIVATE(drawing);

    if (privat != NULL)
    {
        GSList *node = privat->shapes;

        while (node != NULL)
        {
            sch_shape_transform(SCH_SHAPE(node->data), transform);

            node = g_slist_next(node);
        }
    }
}

void
sch_drawing_translate(SchDrawing *drawing, int dx, int dy)
{
    SchDrawingPrivate *privat = SCH_DRAWING_GET_PRIVATE(drawing);

    if (privat != NULL)
    {
        GSList *node = privat->shapes;

        while (node != NULL)
        {
            sch_shape_translate(SCH_SHAPE(node->data), dx, dy);

            node = g_slist_next(node);
        }
    }
}

void
sch_drawing_write(const SchDrawing *drawing, SchOutputStream *stream)
{
    SchDrawingPrivate *privat = SCH_DRAWING_GET_PRIVATE(drawing);

    if (privat != NULL) {

        GSList *node = privat->shapes;

        sch_file_format_2_write_version(privat->file_format, stream, NULL);

        while (node != NULL) {

            sch_shape_write(SCH_SHAPE(node->data), privat->file_format, stream, NULL);

            node = g_slist_next(node);
        }
   }
}

