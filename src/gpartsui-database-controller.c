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

/*! \file gparts-login-ctrl.c
 */

#include <string.h>

#include "gpartsui.h"

#define GPARTSUI_DATABASE_CONTROLLER_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE(object,GPARTSUI_TYPE_DATABASE_CONTROLLER,GPartsUIDatabaseControllerPrivate)

enum
{
    GPARTSUI_DATABASE_CONTROLLER_PROPID_DATABASE_MODEL = 1,

    GPARTSUI_DATABASE_CONTROLLER_PROPID_DISCONNECT_ACTION,
    GPARTSUI_DATABASE_CONTROLLER_PROPID_DROP_ACTION,
    GPARTSUI_DATABASE_CONTROLLER_PROPID_REFRESH_ACTION
};

typedef struct _GPartsUIDatabaseControllerPrivate GPartsUIDatabaseControllerPrivate;

struct _GPartsUIDatabaseControllerPrivate
{
    MiscUIActionController *disconnect_controller;
    MiscUIActionController *drop_controller;
    MiscUIActionController *refresh_controller;
};


static void
gpartsui_database_controller_class_init(gpointer g_class, gpointer g_class_data);

static void
gpartsui_database_controller_dispose(GObject *object);

static void
gpartsui_database_controller_finalize(GObject *object);

static void
gpartsui_database_controller_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
gpartsui_database_controller_instance_init(GTypeInstance *instance, gpointer g_class);

static void
gpartsui_database_controller_set_database(GPartsUIDatabaseController *ctrl, GPartsDatabase *database);

static void
gpartsui_database_controller_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void
gpartsui_database_controller_update_flags(GPartsUIDatabaseController *ctrl);

static void
gpartsui_database_controller_update_valid(GPartsUIDatabaseController *ctrl);

/**** Signal handlers ****/

static void
gpartsui_database_controller_all_notify_cb(GPartsUIDatabaseModel *model, GParamSpec *param, GPartsUIDatabaseController *controller);

static void
gpartsui_database_controller_disconnect_action_cb(GtkAction *action, GPartsUIDatabaseController *controller);

static void
gpartsui_database_controller_drop_action_cb(GtkAction *action, GPartsUIDatabaseController *controller);

static void
gpartsui_database_controller_refresh_action_cb(GtkAction *action, GPartsUIDatabaseController *controller);

static void
gpartsui_database_controller_update_all_actions(GPartsUIDatabaseController *controller);



static void
gpartsui_database_controller_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass* object_class = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(g_class, sizeof(GPartsUIDatabaseControllerPrivate));

    object_class->dispose  = gpartsui_database_controller_dispose;
    object_class->finalize = gpartsui_database_controller_finalize;

    object_class->get_property = gpartsui_database_controller_get_property;
    object_class->set_property = gpartsui_database_controller_set_property;

    g_object_class_install_property(
        object_class,
        GPARTSUI_DATABASE_CONTROLLER_PROPID_DATABASE_MODEL,
        g_param_spec_object("database-model",
                            "Database Model",
                            "Database Model",
                            GPARTSUI_TYPE_DATABASE_MODEL,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(
        object_class,
        GPARTSUI_DATABASE_CONTROLLER_PROPID_DISCONNECT_ACTION,
        g_param_spec_object("disconnect-action",
                            "Disconnect Action",
                            "Disconnect Action",
                            GTK_TYPE_ACTION,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(
        object_class,
        GPARTSUI_DATABASE_CONTROLLER_PROPID_DROP_ACTION,
        g_param_spec_object("drop-action",
                            "Drop Action",
                            "Drop Action",
                            GTK_TYPE_ACTION,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(
        object_class,
        GPARTSUI_DATABASE_CONTROLLER_PROPID_REFRESH_ACTION,
        g_param_spec_object("refresh-action",
                            "",
                            "",
                            GTK_TYPE_ACTION,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gpartsui_database_controller_dispose(GObject *object)
{
    GPartsUIDatabaseControllerPrivate *privat = GPARTSUI_DATABASE_CONTROLLER_GET_PRIVATE(object);

    if (privat != NULL) {

        if (privat->disconnect_controller != NULL) {

            g_object_unref(privat->disconnect_controller);
            privat->disconnect_controller = NULL;
        }

        if (privat->drop_controller != NULL) {

            g_object_unref(privat->drop_controller);
            privat->drop_controller = NULL;
        }

        if (privat->refresh_controller != NULL) {

            g_object_unref(privat->refresh_controller);
            privat->refresh_controller = NULL;
        }
    }

    misc_object_chain_dispose(object);
}

static void
gpartsui_database_controller_finalize(GObject *object)
{
    GPartsUIDatabaseControllerPrivate *privat = GPARTSUI_DATABASE_CONTROLLER_GET_PRIVATE(object);

    misc_object_chain_finalize(object);
}

GType
gpartsui_database_controller_get_type(void)
{
    static GType type = 0;

    if ( type == 0 ) {

        static const GTypeInfo tinfo = {
            sizeof(GPartsUIDatabaseControllerClass),
            NULL,
            NULL,
            gpartsui_database_controller_class_init,
            NULL,
            NULL,
            sizeof(GPartsUIDatabaseController),
            0,
            gpartsui_database_controller_instance_init,
            NULL
            };

        type = g_type_register_static(G_TYPE_OBJECT,
                                      "gpartsui-database-controller",
                                      &tinfo,
                                      0);
    }

    return type;
}

GPartsUIDatabaseModel*
gpartsui_database_controller_get_database_model(GPartsUIDatabaseController *controller)
{
    MiscUIActionModel *database_model = NULL;
    GPartsUIDatabaseControllerPrivate *privat = GPARTSUI_DATABASE_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {
        database_model = miscui_action_controller_get_model(privat->disconnect_controller);
    }

    return (GPartsUIDatabaseModel*) database_model;
}

GtkAction*
gpartsui_database_controller_get_disconnect_action(const GPartsUIDatabaseController *controller)
{
    GtkAction *action = NULL;
    GPartsUIDatabaseControllerPrivate *privat = GPARTSUI_DATABASE_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {
        action = miscui_action_controller_get_action(privat->disconnect_controller);
    }

    return action;
}

GtkAction*
gpartsui_database_controller_get_drop_action(const GPartsUIDatabaseController *controller)
{
    GtkAction *action = NULL;
    GPartsUIDatabaseControllerPrivate *privat = GPARTSUI_DATABASE_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {
        action = miscui_action_controller_get_action(privat->drop_controller);
    }

    return action;
}


static void
gpartsui_database_controller_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    GPartsUIDatabaseController *controller = GPARTSUI_DATABASE_CONTROLLER(object);

    switch (property_id)  {

        case GPARTSUI_DATABASE_CONTROLLER_PROPID_DATABASE_MODEL:
            g_value_take_object(value, gpartsui_database_controller_get_database_model(controller));
            break;

        case GPARTSUI_DATABASE_CONTROLLER_PROPID_DISCONNECT_ACTION:
            g_value_take_object(value, gpartsui_database_controller_get_disconnect_action(controller));
            break;

        case GPARTSUI_DATABASE_CONTROLLER_PROPID_DROP_ACTION:
            g_value_take_object(value, gpartsui_database_controller_get_drop_action(controller));
            break;

        case GPARTSUI_DATABASE_CONTROLLER_PROPID_REFRESH_ACTION:
            g_value_take_object(value, gpartsui_database_controller_get_refresh_action(controller));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

GtkAction*
gpartsui_database_controller_get_refresh_action(const GPartsUIDatabaseController *controller)
{
    GtkAction *action = NULL;
    GPartsUIDatabaseControllerPrivate *privat = GPARTSUI_DATABASE_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        action = miscui_action_controller_get_action(privat->refresh_controller);
    }

    return action;
}


static void
gpartsui_database_controller_instance_init(GTypeInstance *instance, gpointer g_class)
{
    GPartsUIDatabaseControllerPrivate *privat = GPARTSUI_DATABASE_CONTROLLER_GET_PRIVATE(instance);

    if (privat != NULL) {

        privat->disconnect_controller = MISCUI_ACTION_CONTROLLER(g_object_new(
            MISCUI_TYPE_ACTION_CONTROLLER,
            NULL
            ));

        privat->drop_controller = MISCUI_ACTION_CONTROLLER(g_object_new(
            MISCUI_TYPE_ACTION_CONTROLLER,
            NULL
            ));

        privat->refresh_controller = MISCUI_ACTION_CONTROLLER(g_object_new(
            MISCUI_TYPE_ACTION_CONTROLLER,
            NULL
            ));
    }
}

void
gpartsui_database_controller_set_database_model(GPartsUIDatabaseController *controller, GPartsUIDatabaseModel *model)
{
    GPartsUIDatabaseControllerPrivate *privat = GPARTSUI_DATABASE_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        MiscUIActionModel *action_model = NULL;

        action_model = gpartsui_database_model_get_disconnect_action_model(model);

        miscui_action_controller_set_model(privat->disconnect_controller,
                                           action_model);

        if (action_model != NULL) {

            g_object_unref(action_model);
        }

        action_model = gpartsui_database_model_get_drop_action_model(model);

        miscui_action_controller_set_model(privat->drop_controller,
                                           action_model);

        if (action_model != NULL) {

            g_object_unref(action_model);
        }

        action_model = gpartsui_database_model_get_refresh_action_model(model);

        miscui_action_controller_set_model(privat->refresh_controller,
                                           action_model);

        if (action_model != NULL) {

            g_object_unref(action_model);
        }

        g_object_notify(G_OBJECT(controller), "database-model");
    }
}

void
gpartsui_database_controller_set_disconnect_action(GPartsUIDatabaseController *controller, GtkAction *action)
{
    GPartsUIDatabaseControllerPrivate *privat = GPARTSUI_DATABASE_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        miscui_action_controller_set_action(privat->disconnect_controller, action);

        g_object_notify(G_OBJECT(controller), "disconnect-action");
    }
}

void
gpartsui_database_controller_set_drop_action(GPartsUIDatabaseController *controller, GtkAction *action)
{
    GPartsUIDatabaseControllerPrivate *privat = GPARTSUI_DATABASE_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        miscui_action_controller_set_action(privat->drop_controller, action);

        g_object_notify(G_OBJECT(controller), "drop-action");
    }
}

static void
gpartsui_database_controller_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GPartsUIDatabaseController *controller = GPARTSUI_DATABASE_CONTROLLER(object);

    switch (property_id) {

        case GPARTSUI_DATABASE_CONTROLLER_PROPID_DATABASE_MODEL:
            gpartsui_database_controller_set_database_model(controller, GPARTSUI_DATABASE_MODEL(g_value_get_object(value)));
            break;

        case GPARTSUI_DATABASE_CONTROLLER_PROPID_DISCONNECT_ACTION:
            gpartsui_database_controller_set_disconnect_action(controller, GTK_ACTION(g_value_get_object(value)));
            break;

        case GPARTSUI_DATABASE_CONTROLLER_PROPID_DROP_ACTION:
            gpartsui_database_controller_set_drop_action(controller, GTK_ACTION(g_value_get_object(value)));
            break;

        case GPARTSUI_DATABASE_CONTROLLER_PROPID_REFRESH_ACTION:
            gpartsui_database_controller_set_refresh_action(controller, GTK_ACTION(g_value_get_object(value)));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

void
gpartsui_database_controller_set_refresh_action(GPartsUIDatabaseController *controller, GtkAction *action)
{
    GPartsUIDatabaseControllerPrivate *privat = GPARTSUI_DATABASE_CONTROLLER_GET_PRIVATE(controller);

    if (privat != NULL) {

        miscui_action_controller_set_action(privat->refresh_controller, action);

        g_object_notify(G_OBJECT(controller), "refresh-action");
    }
}

