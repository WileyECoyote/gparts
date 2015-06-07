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

/*! \file gparts-category-model.h
 *
 *  \brief Provides a GtkTreeModel for the category view.
 *
 *  Unlike most of the views in GParts, the category view organizes data
 *  in a tree structure.
 */

#define GPARTS_TYPE_CATEGORY_MODEL (gparts_category_model_get_type())
#define GPARTS_CATEGORY_MODEL(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GPARTS_TYPE_CATEGORY_MODEL,GPartsCategoryModel))
#define GPARTS_CATEGORY_MODEL_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),GPARTS_TYPE_CATEGORY_MODEL,GPartsCategoryModelClass))
#define GPARTS_IS_CATEGORY_MODEL(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GPARTS_TYPE_CATEGORY_MODEL))
#define GPARTS_IS_CATEGORY_MODEL_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),GPARTS_TYPE_CATEGORY_MODEL))
#define GPARTS_CATEGORY_MODEL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),GPARTS_TYPE_CATEGORY_MODEL,GPartsCategoryModelClass))

struct _GPartsCategoryModel
{
    GObject parent;
};

struct _GPartsCategoryModelClass
{
    GObjectClass parent;
};

GType
gparts_category_model_get_type( void );

GPartsCategoryModel*
gparts_category_model_new(GPartsDatabase *database);

/*! \brief TODO Finish
 *
 *  \param [in] model The model containing the field.
 *  \param [in] iter An iterator to the selected row.
 *  \param [in] name The column name of the field.
 *  \return The field value as a string.  This value must be freed when no
 *  longer needed.
 */
char* gparts_category_model_get_field(GPartsCategoryModel *model, GtkTreeIter *iter, const char *name);

/*! TODO Finish
 */
void gparts_category_model_set_columns(GPartsCategoryModel* model, GtkTreeView *tree_view);

void gparts_category_model_hide_columns(GtkTreeView *tree_view);
