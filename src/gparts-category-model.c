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

/*! \file gparts-category-model.c
 *
 *  \brief Provides a GtkTreeModel for the category view.
 *
 *  Unlike most of the views in GParts, the category view organizes data
 *  in a tree structure.
 */

#include <string.h>

#include "gpartsui.h"

enum
{
    GPARTS_CATEGORY_MODEL_PROPID_DATABASE = 1
};

typedef struct _GPartsCategoryModelPrivate GPartsCategoryModelPrivate;


typedef struct _GPartsCategoryModelNode GPartsCategoryModelNode;

struct _GPartsCategoryModelNode
{
    GtkTreeIter                     parent;
    GPartsDatabaseResult            *result;
    struct _GPartsCategoryModelNode **children;
};

struct _GPartsCategoryModelPrivate
{
    GPartsDatabase          *database;
    GPartsCategoryModelNode *root;
    int                     stamp;
};

#define GPARTS_CATEGORY_MODEL_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE(object, GPARTS_TYPE_CATEGORY_MODEL, GPartsCategoryModelPrivate)

static void
gparts_category_model_instance_init(GTypeInstance* instance, gpointer g_class);

static void
gparts_category_model_dispose(GObject* object);

static void
gparts_category_model_finalize(GObject* object);

static void
gparts_category_model_get_property(GObject* object, guint property_id, GValue* value, GParamSpec* pspec);

static void
gparts_category_model_set_property(GObject* object, guint property_id, const GValue* value, GParamSpec* pspec);

static void
gparts_load_children(GPartsDatabase* database, GtkTreeIter* iter);

static void
gparts_category_model_tree_model_init(GtkTreeModelIface *iface);

static void
gparts_category_model_set_database(GObject *object, const GValue *value);

static gboolean
gparts_category_model_iter_nth_child(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, int  n);
/**
 *
 *
 *
 *
 */
static void
gparts_load_root(GPartsDatabase* database, GPartsCategoryModelNode **node)
{
    *node = g_new0(GPartsCategoryModelNode, 1);

    gchar* query = "SELECT * FROM Category WHERE ParentID IS NULL";

    (*node)->result = gparts_database_query(database, query, NULL);

    if ((*node)->result != NULL)
    {
        int  rows = gparts_database_result_get_row_count((*node)->result);

        if (rows > 0)
        {
            (*node)->children = g_new0(GPartsCategoryModelNode, rows);
        }
    }
}

/*!
 *
 *
 *
 */
static void
gparts_category_model_load_children(GPartsDatabase* database, GtkTreeIter *iter)
{
    GPartsCategoryModelNode **child;
    int  index;
    GPartsCategoryModelNode *node;

    node = (GPartsCategoryModelNode*) iter->user_data;
    index = GPOINTER_TO_INT(iter->user_data2);
    child = node->children + index;

    if (*child == NULL)
    {
        GValue value0 = {0};
        GValue value1 = {0};

        *child = g_new0(GPartsCategoryModelNode, 1);

        (*child)->parent = *iter;

        gparts_database_result_get_field_value(node->result, index, 0, &value0);

        g_value_init(&value1, G_TYPE_STRING);
        g_value_transform(&value0, &value1);
        g_value_unset(&value0);

        GString *query = g_string_new(NULL);

        g_string_printf(query, "SELECT * FROM Category WHERE ParentID = %s", g_value_get_string(&value1));
        g_value_unset(&value1);

        (*child)->result = gparts_database_query(database, query->str, NULL);
        g_string_free(query, TRUE);

        (*child)->children = g_new0(GPartsCategoryModelNode, gparts_database_result_get_row_count((*child)->result));
    }
}

/*! \brief
 *
 *
 *
 */
static void
gparts_category_model_database_connected_cb(GPartsDatabase *database)
{
}

/*! \brief
 *
 *
 *
 *
 *
 */
static void
gparts_category_model_database_disconnected_cb(GPartsDatabase *database)
{
}

/*! \brief Initialize class data.
 *
 *  \param [in] g_class
 *  \param [in] g_class_data
 */
static void
gparts_category_model_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass* object_class = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(
        g_class,
        sizeof( GPartsCategoryModelPrivate )
        );

    object_class->dispose      = gparts_category_model_dispose;
    object_class->finalize     = gparts_category_model_finalize;
    object_class->get_property = gparts_category_model_get_property;
    object_class->set_property = gparts_category_model_set_property;

    g_object_class_install_property(
        object_class,
        GPARTS_CATEGORY_MODEL_PROPID_DATABASE,
        g_param_spec_object(
            "database",
            "",
            "",
            G_TYPE_OBJECT,    /* \todo See if this can be an interface */
            G_PARAM_READWRITE
            )
        );
}

/** \brief Unreference all referenced objects.
 *
 *  This function provides the first stage of object destruction.
 *
 *  \param [in] object The object to dispose.
 */
static void
gparts_category_model_dispose(GObject *object)
{
    GPartsCategoryModelPrivate *private;

    private = GPARTS_CATEGORY_MODEL_GET_PRIVATE(object);

    gparts_category_model_set_database(object, NULL);

  //  g_object_unref(private->tree_store);
    misc_object_chain_dispose(object);
}

/*! \brief Deallocate all resources.
 *
 *  This function performs the second and last stage of object destruction.
 *
 *  \param [in] object The object to finalize.
 */
static void
gparts_category_model_finalize(GObject *object)
{
    misc_object_chain_finalize(object);
}

gchar*
gparts_category_model_get_field(GPartsCategoryModel* model, GtkTreeIter *iter, const gchar *name)
{
    GPartsCategoryModelNode *node = (GPartsCategoryModelNode*) iter->user_data;
    GPartsCategoryModelPrivate *private = GPARTS_CATEGORY_MODEL_GET_PRIVATE(model);
    gchar *result = NULL;

    if (node->result != NULL)
    {
        int  index;
        gboolean success;

        success = gparts_database_result_get_column_index(node->result, name, &index);

        if (success)
        {
            gboolean error = FALSE;
            GString *buffer = g_string_sized_new(10);
            GValue value = {0};

            gparts_database_result_get_field_value(
                node->result,
                GPOINTER_TO_UINT(iter->user_data2),
                index,
                &value
                );

            /* TODO The following code needs to go elsewhere or use transformations */

            if(G_IS_VALUE(&value))
            {
                if (G_VALUE_HOLDS_STRING(&value))
                {
                    g_string_printf(buffer, "%s", g_value_get_string(&value));
                }
                else if (G_VALUE_HOLDS_DOUBLE(&value))
                {
                    g_string_printf(buffer, "%f", g_value_get_double(&value));
                }
                else if (G_VALUE_HOLDS_INT(&value))
                {
                    g_string_printf(buffer, "%d", g_value_get_int(&value));
                }
                else
                {
                    error = TRUE;
                }

                result = g_string_free(buffer, error);
            }
        }
    }

    return result;
}

/*! \brief
 *
 *
 *
 *
 */
static void
gparts_category_model_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    GPartsCategoryModelPrivate *private;

    private = GPARTS_CATEGORY_MODEL_GET_PRIVATE(object);

    /*	\todo handle case where private is NULL */

    switch ( property_id )
    {
        case GPARTS_CATEGORY_MODEL_PROPID_DATABASE:
            g_value_set_object(value, private->database);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

/*! \brief
 *
 *
 *
 *
 */
GType
gparts_category_model_get_type( void )
{
    static GType type = 0;

    if ( type == 0 )
    {
        static const GTypeInfo tinfo = {
            sizeof(GPartsCategoryModelClass),
            NULL,
            NULL,
            gparts_category_model_class_init,
            NULL,
            NULL,
            sizeof(GPartsCategoryModel),
            0,
            gparts_category_model_instance_init,
            NULL
            };

        static const GInterfaceInfo iinfo = {
            (GInterfaceInitFunc) gparts_category_model_tree_model_init,
            NULL,
            NULL
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "gparts-category-model",
            &tinfo,
            0
            );

        g_type_add_interface_static(
            type,
            GTK_TYPE_TREE_MODEL,
            &iinfo
            );
    }
    return type;
}

/*! \brief Initialize instance data.
 *
 *  \param instance
 *  \param g_class
 */
static void
gparts_category_model_instance_init(GTypeInstance* instance, gpointer g_class)
{
    GPartsCategoryModelPrivate *private;

    private = GPARTS_CATEGORY_MODEL_GET_PRIVATE(instance);

}

GPartsCategoryModel *
gparts_category_model_new(GPartsDatabase *database)
{
    return g_object_new(GPARTS_TYPE_CATEGORY_MODEL, "database", database, NULL);
}

/*! \brief
 *
 *
 *
 *
 */
static void
gparts_category_model_set_database(GObject *object, const GValue *value)
{
    GPartsCategoryModelPrivate *private;

    private = GPARTS_CATEGORY_MODEL_GET_PRIVATE(object);

    if (private->database != NULL)
    {
        g_signal_handlers_disconnect_by_func(
            private->database,
            G_CALLBACK(gparts_category_model_database_connected_cb),
            object
            );

        g_signal_handlers_disconnect_by_func(
            private->database,
            G_CALLBACK(gparts_category_model_database_disconnected_cb),
            object
            );

        g_object_unref(private->database);

        private->database = NULL;

        //gtk_tree_store_clear( private->tree_store );
    }

    /* 	odo Check for correct type before setting database */

    if (TRUE)
    {
        private->database = g_value_get_object(value);
    }

    if (private->database != NULL)
    {
        g_object_ref(private->database);

        g_signal_connect(
            private->database,
            "database-connected",
            G_CALLBACK(gparts_category_model_database_connected_cb),
            object
            );

        g_signal_connect(
            private->database,
            "database-disconnected",
            G_CALLBACK(gparts_category_model_database_disconnected_cb),
            object
            );

        gparts_load_root(private->database, &(private->root));
    }
}

/*! \brief
 *
 *
 *
 *
 */
static void
gparts_category_model_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    switch ( property_id )
    {
        case GPARTS_CATEGORY_MODEL_PROPID_DATABASE:
            gparts_category_model_set_database(object, value);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

/**
 *
 *
 *
 */
static GType
gparts_category_model_get_column_type(GtkTreeModel *tree_model, int  index)
{
    GPartsCategoryModelPrivate *private = GPARTS_CATEGORY_MODEL_GET_PRIVATE(tree_model);

    return gparts_database_result_get_column_type(private->root->result, index);
}

/**
 *
 *
 *
 */
static gboolean
gparts_category_model_get_iter(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path)
{
    GPartsCategoryModelPrivate *private = GPARTS_CATEGORY_MODEL_GET_PRIVATE(tree_model);


    int  c =0;
   int  d = gtk_tree_path_get_depth(path);

    int  index = gtk_tree_path_get_indices(path)[c++];

    GtkTreeIter pos;

    gboolean success = gparts_category_model_iter_nth_child(tree_model, &pos, NULL, index);

       if (!success) return FALSE;
    while (c<d)
    {
        GtkTreeIter zippy;

        zippy = pos;

       index = gtk_tree_path_get_indices(path)[c++];

       gboolean success = gparts_category_model_iter_nth_child(tree_model, &pos, &zippy, index);

       if (!success) return FALSE;
    }

        *iter = pos;

    return TRUE;
}

/**
 *
 *
 *
 */
static GtkTreeModelFlags
gparts_category_model_get_flags(GtkTreeModel *tree_model)
{
    return 0; //GTK_TREE_MODEL_LIST_ONLY;  //FIXME temp
}

/** @brief Returns the number of columns in the treemodel
 *
 *
 *
 */
static int
gparts_category_model_get_n_columns(GtkTreeModel *tree_model)
{
    int  columns = 0;
    GPartsCategoryModelPrivate *private = GPARTS_CATEGORY_MODEL_GET_PRIVATE(tree_model);

    if (private->root->result != NULL)
    {
        columns = gparts_database_result_get_column_count(private->root->result);
    }

    return columns;
}

/**
 *
 *
 *
 */
static GtkTreePath*
gparts_category_model_get_path(GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    GPartsCategoryModelPrivate *private = GPARTS_CATEGORY_MODEL_GET_PRIVATE(tree_model);

    g_debug("gparts_category_model_get_path");

    g_assert(FALSE);

}

/*! \brief Retrieve a value from the tree model.
 *
 *
 *
 */
static void
gparts_category_model_get_value(GtkTreeModel *tree_model, GtkTreeIter *iter, int  column, GValue *value)
{
    GPartsCategoryModelNode *node = (GPartsCategoryModelNode*) iter->user_data;

    gparts_database_result_get_field_value(
        node->result,
        GPOINTER_TO_UINT(iter->user_data2),
        column,
        value
        );

    /* \todo Find a better way to handle NULLs from the database */

    if (!G_IS_VALUE(value))
    {
        g_value_init(value, G_TYPE_STRING);
        g_value_set_static_string(value, "NULL");
    }
}

/*!
 *
 *
 *
 */
static gboolean
gparts_category_model_iter_children(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent)
{
    gparts_category_model_iter_nth_child(tree_model, iter, parent, 0);
}

/**
 *
 *
 *
 */
static gboolean
gparts_category_model_iter_has_child(GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    GPartsCategoryModelPrivate *private = GPARTS_CATEGORY_MODEL_GET_PRIVATE(tree_model);

    gparts_category_model_load_children(private->database, iter);

    GPartsCategoryModelNode *node = (GPartsCategoryModelNode*) iter->user_data;
    int  index = GPOINTER_TO_INT(iter->user_data2);

    GPartsCategoryModelNode **child = node->children + index;

    if ((*child)->result != NULL)
    {
        if ( gparts_database_result_get_row_count((*child)->result) > 0 )
            return TRUE;
    }

    return FALSE;
}

/*!
 *
 *
 *
 */
static gboolean
gparts_category_model_iter_next(GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    guint new_position;
    guint old_position;
    GPartsCategoryModelPrivate *private = GPARTS_CATEGORY_MODEL_GET_PRIVATE(tree_model);

    g_assert(iter != NULL);
    g_assert(iter->stamp == private->stamp);

    old_position = GPOINTER_TO_UINT(iter->user_data2);
    new_position = old_position + 1;

    if (new_position > old_position)
    {
        GPartsCategoryModelNode *node = (GPartsCategoryModelNode*) iter->user_data;

        int  rows = gparts_database_result_get_row_count(node->result);

        if (new_position < rows)
        {
            iter->user_data2 = GUINT_TO_POINTER(new_position);

            return TRUE;
        }
    }

    memset(iter, 0, sizeof(*iter));

    return FALSE;
}

/*! \brief Gets an iterator for the nth child of the parent node.
 *
 *  \param [in]  tree_model The GtkTreeModel.
 *  \param [out] iter       The iterator to the nth child of parent.
 *  \param [in]  parent     An iterator referencing the parent node.
 *  \param [in]  n          Zero based index of the child of the parent to get.
 *  \return TRUE if successful and iter is valid.  FALSE if otherwise.
 */
static gboolean
gparts_category_model_iter_nth_child(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, int  n)
{
    GPartsCategoryModelNode *list;
    GPartsCategoryModelPrivate *private = GPARTS_CATEGORY_MODEL_GET_PRIVATE(tree_model);

    g_assert(iter != NULL);

    if (parent == NULL)
    {
        list = private->root;
    }
    else
    {
        g_assert(parent->stamp == private->stamp);

        GPartsCategoryModelNode *parent_list = (GPartsCategoryModelNode*) parent->user_data;

        g_assert(parent_list != NULL);
        g_assert(parent_list->result != NULL);

        int  offset = GPOINTER_TO_INT(parent->user_data2);

        int  rows = gparts_database_result_get_row_count(parent_list->result);

        g_assert(offset < rows);

        list = *(parent_list->children + offset);
    }

    if (list != NULL && list->result != NULL)
    {
        int  rows = gparts_database_result_get_row_count(list->result);

        if (n < rows)
        {
            iter->stamp = private->stamp;
            iter->user_data = list;
            iter->user_data2 = GINT_TO_POINTER(n);

            return TRUE;
        }
    }

    memset(iter, 0, sizeof(*iter));

    return FALSE;
}

/**
 *
 *
 *
 */
static gboolean
gparts_category_model_iter_n_children(GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    GPartsCategoryModelPrivate *private;

    private = GPARTS_CATEGORY_MODEL_GET_PRIVATE(tree_model);

    return FALSE;

 //   return gtk_tree_model_iter_n_children(GTK_TREE_MODEL(private->tree_store), iter);
}

/*! \brief Gets an iterator for the parent node of a given child node.
 *
 *  \param [in]  tree_model The GtkTreeModel.
 *  \param [out] iter       The parent of the child node.
 *  \param [in]  child      An iterator referencing the child node.
 *  \return TRUE successful and iter is valid.  FALSE if otherwise.
 */
static gboolean
gparts_category_model_iter_parent(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child)
{
    GPartsCategoryModelPrivate *private = GPARTS_CATEGORY_MODEL_GET_PRIVATE(tree_model);

    GPartsCategoryModelNode *node = (GPartsCategoryModelNode*) child->user_data;

    if (node->parent.user_data != NULL)
    {
        *iter = node->parent;

        return TRUE;
    }

    memset(iter, 0, sizeof(*iter));

    return FALSE;
}

/*! \brief
 *
 *
 *
 */
static void
gparts_category_model_tree_model_init(GtkTreeModelIface *iface)
{
    iface->get_flags       = gparts_category_model_get_flags;
    iface->get_n_columns   = gparts_category_model_get_n_columns;
    iface->get_column_type = gparts_category_model_get_column_type;
    iface->get_iter        = gparts_category_model_get_iter;
    iface->get_path        = gparts_category_model_get_path;
    iface->get_value       = gparts_category_model_get_value;
    iface->iter_next       = gparts_category_model_iter_next;
    iface->iter_children   = gparts_category_model_iter_children;
    iface->iter_has_child  = gparts_category_model_iter_has_child;
    iface->iter_n_children = gparts_category_model_iter_n_children;
    iface->iter_nth_child  = gparts_category_model_iter_nth_child;
    iface->iter_parent     = gparts_category_model_iter_parent;
}

/*! \brief Hides the columns not needed for general use.
 *
 *  Before calling this function, the tree view must be set with the columns
 *  for a GPartsCategoryModel.
 *
 *  \param [in] tree_view The tree view with the columns to hide.
 */
void
gparts_category_model_hide_columns(GtkTreeView *tree_view)
{
    int  index = 0;
    GtkTreeViewColumn *column;

    column = gtk_tree_view_get_column(tree_view, index++);

    while (column != NULL) {

        const gchar *name = gtk_tree_view_column_get_title(column);
        gboolean visible = (name != NULL) && (strcmp(name, "CategoryName") == 0);

        gtk_tree_view_column_set_visible(column, visible);

        column = gtk_tree_view_get_column(tree_view, index++);
    }
}

/*! \brief
 *
 *
 *
 */
void
gparts_category_model_set_columns(GPartsCategoryModel* model, GtkTreeView *tree_view)
{
    GPartsCategoryModelPrivate *private = GPARTS_CATEGORY_MODEL_GET_PRIVATE(model);

    if (private->root->result != NULL)
    {
        gparts_result_model_set_columns(private->root->result, tree_view);

        gparts_category_model_hide_columns(tree_view);
    }
}
