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

/*! \file gparts-category-controller.c
 */

#include <string.h>

#include "gpartsui.h"

enum
{
    GPARTS_CATEGORY_CONTROLLER_PROPID_DATABASE_MODEL = 1,
    GPARTS_CATEGORY_CONTROLLER_PROPID_TARGET
};

typedef struct _GPartsCategoryControllerPrivate GPartsCategoryControllerPrivate;

struct _GPartsCategoryControllerPrivate
{
    GPartsUIDatabaseModel *database_model;
    GtkTreeSelection      *selection;
    GtkTreeView           *target;
};

#define GPARTS_CATEGORY_CONTROLLER_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE(object,GPARTS_TYPE_CATEGORY_CONTROLLER,GPartsCategoryControllerPrivate)

/**** Private functions ****/

static void
gparts_category_controller_class_init(void *g_class, void *g_class_data);

static void
gparts_category_controller_dispose(GObject *object);

static char*
gparts_category_controller_get_field(GPartsController *controller, const char *name);

static void
gparts_category_controller_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
gparts_category_controller_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void
gparts_category_controller_update(GPartsCategoryController *controller);

/**** Signal handlers ****/

static void
gparts_category_controller_changed_cb(GtkTreeSelection *selection, GPartsCategoryController *controller);

static void
gparts_category_controller_database_controller_notify_cb(GPartsUIDatabaseController *database, GParamSpec *pspec, GPartsCategoryController *controller);

/* \todo Support this function */
static void
gparts_category_controller_show_cb(GtkWidget *widget, GPartsCategoryController *controller);

static void
gparts_category_controller_changed_cb(GtkTreeSelection *selection, GPartsCategoryController *controller)
{
    g_signal_emit_by_name(controller, "updated");
}

static void
gparts_category_controller_class_init(void *g_class, void *g_class_data)
{
    GObjectClass* object_class = G_OBJECT_CLASS(g_class);
    GPartsControllerClass *klasse = GPARTS_CONTROLLER_CLASS(g_class);

    g_type_class_add_private(g_class, sizeof(GPartsCategoryControllerPrivate));

    object_class->dispose  = gparts_category_controller_dispose;

    object_class->get_property = gparts_category_controller_get_property;
    object_class->set_property = gparts_category_controller_set_property;

    klasse->get_field = gparts_category_controller_get_field;

    g_object_class_install_property(
        object_class,
        GPARTS_CATEGORY_CONTROLLER_PROPID_DATABASE_MODEL,
        g_param_spec_object("database-model",
                            "",
                            "",
                            GPARTSUI_TYPE_DATABASE_MODEL,
                            G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        GPARTS_CATEGORY_CONTROLLER_PROPID_TARGET,
        g_param_spec_object("target",
                            "",
                            "",
                            GTK_TYPE_TREE_VIEW,
                            G_PARAM_READWRITE));

     g_signal_new("updated",
        G_TYPE_FROM_CLASS(object_class),
        G_SIGNAL_RUN_FIRST,
        0,
        NULL,
        NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE,
        0);
}

static void
gparts_category_controller_database_model_notify_cb(GPartsUIDatabaseModel *database, GParamSpec *pspec, GPartsCategoryController *controller)
{
    gparts_category_controller_update(controller);
}

static void
gparts_category_controller_dispose(GObject *object)
{
    GPartsCategoryController *controller = GPARTS_CATEGORY_CONTROLLER(object);

    gparts_category_controller_set_database_model(controller, NULL);
    gparts_category_controller_set_target(controller, NULL);

    misc_object_chain_dispose(object);
}

static char*
gparts_category_controller_get_field(GPartsController *controller, const char *name)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    GPartsCategoryControllerPrivate *privat = GPARTS_CATEGORY_CONTROLLER_GET_PRIVATE(controller);
    gboolean success;
    char *value = NULL;

    g_debug("get field");

    success = gtk_tree_selection_get_selected(privat->selection, &model, &iter);

    if (success && GPARTS_IS_CATEGORY_MODEL(model)) {

        value = gparts_category_model_get_field(GPARTS_CATEGORY_MODEL(model), &iter, name);
    }

    g_debug("get field 2");

    return value;
}

GType
gparts_category_controller_get_type(void)
{
    static GType type = 0;

    if ( type == 0 ) {

        static const GTypeInfo tinfo = {
            sizeof(GPartsCategoryControllerClass),
            NULL,
            NULL,
            gparts_category_controller_class_init,
            NULL,
            NULL,
            sizeof(GPartsCategoryController),
            0,
            NULL,
            NULL
            };

        type = g_type_register_static(
            GPARTS_TYPE_CONTROLLER,
            "gparts-category-controller",
            &tinfo,
            0
            );
    }

    return type;
}

static void
gparts_category_controller_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
}

GPartsCategoryController*
gparts_category_controller_new()
{
}

static void
gparts_category_controller_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GPartsCategoryController *controller = GPARTS_CATEGORY_CONTROLLER(object);

    switch (property_id)
    {
        case GPARTS_CATEGORY_CONTROLLER_PROPID_DATABASE_MODEL:
            gparts_category_controller_set_database_model(controller, g_value_get_object(value));
            break;

        case GPARTS_CATEGORY_CONTROLLER_PROPID_TARGET:
            gparts_category_controller_set_target(controller, g_value_get_object(value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

void
gparts_category_controller_set_database_model(GPartsCategoryController *controller, GPartsUIDatabaseModel *database)
{
    GPartsCategoryControllerPrivate *privat = GPARTS_CATEGORY_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        if (privat->database_model != NULL) {

            g_signal_handlers_disconnect_by_func(
                privat->database_model,
                G_CALLBACK(gparts_category_controller_database_model_notify_cb),
                controller);

            g_object_unref(privat->database_model);
        }

        privat->database_model = database;

        if (privat->database_model != NULL) {

            g_object_ref(privat->database_model);

            g_signal_connect(
                privat->database_model,
                "notify::database",
                G_CALLBACK(gparts_category_controller_database_model_notify_cb),
                controller);
        }

        gparts_category_controller_update(controller);

        g_object_notify(G_OBJECT(controller), "database-model");
    }
}

void
gparts_category_controller_set_target(GPartsCategoryController *controller, GtkTreeView *target)
{
    GPartsCategoryControllerPrivate *private = GPARTS_CATEGORY_CONTROLLER_GET_PRIVATE(controller);

    if (private->selection != NULL) {

         g_signal_handlers_disconnect_by_func(private->selection,
            G_CALLBACK(gparts_category_controller_changed_cb),
            controller);

        g_object_unref(private->selection);
    }

    if (private->target != NULL) {

        g_object_unref(private->target);
    }

    private->selection = NULL;
    private->target = target;

    if (private->target != NULL) {

        g_object_ref(private->target);

        private->selection = gtk_tree_view_get_selection(private->target);

        if (private->selection != NULL) {

            g_object_ref(private->selection);

            g_signal_connect(private->selection, "changed",
                G_CALLBACK(gparts_category_controller_changed_cb),
                controller);
        }
    }

    gparts_category_controller_update(controller);
}

static void
gparts_category_controller_update(GPartsCategoryController *controller)
{
    GPartsCategoryControllerPrivate *privat = GPARTS_CATEGORY_CONTROLLER_GET_PRIVATE(controller);

    if (privat->target != NULL) {

        GPartsCategoryModel *model = NULL;
        GPartsDatabase *database = NULL;

        if (privat->database_model != NULL) {

            database = gpartsui_database_model_get_database(privat->database_model);
        }

        if (database != NULL) {

            model = gparts_category_model_new(database);

            gparts_category_model_set_columns(model, privat->target);
        }

        gtk_tree_view_set_model(privat->target, GTK_TREE_MODEL(model));

        if (model != NULL) {

            g_object_unref(model);
        }
    }
}
