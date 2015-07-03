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

/*! \file gparts-result-model.h
 */

#define GPARTS_TYPE_RESULT_MODEL (gparts_result_model_get_type())
#define GPARTS_RESULT_MODEL(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GPARTS_TYPE_RESULT_MODEL,GPartsResultModel))
#define GPARTS_RESULT_MODEL_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),GPARTS_TYPE_RESULT_MODEL,GPartsResultModelClass))
#define GPARTS_IS_RESULT_MODEL(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GPARTS_TYPE_RESULT_MODEL))
#define GPARTS_IS_RESULT_MODEL_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),GPARTS_TYPE_RESULT_MODEL))
#define GPARTS_RESULT_MODEL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),GPARTS_TYPE_RESULT_MODEL,GPartsResultModelClass))

struct _GPartsResultModel
{
    GObject parent;
};

struct _GPartsResultModelClass
{
    GObjectClass parent;
};

unsigned int
gparts_result_model_get_type(void);

GPartsResultModel*
gparts_result_model_new(GPartsDatabaseResult *result);

gboolean
gparts_result_model_get_column_index(GPartsResultModel *result_model, const gchar *name, gint *index);

gchar*
gparts_result_model_get_field(GPartsResultModel *model, GtkTreeIter *iter, const gchar *name);

void
gparts_result_model_set_columns(GPartsDatabaseResult *result, GtkTreeView *tree_view);

/*! \brief Set the database result for this model
 *
 *  \param [in] model
 *  \param [in] result
 */
void
gparts_result_model_set_result(GPartsResultModel *model, GPartsDatabaseResult *result);


