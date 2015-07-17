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

/*! \file gpview-company-view.c
 */

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "misc-macro.h"

#include "schgui.h"
#include "gparts.h"
#include "gpform.h"
#include "gpview.h"

#define GPVIEW_PART_VIEW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),GPVIEW_TYPE_PART_VIEW,GPViewPartViewPrivate))

enum
{
    GPVIEW_PART_VIEW_PART_ID = 1,
    GPVIEW_PART_VIEW_CONTROLLER,
    GPVIEW_PART_VIEW_DATABASE,
    GPVIEW_PART_VIEW_DRAWING,
    GPVIEW_PART_VIEW_RESULT,
    GPVIEW_PART_VIEW_WEBSITES
};

typedef struct _GPViewPartViewPrivate GPViewPartViewPrivate;

struct _GPViewPartViewPrivate
{
    GPViewPartCtrl       *controller;
    GPartsDatabase       *database;

    GPViewCategoryWidget *category_widget;

    GPViewResultAdapter  *part_adapter;
    GtkTreeView          *part_tree;
    GPartsDatabaseResult *part_result;
    GtkTreeSelection     *part_selection;
    GHashTable           *part_table;

    GPViewResultAdapter  *device_adapter;
    GtkTreeView          *device_tree;
    GPartsDatabaseResult *device_result;
    GtkTreeSelection     *device_selection;

    SchGUIDrawingView    *drawing_view;
    SchDrawing           *drawing;
};

static void
gpview_part_view_activate(GPViewViewInterface *widget);

static void
gpview_part_view_part_changed_cb(GtkTreeSelection *selection, GPViewPartView *view);

static void
gpview_part_view_class_init(gpointer g_class, gpointer g_class_data);

static void
gpview_part_view_deactivate(GPViewViewInterface *widget);

static void
gpview_part_view_dispose(GObject *object);

static void
gpview_part_view_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
gpview_part_view_init(GTypeInstance *instance, gpointer g_class);

static void
gpview_part_view_init_view_interface(gpointer iface, gpointer user_data);

static void
gpview_part_view_set_device_result(GPViewPartView *view, GPartsDatabaseResult *result);

static void
gpview_part_view_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void
gpview_part_view_set_drawing(GPViewPartView *view, SchDrawing *drawing);

static void
gpview_part_view_set_part_result(GPViewPartView *view, GPartsDatabaseResult *result);

static void
gpview_part_view_update_cb(GObject *unused, GParamSpec *pspec, GPViewPartView *view);

static void
xxx(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, GPViewPartView *view);


static void
gpview_part_view_activate(GPViewViewInterface *widget)
{
    GPViewPartView *view = GPVIEW_PART_VIEW(widget);

    if (view == NULL)
    {
        g_critical("Unable to obatain an instance from GPViewViewInterface");
    }
    else
    {
        GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(view);

        if (priv == NULL)
        {
            g_critical("Unable to obtain priv data for a GPViewPartView");
        }
        else if (priv->controller == NULL)
        {
            g_critical("GPViewPartView has a NULL controller");
        }
        else
        {
            gpview_part_ctrl_set_current_view(priv->controller, view);
        }
    }
}

static void
gpview_part_view_device_changed_cb(GtkTreeSelection *selection, GPViewPartView *view)
{
  if (view != NULL) {

    GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(view);

    if (priv != NULL) {

      GStrv devices = NULL;
      SchDrawing *drawing = NULL;
      int index;
      if (gpview_result_adapter_get_column_index(priv->device_adapter, "SymbolName", &index))
      {
        devices = gpview_result_adapter_get_fields(
          priv->device_adapter,
          priv->device_selection,
          index
        );
      }


      if (devices != NULL) {

        if (g_strv_length(devices) == 1) {

          SchComponent *component;
          SchLoader    *loader = sch_loader_get_default();
          SchDrawing   *symbol = NULL;

          g_debug("Device = %s", *devices);

          if (loader != NULL) {

            symbol = sch_loader_load_symbol(loader, *devices, NULL);
          }

          component = sch_component_instantiate(sch_config_new(), symbol);
          misc_object_unref(symbol);

          if (component != NULL) {

            if (priv->part_table != NULL) {

              GRegex *regex = misc_macro_new_regex();
              sch_shape_expand_macros(SCH_SHAPE(component), regex, priv->part_table);
              g_regex_unref(regex);
            }

            drawing = sch_drawing_new();
            sch_drawing_append_shape(drawing, SCH_SHAPE(component));
            g_object_unref(component);
          }
        }

        g_strfreev(devices);
      }

      gpview_part_view_set_drawing(view, drawing);
      misc_object_unref(drawing);
    }
  }
}


static void
gpview_part_view_part_changed_cb(GtkTreeSelection *selection, GPViewPartView *view)
{
    if (view != NULL)
    {
        GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(view);

        if (priv != NULL)
        {
            GStrv devices = NULL;
            int index;
            GPartsDatabaseResult *result = NULL;

            if (gpview_result_adapter_get_column_index(priv->part_adapter, "DeviceID", &index))
            {
                devices = gpview_result_adapter_get_fields(
                    priv->part_adapter,
                    priv->part_selection,
                    index
                    );
            }

            if (devices != NULL)
            {
                if (g_strv_length(devices) == 1)
                {
                    GString *query = g_string_new(NULL);

                    g_string_printf(query, "SELECT * FROM SymbolV WHERE DeviceID = '%s'", *devices);

                    result = gparts_database_query(priv->database, query->str, NULL);
                    g_string_free(query, TRUE);
                }

                g_strfreev(devices);
            }

            gpview_part_view_set_device_result(view, result);
            misc_object_unref(result);


            if (gtk_tree_selection_count_selected_rows(priv->part_selection) == 1)
            {
                gtk_tree_selection_selected_foreach(
                    priv->part_selection,
                    (GtkTreeSelectionForeachFunc) xxx,
                    view
                    );
            }
            else
            {
                g_hash_table_unref(priv->part_table);
                priv->part_table = NULL;
            }
        }
    }
}

static void
xxx(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, GPViewPartView *view)
{
    GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(view);

    if (priv != NULL) {

        g_hash_table_unref(priv->part_table);
        priv->part_table = gpview_result_adapter_get_table(priv->part_adapter, iter);
    }

    //gpview_part_view_set_table(view, table);
    //g_hash_table_unref(table);
}


static void
gpview_part_view_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *klasse = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(g_class, sizeof(GPViewPartViewPrivate));

    klasse->dispose = gpview_part_view_dispose;

    klasse->get_property = gpview_part_view_get_property;
    klasse->set_property = gpview_part_view_set_property;

    g_object_class_install_property(
        klasse,
        GPVIEW_PART_VIEW_PART_ID,
        g_param_spec_string(
            "company-id",
            "Company ID",
            "Company ID",
            NULL,
            G_PARAM_LAX_VALIDATION | G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPVIEW_PART_VIEW_CONTROLLER,
        g_param_spec_object(
            "controller",
            "Controller",
            "Controller",
            GPVIEW_TYPE_PART_CTRL,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPVIEW_PART_VIEW_DATABASE,
        g_param_spec_object(
            "database",
            "Database",
            "Database",
            GPARTS_TYPE_DATABASE,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPVIEW_PART_VIEW_DRAWING,
        g_param_spec_object(
            "drawing",
            "Drawing",
            "Drawing",
            SCH_TYPE_DRAWING,
            G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPVIEW_PART_VIEW_RESULT,
        g_param_spec_object(
            "result",
            "Database Result",
            "Database Result",
            GPARTS_TYPE_DATABASE_RESULT,
            G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPVIEW_PART_VIEW_WEBSITES,
        g_param_spec_string(
            "websites",
            "Websites",
            "Websites",
            NULL,
            G_PARAM_LAX_VALIDATION | G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );
}

static void
gpview_part_view_deactivate(GPViewViewInterface *widget)
{
    GPViewPartView *view = GPVIEW_PART_VIEW(widget);

    if (view == NULL)
    {
        g_critical("Unable to obatain an instance from GPViewViewInterface");
    }
    else
    {
        GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(view);

        if (priv == NULL)
        {
            g_critical("Unable to obtain priv data for a GPViewPartView");
        }
        else if (priv->controller == NULL)
        {
            g_critical("UPViewCompanyView has a NULL controller");
        }
        else
        {
            gpview_part_ctrl_set_current_view(priv->controller, NULL);
        }
    }
}


static void
gpview_part_view_dispose(GObject *object)
{
    GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(object);

    if (priv != NULL)
    {
        misc_object_unref(priv->controller);
        priv->controller = NULL;

        misc_object_unref(priv->database);
        priv->database = NULL;
    }

    misc_object_chain_dispose(object);
}


char*
gpview_part_view_get_create_form(const GPViewPartView *view)
{
    char *form = NULL;

    if (view != NULL)
    {
        GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(view);

        if (priv != NULL)
        {
            form = gpview_category_widget_get_create_form(priv->category_widget);
        }
    }

    return form;
}

SchDrawing*
gpview_part_view_get_drawing(const GPViewPartView *view)
{
    SchDrawing *drawing = NULL;

    if (view != NULL) {

        GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(view);

        if (priv != NULL) {

            drawing = priv->drawing;

            if (drawing != NULL) {

                g_object_ref(drawing);
            }
        }
    }

    return drawing;
}

char*
gpview_part_view_get_edit_form(const GPViewPartView *view)
{
    char *form = NULL;

    if (view != NULL) {

        GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(view);

        if (priv != NULL) {

            form = gpview_category_widget_get_edit_form(priv->category_widget);
        }
    }

    return form;
}

static void
gpview_part_view_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(object);

    if (priv != NULL) {

        switch (property_id) {

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

unsigned int
gpview_part_view_get_type(void)
{
    static unsigned int type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(GPViewPartViewClass),    /* class_size */
            NULL,                           /* base_init */
            NULL,                           /* base_finalize */
            gpview_part_view_class_init,    /* class_init */
            NULL,                           /* class_finalize */
            NULL,                           /* class_data */
            sizeof(GPViewPartView),         /* instance_size */
            0,                              /* n_preallocs */
            gpview_part_view_init,          /* instance_init */
            NULL                            /* value_table */
            };

        static const GInterfaceInfo iinfo = {
            gpview_part_view_init_view_interface,    /* interface_init */
            NULL,                                    /* interface_finalize */
            NULL                                     /* interface_data */
            };

        type = g_type_register_static(GTK_TYPE_HPANED, "GPViewPartView",
                                      &tinfo, 0);

        g_type_add_interface_static(type, GPVIEW_TYPE_VIEW_INTERFACE, &iinfo);
    }

    return type;
}

GStrv
gpview_part_view_get_websites(const GPViewPartView *view)
{
  GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(view);
  GStrv websites = NULL;

  if (priv != NULL) {

    //    int index;
    ;
    //    if (gpview_result_adapter_get_column_index(priv->adapter, "Website", &index))
    //    {
      //          websites = gpview_result_adapter_get_fields(priv->adapter, priv->selection, index);
  }
  return websites;
}

static void
gpview_part_view_init(GTypeInstance *instance, void *g_class)
{
    GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(instance);

    if (priv != NULL) {

        GtkHPaned *hpaned;
        GtkWidget *scrolled;
        GtkVPaned *vpaned;

        priv->category_widget = gpview_category_widget_new();

        gtk_container_add(GTK_CONTAINER(instance), GTK_WIDGET(priv->category_widget));

        priv->part_tree = GTK_TREE_VIEW(gtk_tree_view_new());

        priv->part_selection = gtk_tree_view_get_selection(priv->part_tree);

        gtk_tree_selection_set_mode(priv->part_selection, GTK_SELECTION_MULTIPLE);

        scrolled = gtk_scrolled_window_new( NULL /* hadjustment */, NULL /* vadjustment */);

        gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(instance),
                                        GTK_POLICY_AUTOMATIC,   /* hscrollbar_policy */
                                        GTK_POLICY_AUTOMATIC    /* vscrollbar_policy */
                                      );

        gtk_container_add(GTK_CONTAINER(scrolled), GTK_WIDGET(priv->part_tree));

        priv->drawing_view = schgui_drawing_view_new();

        priv->device_tree = GTK_TREE_VIEW(gtk_tree_view_new());

        priv->device_selection = gtk_tree_view_get_selection(priv->device_tree);

        gtk_tree_selection_set_mode(priv->device_selection, GTK_SELECTION_BROWSE);

        vpaned = GTK_VPANED(gtk_vpaned_new());

        gtk_paned_set_position(GTK_PANED(vpaned), 240);

        gtk_container_add(GTK_CONTAINER(vpaned), GTK_WIDGET(priv->drawing_view));

        gtk_container_add(GTK_CONTAINER(vpaned), GTK_WIDGET(priv->device_tree));

        hpaned = GTK_HPANED(gtk_hpaned_new());

        gtk_paned_set_position(GTK_PANED(hpaned), 400);

        gtk_container_add(GTK_CONTAINER(hpaned), GTK_WIDGET(scrolled));

        gtk_container_add(GTK_CONTAINER(hpaned), GTK_WIDGET(vpaned));

        gtk_container_add(GTK_CONTAINER(instance), GTK_WIDGET(hpaned));

        gtk_paned_set_position(GTK_PANED(instance), 200);

        g_signal_connect(priv->device_selection, "changed",
                         G_CALLBACK(gpview_part_view_device_changed_cb),
                         instance);

        g_signal_connect(priv->part_selection, "changed",
                         G_CALLBACK(gpview_part_view_part_changed_cb),
                         instance);

        g_signal_connect(priv->category_widget, "notify::view-name",
                         G_CALLBACK(gpview_part_view_update_cb),
                         instance);

        g_signal_connect(instance, "notify::database",
                         G_CALLBACK(gpview_part_view_update_cb),
                         instance);
    }
}

static void
gpview_part_view_init_view_interface(gpointer iface, gpointer user_data)
{
    GPViewViewInterface *iface2 = (GPViewViewInterface*) iface;

    iface2->activate   = gpview_part_view_activate;
    iface2->deactivate = gpview_part_view_deactivate;
}

GPViewPartView*
gpview_part_view_new()
{
    return GPVIEW_PART_VIEW(g_object_new(GPVIEW_TYPE_PART_VIEW, NULL));
}

GPViewPartView*
gpview_part_view_new_with_controller(GPViewPartCtrl *ctrl)
{
    return GPVIEW_PART_VIEW(g_object_new(GPVIEW_TYPE_PART_VIEW,
                                         "controller", ctrl,
                                         NULL));
}

void
gpview_part_view_set_controller(GPViewPartView *view, GPViewPartCtrl *ctrl)
{
    GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(view);

    if (priv != NULL) {

        if (priv->controller != NULL) {

            //g_signal_handlers_disconnect_by_func(
            //    priv->database,
            //    G_CALLBACK(gpview_part_view_notify_connected_cb),
            //    view
            //    );

            g_object_unref(priv->controller);
        }

        priv->controller = ctrl;

        if (priv->controller != NULL) {

            g_object_ref(priv->controller);

            //g_signal_connect(
            //    priv->database,
            //    "notify::connected",
            //    G_CALLBACK(gpview_part_view_notify_connected_cb),
            //    view
            //    );
        }

        g_object_notify(G_OBJECT(view), "controller");
    }
}

void
gpview_part_view_set_database(GPViewPartView *view, GPartsDatabase *database)
{
  GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(view);

  if (priv != NULL) {

    if (priv->database != NULL) {

      g_signal_handlers_disconnect_by_func(priv->database,
                                           G_CALLBACK(gpview_part_view_update_cb),
                                           view);

      g_object_unref(priv->database);
    }

    priv->database = database;

    if (priv->database != NULL) {

      g_object_ref(priv->database);

      g_signal_connect(priv->database, "notify::connected",
                       G_CALLBACK(gpview_part_view_update_cb), view);
    }

    gpview_category_widget_set_database(priv->category_widget, priv->database);

    g_object_notify(G_OBJECT(view), "database");
  }
}

static void
gpview_part_view_set_device_result(GPViewPartView *view, GPartsDatabaseResult *result)
{
    GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(view);

    if (priv != NULL)
    {
        misc_object_unref(priv->device_adapter);
        misc_object_unref(priv->device_result);

        priv->device_adapter = NULL;
        priv->device_result = result;

        if (priv->device_result != NULL)
        {
            g_object_ref(priv->device_result);

            priv->device_adapter = gpview_result_adapter_new(priv->device_result);

            if (priv->device_adapter != NULL)
            {
                gpview_result_adapter_adjust_columns(priv->device_adapter, priv->device_tree);
            }

            gtk_tree_view_set_model(priv->device_tree, GTK_TREE_MODEL(priv->device_adapter));
        }

        g_object_notify(G_OBJECT(view), "device-result");
    }
}


static void
gpview_part_view_set_drawing(GPViewPartView *view, SchDrawing *drawing)
{
    GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(view);

    if (priv != NULL)
    {
        misc_object_unref(priv->drawing);

        priv->drawing = drawing;

        if (priv->drawing != NULL)
        {
            g_object_ref(priv->drawing);
        }

        if (priv->drawing_view != NULL)
        {
            schgui_drawing_view_set_drawing(priv->drawing_view, priv->drawing);
        }

        g_object_notify(G_OBJECT(view), "drawing");
    }
}


static void
gpview_part_view_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GPViewPartView *view = GPVIEW_PART_VIEW(object);

    if (view != NULL)
    {
        switch (property_id)
        {
            case GPVIEW_PART_VIEW_CONTROLLER:
                gpview_part_view_set_controller(view, g_value_get_object(value));
                break;

            case GPVIEW_PART_VIEW_DATABASE:
                gpview_part_view_set_database(view, g_value_get_object(value));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

static void
gpview_part_view_set_part_result(GPViewPartView *view, GPartsDatabaseResult *result)
{
    GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(view);

    if (priv != NULL)
    {
        misc_object_unref(priv->part_adapter);
        misc_object_unref(priv->part_result);

        priv->part_adapter = NULL;
        priv->part_result = result;

        if (priv->part_result != NULL)
        {
            g_object_ref(priv->part_result);

            priv->part_adapter = gpview_result_adapter_new(priv->part_result);

            if (priv->part_adapter != NULL)
            {
                gpview_result_adapter_adjust_columns(priv->part_adapter, priv->part_tree);
            }

            gtk_tree_view_set_model(priv->part_tree, GTK_TREE_MODEL(priv->part_adapter));
        }

        g_object_notify(G_OBJECT(view), "part-result");
    }
}

static void
gpview_part_view_update_cb(GObject *unused, GParamSpec *pspec, GPViewPartView *view)
{
    GPViewPartViewPrivate *priv = GPVIEW_PART_VIEW_GET_PRIVATE(view);

    if (priv != NULL)
    {
        if (priv->database != NULL)
        {
            GPartsDatabaseResult *result = NULL;
            char *view_name = gpview_category_widget_get_view_name(priv->category_widget);

            if (view_name != NULL)
            {
                GString *query = g_string_new(NULL);

                g_string_printf(query, "SELECT * FROM %s", view_name);
                g_free(view_name);

                result = gparts_database_query(priv->database, query->str, NULL);
                g_string_free(query, TRUE);
            }

            gpview_part_view_set_part_result(view, result);
            misc_object_unref(result);
        }
    }
}

