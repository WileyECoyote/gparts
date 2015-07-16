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

/*! \file gpview-part-ctrl.h
 */

#define GPVIEW_TYPE_PART_CTRL (gpview_part_ctrl_get_type())
#define GPVIEW_PART_CTRL(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GPVIEW_TYPE_PART_CTRL,GPViewPartCtrl))
#define GPVIEW_PART_CTRL_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),GPVIEW_TYPE_PART_CTRL,GPViewPartCtrlClass))
#define GPVIEW_IS_PART_CTRL(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GPVIEW_TYPE_PART_CTRL))
#define GPVIEW_IS_PART_CTRL_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),GPVIEW_TYPE_PART_CTRL))
#define GPVIEW_PART_CTRL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),GPVIEW_TYPE_PART_CTRL,GPViewPartCtrlClass))

/*! \extends GObject */
struct _GPViewPartCtrl
{
    GObject parent;
};

/*! \private */
struct _GPViewPartCtrlClass
{
    GObjectClass parent;
};

/*! \private */
GType
gpview_part_ctrl_get_type(void);

GPViewPartCtrl*
gpview_part_ctrl_new(void);

GPViewPartCtrl*
gpview_part_ctrl_new_with_manager(GtkUIManager *manager);

void
gpview_part_ctrl_set_current_view(GPViewPartCtrl *ctrl, GPViewPartView *view);

void
gpview_part_ctrl_set_form_factory(GPViewPartCtrl *ctrl, GPFormFactory *factory);

void
gpview_part_ctrl_set_ui_manager(GPViewPartCtrl *ctrl, GtkUIManager *manager);

void
gpview_part_ctrl_set_database(GPViewPartCtrl *ctrl, GPartsDatabase *database);
