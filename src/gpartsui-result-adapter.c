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

/*! \file gpartsui-result-adapter.c
 */

#include "gpartsui.h"

#define GPARTSUI_RESULT_ADAPTER_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE(object,GPARTSUI_TYPE_RESULT_ADAPTER,GPartsUIResultAdapterPrivate)

enum
{
    GPARTSUI_RESULT_ADAPTER_PROPID_RESULT = 1
};

typedef struct _GPartsUIResultAdapterGetFieldsProcData GPartsUIResultAdapterGetFieldsProcData;
typedef struct _GPartsUIResultAdapterPrivate GPartsUIResultAdapterPrivate;

struct _GPartsUIResultAdapterPrivate
{
    GPartsDatabaseResult *result;
    int                   stamp;
};

struct _GPartsUIResultAdapterGetFieldsProcData
{
   const
   GPartsUIResultAdapter *adapter;
   GPtrArray             *array;
   int                    index;
};

static void
gpartsui_result_adapter_class_init(void *g_class, void *g_class_data);

static void
gpartsui_result_adapter_dispose(GObject *object);

static void
gpartsui_result_adapter_finalize(GObject *object);

static GType
gpartsui_result_adapter_get_column_type(GtkTreeModel *tree_adapter, int index);

static void
gpartsui_result_adapter_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec);

static GtkTreeModelFlags
gpartsui_result_adapter_get_flags(GtkTreeModel *tree_adapter);

static int
gpartsui_result_adapter_get_iter(GtkTreeModel *tree_adapter, GtkTreeIter *iter, GtkTreePath *path);

static int
gpartsui_result_adapter_get_n_columns(GtkTreeModel *tree_adapter);

static GtkTreePath*
gpartsui_result_adapter_get_path(GtkTreeModel *tree_adapter, GtkTreeIter *iter);

static void
gpartsui_result_adapter_get_value(GtkTreeModel *tree_adapter, GtkTreeIter *iter, int column, GValue *value);

static void
gpartsui_result_adapter_instance_init(GTypeInstance *instance, void *g_class);

/*static*/ int
gpartsui_result_adapter_iter_children(GtkTreeModel *tree_adapter, GtkTreeIter *iter, GtkTreeIter *parent);

static int
gpartsui_result_adapter_iter_has_child(GtkTreeModel *tree_adapter, GtkTreeIter *iter);

static int
gpartsui_result_adapter_iter_next(GtkTreeModel *tree_adapter, GtkTreeIter *iter);

static int
gpartsui_result_adapter_iter_n_children(GtkTreeModel *tree_adapter, GtkTreeIter *iter);

static int
gpartsui_result_adapter_iter_nth_child(GtkTreeModel *tree_adapter, GtkTreeIter *iter, GtkTreeIter *parent, int n);

static int
gpartsui_result_adapter_iter_parent(GtkTreeModel *tree_adapter, GtkTreeIter *iter, GtkTreeIter *child);

static void
gpartsui_result_adapter_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec);

static void
gpartsui_result_adapter_set_result(GPartsUIResultAdapter *adapter, GPartsDatabaseResult *result);

static void
gpartsui_result_adapter_tree_adapter_init(GtkTreeModelIface *iface);

static void
gpartsui_result_adapter_get_fields_proc(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, void *user_data);


static void
gpartsui_result_adapter_class_init(void *g_class, void *g_class_data)
{
    GObjectClass* object_class = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(
        g_class,
        sizeof(GPartsUIResultAdapterPrivate)
        );

    object_class->dispose      = gpartsui_result_adapter_dispose;
    object_class->finalize     = gpartsui_result_adapter_finalize;
    object_class->get_property = gpartsui_result_adapter_get_property;
    object_class->set_property = gpartsui_result_adapter_set_property;

    g_object_class_install_property(
        object_class,
        GPARTSUI_RESULT_ADAPTER_PROPID_RESULT,
        g_param_spec_object(
            "result",
            "Database Result",
            "Database Result",
            GPARTS_TYPE_DATABASE_RESULT,
            G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );
}

static void
gpartsui_result_adapter_dispose(GObject *object)
{
    gpartsui_result_adapter_set_result(GPARTSUI_RESULT_ADAPTER(object), NULL);

    misc_object_chain_dispose(object);
}

static void
gpartsui_result_adapter_finalize(GObject *object)
{
    misc_object_chain_finalize(object);
}

int
gpartsui_result_adapter_get_column_index(GPartsUIResultAdapter *result_adapter, const gchar *name, int *index)
{
    GPartsUIResultAdapterPrivate *private = GPARTSUI_RESULT_ADAPTER_GET_PRIVATE(result_adapter);

    return gparts_database_result_get_column_index(private->result, name, index);
}

/*! \brief Gets the GType of a given column.
 *
 *  \param [in] tree_adapter The GPartsUIResultAdapter.
 *  \param [in] index      The zero based index of the column.
 *  \return The column's GType.
 */
static GType
gpartsui_result_adapter_get_column_type(GtkTreeModel *tree_adapter, int index)
{
    GPartsUIResultAdapterPrivate *privat = GPARTSUI_RESULT_ADAPTER_GET_PRIVATE(tree_adapter);
    GType type = G_TYPE_INVALID;

    if (privat != NULL) {

        type = gparts_database_result_get_column_type(privat->result, index);
    }

    return type;
}

gchar*
gpartsui_result_adapter_get_field(GPartsUIResultAdapter *model, GtkTreeIter *iter, const gchar *name)
{
  GPartsUIResultAdapterPrivate *privat = GPARTSUI_RESULT_ADAPTER_GET_PRIVATE(model);
  gchar *result = NULL;

  if ((privat != NULL) && (privat->result != NULL)) {

    int index;
    int success;

    success = gparts_database_result_get_column_index(privat->result, name, &index);

    if (success)
    {
      int error = FALSE;
      GString *buffer = g_string_sized_new(10);
      GValue value = {0};

      gparts_database_result_get_field_value(
        privat->result,
        GPOINTER_TO_UINT(iter->user_data),
                                             index,
                                             &value);

      if(G_IS_VALUE(&value)) {

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
        else if (g_value_type_transformable(G_VALUE_TYPE(&value), G_TYPE_STRING))
        {
          GValue temp = {0};

          g_value_init(&temp, G_TYPE_STRING);
          g_value_transform(&value, &temp);

          g_string_printf(buffer, "%s", g_value_get_string(&temp));
          g_value_unset(&temp);
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

GStrv
gpartsui_result_adapter_get_fields(const GPartsUIResultAdapter *adapter, GtkTreeSelection *selection, int index)
{
    GStrv fields = NULL;

    if (selection != NULL) {

        GPartsUIResultAdapterGetFieldsProcData data;

        /*  GPartsUIResultAdapter * = GPartsUIResultAdapter*/
        data.adapter = adapter;
        data.array   = NULL;
        data.index   = index;

        gtk_tree_selection_selected_foreach(selection, gpartsui_result_adapter_get_fields_proc, &data);

        if (data.array != NULL) {

            if (data.array->len > 0) {

                g_ptr_array_add(data.array, NULL);

                fields = (GStrv) g_ptr_array_free(data.array, FALSE);
            }
            else {

                g_ptr_array_free(data.array, TRUE);
            }
        }
    }

    return fields;
}

static void
gpartsui_result_adapter_get_fields_proc(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, void *user_data)
{
  GPartsUIResultAdapterGetFieldsProcData *data = (GPartsUIResultAdapterGetFieldsProcData*) user_data;

  if (data != NULL) {

    GPartsUIResultAdapterPrivate *privat = GPARTSUI_RESULT_ADAPTER_GET_PRIVATE(data->adapter);

    if (privat != NULL) {

      GValue value = {0};

      gparts_database_result_get_field_value(
        privat->result,
        GPOINTER_TO_UINT(iter->user_data),
                                             data->index,
                                             &value
      );

      if(G_IS_VALUE(&value)) {

        GString *buffer = g_string_sized_new(10);
        int error = FALSE;

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
        else if (g_value_type_transformable(G_VALUE_TYPE(&value), G_TYPE_STRING))
        {
          GValue temp = {0};

          g_value_init(&temp, G_TYPE_STRING);
          g_value_transform(&value, &temp);

          g_string_printf(buffer, "%s", g_value_get_string(&temp));
          g_value_unset(&temp);
        }
        else
        {
          error = TRUE;
        }

        if (!error)
        {
          if (buffer->str[0] != '\0')
          {
            if (data->array == NULL)
            {
              data->array = g_ptr_array_new();
            }

            g_ptr_array_add(data->array, g_string_free(buffer, FALSE));
          }
          else
          {
            g_string_free(buffer, TRUE);
          }
        }
        else
        {
          g_string_free(buffer, TRUE);
        }
      }
    }
  }
}

/*! \brief
 *
 *
 *
 *
 */
static void
gpartsui_result_adapter_get_property(GObject *object, unsigned int property_id, GValue *value, GParamSpec *pspec)
{
    GPartsUIResultAdapterPrivate *privat = GPARTSUI_RESULT_ADAPTER_GET_PRIVATE(object);

    if (privat != NULL) {

        switch ( property_id ) {

            case GPARTSUI_RESULT_ADAPTER_PROPID_RESULT:
                g_value_set_object(value, privat->result);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

/*! \brief Gets this model's GtkTreeModelFlags.
 *
 *  \param [in]  tree_adapter The GPartsUIResultAdapter.
 *  \return This model's GtkTreeModelFlags.
 */
static GtkTreeModelFlags
gpartsui_result_adapter_get_flags(GtkTreeModel *tree_adapter)
{
    return GTK_TREE_MODEL_LIST_ONLY;
}


/*! \brief Converts a tree path to a tree iterator.
 *
 *  This function will fail if the path does not refer to a valid node in the
 *  tree model.
 *
 *  \param [in]  tree_adapter The GPartsUIResultAdapter
 *  \param [out] iter       If successful, a GtkTreeIter.  Invalid if unsuccessful.
 *  \param [in]  path       The GtkTreePath to convert to a GtkTreeIter.
 *  \return TRUE if successful and iter is valid.  FALSE if otherwise and the
 *  iter will not be valid.
 */
static int
gpartsui_result_adapter_get_iter(GtkTreeModel *tree_adapter, GtkTreeIter *iter, GtkTreePath *path)
{
    GPartsUIResultAdapterPrivate *private = GPARTSUI_RESULT_ADAPTER_GET_PRIVATE(tree_adapter);

    int index = gtk_tree_path_get_indices(path)[0];

    unsigned int rows = gparts_database_result_get_row_count(private->result);

    if (index < rows) {

        iter->stamp = private->stamp;
        iter->user_data = GUINT_TO_POINTER(index);

        return TRUE;
    }

    return FALSE;
}
/*! \brief Returns the number of columns in the tree model.
 *
 *  \param [in] tree_adapter The GPartsUIResultAdapter.
 *  \param [in] tree_adapter The given tree model.
 *  \return The number of columns in the tree model.
 */
static int
gpartsui_result_adapter_get_n_columns(GtkTreeModel *tree_adapter)
{
    GPartsUIResultAdapterPrivate *private =
    GPARTSUI_RESULT_ADAPTER_GET_PRIVATE(tree_adapter);

    return gparts_database_result_get_column_count(private->result);
}

/*! \brief Converts a GtkTreeIter to a GtkTreePath.
 *
 *  \param [in] tree_adapter The GPartsUIResultAdapter.
 *  \param [in] iter       The iterator to convert to a tree path.
 *  \return A GtkTreePath that refers to the same node as the iterator.
 */
static GtkTreePath*
gpartsui_result_adapter_get_path(GtkTreeModel *tree_adapter, GtkTreeIter *iter)
{
    GPartsUIResultAdapterPrivate *private =
    GPARTSUI_RESULT_ADAPTER_GET_PRIVATE(tree_adapter);

    g_assert(iter != NULL);
    g_assert(iter->stamp == private->stamp);

    GtkTreePath *tree_path = gtk_tree_path_new();

    gtk_tree_path_append_index(tree_path, GPOINTER_TO_INT(iter->user_data));

    return tree_path;
}

/*! \brief Register this class's GType.
 *
 *  \return The GType of this class.
 */
GType
gpartsui_result_adapter_get_type( void )
{
  static GType type = 0;

  if ( type == 0 ) {

    static const GTypeInfo tinfo = {
      sizeof(GPartsUIResultAdapterClass),
      NULL,
      NULL,
      gpartsui_result_adapter_class_init,
      NULL,
      NULL,
      sizeof(GPartsUIResultAdapter),
      0,
      gpartsui_result_adapter_instance_init,
      NULL
    };

    static const GInterfaceInfo iinfo = {
      (GInterfaceInitFunc) gpartsui_result_adapter_tree_adapter_init,
      NULL,
      NULL
    };

    type = g_type_register_static(
      G_TYPE_OBJECT,
      "gpartsui-result-adapter",
      &tinfo,
      0);

    g_type_add_interface_static(
      type,
      GTK_TYPE_TREE_MODEL,
      &iinfo);
  }
  return type;
}

/*! \brief Get a value from the tree model.
 *
 *  This function turns invalid values into blanks.  So, an invalid column or
 *  row will still result in a valid GValue.
 *
 *  \param [in]  tree_adapter The GPartsUIResultAdapter.
 *  \param [in]  iter       An iterator referencing the value's row.
 *  \param [in]  column     A zero based index of the value's column.
 *  \param [out] value      The value of the cell as a GValue.
 */
static void
gpartsui_result_adapter_get_value(GtkTreeModel *tree_adapter, GtkTreeIter *iter, int column, GValue *value)
{
    GPartsUIResultAdapterPrivate *private = GPARTSUI_RESULT_ADAPTER_GET_PRIVATE(tree_adapter);

    g_assert(iter != NULL);
    g_assert(iter->stamp == private->stamp);

    gparts_database_result_get_field_value(private->result,
        GPOINTER_TO_UINT(iter->user_data),
        column,
        value);

    /* Turn uninitialized values into blanks */

    if (!G_IS_VALUE(value)) {
        g_value_init(value, G_TYPE_STRING);
    }
}

/*! \brief Initialize instance data.
 *
 *  \param instance
 *  \param g_class
 */
static void
gpartsui_result_adapter_instance_init(GTypeInstance* instance, void *g_class)
{
    GPartsUIResultAdapterPrivate *privat = GPARTSUI_RESULT_ADAPTER_GET_PRIVATE(instance);

    if (privat != NULL) {
        privat->stamp = g_random_int();
    }
}

/*! \brief Gets an iterator to a list's first child.
 *
 *  If the parent is NULL, this function gets an iterator to the first node in
 *  the root list.  Since this model does not implement trees, all other cases
 *  return FALSE.
 *
 *  \param [in]  tree_adapter The GPartsUIResultAdapter.
 *  \param [out] iter       If successful, the first child of parent.
 *  \param [in]  parent     The parent node.
 *  \retval TRUE  if successful and iter is valid.
 *  \retval FALSE if otherwise and the iter will not be valid.
 */
/*static*/ int
gpartsui_result_adapter_iter_children(GtkTreeModel *tree_adapter, GtkTreeIter *iter, GtkTreeIter *parent)
{
    int success = FALSE;

    if (parent == NULL) {

      GPartsUIResultAdapterPrivate *privat =
      GPARTSUI_RESULT_ADAPTER_GET_PRIVATE(tree_adapter);

        if (privat != NULL)
        {
            iter->stamp = privat->stamp;
            iter->user_data = GINT_TO_POINTER(0);

            success = TRUE;
        }
    }

    return success;
}

/*! \brief Determines if a node has children.
 *
 *  Since this model only handles lists, this function always returns FALSE.
 *
 *  \param [in]  tree_adapter Unused.
 *  \param [out] iter       Unused.
 *  \return FALSE, always.
 */
static int
gpartsui_result_adapter_iter_has_child(GtkTreeModel *tree_adapter, GtkTreeIter *iter)
{
    return FALSE;
}

/*! \brief Get the next node in sequence.
 *
 *  \param [in]     tree_adapter The GPartsUIResultAdapter.
 *  \param [in,out] iter       The node on entry.  If successful, the next node.
 *  \return TRUE if successful and iter is valid.  FALSE if otherwise and the
 *  iter will not be valid.
 */
static int
gpartsui_result_adapter_iter_next(GtkTreeModel *tree_adapter, GtkTreeIter *iter)
{
    unsigned int new_position;
    unsigned int old_position;
    GPartsUIResultAdapterPrivate *private = GPARTSUI_RESULT_ADAPTER_GET_PRIVATE(tree_adapter);

    g_assert(iter != NULL);
    g_assert(iter->stamp == private->stamp);

    old_position = GPOINTER_TO_UINT(iter->user_data);

    new_position = old_position + 1;

    if (new_position > old_position) {

        unsigned int rows = gparts_database_result_get_row_count(private->result);

        if (new_position < rows) {

            iter->user_data = GUINT_TO_POINTER(new_position);

            return TRUE;
        }
    }

    return FALSE;
}

/*! \brief Returns the number of children a node contains.
 *
 *  Since this model only handles lists, this function returns 0.
 *
 *  \param [in] tree_adapter The GPartsUIResultAdapter.
 *  \param [in] iter       The given node.
 *  \return The number of iter's children.
 */
static int
gpartsui_result_adapter_iter_n_children(GtkTreeModel *tree_adapter, GtkTreeIter *iter)
{
    int rows = 0;

    if (iter == NULL) {

        GPartsUIResultAdapterPrivate *privat = GPARTSUI_RESULT_ADAPTER_GET_PRIVATE(tree_adapter);

        if (privat != NULL) {

            rows = gparts_database_result_get_row_count(privat->result);
        }
    }

    return rows;
}

/*! \brief Gets an iterator to a node's nth child.
 *
 *  If the parent is NULL, this function gets an iterator to the nth node in
 *  the root list.  Since this model only implements lists, in all other cases,
 *  this function returns FALSE.
 *
 *  \param [in]  tree_adapter The GPartsUIResultAdapter.
 *  \param [out] iter       If successful, the nth child of the parent node.
 *  \param [in]  parent     The parent node.
 *  \param [in]  n          The zero based index of the child node.
 *  \return TRUE if successful and iter is valid.  FALSE if otherwise and the
 *  iter will not be valid.
 */
static int
gpartsui_result_adapter_iter_nth_child(GtkTreeModel *tree_adapter, GtkTreeIter *iter, GtkTreeIter *parent, int n)
{
    int success = FALSE;

    g_assert(iter != NULL);

    if (parent == NULL) {

        GPartsUIResultAdapterPrivate *privat = GPARTSUI_RESULT_ADAPTER_GET_PRIVATE(tree_adapter);

        if (privat != NULL) {

            unsigned int rows = gparts_database_result_get_row_count(privat->result);

            if (n < rows) {

                iter->stamp = privat->stamp;
                iter->user_data = GINT_TO_POINTER(n);

                success = TRUE;
            }
        }
    }

    return success;
}

/*! \brief Obtains the parent node of a child node.
 *
 *  Since this model only implements lists, this function always returns FALSE.
 *
 *  \param [in]  tree_adapter The GPartsUIResultAdapter.
 *  \param [out] iter       If successful, the parent node.
 *  \param [in]  child      The child node.
 *  \return TRUE if successful and iter is valid.  FALSE if otherwise and the
 *  iter will not be valid.
 */
static int
gpartsui_result_adapter_iter_parent(GtkTreeModel *tree_adapter, GtkTreeIter *iter, GtkTreeIter *child)
{
    return FALSE;
}

GPartsUIResultAdapter*
gpartsui_result_adapter_new(GPartsDatabaseResult *result)
{
  return GPARTSUI_RESULT_ADAPTER(g_object_new(GPARTSUI_TYPE_RESULT_ADAPTER,
                                              "result", result,
                                              NULL));
}

void
gpartsui_result_adapter_adjust_columns(const GPartsUIResultAdapter *adapter, GtkTreeView *view)
{
  GPartsUIResultAdapterPrivate *privat =
  GPARTSUI_RESULT_ADAPTER_GET_PRIVATE(adapter);

  if (privat != NULL) {

    int columns;
    GtkTreeViewColumn *column;
    int index;
    GValue value = {0};

    columns = gparts_database_result_get_column_count(privat->result);

    g_value_init(&value, G_TYPE_STRING);

    for(index=0; index<columns; index++) {

      GList *list;

      column = gtk_tree_view_get_column(view, index);

      if (column == NULL) {

        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

        column = gtk_tree_view_column_new();

        g_object_set(
          column,
          "resizable", TRUE,
          "reorderable", TRUE,
          "sizing", GTK_TREE_VIEW_COLUMN_GROW_ONLY,
          NULL
        );

        gtk_tree_view_column_pack_start(column, renderer, TRUE);

        gtk_tree_view_column_add_attribute(column, renderer, "text", index);

        gtk_tree_view_append_column(view, column);

      }

      list = gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(column));

      if (list != NULL) {

        GList *node = g_list_first(list);

        while (node != NULL) {

          GType type = gparts_database_result_get_column_type(privat->result, index);

          if (type == G_TYPE_STRING) {

            g_object_set(G_OBJECT(node->data), "xalign", 0.0, NULL );
          }
          else {

            g_object_set(G_OBJECT(node->data), "xalign", 1.0, NULL );
          }

          node = g_list_next(node);
        }
      }

      g_list_free(list);

      gparts_database_result_get_column_value(privat->result, index, &value);
      g_object_set_property(G_OBJECT(column), "title", &value);
    }

    g_value_unset(&value);

    /* Delete any additional columns */

    column = gtk_tree_view_get_column(view, columns);

    while (column != NULL) {
      gtk_tree_view_remove_column(view, column);
      column = gtk_tree_view_get_column(view, columns);
    }
  }
}

static void
gpartsui_result_adapter_set_property(GObject *object, unsigned int property_id, const GValue *value, GParamSpec *pspec)
{
    switch ( property_id ) {

        case GPARTSUI_RESULT_ADAPTER_PROPID_RESULT:
            gpartsui_result_adapter_set_result(GPARTSUI_RESULT_ADAPTER(object), GPARTS_DATABASE_RESULT(g_value_get_object(value)));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static void
gpartsui_result_adapter_set_result(GPartsUIResultAdapter *adapter, GPartsDatabaseResult *result)
{
    GPartsUIResultAdapterPrivate *privat = GPARTSUI_RESULT_ADAPTER_GET_PRIVATE(adapter);

    if (privat != NULL) {

        if (privat->result != NULL) {

            g_object_unref(privat->result);
        }

        privat->result = result;

        if (privat->result != NULL) {

            g_object_ref(privat->result);
        }

        g_object_notify(G_OBJECT(adapter), "result");
    }
}

/*! \brief Initialize the tree model interface.
 *
 *  \param [in] iface A pointer to the tree model interface.
 */
static void
gpartsui_result_adapter_tree_adapter_init(GtkTreeModelIface *iface)
{
    if (iface != NULL) {

        iface->get_flags       = gpartsui_result_adapter_get_flags;
        iface->get_n_columns   = gpartsui_result_adapter_get_n_columns;
        iface->get_column_type = gpartsui_result_adapter_get_column_type;
        iface->get_iter        = gpartsui_result_adapter_get_iter;
        iface->get_path        = gpartsui_result_adapter_get_path;
        iface->get_value       = gpartsui_result_adapter_get_value;
        iface->iter_next       = gpartsui_result_adapter_iter_next;
        iface->iter_children   = gpartsui_result_adapter_iter_children;
        iface->iter_has_child  = gpartsui_result_adapter_iter_has_child;
        iface->iter_n_children = gpartsui_result_adapter_iter_n_children;
        iface->iter_nth_child  = gpartsui_result_adapter_iter_nth_child;
        iface->iter_parent     = gpartsui_result_adapter_iter_parent;
    }
}
