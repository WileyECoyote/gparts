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

/*! \file gpview-result-adapter.h
 */

#define GPVIEW_TYPE_RESULT_ADAPTER (gpview_result_adapter_get_type())
#define GPVIEW_RESULT_ADAPTER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GPVIEW_TYPE_RESULT_ADAPTER,GPViewResultAdapter))
#define GPVIEW_RESULT_ADAPTER_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),GPVIEW_TYPE_RESULT_ADAPTER,GPViewResultAdapterClass))
#define GPVIEW_IS_RESULT_ADAPTER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GPVIEW_TYPE_RESULT_ADAPTER))
#define GPVIEW_IS_RESULT_ADAPTER_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),GPVIEW_TYPE_RESULT_ADAPTER))
#define GPVIEW_RESULT_ADAPTER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),GPVIEW_TYPE_RESULT_ADAPTER,GPViewResultAdapterClass))

struct _GPViewResultAdapter
{
    GObject parent;
};

/*! \private */
struct _GPViewResultAdapterClass
{
    GObjectClass parent;
};

/*! \private */
GType
gpview_result_adapter_get_type(void);

/*! \brief Adjust the GtkTreeView's columns to match this adapter's.
 *
 *  \public
 *  \memberof _GPViewResultAdapter
 *
 *  \param [in]     adapter
 *  \param [in,out] view
 */
void
gpview_result_adapter_adjust_columns(const GPViewResultAdapter *adapter, GtkTreeView *view);

/*! \brief Get the index of a column, given the name
 *
 *  \param [in]  adapter
 *  \param [in]  name
 *  \param [out] index
 *  \retval TRUE
 *  \retval FALSE
 */
gboolean
gpview_result_adapter_get_column_index(GPViewResultAdapter *adapter, const gchar *name, gint *index);


GStrv
gpview_result_adapter_get_fields(const GPViewResultAdapter *adapter, GtkTreeSelection *selection, gint index);

/*! \brief Get the database result
 *
 *  \param [in] adapter
 *  \return
 */
GPartsDatabaseResult*
gpview_result_adapter_get_result(const GPViewResultAdapter *adapter);

GHashTable*
gpview_result_adapter_get_table(GPViewResultAdapter *adapter, GtkTreeIter *iter);

/*! \brief Create a new adapter
 *
 * \param [in] result
 * \return
 */
GPViewResultAdapter*
gpview_result_adapter_new(GPartsDatabaseResult *result);

