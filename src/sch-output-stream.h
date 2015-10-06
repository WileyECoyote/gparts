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

/*! \file sch-output-stream.h
 *
 *  A series of functions for writing gEDA schematic and symbol shapes to
 *  output streams.
 *
 *  For interoperability, this class intends to support all versions of the
 *  gEDA file format. Each write function name ends with the number of the file
 *  format where this variant's format first appeared. Currently, all function
 *  names end with 2 until support for format 1 gets added.
 */

#define SCH_TYPE_OUTPUT_STREAM (sch_output_stream_get_type())
#define SCH_OUTPUT_STREAM(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SCH_TYPE_OUTPUT_STREAM,SchOutputStream))
#define SCH_OUTPUT_STREAM_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),SCH_TYPE_OUTPUT_STREAM,SchOutputStreamClass))
#define SCH_IS_OUTPUT_STREAM(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SCH_TYPE_OUTPUT_STREAM))
#define SCH_IS_OUTPUT_STREAM_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),SCH_TYPE_OUTPUT_STREAM))
#define SCH_OUTPUT_STREAM_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),SCH_TYPE_OUTPUT_STREAM,SchOutputStreamClass))

struct _SchOutputStream
{
    GObject parent;
};

struct _SchOutputStreamClass
{
    GObjectClass parent;
};

GType sch_output_stream_get_type(void);

/*! \brief Create a new schematic output stream
 *
 *  \param [in] stream The underlying stream for output.
 */
SchOutputStream *sch_output_stream_new(GOutputStream *stream);

/*! \brief Write an arc to an output stream in the gEDA schematic/symbol format
 *
 *  \param [in]  stream The stream to write the arc to.
 *  \param [in]  shape  The arc to write.
 *  \param [out] error  An error, if any, using the GError protocol.
 */
void
sch_output_stream_write_arc_2(SchOutputStream *stream, const SchArc *shape, GError **error);

void
sch_output_stream_write_attributes_begin(SchOutputStream *stream, GError **error);

void
sch_output_stream_write_attributes_end(SchOutputStream *stream, GError **error);

/*! \brief Write a box to an output stream in the gEDA schematic/symbol format
 *
 *  \param [in]  stream The stream to write the box to.
 *  \param [in]  shape  The box to write.
 *  \param [out] error  An error, if any, using the GError protocol.
 */
void
sch_output_stream_write_box_2(SchOutputStream *stream, const SchBox *shape, GError **error);

/*! \brief Write a bus to an output stream in the gEDA schematic/symbol format
 *
 *  \param [in]  stream The stream to write the bus to.
 *  \param [in]  shape  The bus to write.
 *  \param [out] error  An error, if any, using the GError protocol.
 */
void
sch_output_stream_write_bus_2(SchOutputStream *stream, const SchBus *shape, GError **error);

/*! \brief Write a circle to an output stream in the gEDA schematic/symbol format
 *
 *  \param [in]  stream The stream to write the circle to.
 *  \param [in]  shape  The circle to write.
 *  \param [out] error  An error, if any, using the GError protocol.
 */
void
sch_output_stream_write_circle_2(SchOutputStream *stream, const SchCircle *shape, GError **error);

/*! \brief Write a component to an output stream in the gEDA schematic/symbol format
 *
 *  \param [in]  stream The stream to write the component to.
 *  \param [in]  shape  The component to write.
 *  \param [out] error  An error, if any, using the GError protocol.
 */
void
sch_output_stream_write_component_2(SchOutputStream *stream, const SchComponent *shape, GError **error);

/*! \brief Write a line to an output stream in the gEDA schematic/symbol format
 *
 *  \param [in]  stream The stream to write the line to.
 *  \param [in]  shape  The line to write.
 *  \param [out] error  An error, if any, using the GError protocol.
 */
void
sch_output_stream_write_line_2(SchOutputStream *stream, const SchLine *shape, GError **error);

/*! \brief Write a net to an output stream in the gEDA schematic/symbol format
 *
 *  \param [in]  stream The stream to write the net to.
 *  \param [in]  shape  The net to write.
 *  \param [out] error  An error, if any, using the GError protocol.
 */
void
sch_output_stream_write_net_2(SchOutputStream *stream, const SchNet *shape, GError **error);

/*! \brief Write a path to an output stream in the gEDA schematic/symbol format
 *
 *  Paths first appeared in file format 2. File format 1 does not support paths.
 *
 *  \param [in]  stream The stream to write the path to.
 *  \param [in]  shape  The path to write.
 *  \param [out] error  An error, if any, using the GError protocol.
 */
void sch_output_stream_write_path_2(SchOutputStream *stream, const SchPath *shape, GError **error);

/*! \brief Write a picture to an output stream in the gEDA schematic/symbol format
 *
 *  \param [in]  stream The stream to write the picture to.
 *  \param [in]  shape  The picture to write.
 *  \param [out] error  An error, if any, using the GError protocol.
 */
void
sch_output_stream_write_picture_2(SchOutputStream *stream, const SchPicture *shape, GError **error);

/*! \brief Write a pin to an output stream in the gEDA schematic/symbol format
 *
 *  \param [in]  stream The stream to write the pin to.
 *  \param [in]  shape  The pin to write.
 *  \param [out] error  An error, if any, using the GError protocol.
 */
void
sch_output_stream_write_pin_2(SchOutputStream *stream, const SchPin *shape, GError **error);

/*! \brief Write a string to the underlying output stream
 *
 *  This function appends a newline to the output string after writing the
 *   string.
 *
 *  \param [in]  stream The stream to write the string to.
 *  \param [in]  shape  The string.
 *  \param [out] error  An error, if any, using the GError protocol.
 */
void
sch_output_stream_write_string(SchOutputStream *stream, const char *string, GError **error);

/*! \brief Write a text object to an output stream in the gEDA schematic/symbol format
 *
 *  \param [in]  stream The stream to write the text object to.
 *  \param [in]  shape  The text object to write.
 *  \param [out] error  An error, if any, using the GError protocol.
 */
void
sch_output_stream_write_text_2(SchOutputStream *stream, const SchText *shape, GError **error);

/*! \brief Write the version information to the output stream in the gEDA schematic/symbol format
 *
 *  \param [in]  stream         The stream to write the text object to.
 *  \param [in]  package_date   The text object to write.
 *  \param [in]  format_version The text object to write.
 *  \param [out] error          An error, if any, using the GError protocol.
 */
void
sch_output_stream_write_version(SchOutputStream *stream, const char *package_date, int format_version, GError **error);

