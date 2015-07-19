/* gEDA - GPL Electronic Design Automation
 * gparts - gEDA Parts Manager
 * Copyright (C) 2012 Edward C. Hennessy
 * Copyright (C) 2012 gEDA Contributors (see ChangeLog for details)
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

/*! \file gpview-symbol-view.h
 */

#define GPVIEW_TYPE_SYMBOL_CTRL (gpview_symbol_ctrl_get_type())
#define GPVIEW_SYMBOL_CTRL(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GPVIEW_TYPE_SYMBOL_CTRL,GPViewSymbolCtrl))
#define GPVIEW_SYMBOL_CTRL_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),GPVIEW_TYPE_SYMBOL_CTRL,GPViewSymbolCtrlClass))
#define GPVIEW_IS_SYMBOL_CTRL(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GPVIEW_TYPE_SYMBOL_CTRL))
#define GPVIEW_IS_SYMBOL_CTRL_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),GPVIEW_TYPE_SYMBOL_CTRL))
#define GPVIEW_SYMBOL_CTRL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),GPVIEW_TYPE_SYMBOL_CTRL,GPViewSymbolCtrlClass))

/*! \extends GObject */
struct _GPViewSymbolCtrl
{
    GObject parent;
};

/*! \private */
struct _GPViewSymbolCtrlClass
{
    GObjectClass parent;
};

/*! \private */
GType
gpview_symbol_ctrl_get_type(void);

/*! \brief Create a new symbol view controller
 *
 *  \return A new symbol view controller [transfer full]
 */
GPViewSymbolCtrl*
gpview_symbol_ctrl_new(void);

/*! \brief Set the current view for this controller
 *
 *  This variant of the constructor creates a new symbol view controller
 *  and associates it with a UI manager.
 *
 *  \return A new symbol view controller [transfer full]
 */
GPViewSymbolCtrl*
gpview_symbol_ctrl_new_with_manager(GtkUIManager *manager);

/*! \brief Get the current view associated with this controller.
 *
 *  Use g_object_unref() on the returned pointer when no longer
 *  needed.
 *
 *  \param [in] ctrl The controller [allow none]
 *  \return The current view associated with this controller. [transfer full]
 */
GPViewSymbolView*
gpview_symbol_ctrl_get_current_view(const GPViewSymbolCtrl *ctrl);

/*! \brief Get the database associated with this controller.
 *
 *  Use g_object_unref() on the returned pointer when no longer
 *  needed.
 *
 *  \param [in] ctrl The controller [allow none]
 *  \return The database associated with this controller. [transfer full]
 */
GPartsDatabase*
gpview_symbol_ctrl_get_database(const GPViewSymbolCtrl *ctrl);

/*! \brief Get the UI manger associated with this controller.
 *
 *  Use g_object_unref() on the returned pointer when no longer
 *  needed.
 *
 *  \param [in] ctrl The controller [allow none]
 *  \return The UI manager associated with this controller. [transfer full]
 */
GtkUIManager*
gpview_symbol_ctrl_get_ui_manager(const GPViewSymbolCtrl *ctrl);

/*! \brief Set the current view for this controller
 *
 *  \param [in] ctrl The controller.
 *  \param [in] view The current view for this controller. [alow none]
 */
void
gpview_symbol_ctrl_set_current_view(GPViewSymbolCtrl *ctrl, GPViewSymbolView *view);


void
gpview_symbol_ctrl_set_form_factory(GPViewSymbolCtrl *ctrl, GPFormFactory *factory);

/*! \brief Set the database for this controller
 *
 *  \param [in] ctrl The controller.
 *  \param [in] view The database for this controller. [alow none]
 */
void
gpview_symbol_ctrl_set_database(GPViewSymbolCtrl *ctrl, GPartsDatabase *database);

/*! \brief Set UI manager for this controller
 *
 *  \param [in] ctrl The controller.
 *  \param [in] manager The manager for this controller. [alow none]
 */
void
gpview_symbol_ctrl_set_ui_manager(GPViewSymbolCtrl *view, GtkUIManager *manager);
