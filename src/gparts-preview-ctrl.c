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

/*! \file gparts-preview-ctrl.c
 */

#include "gpartsui.h"
#include "schgui-clipboard.h"
#include "misc-macro.h"

enum
{
    GPARTS_PREVIEW_CTRL_PROPID_COPY_ACTION = 1,

    GPARTS_PREVIEW_CTRL_PROPID_ATTRIB_SOURCE,
    GPARTS_PREVIEW_CTRL_PROPID_SYMBOL_SOURCE,
    GPARTS_PREVIEW_CTRL_PROPID_TARGET,
};

typedef struct _GPartsPreviewCtrlPrivate GPartsPreviewCtrlPrivate;

struct _GPartsPreviewCtrlPrivate
{
    GtkAction        *copy_action;

    SchGUIClipboard  *clipboard;

    GtkTreeView      *symbol_source;
    GtkTreeView      *attrib_source;
    GtkDrawingArea   *target;
};

#define GPARTS_PREVIEW_CTRL_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE(object,GPARTS_TYPE_PREVIEW_CTRL,GPartsPreviewCtrlPrivate)

static char *
gparts_preview_ctrl_build(GPartsPreviewCtrl *controller);

static void
gparts_preview_ctrl_class_init(gpointer g_class, gpointer g_class_data);

static void
gparts_preview_ctrl_dispose(GObject *object);

static void
gparts_preview_ctrl_finalize(GObject *object);

static void
gparts_preview_ctrl_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
gparts_preview_ctrl_set_copy_action(GPartsController *controller, GtkAction *action);

static void
gparts_preview_ctrl_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void
gparts_preview_ctrl_set_result(GPartsPreviewCtrl *query, GPartsDatabaseResult *result);

/**** Signal handlers ****/

static void
gparts_preview_ctrl_copy_action_cb(GtkAction *action, GPartsPreviewCtrl *preview_ctrl);

static void
gparts_preview_ctrl_show_cb(GtkWidget *widget, GPartsPreviewCtrl *controller);

static void
gparts_preview_ctrl_updated_cb(GtkWidget *widget, GPartsPreviewCtrl *controller)
{
    GPartsPreviewCtrlPrivate *privat = GPARTS_PREVIEW_CTRL_GET_PRIVATE(controller);

    if (privat != NULL) {

        SchComponent *component;
        SchDrawing   *drawing = NULL;
        SchDrawing   *symbol = NULL;
        char         *symbol_name;

        GPartsController *control = (GPartsController *)controller;

        //symbol_name = gparts_controller_get_field(privat->symbol_source, "SymbolName");
        symbol_name = gparts_controller_get_field(control, "SymbolName");

        g_debug("Symbol name: %s", symbol_name);

        if (symbol_name != NULL) {

            SchLoader *loader = sch_loader_get_default();

            if (loader != NULL) {

                symbol = sch_loader_load_symbol(loader, symbol_name, NULL);
            }

            g_free(symbol_name);
        }

        component = sch_component_instantiate(sch_config_new(), symbol);

        if (symbol != NULL) {

            g_object_unref(symbol);
        }

        if (component != NULL) {

            GRegex *regex     = misc_macro_new_regex();

            //GHashTable *table = gparts_controller_get_table(privat->attrib_source);
            GHashTable *table = gparts_controller_get_table(control);

            sch_shape_expand_macros(SCH_SHAPE(component), regex, table);
            g_regex_unref(regex);
            g_hash_table_unref(table);

            drawing = sch_drawing_new();

            sch_drawing_append_shape(drawing, SCH_SHAPE(component));
            g_object_unref(component);
        }

        SchGUIDrawingView *widget =(SchGUIDrawingView*)privat->target;

        schgui_drawing_view_set_drawing(widget, drawing);

        if (drawing != NULL) {

            g_object_unref(drawing);
        }
    }
}



/*! \brief Initialize class data.
 *
 *  \param [in] g_class
 *  \param [in] g_class_data
 */
static void
gparts_preview_ctrl_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass* object_class = G_OBJECT_CLASS(g_class);
    GPartsControllerClass *klasse = GPARTS_CONTROLLER_CLASS(g_class);

    g_type_class_add_private(g_class, sizeof(GPartsPreviewCtrlPrivate));

    object_class->dispose  = gparts_preview_ctrl_dispose;
    object_class->finalize = gparts_preview_ctrl_finalize;

    object_class->get_property = gparts_preview_ctrl_get_property;
    object_class->set_property = gparts_preview_ctrl_set_property;

    klasse->set_copy_action = gparts_preview_ctrl_set_copy_action;

    g_object_class_install_property(
        object_class,
        GPARTS_PREVIEW_CTRL_PROPID_COPY_ACTION,
        g_param_spec_object(
            "copy-action",
            "",
            "",
            GTK_TYPE_ACTION,
            G_PARAM_READWRITE
            )
        );

    g_object_class_install_property(
        object_class,
        GPARTS_PREVIEW_CTRL_PROPID_ATTRIB_SOURCE,
        g_param_spec_object(
            "attrib-source",
            "",
            "",
            G_TYPE_OBJECT,
            G_PARAM_READWRITE
            )
        );

    g_object_class_install_property(
        object_class,
        GPARTS_PREVIEW_CTRL_PROPID_SYMBOL_SOURCE,
        g_param_spec_object(
            "symbol-source",
            "",
            "",
            G_TYPE_OBJECT,
            G_PARAM_READWRITE
            )
        );

    g_object_class_install_property(
        object_class,
        GPARTS_PREVIEW_CTRL_PROPID_TARGET,
        g_param_spec_object(
            "target",
            "",
            "",
            GTK_TYPE_DRAWING_AREA,
            G_PARAM_READWRITE
            )
        );

}

static void
gparts_preview_ctrl_copy_action_cb(GtkAction *action, GPartsPreviewCtrl *preview_ctrl)
{
    GPartsPreviewCtrlPrivate *privat = GPARTS_PREVIEW_CTRL_GET_PRIVATE(preview_ctrl);

    if (privat != NULL) {

        SchGUIDrawingView *widget;
        SchDrawing        *drawing;

        widget = (SchGUIDrawingView*)privat->target;

        drawing = schgui_drawing_view_get_drawing(widget);

        if (drawing != NULL) {

            if (privat->clipboard == NULL) {
                privat->clipboard = schgui_clipboard_new();
            }

            schgui_clipboard_copy_drawing(privat->clipboard, drawing);

            g_object_unref(drawing);
        }
    }
}

static void
gparts_preview_ctrl_dispose(GObject *object)
{
    //gparts_preview_ctrl_set_database(object, NULL);
    //gparts_preview_ctrl_set_target(object, NULL);

    misc_object_chain_dispose(object);
}

/*! \brief Deallocate all resources.
 *
 *  This function performs the second and last stage of object destruction.
 *
 *  \param [in] object The object to finalize.
 */
static void
gparts_preview_ctrl_finalize(GObject *object)
{
    g_debug("gparts_preview_ctrl_finalize");

    GPartsPreviewCtrlPrivate *private = GPARTS_PREVIEW_CTRL_GET_PRIVATE(object);

    misc_object_chain_finalize(object);
}

static void
gparts_preview_ctrl_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
}

GType
gparts_preview_ctrl_get_type(void)
{
    static GType type = 0;

    if ( type == 0 )
    {
        static const GTypeInfo tinfo = {
            sizeof(GPartsPreviewCtrlClass),
            NULL,
            NULL,
            gparts_preview_ctrl_class_init,
            NULL,
            NULL,
            sizeof(GPartsPreviewCtrl),
            0,
            NULL,
            NULL
            };

        type = g_type_register_static(
            GPARTS_TYPE_CONTROLLER,
            "gparts-view-controller",
            &tinfo,
            0
            );
    }
    return type;
}

GPartsPreviewCtrl*
gparts_preview_ctrl_new()
{
}

static void
gparts_preview_notify_drawing_cb(GObject *object, GParamSpec *param, gpointer user_data)
{
    GPartsPreviewCtrlPrivate *privat = GPARTS_PREVIEW_CTRL_GET_PRIVATE(user_data);

    if (privat != NULL) {

        SchGUIDrawingView *widget = (SchGUIDrawingView*) object; /* WEH??? */
        SchDrawing *drawing       = schgui_drawing_view_get_drawing(widget);

        if (drawing != NULL) {

            gtk_action_set_sensitive(privat->copy_action, TRUE);

            g_object_unref(drawing);
        }
        else {

            gtk_action_set_sensitive(privat->copy_action, FALSE);
        }
    }
}

static void
gparts_preview_ctrl_show_cb(GtkWidget *widget, GPartsPreviewCtrl *preview_ctrl)
{
}

static void
gparts_preview_ctrl_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GPartsPreviewCtrl *preview_ctrl = GPARTS_PREVIEW_CTRL(object);

    switch (property_id) {

        case GPARTS_PREVIEW_CTRL_PROPID_COPY_ACTION:
            gparts_preview_ctrl_set_copy_action((GPartsController*)preview_ctrl, g_value_get_object(value));
            break;

        case GPARTS_PREVIEW_CTRL_PROPID_ATTRIB_SOURCE:
            gparts_preview_ctrl_set_attrib_source(preview_ctrl, g_value_get_object(value));
            break;

        case GPARTS_PREVIEW_CTRL_PROPID_SYMBOL_SOURCE:
            gparts_preview_ctrl_set_symbol_source(preview_ctrl, g_value_get_object(value));
            break;

        case GPARTS_PREVIEW_CTRL_PROPID_TARGET:
            gparts_preview_ctrl_set_target(preview_ctrl, g_value_get_object(value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static void
gparts_preview_ctrl_set_copy_action(GPartsController *controller, GtkAction *action)
{
    GPartsPreviewCtrlPrivate *privat = GPARTS_PREVIEW_CTRL_GET_PRIVATE(controller);

    if (privat->copy_action != action) {

        if (privat->copy_action != NULL) {

            g_signal_handlers_disconnect_by_func(
                privat->copy_action,
                G_CALLBACK(gparts_preview_ctrl_copy_action_cb),
                controller
                );

            g_object_unref(privat->copy_action);
        }

        privat->copy_action = action;

        if (privat->copy_action != NULL) {

            g_object_ref(privat->copy_action);

            g_signal_connect(privat->copy_action,
                             "activate",
                             G_CALLBACK(gparts_preview_ctrl_copy_action_cb),
                             controller);

            gtk_action_set_label(privat->copy_action, "_Copy Component");
            //gtk_action_set_sensitive(privat->copy_action, (privat->database != NULL));
        }

        g_object_notify(G_OBJECT(controller), "copy-action");
    }
}

void
gparts_preview_ctrl_set_attrib_source(GPartsPreviewCtrl *preview_ctrl, GtkTreeView *attrib_source)
{
    GPartsPreviewCtrlPrivate *privat = GPARTS_PREVIEW_CTRL_GET_PRIVATE(preview_ctrl);

    if (privat->attrib_source = attrib_source) {

        if (privat->attrib_source != NULL) {

            g_signal_handlers_disconnect_by_func(
                privat->attrib_source,
                G_CALLBACK(gparts_preview_ctrl_updated_cb),
                preview_ctrl);

            g_object_unref(privat->attrib_source);
        }

        privat->attrib_source = attrib_source;

        if (privat->attrib_source != NULL) {

            g_object_ref(privat->attrib_source);

            g_signal_connect(privat->attrib_source,
                             "updated",
                             G_CALLBACK(gparts_preview_ctrl_updated_cb),
                             preview_ctrl);
        }

        g_object_notify(G_OBJECT(preview_ctrl), "attrib-source");
    }
}


void
gparts_preview_ctrl_set_symbol_source(GPartsPreviewCtrl *preview_ctrl, GtkTreeView *symbol_source)
{
    GPartsPreviewCtrlPrivate *privat = GPARTS_PREVIEW_CTRL_GET_PRIVATE(preview_ctrl);

    if (privat->symbol_source = symbol_source) {

        if (privat->symbol_source != NULL) {

            g_signal_handlers_disconnect_by_func(
                privat->symbol_source,
                G_CALLBACK(gparts_preview_ctrl_updated_cb),
                preview_ctrl
                );

            g_object_unref(privat->symbol_source);
        }

        privat->symbol_source = symbol_source;

        if (privat->symbol_source != NULL) {

            g_object_ref(privat->symbol_source);

            g_signal_connect(
                privat->symbol_source,
                "updated",
                G_CALLBACK(gparts_preview_ctrl_updated_cb),
                preview_ctrl
                );
        }

        g_object_notify(G_OBJECT(preview_ctrl), "symbol-source");
    }
}

void
gparts_preview_ctrl_set_target(GPartsPreviewCtrl *preview_ctrl, GtkDrawingArea *target)
{
  GPartsPreviewCtrlPrivate *privat = GPARTS_PREVIEW_CTRL_GET_PRIVATE(preview_ctrl);

  if (privat->target != target) {

    if (privat->target != NULL) {

      g_signal_handlers_disconnect_by_func(privat->target,
                                           G_CALLBACK(gparts_preview_notify_drawing_cb),
                                           preview_ctrl);

      g_signal_handlers_disconnect_by_func(privat->target,
                                           G_CALLBACK(gparts_preview_ctrl_show_cb),
                                           preview_ctrl);

      g_object_unref(privat->target);
    }

    privat->target = target;

    if (privat->target != NULL) {

      g_object_ref(privat->target);

      g_signal_connect(privat->target,
                       "notify::drawing",
                       G_CALLBACK(gparts_preview_notify_drawing_cb),
                       preview_ctrl);

      g_signal_connect(privat->target,
                       "show",
                       G_CALLBACK(gparts_preview_ctrl_show_cb),
                       preview_ctrl);
    }

    g_object_notify(G_OBJECT(preview_ctrl), "target");
  }
}
