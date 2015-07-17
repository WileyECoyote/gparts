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
        GPViewPartViewPrivate *privat = GPVIEW_PART_VIEW_GET_PRIVATE(view);

        if (privat == NULL)
        {
            g_critical("Unable to obtain private data for a GPViewPartView");
        }
        else if (privat->controller == NULL)
        {
            g_critical("GPViewPartView has a NULL controller");
        }
        else
        {
            gpview_part_ctrl_set_current_view(privat->controller, view);
        }
    }
}

static void
gpview_part_view_device_changed_cb(GtkTreeSelection *selection, GPViewPartView *view)
{
    if (view != NULL)
    {
        GPViewPartViewPrivate *privat = GPVIEW_PART_VIEW_GET_PRIVATE(view);

        if (privat != NULL)
        {
            GStrv devices = NULL;
            SchDrawing *drawing = NULL;
            int index;
            if (gpview_result_adapter_get_column_index(privat->device_adapter, "SymbolName", &index))
            {
                devices = gpview_result_adapter_get_fields(
                    privat->device_adapter,
                    privat->device_selection,
                    index
                    );
            }


            if (devices != NULL)
            {
                if (g_strv_length(devices) == 1)
                {
                    SchComponent *component;
                    SchLoader    *loader = sch_loader_get_default();
                    SchDrawing   *symbol = NULL;

                    g_debug("Device = %s", *devices);

                    if (loader != NULL)
                    {
                        symbol = sch_loader_load_symbol(loader, *devices, NULL);
                    }

                    component = sch_component_instantiate(sch_config_new(), symbol);
                    misc_object_unref(symbol);

                    if (component != NULL)
                    {
                        if (privat->part_table != NULL)
                        {
                            GRegex *regex = misc_macro_new_regex();
                            sch_shape_expand_macros(SCH_SHAPE(component), regex, privat->part_table);
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
        GPViewPartViewPrivate *privat = GPVIEW_PART_VIEW_GET_PRIVATE(view);

        if (privat != NULL)
        {
            GStrv devices = NULL;
            int index;
            GPartsDatabaseResult *result = NULL;

            if (gpview_result_adapter_get_column_index(privat->part_adapter, "DeviceID", &index))
            {
                devices = gpview_result_adapter_get_fields(
                    privat->part_adapter,
                    privat->part_selection,
                    index
                    );
            }

            if (devices != NULL)
            {
                if (g_strv_length(devices) == 1)
                {
                    GString *query = g_string_new(NULL);

                    g_string_printf(query, "SELECT * FROM SymbolV WHERE DeviceID = '%s'", *devices);

                    result = gparts_database_query(privat->database, query->str, NULL);
                    g_string_free(query, TRUE);
                }

                g_strfreev(devices);
            }

            gpview_part_view_set_device_result(view, result);
            misc_object_unref(result);


            if (gtk_tree_selection_count_selected_rows(privat->part_selection) == 1)
            {
                gtk_tree_selection_selected_foreach(
                    privat->part_selection,
                    (GtkTreeSelectionForeachFunc) xxx,
                    view
                    );
            }
            else
            {
                g_hash_table_unref(privat->part_table);
                privat->part_table = NULL;
            }
        }
    }
}

static void
xxx(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, GPViewPartView *view)
{
    GPViewPartViewPrivate *privat = GPVIEW_PART_VIEW_GET_PRIVATE(view);

    if (privat != NULL)
    {
        g_hash_table_unref(privat->part_table);
        privat->part_table = gpview_result_adapter_get_table(privat->part_adapter, iter);
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
        GPViewPartViewPrivate *privat = GPVIEW_PART_VIEW_GET_PRIVATE(view);

        if (privat == NULL)
        {
            g_critical("Unable to obtain private data for a GPViewPartView");
        }
        else if (privat->controller == NULL)
        {
            g_critical("UPViewCompanyView has a NULL controller");
        }
        else
        {
            gpview_part_ctrl_set_current_view(privat->controller, NULL);
        }
    }
}


static void
gpview_part_view_dispose(GObject *object)
{
    GPViewPartViewPrivate *privat = GPVIEW_PART_VIEW_GET_PRIVATE(object);

    if (privat != NULL)
    {
        misc_object_unref(privat->controller);
        privat->controller = NULL;

        misc_object_unref(privat->database);
        privat->database = NULL;
    }

    misc_object_chain_dispose(object);
}


char*
gpview_part_view_get_create_form(const GPViewPartView *view)
{
    char *form = NULL;

    if (view != NULL)
    {
        GPViewPartViewPrivate *privat = GPVIEW_PART_VIEW_GET_PRIVATE(view);

        if (privat != NULL)
        {
            form = gpview_category_widget_get_create_form(privat->category_widget);
        }
    }

    return form;
}

SchDrawing*
gpview_part_view_get_drawing(const GPViewPartView *view)
{
    SchDrawing *drawing = NULL;

    if (view != NULL) {

        GPViewPartViewPrivate *privat = GPVIEW_PART_VIEW_GET_PRIVATE(view);

        if (privat != NULL) {

            drawing = privat->drawing;

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

        GPViewPartViewPrivate *privat = GPVIEW_PART_VIEW_GET_PRIVATE(view);

        if (privat != NULL) {

            form = gpview_category_widget_get_edit_form(privat->category_widget);
        }
    }

    return form;
}

static void
gpview_part_view_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    GPViewPartViewPrivate *privat = GPVIEW_PART_VIEW_GET_PRIVATE(object);

    if (privat != NULL) {

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
  GPViewPartViewPrivate *private = GPVIEW_PART_VIEW_GET_PRIVATE(view);
  GStrv websites = NULL;

  if (private != NULL) {

    //    int index;
    ;
    //    if (gpview_result_adapter_get_column_index(privat->adapter, "Website", &index))
    //    {
      //          websites = gpview_result_adapter_get_fields(privat->adapter, privat->selection, index);
  }
  return websites;
}

static void
gpview_part_view_init(GTypeInstance *instance, gpointer g_class)
{
    GPViewPartViewPrivate *privat = GPVIEW_PART_VIEW_GET_PRIVATE(instance);

    if (privat != NULL) {

        GtkHPaned         *hpaned;
        GtkScrolledWindow *scrolled;
        GtkVPaned         *vpaned;

        privat->category_widget = gpview_category_widget_new();

        gtk_container_add(
            GTK_CONTAINER(instance),
            GTK_WIDGET(privat->category_widget)
            );

        privat->part_tree = GTK_TREE_VIEW(gtk_tree_view_new());

        privat->part_selection = gtk_tree_view_get_selection(privat->part_tree);

        gtk_tree_selection_set_mode(privat->part_selection, GTK_SELECTION_MULTIPLE);

        scrolled = gtk_scrolled_window_new( NULL /* hadjustment, */ NULL /* vadjustment */);

        gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(instance),
                                        GTK_POLICY_AUTOMATIC,   /* hscrollbar_policy */
                                        GTK_POLICY_AUTOMATIC    /* vscrollbar_policy */
                                      );

        gtk_container_add(GTK_CONTAINER(scrolled), GTK_WIDGET(privat->part_tree));

        privat->drawing_view = schgui_drawing_view_new();

        privat->device_tree = GTK_TREE_VIEW(gtk_tree_view_new());

        privat->device_selection = gtk_tree_view_get_selection(privat->device_tree);

        gtk_tree_selection_set_mode(privat->device_selection, GTK_SELECTION_BROWSE);

        vpaned = GTK_VPANED(gtk_vpaned_new());

        gtk_paned_set_position(GTK_PANED(vpaned), 240);

        gtk_container_add(
            GTK_CONTAINER(vpaned),
            GTK_WIDGET(privat->drawing_view)
            );

        gtk_container_add(
            GTK_CONTAINER(vpaned),
            GTK_WIDGET(privat->device_tree)
            );

        hpaned = GTK_HPANED(gtk_hpaned_new());

        gtk_paned_set_position(
            GTK_PANED(hpaned),
            400
            );

        gtk_container_add(
            GTK_CONTAINER(hpaned),
            GTK_WIDGET(scrolled)
            );

        gtk_container_add(
            GTK_CONTAINER(hpaned),
            GTK_WIDGET(vpaned)
            );

        gtk_container_add(
            GTK_CONTAINER(instance),
            GTK_WIDGET(hpaned)
            );

        gtk_paned_set_position(
            GTK_PANED(instance),
            200
            );

        g_signal_connect(
            privat->device_selection,
            "changed",
            G_CALLBACK(gpview_part_view_device_changed_cb),
            instance
            );

        g_signal_connect(
            privat->part_selection,
            "changed",
            G_CALLBACK(gpview_part_view_part_changed_cb),
            instance
            );

        g_signal_connect(
            privat->category_widget,
            "notify::view-name",
            G_CALLBACK(gpview_part_view_update_cb),
            instance
            );

        g_signal_connect(
            instance,
            "notify::database",
            G_CALLBACK(gpview_part_view_update_cb),
            instance
            );
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
    return GPVIEW_PART_VIEW(g_object_new(
        GPVIEW_TYPE_PART_VIEW,
        "controller", ctrl,
        NULL
        ));
}

void
gpview_part_view_set_controller(GPViewPartView *view, GPViewPartCtrl *ctrl)
{
    GPViewPartViewPrivate *privat = GPVIEW_PART_VIEW_GET_PRIVATE(view);

    if (privat != NULL)
    {
        if (privat->controller != NULL)
        {
            //g_signal_handlers_disconnect_by_func(
            //    privat->database,
            //    G_CALLBACK(gpview_part_view_notify_connected_cb),
            //    view
            //    );

            g_object_unref(privat->controller);
        }

        privat->controller = ctrl;

        if (privat->controller != NULL)
        {
            g_object_ref(privat->controller);

            //g_signal_connect(
            //    privat->database,
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
    GPViewPartViewPrivate *privat = GPVIEW_PART_VIEW_GET_PRIVATE(view);

    if (privat != NULL)
    {
        if (privat->database != NULL)
        {
            g_signal_handlers_disconnect_by_func(
                privat->database,
                G_CALLBACK(gpview_part_view_update_cb),
                view
                );

            g_object_unref(privat->database);
        }

        privat->database = database;

        if (privat->database != NULL)
        {
            g_object_ref(privat->database);

            g_signal_connect(
                privat->database,
                "notify::connected",
                G_CALLBACK(gpview_part_view_update_cb),
                view
                );
        }

        gpview_category_widget_set_database(
            privat->category_widget,
            privat->database
            );

        g_object_notify(G_OBJECT(view), "database");
    }
}

static void
gpview_part_view_set_device_result(GPViewPartView *view, GPartsDatabaseResult *result)
{
    GPViewPartViewPrivate *privat = GPVIEW_PART_VIEW_GET_PRIVATE(view);

    if (privat != NULL)
    {
        misc_object_unref(privat->device_adapter);
        misc_object_unref(privat->device_result);

        privat->device_adapter = NULL;
        privat->device_result = result;

        if (privat->device_result != NULL)
        {
            g_object_ref(privat->device_result);

            privat->device_adapter = gpview_result_adapter_new(privat->device_result);

            if (privat->device_adapter != NULL)
            {
                gpview_result_adapter_adjust_columns(privat->device_adapter, privat->device_tree);
            }

            gtk_tree_view_set_model(privat->device_tree, GTK_TREE_MODEL(privat->device_adapter));
        }

        g_object_notify(G_OBJECT(view), "device-result");
    }
}


static void
gpview_part_view_set_drawing(GPViewPartView *view, SchDrawing *drawing)
{
    GPViewPartViewPrivate *privat = GPVIEW_PART_VIEW_GET_PRIVATE(view);

    if (privat != NULL)
    {
        misc_object_unref(privat->drawing);

        privat->drawing = drawing;

        if (privat->drawing != NULL)
        {
            g_object_ref(privat->drawing);
        }

        if (privat->drawing_view != NULL)
        {
            schgui_drawing_view_set_drawing(privat->drawing_view, privat->drawing);
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
    GPViewPartViewPrivate *privat = GPVIEW_PART_VIEW_GET_PRIVATE(view);

    if (privat != NULL)
    {
        misc_object_unref(privat->part_adapter);
        misc_object_unref(privat->part_result);

        privat->part_adapter = NULL;
        privat->part_result = result;

        if (privat->part_result != NULL)
        {
            g_object_ref(privat->part_result);

            privat->part_adapter = gpview_result_adapter_new(privat->part_result);

            if (privat->part_adapter != NULL)
            {
                gpview_result_adapter_adjust_columns(privat->part_adapter, privat->part_tree);
            }

            gtk_tree_view_set_model(privat->part_tree, GTK_TREE_MODEL(privat->part_adapter));
        }

        g_object_notify(G_OBJECT(view), "part-result");
    }
}

static void
gpview_part_view_update_cb(GObject *unused, GParamSpec *pspec, GPViewPartView *view)
{
    GPViewPartViewPrivate *privat = GPVIEW_PART_VIEW_GET_PRIVATE(view);

    if (privat != NULL)
    {
        if (privat->database != NULL)
        {
            GPartsDatabaseResult *result = NULL;
            char *view_name = gpview_category_widget_get_view_name(privat->category_widget);

            if (view_name != NULL)
            {
                GString *query = g_string_new(NULL);

                g_string_printf(query, "SELECT * FROM %s", view_name);
                g_free(view_name);

                result = gparts_database_query(privat->database, query->str, NULL);
                g_string_free(query, TRUE);
            }

            gpview_part_view_set_part_result(view, result);
            misc_object_unref(result);
        }
    }
}

