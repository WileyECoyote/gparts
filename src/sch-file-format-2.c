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

/*! \file sch-file-format-2.c
 */

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

#include "sch.h"

#define SCH_FILE_FORMAT_2_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCH_TYPE_FILE_FORMAT_2,SchFileFormat2Private))

enum
{
    SCH_FILE_FORMAT_2_PACKAGE_DATE = 1,
};

typedef struct _SchFileFormat2Private SchFileFormat2Private;

struct _SchFileFormat2Private
{
    char *package_date;
};


static void
sch_file_format_2_class_init(gpointer g_class, gpointer g_class_data);

static void
sch_file_format_2_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec);

static void
sch_file_format_2_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec);


static void
sch_file_format_2_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *klasse = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(klasse, sizeof(SchFileFormat2Private));

    klasse->get_property = sch_file_format_2_get_property;
    klasse->set_property = sch_file_format_2_set_property;

    g_object_class_install_property(
        klasse,
        SCH_FILE_FORMAT_2_PACKAGE_DATE,
        g_param_spec_string(
            "package-date",
            "Package Date",
            "Package Date",
            "20100414",
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );
}

static void
sch_file_format_2_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec)
{
  SchFileFormat2        *format = SCH_FILE_FORMAT_2 (object);
  SchFileFormat2Private *privat = SCH_FILE_FORMAT_2_GET_PRIVATE(format);

  if (privat != NULL) {

    switch (property_id) {

      case SCH_FILE_FORMAT_2_PACKAGE_DATE:
        g_value_set_string(value, privat->package_date);
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
  }
}

GType
sch_file_format_2_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(SchFileFormat2Class),    /* class_size */
            NULL,                           /* base_init */
            NULL,                           /* base_finalize */
            sch_file_format_2_class_init,   /* class_init */
            NULL,                           /* class_finalize */
            NULL,                           /* class_data */
            sizeof(SchFileFormat2),         /* instance_size */
            0,                              /* n_preallocs */
            NULL,                           /* instance_init */
            NULL                            /* value_table */
            };

        type = g_type_register_static(
            G_TYPE_OBJECT, "SchFileFormat2",
            &tinfo,
            0
            );
    }

    return type;
}

static void
sch_file_format_2_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec)
{
  SchFileFormat2Private *privat = SCH_FILE_FORMAT_2_GET_PRIVATE(object);

  if (privat != NULL) {

    switch (property_id) {

      case SCH_FILE_FORMAT_2_PACKAGE_DATE:
        privat->package_date = g_value_dup_string(value);
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
  }
}

void
sch_file_format_2_write_arc(const SchFileFormat2 *format, SchOutputStream *stream, const SchArc *shape, GError **error)
{
    sch_output_stream_write_arc_2(stream, shape, error);
}

void
sch_file_format_2_write_attributes_begin(const SchFileFormat2 *format, SchOutputStream *stream, GError **error)
{
    sch_output_stream_write_attributes_begin(stream, error);
}

void
sch_file_format_2_write_attributes_end(const SchFileFormat2 *format, SchOutputStream *stream, GError **error)
{
    sch_output_stream_write_attributes_end(stream, error);
}

void
sch_file_format_2_write_box(const SchFileFormat2 *format, SchOutputStream *stream, const SchBox *shape, GError **error)
{
    sch_output_stream_write_box_2(stream, shape, NULL);
}

void
sch_file_format_2_write_bus(const SchFileFormat2 *format, SchOutputStream *stream, const SchBus *shape, GError **error)
{
    sch_output_stream_write_bus_2(stream, shape, NULL);
}

void
sch_file_format_2_write_circle(const SchFileFormat2 *format, SchOutputStream *stream, const SchCircle *shape, GError **error)
{
    sch_output_stream_write_circle_2(stream, shape, NULL);
}

void
sch_file_format_2_write_component(const SchFileFormat2 *format, SchOutputStream *stream, const SchComponent *shape, GError **error)
{
  SchAttributes *attributes = sch_shape_get_attributes((SchShape*)shape);

  sch_output_stream_write_component_2(stream, shape, NULL);

  if (attributes != NULL) {
    sch_attributes_write(attributes, format, stream, error);
    g_object_unref(attributes);
  }
}

void
sch_file_format_2_write_line(const SchFileFormat2 *format, SchOutputStream *stream, const SchLine *shape, GError **error)
{
    sch_output_stream_write_line_2(stream, shape, NULL);
}

void
sch_file_format_2_write_net(const SchFileFormat2 *format, SchOutputStream *stream, const SchNet *shape, GError **error)
{
    sch_output_stream_write_net_2(stream, shape, NULL);
}


void
sch_file_format_2_write_path(const SchFileFormat2 *format, SchOutputStream *stream, const SchPath *shape, GError **error)
{
    sch_output_stream_write_path_2(stream, shape, NULL);
}

void
sch_file_format_2_write_picture(const SchFileFormat2 *format, SchOutputStream *stream, const SchPicture *shape, GError **error)
{
    sch_output_stream_write_picture_2(stream, shape, NULL);
}

void
sch_file_format_2_write_pin(const SchFileFormat2 *format, SchOutputStream *stream, const SchPin *shape, GError **error)
{
    SchAttributes *attributes = sch_shape_get_attributes((SchShape*)shape);

    sch_output_stream_write_pin_2(stream, shape, NULL);

    if (attributes != NULL)
    {
        sch_attributes_write(attributes, format, stream, error);

        g_object_unref(attributes);
    }
}

void
sch_file_format_2_write_text(const SchFileFormat2 *format, SchOutputStream *stream, const SchText *shape, GError **error)
{
    SchMultiline *multiline;

    sch_output_stream_write_text_2(stream, shape, NULL);

    multiline = sch_text_get_multiline(SCH_TEXT(shape));

    sch_output_stream_write_string(stream, sch_multiline_peek_plain(multiline, 0), NULL);

    if (multiline != NULL)
    {
        g_object_unref(multiline);
    }
}

void
sch_file_format_2_write_version(const SchFileFormat2 *format, SchOutputStream *stream, GError **error)
{
    SchFileFormat2Private *privat = SCH_FILE_FORMAT_2_GET_PRIVATE(format);

    g_debug("Begin writing schematic file %p", format);
    g_debug("%s", G_OBJECT_TYPE_NAME(format));

    if (privat != NULL)
    {
        sch_output_stream_write_version(stream, "20100416", 2, NULL);
    }
    else
    {
        g_debug("privat is null");
    }
}

