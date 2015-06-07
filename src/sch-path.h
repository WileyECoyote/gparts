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

/*! \file sch-path.h
 */

#define SCH_TYPE_PATH (sch_path_get_type())
#define SCH_PATH(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SCH_TYPE_PATH,SchPath))
#define SCH_PATH_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),SCH_TYPE_PATH,SchPathClass))
#define SCH_IS_PATH(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SCH_TYPE_PATH))
#define SCH_IS_PATH_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),SCH_TYPE_PATH))
#define SCH_PATH_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),SCH_TYPE_PATH,SchPathClass))

/*! \extends SchShape
 */
struct _SchPath
{
    SchShape parent;
};

/*! \private
 */
struct _SchPathClass
{
    SchShapeClass parent;
};

GType
sch_path_get_type(void);

/*! \brief Append a command to the path
 *
 *  The first command in a path must be a moveto. Regardless of an absolute or
 *  relative moveto, the first command in the path gets interpreted as an
 *  absolute moveto.
 *
 *  \todo Should this command return an error if the first command is not a
 *   moveto?
 *
 *  \param [in] path The path
 *  \param [in] command The command to append
 */
void
sch_path_append(SchPath *path, const SchPathCommand *command);

/*! \brief Return the number of commands in the path
 *
 *  \param [in] path The path
 *  \return The number of commands in the path
 */
int
sch_path_count(const SchPath *path);

/*! \brief Call a function for each command in the path
 *
 *  \param [in] path
 *  \param [in] func
 *  \param [in] user_data
 */
void
sch_path_foreach(SchPath *path, GFunc func, gpointer user_data);

/*! \brief Get the color of the path
 *
 *  \param [in] path The path
 *  \return The color index
 */
int
sch_path_get_color(const SchPath *path);

/*! \brief Get the fill style of the path
 *
 *  \param [in]  path  The path
 *  \param [out] style The fill style of the path
 */
void
sch_path_get_fill_style(const SchPath *path, SchFillStyle *style);

/*! \brief Get the line width of the path
 *
 *  \param [in] path The path
 *  \return The line width
 */
int
sch_path_get_line_width(const SchPath *path);


/*! \brief Create an empty path
 *
 *  \param [in] config The configuration
 *  \return A new path
 */
SchPath*
sch_path_new(const SchConfig *config);

