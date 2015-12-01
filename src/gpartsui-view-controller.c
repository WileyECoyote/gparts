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

/*! \file gparts-controller.c
 */

#include "gpartsui.h"

#define GPARTSUI_VIEW_CONTROLLER_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE(object,GPARTSUI_TYPE_VIEW_CONTROLLER,GPartsUIViewControllerPrivate)

typedef struct _GPartsUIViewControllerPrivate GPartsUIViewControllerPrivate;

struct _GPartsUIViewControllerPrivate
{
    MiscUIActionController *copy_controller;
    MiscUIActionController *delete_controller;
    MiscUIActionController *edit_controller;
    MiscUIActionController *insert_controller;
    MiscUIActionController *open_document_controller;
    MiscUIActionController *open_website_controller;
    MiscUIActionController *paste_controller;
};

static void
gpartsui_view_controller_init(GTypeInstance *instance, void *g_class);

static void
gpartsui_view_controller_set_copy_action_base(void *controller, GtkAction *action);

static void
gpartsui_view_controller_set_delete_action_base(void *controller, GtkAction *action);

static void
gpartsui_view_controller_set_edit_action_base(void *controller, GtkAction *action);

static void
gpartsui_view_controller_set_insert_action_base(void *controller, GtkAction *action);

static void
gpartsui_view_controller_set_open_document_action_base(void *controller, GtkAction *action);

static void
gpartsui_view_controller_set_open_website_action_base(void *controller, GtkAction *action);

static void
gpartsui_view_controller_set_paste_action_base(void *controller, GtkAction *action);


static void
gpartsui_view_controller_class_init(void *g_class, void *g_class_data)
{
    GPartsUIViewControllerClass *klasse = GPARTSUI_VIEW_CONTROLLER_CLASS(g_class);

    g_type_class_add_private(g_class, sizeof(GPartsUIViewControllerPrivate));

    klasse->set_copy_action          = gpartsui_view_controller_set_copy_action_base;
    klasse->set_delete_action        = gpartsui_view_controller_set_delete_action_base;
    klasse->set_edit_action          = gpartsui_view_controller_set_edit_action_base;
    klasse->set_insert_action        = gpartsui_view_controller_set_insert_action_base;
    klasse->set_open_document_action = gpartsui_view_controller_set_open_document_action_base;
    klasse->set_open_website_action  = gpartsui_view_controller_set_open_website_action_base;
    klasse->set_paste_action         = gpartsui_view_controller_set_paste_action_base;
}

gchar*
gpartsui_view_controller_get_field(GPartsUIViewController *controller, const char *name)
{
    char *field = NULL;

    if (controller != NULL) {

        GPartsUIViewControllerClass *klasse = GPARTSUI_VIEW_CONTROLLER_GET_CLASS(controller);

        if ((klasse != NULL) && (klasse->get_field != NULL)) {

            field = klasse->get_field(controller, name);
        }
    }

    return field;
}

GHashTable*
gpartsui_view_controller_get_table(GPartsUIViewController *controller)
{
    GHashTable *table = NULL;

    if (controller != NULL) {

        GPartsUIViewControllerClass *klasse = GPARTSUI_VIEW_CONTROLLER_GET_CLASS(controller);

        if ((klasse != NULL) && (klasse->get_table != NULL)) {

            table = klasse->get_table(controller);
        }
    }

    return table;
}

GType
gpartsui_view_controller_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(GPartsUIViewControllerClass),    /* class_size */
            NULL,                             /* base_init */
            NULL,                             /* base_finalize */
            gpartsui_view_controller_class_init,     /* class_init */
            NULL,                             /* class_finalize */
            NULL,                             /* class_data */
            sizeof(GPartsUIViewController),         /* instance_size */
            0,                                /* n_preallocs */
            gpartsui_view_controller_init,           /* instance_init */
            NULL                              /* value_table */
            };

        type = g_type_register_static(G_TYPE_OBJECT, "GPartsUIViewController",
                                      &tinfo,
                                      G_TYPE_FLAG_ABSTRACT);
    }

    return type;
}

static void
gpartsui_view_controller_init(GTypeInstance *instance, void *g_class)
{
    GPartsUIViewControllerPrivate *privat = GPARTSUI_VIEW_CONTROLLER_GET_PRIVATE(instance);

    if (privat != NULL) {

        privat->copy_controller = MISCUI_ACTION_CONTROLLER(g_object_new(
            MISCUI_TYPE_ACTION_CONTROLLER,
            "label",     "_Copy",
            "sensitive", FALSE,
            NULL));

        privat->delete_controller = MISCUI_ACTION_CONTROLLER(g_object_new(
            MISCUI_TYPE_ACTION_CONTROLLER,
            "label",     "Delete",
            "sensitive", FALSE,
            NULL
            ));

        privat->edit_controller = MISCUI_ACTION_CONTROLLER(g_object_new(
            MISCUI_TYPE_ACTION_CONTROLLER,
            "label",     "Edit",
            "sensitive", FALSE,
            NULL));

        privat->insert_controller = MISCUI_ACTION_CONTROLLER(g_object_new(
            MISCUI_TYPE_ACTION_CONTROLLER,
            "label",     "Insert",
            "sensitive", FALSE,
            NULL));

        privat->open_document_controller = MISCUI_ACTION_CONTROLLER(g_object_new(
            MISCUI_TYPE_ACTION_CONTROLLER,
            "label",     "Open Document",
            "sensitive", FALSE,
            NULL));

        privat->open_website_controller = MISCUI_ACTION_CONTROLLER(g_object_new(
            MISCUI_TYPE_ACTION_CONTROLLER,
            "label",     "Open Website",
            "sensitive", FALSE,
            NULL));

        privat->paste_controller = MISCUI_ACTION_CONTROLLER(g_object_new(
            MISCUI_TYPE_ACTION_CONTROLLER,
            "label",     "_Paste",
            "sensitive", FALSE,
            NULL));
    }
}

void
gpartsui_view_controller_set_copy_action(GPartsUIViewController *controller, GtkAction *action)
{
  if (controller != NULL) {

    GPartsUIViewControllerClass *klasse = GPARTSUI_VIEW_CONTROLLER_GET_CLASS(controller);

    if (klasse == NULL) {

      g_critical("Unable to get GPartsUIViewControllerClass from parameter");
    }
    else if (klasse->set_copy_action == NULL) {

      g_critical("GPartsUIViewControllerClass contains NULL set_copy_action() function pointer");
    }
    else {

      klasse->set_copy_action(controller, action);
    }
  }
}

static void
gpartsui_view_controller_set_copy_action_base(void *pcontroller, GtkAction *action)
{
  GPartsUIViewController *controller = pcontroller;
  GPartsUIViewControllerPrivate *privat =
  GPARTSUI_VIEW_CONTROLLER_GET_PRIVATE(controller);

  if (privat != NULL) {

    miscui_action_controller_set_action(privat->copy_controller, action);
  }
}

void
gpartsui_view_controller_set_delete_action(GPartsUIViewController *controller, GtkAction *action)
{
  if (controller != NULL) {

    GPartsUIViewControllerClass *klasse = GPARTSUI_VIEW_CONTROLLER_GET_CLASS(controller);

    if (klasse == NULL) {

      g_critical("Unable to get GPartsUIViewControllerClass from parameter");
    }
    else if (klasse->set_delete_action == NULL) {

      g_critical("GPartsUIViewControllerClass contains NULL set_delete_action() function pointer");
    }
    else {

      klasse->set_delete_action(controller, action);
    }
  }
}

static void
gpartsui_view_controller_set_delete_action_base(void *pcontroller, GtkAction *action)
{
  GPartsUIViewController *controller = pcontroller;
  GPartsUIViewControllerPrivate *privat =
  GPARTSUI_VIEW_CONTROLLER_GET_PRIVATE(controller);

  if (privat != NULL) {

    miscui_action_controller_set_action(privat->delete_controller, action);
  }
}

void
gpartsui_view_controller_set_edit_action(GPartsUIViewController *controller, GtkAction *action)
{
  if (controller != NULL) {

    GPartsUIViewControllerClass *klasse = GPARTSUI_VIEW_CONTROLLER_GET_CLASS(controller);

    if (klasse == NULL) {

      g_critical("Unable to get GPartsUIViewControllerClass from parameter");
    }
    else if (klasse->set_edit_action == NULL) {

      g_critical("GPartsUIViewControllerClass contains NULL set_edit_action() function pointer");
    }
    else {

      klasse->set_edit_action(controller, action);
    }
  }
}

static void
gpartsui_view_controller_set_edit_action_base(void *pcontroller, GtkAction *action)
{
  GPartsUIViewController *controller = pcontroller;
  GPartsUIViewControllerPrivate *privat =
  GPARTSUI_VIEW_CONTROLLER_GET_PRIVATE(controller);

  if (privat != NULL) {
    miscui_action_controller_set_action(privat->edit_controller, action);
  }
}


void
gpartsui_view_controller_set_insert_action(GPartsUIViewController *controller, GtkAction *action)
{
  if (controller != NULL) {

    GPartsUIViewControllerClass *klasse = GPARTSUI_VIEW_CONTROLLER_GET_CLASS(controller);

    if (klasse == NULL) {

      g_critical("Unable to get GPartsUIViewControllerClass from parameter");
    }
    else if (klasse->set_insert_action == NULL) {

      g_critical("GPartsUIViewControllerClass contains NULL set_insert_action() function pointer");
    }
    else {

      klasse->set_insert_action(controller, action);
    }
  }
}

static void
gpartsui_view_controller_set_insert_action_base(void *pcontroller, GtkAction *action)
{
  GPartsUIViewController *controller = pcontroller;
  GPartsUIViewControllerPrivate *privat =
  GPARTSUI_VIEW_CONTROLLER_GET_PRIVATE(controller);

  if (privat != NULL) {

    miscui_action_controller_set_action(privat->insert_controller, action);
  }
}

void
gpartsui_view_controller_set_open_document_action(GPartsUIViewController *controller, GtkAction *action)
{
  if (controller != NULL) {

    GPartsUIViewControllerClass *klasse = GPARTSUI_VIEW_CONTROLLER_GET_CLASS(controller);

    if (klasse == NULL) {

      g_critical("Unable to get GPartsUIViewControllerClass from parameter");
    }
    else if (klasse->set_open_document_action == NULL) {

      g_critical("GPartsUIViewControllerClass contains NULL set_open_document_action() function pointer");
    }
    else {

      klasse->set_open_document_action(controller, action);
    }
  }
}

static void
gpartsui_view_controller_set_open_document_action_base(void *pcontroller, GtkAction *action)
{
  GPartsUIViewController *controller = pcontroller;
  GPartsUIViewControllerPrivate *privat =
  GPARTSUI_VIEW_CONTROLLER_GET_PRIVATE(controller);

  if (privat != NULL) {
    miscui_action_controller_set_action(privat->open_document_controller, action);
  }
}

void
gpartsui_view_controller_set_open_website_action(GPartsUIViewController *controller, GtkAction *action)
{
  if (controller != NULL) {

    GPartsUIViewControllerClass *klasse = GPARTSUI_VIEW_CONTROLLER_GET_CLASS(controller);

    if (klasse == NULL) {

      g_critical("Unable to get GPartsUIViewControllerClass from parameter");
    }
    else if (klasse->set_open_website_action == NULL) {

      g_critical("GPartsUIViewControllerClass contains NULL set_open_website_action() function pointer");
    }
    else {

      klasse->set_open_website_action(controller, action);
    }
  }
}

static void
gpartsui_view_controller_set_open_website_action_base(void *pcontroller, GtkAction *action)
{
  GPartsUIViewController *controller = pcontroller;
  GPartsUIViewControllerPrivate *privat =
  GPARTSUI_VIEW_CONTROLLER_GET_PRIVATE(controller);

  if (privat != NULL) {
    miscui_action_controller_set_action(privat->open_website_controller, action);
  }
}

void
gpartsui_view_controller_set_paste_action(GPartsUIViewController *controller, GtkAction *action)
{
  if (controller != NULL) {

    GPartsUIViewControllerClass *klasse = GPARTSUI_VIEW_CONTROLLER_GET_CLASS(controller);

    if (klasse == NULL) {

      g_critical("Unable to get GPartsUIViewControllerClass from parameter");
    }
    else if (klasse->set_paste_action == NULL) {

      g_critical("GPartsUIViewControllerClass contains NULL set_paste_action() function pointer");
    }
    else {

      klasse->set_paste_action(controller, action);
    }
  }
}

static void
gpartsui_view_controller_set_paste_action_base(void *pcontroller, GtkAction *action)
{
  GPartsUIViewController *controller = pcontroller;
  GPartsUIViewControllerPrivate *privat =
  GPARTSUI_VIEW_CONTROLLER_GET_PRIVATE(controller);

  if (privat != NULL) {
    miscui_action_controller_set_action(privat->paste_controller, action);
  }
}

