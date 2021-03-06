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

/*! \file gpview-company-view.h
 */

#define GPVIEW_TYPE_COMPANY_VIEW (gpview_company_view_get_type())
#define GPVIEW_COMPANY_VIEW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GPVIEW_TYPE_COMPANY_VIEW,GPViewCompanyView))
#define GPVIEW_COMPANY_VIEW_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),GPVIEW_TYPE_COMPANY_VIEW,GPViewCompanyViewClass))
#define GPVIEW_IS_COMPANY_VIEW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GPVIEW_TYPE_COMPANY_VIEW))
#define GPVIEW_IS_COMPANY_VIEW_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),GPVIEW_TYPE_COMPANY_VIEW))
#define GPVIEW_COMPANY_VIEW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),GPVIEW_TYPE_COMPANY_VIEW,GPViewCompanyViewClass))

/*! \extends GtkContainer */
struct _GPViewCompanyView
{
    GtkScrolledWindow parent;
};

/*! \private */
struct _GPViewCompanyViewClass
{
    GtkScrolledWindowClass parent;
};

/*! \private */
GType
gpview_company_view_get_type(void);

/*! \brief Get the controller associated with this view
 *
 *  Use g_object_free() on the returned pointer when no longer needed.
 *
 *  \param [in] view The view
 *  \return The controller associated with this view
 */
GPViewCompanyCtrl*
gpview_company_view_get_controller(const GPViewCompanyView *view);

/*! \brief Get the selected company IDs
 *
 *  Use g_strfreev() on the returned pointer when no longer needed.
 *
 *  \param [in] view The view [allow none]
 *  \return The selected Company IDs
 */
GStrv
gpview_company_view_get_ids(const GPViewCompanyView *view);

/*! \brief Get the selected company websites
 *
 *  Use g_strfreev() on the returned pointer when no longer needed.
 *
 *  \param [in] view The view [allow none]
 *  \return The selected Company websites
 */
GStrv
gpview_company_view_get_websites(const GPViewCompanyView *view);

/*! \brief Create a new company view
 *
 *  \return A new company view
 */
GPViewCompanyView*
gpview_company_view_new(void);

/*! \brief Create a new company view
 *
 *  \param [in] ctrl The controller to associate with this view
 *  \return A new company view
 */
GPViewCompanyView*
gpview_company_view_new_with_controller(GPViewCompanyCtrl *ctrl);

/*! \brief Set the controller associated with this view
 *
 *  \param [in] view The view
 *  \param [in] ctrl The controller to associate with this view
 */
void
gpview_company_view_set_controller(GPViewCompanyView *view, GPViewCompanyCtrl *ctrl);

