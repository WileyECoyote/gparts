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

/*! \file sch-text.h
 */

#define SCH_TEXT_ALIGN_BOTTOM_LEFT    0
#define SCH_TEXT_ALIGN_MIDDLE_LEFT    1
#define SCH_TEXT_ALIGN_TOP_LEFT       2
#define SCH_TEXT_ALIGN_BOTTOM_CENTER  3
#define SCH_TEXT_ALIGN_CENTER         4
#define SCH_TEXT_ALIGN_TOP_CENTER     5
#define SCH_TEXT_ALIGN_BOTTOM_RIGHT   6
#define SCH_TEXT_ALIGN_MIDDLE_RIGHT   7
#define SCH_TEXT_ALIGN_TOP_RIGHT      8


#define SCH_TEXT_SHOW_ALL    0
#define SCH_TEXT_SHOW_VALUE  1
#define SCH_TEXT_SHOW_NAME   2

#define SCH_TEXT_VISIBLE_INVISIBLE  0
#define SCH_TEXT_VISIBLE_VISIBLE    1


#define SCH_TYPE_TEXT (sch_text_get_type())
#define SCH_TEXT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SCH_TYPE_TEXT,SchText))
#define SCH_TEXT_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),SCH_TYPE_TEXT,SchTextClass))
#define SCH_IS_TEXT(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SCH_TYPE_TEXT))
#define SCH_IS_TEXT_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),SCH_TYPE_TEXT))
#define SCH_TEXT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),SCH_TYPE_TEXT,SchTextClass))

/*! \brief An object for text
 *
 *  \extends SchShape
 */
struct _SchText
{
    SchShape parent;
};

/*! \private */
struct _SchTextClass
{
    SchShapeClass parent;

    GRegex *regex;
};

/*! \private */
GType
sch_text_get_type(void);

/*! \brief Make a deep copy of the given text object
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object to clone
 *  \return A deep copy of the given text object
 */
SchText *sch_text_clone(const SchText *shape);

/*! \brief Get the alignment of the text
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \return The alignment of the text
 */
int sch_text_get_alignment(const SchText *shape);

/*! \brief Get the angle of the text
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \return The angle of the text
 */
int sch_text_get_angle(const SchText *shape);

/*! \brief Get the attribute name
 *
 *  \public
 *  \memberof _SchText
 *
 *  Get the attribute name for this text object. If this text object
 *  does not represent an attribute, this function returns NULL.
 *  When no longer needed, call g_free() on the returned string.
 *
 *  \param [in] shape The text object
 *  \return The attribute name
 */
char *sch_text_get_attribute_name(const SchText *shape);

/*! \brief Get the attribute value
 *
 *  \public
 *  \memberof _SchText
 *
 *  Get the attribute value for this text object. If this text object
 *  does not represent an attribute, this function returns NULL.
 *  When no longer needed, call g_free() on the returned string.
 *
 *  \param [in] shape The text object
 *  \return The attribute value
 */
char *sch_text_get_attribute_value(const SchText *shape);

/*! \brief Get the color of the text
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \return The color of the text
 */
int sch_text_get_color(const SchText *shape);

/*! \brief Get the content of the text
 *
 *  \deprecated Use get/set string instead
 *  \public
 *  \memberof _SchText
 *
 *  When no longer needed, call g_objectunref() on the returned pointer.
 *
 *  \param [in] text The text object
 *  \return A pointer to the multiline object
 */
SchMultiline *sch_text_get_multiline(const SchText *text);


/*! \brief Get the portion of attributes to show
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \return The portion of attributes to show
 */
int sch_text_get_show(const SchText *shape);

/*! \brief Get the string of attributes to show
 *  Returns portion based on show_name_value property.
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \return The portion of attributes to show
 */
char *sch_text_get_shown_string(const SchText *text);

/*! \brief Get the point size of the text
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \return The point size of the text
 */
int sch_text_get_size(const SchText *shape);

/*! \brief Get all he text as one string
 *
 *  \public
 *  \memberof _SchText
 *
 *  When no longer needed, call g_free() on the returned string.
 *
 *  \param [in] shape The text object
 *  \return All the text as one string
 */
char *sch_text_get_string(const SchText *shape);

/*! \brief Get the visiblility of the text
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \return zero The text is invisible
 *  \return non-zero The text is visible
 */
int sch_text_get_visible(const SchText *shape);

/*! \brief Get the x coordinate of the text insertion point
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \return The x coordinate of the text insertion point
 */
int sch_text_get_x(const SchText *shape);

/*! \brief Get the y coordinate of the text insertion point
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \return The y coordinate of the text insertion point
 */
int sch_text_get_y(const SchText *shape);

/*! \brief Create a text object
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] config The configuration
 *  \return The text object
 */
SchText *sch_text_new(const SchConfig *config);

/*! \brief Set the alignment of the text
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \param [in] alignment
 */
void sch_text_set_alignment(SchText *shape, int alignment);

/*! \brief Set the angle of the text
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \param [in] angle
 */
void sch_text_set_angle(SchText *shape, int angle);

/*! \brief Set the color of the text
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \param [in] color The color
 */
void sch_text_set_color(SchText *shape, int color);

/*! \brief
 *
 *  \deprecated
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] text The text object
 *  \return
 */
void sch_text_set_multiline(SchText *text, SchMultiline *multiline);

/*! \brief Set which portions of attributes show
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \param [in] show The portion of the attribute that is shown
 */
void sch_text_set_show(SchText *shape, int show);

/*! \brief Set the point size of the text
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \param [in] size The point size of the text
 */
void sch_text_set_size(SchText *shape, int size);

/*! \brief Set all the text as one string
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \param [in] string All the text as one string
 */
void sch_text_set_string(SchText *shape, const char *string);

/*! \brief Set the visibility of the text
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \param [in] visible Zero is invisible, non-zero is visible
 */
void sch_text_set_visible(SchText *shape, int visible);

/*! \brief Set the x coordinate of the text insertion point
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \param [in] x The x coordinate
 */
void sch_text_set_x(SchText *shape, int x);

/*! \brief Set the y coordinate of the text insertion point
 *
 *  \public
 *  \memberof _SchText
 *
 *  \param [in] shape The text object
 *  \param [in] y The y coordinate
 */
void sch_text_set_y(SchText *shape, int y);
