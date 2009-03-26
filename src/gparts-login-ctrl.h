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

/*! \file gparts-login-ctrl.h
 */

#define GPARTS_TYPE_LOGIN_CTRL (gparts_login_ctrl_get_type())
#define GPARTS_LOGIN_CTRL(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GPARTS_TYPE_LOGIN_CTRL,GPartsLoginCtrl))
#define GPARTS_LOGIN_CTRL_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),GPARTS_TYPE_LOGIN_CTRL,GPartsLoginCtrlClass))
#define GPARTS_IS_LOGIN_CTRL(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GPARTS_TYPE_LOGIN_CTRL))
#define GPARTS_IS_LOGIN_CTRL_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),GPARTS_TYPE_LOGIN_CTRL))
#define GPARTS_LOGIN_CTRL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),GPARTS_TYPE_LOGIN_CTRL,GPartsLoginCtrlClass))

typedef struct _GPartsLoginCtrl GPartsLoginCtrl;
typedef struct _GPartsLoginCtrlClass GPartsLoginCtrlClass;

struct _GPartsLoginCtrl
{
    GObject parent;
};

struct _GPartsLoginCtrlClass
{
    GObjectClass parent;
};

GType
gparts_login_ctrl_get_type(void);

/*! \brief Gets the database currently associated with this controller.
 *
 *  \param [in] login_ctrl The login controller.
 *  \return The database associated with this ctrl.  If no database, then NULL.
 */
GPartsDatabase*
gparts_login_ctrl_get_database(GPartsLoginCtrl *login_ctrl);

/*! \brief Sets the action to connect to a database.
 *
 *  \param [in] login_ctrl The login controller.
 *  \param [in] action The action that triggers the connect.
 */
void
gparts_login_ctrl_set_connect_action(GPartsLoginCtrl *login_ctrl, GtkAction *action);

/*! \brief Sets the action to disconnect from the database.
 *
 *  \param [in] login_ctrl The login controller.
 *  \param [in] action The action that triggers the disconnect.
 */
void
gparts_login_ctrl_set_disconnect_action(GPartsLoginCtrl *login_ctrl, GtkAction *action);

/*! \brief Sets the action to refresh all database views.
 *
 *  \param [in] login_ctrl The login controller.
 *  \param [in] action The action that triggers the refresh.
 */
void
gparts_login_ctrl_set_refresh_action(GPartsLoginCtrl *login_ctrl, GtkAction *action);

