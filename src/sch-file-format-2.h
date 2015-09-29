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

/*! \file sch-file-format-2.h
 */
#define SCH_TYPE_FILE_FORMAT_2 (sch_file_format_2_get_type())
#define SCH_FILE_FORMAT_2(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SCH_TYPE_FILE_FORMAT_2,SchFileFormat2))
#define SCH_FILE_FORMAT_2_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),SCH_TYPE_FILE_FORMAT_2,SchFileFormat2Class))
#define SCH_IS_FILE_FORMAT_2(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SCH_TYPE_FILE_FORMAT_2))
#define SCH_IS_FILE_FORMAT_2_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),SCH_TYPE_FILE_FORMAT_2))
#define SCH_FILE_FORMAT_2_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),SCH_TYPE_FILE_FORMAT_2,SchFileFormat2Class))


struct _SchFileFormat2
{
    GObject parent;
};

struct _SchFileFormat2Class
{
    GObjectClass parent;
};

GType
sch_file_format_2_get_type(void);

void
sch_file_format_2_write_arc(const SchFileFormat2 *format, SchOutputStream *stream, const SchArc *shape, GError **error);

void
sch_file_format_2_write_attributes_begin(const SchFileFormat2 *format, SchOutputStream *stream, GError **error);

void
sch_file_format_2_write_attributes_end(const SchFileFormat2 *format, SchOutputStream *stream, GError **error);

void
sch_file_format_2_write_box(const SchFileFormat2 *format, SchOutputStream *stream, const SchBox *shape, GError **error);

void
sch_file_format_2_write_bus(const SchFileFormat2 *format, SchOutputStream *stream, const SchBus *shape, GError **error);

void
sch_file_format_2_write_circle(const SchFileFormat2 *format, SchOutputStream *stream, const SchCircle *shape, GError **error);

void
sch_file_format_2_write_component(const SchFileFormat2 *format, SchOutputStream *stream, const SchComponent *shape, GError **error);

void
sch_file_format_2_write_line(const SchFileFormat2 *format, SchOutputStream *stream, const SchLine *shape, GError **error);

void
sch_file_format_2_write_net(const SchFileFormat2 *format, SchOutputStream *stream, const SchNet *shape, GError **error);

void
sch_file_format_2_write_path(const SchFileFormat2 *format, SchOutputStream *stream, const SchPath *shape, GError **error);

void
sch_file_format_2_write_picture(const SchFileFormat2 *format, SchOutputStream *stream, const SchPicture *shape, GError **error);

void
sch_file_format_2_write_pin(const SchFileFormat2 *format, SchOutputStream *stream, const SchPin *shape, GError **error);

void
sch_file_format_2_write_text(const SchFileFormat2 *format, SchOutputStream *stream, const SchText *shape, GError **error);

void
sch_file_format_2_write_version(const SchFileFormat2 *format, SchOutputStream *stream, GError **error);

