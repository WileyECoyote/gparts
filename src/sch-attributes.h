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

/*! \file sch-attributes.h
 *
 *  \brief A class storing schematic attributes.
 */

#define SCH_TYPE_ATTRIBUTES (sch_attributes_get_type())
#define SCH_ATTRIBUTES(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SCH_TYPE_ATTRIBUTES,SchAttributes))
#define SCH_ATTRIBUTES_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),SCH_TYPE_ATTRIBUTES,SchAttributesClass))
#define SCH_IS_ATTRIBUTES(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SCH_TYPE_ATTRIBUTES))
#define SCH_IS_ATTRIBUTES_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),SCH_TYPE_ATTRIBUTES))
#define SCH_ATTRIBUTES_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),SCH_TYPE_ATTRIBUTES,SchAttributesClass))

struct _SchAttributes
{
    GObject parent;
};

struct _SchAttributesClass
{
    GObjectClass parent;
};

GType
sch_attributes_get_type(void);

/*! \brief Appends a shape to the attributes
 *
 *  \param [in] attributes The attributes.
 *  \param [in] shape      The shape.
 */
void
sch_attributes_append(SchAttributes *attributes, SchShape *shape);

/*! \brief Expand macros within the attributes
 *
 *  Macros in the attributes have the form of $(name). Whenever this
 *  function encounters a $(name), it replaces the string with the value
 *  from the hash table.
 *
 *  The hash table keys are strings containing the macro names and the values
 *  are strings containing the macro values.
 *
 *  \param [in] attributes The attributes
 *  \param [in] regex      The regex used to find and replace macros
 *  \param [in] macros     The macros
 */
void
sch_attributes_expand_macros(SchAttributes *attributes, const GRegex *regex, const GHashTable *table);

void
sch_attributes_find_macros(SchAttributes *attributes, const GRegex *regex, GHashTable *table);

/*! \brief Call a function for each shape in the attributes
 *
 *  void func(SchShape *shape, gpointer user_data);
 *
 *  \param [in] The drawing
 *  \param [in] The function to call for each shape in the attributes
 *  \param [in] Additional data to pass to the function
 */
void
sch_attributes_foreach(SchAttributes *attributes, GFunc func, gpointer user_data);

/*! \brief Writes schematic attributes to an output stream.
 *
 *  \param [in]  attributes The attributes.
 *  \param [in]  format     The format to use when writing.
 *  \param [in]  stream     The stream to write to.
 *  \param [out] error      The error, if any, using GError protocol.
 */
void
sch_attributes_write(SchAttributes *attributes, const SchFileFormat2 *format, SchOutputStream *stream, GError **error);

