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

/*! \file gparts-controller.h
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

#define GPARTS_TYPE_CONTROLLER (gparts_controller_get_type())
#define GPARTS_CONTROLLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GPARTS_TYPE_CONTROLLER,GPartsController))
#define GPARTS_CONTROLLER_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),GPARTS_TYPE_CONTROLLER,GPartsControllerClass))
#define GPARTS_IS_CONTROLLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GPARTS_TYPE_CONTROLLER))
#define GPARTS_IS_CONTROLLER_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),GPARTS_TYPE_CONTROLLER))
#define GPARTS_CONTROLLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),GPARTS_TYPE_CONTROLLER,GPartsControllerClass))

struct _GPartsController
{
    GObject parent;
};

/*! \private */
struct _GPartsControllerClass
{
    GObjectClass parent;

    char* (*get_field)(GPartsController *controller, const char *name);

    GHashTable* (*get_table)(GPartsController *controller);

    void (*set_copy_action)(GPartsController *controller, GtkAction *action);
    void (*set_delete_action)(GPartsController *controller, GtkAction *action);
    void (*set_edit_action)(GPartsController *controller, GtkAction *action);
    void (*set_insert_action)(GPartsController *controller, GtkAction *action);
    void (*set_open_website_action)(GPartsController *controller, GtkAction *action);
    void (*set_paste_action)(GPartsController *controller, GtkAction *action);
};

/*! \private */
GType
gparts_controller_get_type(void);

/*! \brief Gets the value of a field.
 *
 *  \param [in] controller The controller containing the data.
 *  \param [in] name The name of the field to get.
 *  \return A NULL terminated string containing the value of the field.  The
 *  caller must free the memory when no longer needed.  This function returns
 *  NULL if it cannot produce a value.
 */
char*
gparts_controller_get_field(GPartsController *controller, const char *name);

GHashTable*
gparts_controller_get_table(GPartsController *controller);

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
gparts_controller_set_copy_action(GPartsController *controller, GtkAction *action);

/*! \brief Set the \a delete-action for this controller
 *
 *  See gparts_controller_set_copy_action() for a detailed description.
 *
 *  \param [in] controller A pointer to the controller
 *  \param [in] action     A pointer to the \a delete-action
 */
void
gparts_controller_set_delete_action(GPartsController *controller, GtkAction *action);

/*! \brief Set the \a edit-action for this controller
 *
 *  See gparts_controller_set_copy_action() for a detailed description.
 *
 *  \param [in] controller A pointer to the controller
 *  \param [in] action     A pointer to the \a edit-action
 */
void
gparts_controller_set_edit_action(GPartsController *controller, GtkAction *action);

/*! \brief Set the \a insert-action for this controller
 *
 *  See gparts_controller_set_copy_action() for a detailed description.
 *
 *  \param [in] controller A pointer to the controller
 *  \param [in] action     A pointer to the \a insert-action
 */
void
gparts_controller_set_insert_action(GPartsController *controller, GtkAction *action);

/*! \brief Set the \a open-website-action for this controller
 *
 *  See gparts_controller_set_copy_action() for a detailed description.
 *
 *  \param [in] controller A pointer to the controller
 *  \param [in] action     A pointer to the \a open-website-action
 */
void
gparts_controller_set_open_website_action(GPartsController *controller, GtkAction *action);

/*! \brief Set the \a paste-action for this controller
 *
 *  See gparts_controller_set_copy_action() for a detailed description.
 *
 *  \param [in] controller A pointer to the controller
 *  \param [in] action     A pointer to the \a paste-action
 */
void
gparts_controller_set_paste_action(GPartsController *controller, GtkAction *action);


