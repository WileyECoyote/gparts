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

/*! \file sch-component.h
 */

#define SCH_TYPE_COMPONENT (sch_component_get_type())
#define SCH_COMPONENT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SCH_TYPE_COMPONENT,SchComponent))
#define SCH_COMPONENT_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),SCH_TYPE_COMPONENT,SchComponentClass))
#define SCH_IS_COMPONENT(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SCH_TYPE_COMPONENT))
#define SCH_IS_COMPONENT_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),SCH_TYPE_COMPONENT))
#define SCH_COMPONENT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),SCH_TYPE_COMPONENT,SchComponentClass))

/*! \extends SchShape
 */
struct _SchComponent
{
    SchShape parent;
};

/*! \private
 */
struct _SchComponentClass
{
    SchShapeClass parent;
};

GType
sch_component_get_type(void);

/*! \brief Get the rotation angle of the component
 *
 *  \param [in] shape The component
 *  \return The rotation angle of the component
 */
int
sch_component_get_angle(const SchComponent *shape);

/*! \brief Get the basename of the component
 *
 *  When no longer needed, call g_free() on the returned pointer
 *
 *  \param [in] shape The component
 *  \return The basename of the component
 */
char*
sch_component_get_basename(const SchComponent *component);

/*! \brief Get the symbol drawing for this component
 *
 *  When no longer needed, call g_object_unref on the returned pointer.
 *
 *  \param [in] shape The component
 *  \return The symbol drawing
 */
SchDrawing*
sch_component_get_drawing(const SchComponent *shape);

/*! \brief
 *
 *  \param [in] shape The component
 *  \return
 */
void
sch_component_get_insertion_point(const SchComponent *shape, int *x, int *y);

/*! \brief Get if the component reflects on the y axis
 *
 *  \param [in] shape The component
 *  \return
 */
int
sch_component_get_mirror(const SchComponent *shape);

/*! \brief
 *
 *  \param [in] shape The component
 *  \return
 */
void
sch_component_get_orientation(const SchComponent *shape, int *angle, int *mirror);

/*! \brief Get the x coordinate of the insertion point
 *
 *  \param [in] shape The component
 *  \return The x coordinate of the insertion point
 */
int
sch_component_get_x(const SchComponent *shape);

/*! \brief Get the y coordinate of the insertion point
 *
 *  \param [in] shape The component
 *  \return The y coordinate of the insertion point
 */
int
sch_component_get_y(const SchComponent *shape);

/*! \brief Instantiate a component
 *
 *  \param [in] config  The configuration
 *  \param [in] drawing The symbol drawing
 */
SchComponent*
sch_component_instantiate(const SchConfig *config, SchDrawing *drawing);

/*! \brief Create a new, empty component
 *
 *  \param [in] config The configuration
 */
SchComponent*
sch_component_new(const SchConfig *config);

/*! \brief Set the rotation angle for the component
 *
 * \param [in] shape The component
 * \param [in] angle The rotation angle
 */
void
sch_component_set_angle(SchComponent *shape, int angle);

/*! \brief Set the symbol drawing for this component
 *
 *  \param [in] shape The component
 *  \param [in] drawing The symbol drawing
 */
void
sch_component_set_drawing(SchComponent *shape, SchDrawing *drawing);

/*! \brief Set if the component reflects on the y axis
 *
 * \param [in] shape The component
 * \param [in] mirror
 */
void
sch_component_set_mirror(SchComponent *shape, int mirror);

/*! \brief Set if the component is selectable or not
 *
 * \param [in] shape The component
 * \param [in] selectable
 */
void
sch_component_set_selectable(SchComponent *shape, int selectable);

/*! \brief Set the x coordinate of the insertion point
 *
 * \param [in] shape The component
 * \param [in] x The x coordinate of the insertion point
 */
void
sch_component_set_x(SchComponent *shape, int x);

/*! \brief Set the y coordinate of the insertion point
 *
 * \param [in] shape The component
 * \param [in] y The y coordinate of the insertion point
 */
void
sch_component_set_y(SchComponent *shape, int y);

