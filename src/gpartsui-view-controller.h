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

/*! \file gpartsui-view-controller.h
 *
 *  \brief A base class for view controllers.
 *
 *  Two options for implementation of actions deligated to the view controller
 *  include:
 *
 *  - making the GtkActions properties of the controller and making the
 *  controller responsible for maintaining the GtkAction state and handling
 *  signals emitted from the GtkAction. Setting the GtkAction to NULL revokes
 *  responsibility. This mechanism does not lend itself to actions that
 *  have one source or 'owner' and many controllers must handle the event.
 *
 *  - making the action's label and sensitivity properties of the controller.
 *  The application controller responds to the notify signals and updates the
 *  GtkAction's state. When the GtkAction emits an activate signal, the
 *  App Controller calls one of this controller's methods. The application
 *  controller ignores signals from inactive controllers.
 *
 *  Implementation currently uses the former. As implementation progresses,
 *  the other mechanism may be more suitable.
 */

#define GPARTSUI_TYPE_VIEW_CONTROLLER (gpartsui_view_controller_get_type())
#define GPARTSUI_VIEW_CONTROLLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GPARTSUI_TYPE_VIEW_CONTROLLER,GPartsUIViewController))
#define GPARTSUI_VIEW_CONTROLLER_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),GPARTSUI_TYPE_VIEW_CONTROLLER,GPartsUIViewControllerClass))
#define GPARTSUI_IS_VIEW_CONTROLLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GPARTSUI_TYPE_VIEW_CONTROLLER))
#define GPARTSUI_IS_VIEW_CONTROLLER_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),GPARTSUI_TYPE_VIEW_CONTROLLER))
#define GPARTSUI_VIEW_CONTROLLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),GPARTSUI_TYPE_VIEW_CONTROLLER,GPartsUIViewControllerClass))

typedef void (*GPartsUIController)(void *, GtkAction *action);

struct _GPartsUIViewController
{
    GObject parent;
};

/*! \private */
struct _GPartsUIViewControllerClass
{
    GObjectClass parent;

    char * (*get_field)(GPartsUIViewController *controller, const char *name);
    GHashTable* (*get_table)(GPartsUIViewController *controller);

    GPartsUIController set_copy_action;
    GPartsUIController set_delete_action;
    GPartsUIController set_edit_action;
    GPartsUIController set_insert_action;
    GPartsUIController set_open_document_action;
    GPartsUIController set_open_website_action;
    GPartsUIController set_paste_action;

};

/*! \private */
GType
gpartsui_view_controller_get_type(void);

/*! \brief Gets the value of a field.
 *
 *  \param [in] controller The controller containing the data.
 *  \param [in] name The name of the field to get.
 *  \return A NULL terminated string containing the value of the field.  The
 *  caller must free the memory when no longer needed.  This function returns
 *  NULL if it cannot produce a value.
 */
gchar*
gpartsui_view_controller_get_field(GPartsUIViewController *controller, const gchar *name);

GHashTable*
gpartsui_view_controller_get_table(GPartsUIViewController *controller);

/*! \brief Set the \a copy-action for this controller
 *
 *  When the application controller sets the view controller's \a copy-action
 *  to a GtkAction, the view controller becomes responsible for handling
 *  \a activate signals from the GtkAction. The view controller must maintain
 *  the GtkAction's label and sensitive properties. The view controller must
 *  keep these properties in sync with it's internal state.
 *
 *  The view controller must set the state of the GtkAction to match it's
 *  internal state when this function is called.
 *
 *  When the application controller sets the view controller's \copy-action
 *  to NULL, the view controller is no longer allowed to handle signals
 *  or update the GtkAction's properties.
 *
 *  The base class behavior of this function sets the action's label to a
 *  default value and disables the action. Derived classes that do not
 *  implement a \a copy-action can use the base class behavior.
 *
 *  \param [in] controller A pointer to the controller
 *  \param [in] action     A pointer to the \a copy-action
 */
void
gpartsui_view_controller_set_copy_action(GPartsUIViewController *controller, GtkAction *action);

/*! \brief Set the \a delete-action for this controller
 *
 *  See gpartsui_view_controller_set_copy_action() for a detailed description.
 *
 *  \param [in] controller A pointer to the controller
 *  \param [in] action     A pointer to the \a delete-action
 */
void
gpartsui_view_controller_set_delete_action(GPartsUIViewController *controller, GtkAction *action);

/*! \brief Set the \a edit-action for this controller
 *
 *  See gpartsui_view_controller_set_copy_action() for a detailed description.
 *
 *  \param [in] controller A pointer to the controller
 *  \param [in] action     A pointer to the \a edit-action
 */
void
gpartsui_view_controller_set_edit_action(GPartsUIViewController *controller, GtkAction *action);

/*! \brief Set the \a insert-action for this controller
 *
 *  See gpartsui_view_controller_set_copy_action() for a detailed description.
 *
 *  \param [in] controller A pointer to the controller
 *  \param [in] action     A pointer to the \a insert-action
 */
void
gpartsui_view_controller_set_insert_action(GPartsUIViewController *controller, GtkAction *action);

/*! \brief Set the \a open-document-action for this controller
 *
 *  See gpartsui_view_controller_set_copy_action() for a detailed description.
 *
 *  \param [in] controller A pointer to the controller
 *  \param [in] action     A pointer to the \a open-document-action
 */
void
gpartsui_view_controller_set_open_document_action(GPartsUIViewController *controller, GtkAction *action);

/*! \brief Set the \a open-website-action for this controller
 *
 *  See gpartsui_view_controller_set_copy_action() for a detailed description.
 *
 *  \param [in] controller A pointer to the controller
 *  \param [in] action     A pointer to the \a open-website-action
 */
void
gpartsui_view_controller_set_open_website_action(GPartsUIViewController *controller, GtkAction *action);

/*! \brief Set the \a paste-action for this controller
 *
 *  See gpartsui_view_controller_set_copy_action() for a detailed description.
 *
 *  \param [in] controller A pointer to the controller
 *  \param [in] action     A pointer to the \a paste-action
 */
void
gpartsui_view_controller_set_paste_action(GPartsUIViewController *controller, GtkAction *action);


