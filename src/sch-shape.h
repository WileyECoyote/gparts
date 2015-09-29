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

/*! \file sch-shape.h
 *
 *  \brief An abstract base class for schematic shapes.
 */

#define SCH_TYPE_SHAPE (sch_shape_get_type())
#define SCH_SHAPE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SCH_TYPE_SHAPE,SchShape))
#define SCH_SHAPE_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),SCH_TYPE_SHAPE,SchShapeClass))
#define SCH_IS_SHAPE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SCH_TYPE_SHAPE))
#define SCH_IS_SHAPE_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),SCH_TYPE_SHAPE))
#define SCH_SHAPE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),SCH_TYPE_SHAPE,SchShapeClass))

/* typedefs in sch-forward.h */

/*! \brief A base class for objects in a schematic
 */
struct _SchShape
{
    GObject parent;
};

struct _SchShapeClass
{
    GObjectClass parent;

    void (*expand_macros)(SchShape *shape, const GRegex *regex, const GHashTable *table);
    void (*find_macros)(const SchShape *shape, const GRegex *regex, GHashTable *table);
    void (*rotate)(SchShape *shape, int angle);
    void (*transform)(SchShape *shape, const GeomTransform *transform);
    void (*translate)(SchShape *shape, int dx, int dy);
    void (*write)(SchShape *shape, SchFileFormat2 *format, SchOutputStream *stream, GError **error);
};

GType
sch_shape_get_type(void);

/*! \brief Expand macros found in the given string
 *
 *  The hash table keys are strings containing the macro names and the values
 *  are strings containing the macro values.
 *
 *  When no longer needed, call g_free() on the returned string.
 *
 *  \param [in,out] shape The shape
 *  \param [in]     regex  The regex used to find and replace macros
 *  \param [in]     table  A table of macro names and values.
 */
void sch_shape_expand_macros(SchShape *shape, const GRegex *regex, const GHashTable *table);

/*! \brief Finds all the macros in the given string
 *
 *  This function finds all the macros in the given string and adds the
 *  names as hash table keys. New names added to the table are inserted
 *  with the value NULL. If the name already exists in the table, the
 *  data is not overwritten.
 *
 *  \param [in]     shape  The shape
 *  \param [in]     regex  The regex used to find and replace macros
 *  \param [in,out] table  A table of macro names and values.
 */
void sch_shape_find_macros(const SchShape *shape, const GRegex *regex, GHashTable *table);

void sch_shape_rotate(SchShape *shape, int angle);

void sch_shape_transform(SchShape *shape, const struct _GeomTransform *transform);

void sch_shape_translate(SchShape *shape, int dx, int dy);

/*! \brief Get the attributes associated with this shape.
 *
 *  The caller must call g_object_unref() when the attributes are no longer
 *  needed.
 *
 *  \param [in] shape The shape
 *  \return The shape's attributes, or NULL if none.
 */
SchAttributes *sch_shape_get_attributes(SchShape *shape);

/*! \brief Set the attributes associated with this shape.
 *
 *  \param [in] shape The shape.
 *  \param [in] attributes The attributes.
 */
void sch_shape_set_attributes(SchShape *shape, SchAttributes *attributes);

/*! \brief Write the shape to an output stream.
 *
 *  \param [in]  shape  The shape.
 *  \param [in]  format The format to write the shape in.
 *  \param [in]  stream The stream to write the shape to.
 *  \param [out] error  The error, if any, using the GError protocol.
 */
void sch_shape_write(SchShape *shape, const SchFileFormat2 *format, SchOutputStream *stream, GError **error);

