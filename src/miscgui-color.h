/* gEDA - GPL Electronic Design Automation
 * gparts - gEDA Parts Manager
 * Copyright (C) 2011 Edward C. Hennessy
 * Copyright (C) 2011 gEDA Contributors (see ChangeLog for details)
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

/*! \file miscgui-color.h
 *
 *  \ingroup miscgui
 *
 *  The functionality in this module will be available in GTK 3.0.
 */

#define MISCGUI_TYPE_COLOR (miscgui_color_get_type())
#define MISCGUI_COLOR(obj) ((MiscGUIColor*)obj)

typedef struct _MiscGUIColor MiscGUIColor;

/*! \brief A boxed type for a color
 *
 *  \ingroup miscgui
 */
struct _MiscGUIColor
{
    double red;
    double green;
    double blue;
    double alpha;
};

/*! \brief Makes a copy of a line.
 *
 *  \public
 *  \memberof _MiscGUIColor
 *
 *  \param [in] color The color to copy
 *  \return A dynamically allocated copy of the given color
 */
MiscGUIColor*
miscgui_color_copy(const MiscGUIColor *color);

/*! \brief Free a dynamically allocated color
 *
 *  \public
 *  \memberof _MiscGUIColor
 *
 *  \param [in] color Pointer to MiscGUIColor to free
 */
void
miscgui_color_free (void *color);
//miscgui_color_free(MiscGUIColor *color);

/* \private */
GType
miscgui_color_get_type(void);

/*! \brief Initialize a color
 *
 *  \public
 *  \memberof _MiscGUIColor
 *
 *  \param [in,out] color The color to initialize
 */
void
miscgui_color_init(MiscGUIColor *color);

/*! \brief Allocate a new color
 *
 *  \public
 *  \memberof _MiscGUIColor
 *
 *  \return A dynamically allocated color
 */
MiscGUIColor*
miscgui_color_new(void);

/*! \brief Allocate a new color given a name
 *
 *  \public
 *  \memberof _MiscGUIColor
 *
 *  \return A dynamically allocated color
 */
MiscGUIColor*
miscgui_color_new_from_name(const gchar *name);

