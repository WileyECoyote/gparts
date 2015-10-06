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

/*! \file schgui-clipboard.c
 */

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "sch.h"

#include "schgui-clipboard.h"

#define SCHGUI_CLIPBOARD_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCHGUI_TYPE_CLIPBOARD,SchGUIClipboardPrivate))

enum
{
    SCHGUI_CLIPBOARD_CLIPBOARD = 1,
    SCHGUI_CLIPBOARD_DRAWING,
};

typedef struct _SchGUIClipboardPrivate SchGUIClipboardPrivate;

struct _SchGUIClipboardPrivate
{
    GtkClipboard *clipboard;
    SchDrawing   *drawing;
};

static const GtkTargetEntry schgui_clipboard_targets[] =
{
//    { "UTF8_STRING",                  0, 1 },
    { "application/x-geda-schematic", 0, 2 }
};

#define SCHGUI_CLIPBOARD_TARGET_COUNT(a) (sizeof(a)/sizeof(GtkTargetEntry))



static void
schgui_clipboard_class_init(void *g_class, void *g_class_data);

static void
schgui_clipboard_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
schgui_clipboard_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void
schgui_clipboard_clear_cb(GtkClipboard *clipboard, void *user_data);

static void
schgui_clipboard_get_cb(GtkClipboard *clipboard, GtkSelectionData *data, guint info, void *user_data);



static void
schgui_clipboard_class_init(void *g_class, void *g_class_data)
{
    GObjectClass *klasse = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(klasse, sizeof(SchGUIClipboardPrivate));

    klasse->get_property = schgui_clipboard_get_property;
    klasse->set_property = schgui_clipboard_set_property;

    g_object_class_install_property(
        klasse,
        SCHGUI_CLIPBOARD_CLIPBOARD,
        g_param_spec_object(
            "clipboard",
            "Clipboard",
            "Clipboard",
            GTK_TYPE_CLIPBOARD,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCHGUI_CLIPBOARD_DRAWING,
        g_param_spec_object(
            "drawing",
            "Drawing",
            "Drawing",
            SCH_TYPE_DRAWING,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );
}

static void
schgui_clipboard_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    SchGUIClipboardPrivate *privat = SCHGUI_CLIPBOARD_GET_PRIVATE(object);

    if (privat != NULL)
    {
        switch (property_id)
        {
            case SCHGUI_CLIPBOARD_DRAWING:
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
schgui_clipboard_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(SchGUIClipboardClass),    /* class_size */
            NULL,                    /* base_init */
            NULL,                    /* base_finalize */
            schgui_clipboard_class_init,     /* class_init */
            NULL,                    /* class_finalize */
            NULL,                    /* class_data */
            sizeof(SchGUIClipboard),         /* instance_size */
            0,                       /* n_preallocs */
            NULL,                    /* instance_init */
            NULL                     /* value_table */
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "SchGUIClipboard",
            &tinfo,
            0
            );
    }

    return type;
}

static void
schgui_clipboard_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    SchGUIClipboardPrivate *privat = SCHGUI_CLIPBOARD_GET_PRIVATE(object);

    if (privat != NULL)
    {
        switch (property_id)
        {
            case SCHGUI_CLIPBOARD_CLIPBOARD:
                /* construct only */
                privat->clipboard = g_value_dup_object(value);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

void
schgui_clipboard_copy_drawing(SchGUIClipboard *clipboard, SchDrawing *drawing)
{
  SchGUIClipboardPrivate *privat = SCHGUI_CLIPBOARD_GET_PRIVATE(clipboard);

  if (privat != NULL) {

    if (privat->drawing != NULL) {

      g_object_unref(privat->drawing);
    }

    privat->drawing = drawing;

    if (privat->drawing != NULL) {

      g_object_ref(privat->drawing);

      g_debug("Clipboard %p", privat->clipboard);
      g_debug("%d", SCHGUI_CLIPBOARD_TARGET_COUNT(schgui_clipboard_targets));

      gtk_clipboard_set_with_data(privat->clipboard,
                                  schgui_clipboard_targets,
                                  SCHGUI_CLIPBOARD_TARGET_COUNT(schgui_clipboard_targets),
                                  schgui_clipboard_get_cb,
                                  schgui_clipboard_clear_cb,
                                  clipboard
      );
    }
  }
}

static void
schgui_clipboard_get_cb(GtkClipboard *clipboard, GtkSelectionData *data,
                        unsigned int info,        void *user_data)
{
  SchGUIClipboardPrivate *privat = SCHGUI_CLIPBOARD_GET_PRIVATE(user_data);

  if ((privat != NULL) && (privat->drawing != NULL)) {

    GMemoryOutputStream *mostream;
    GOutputStream       *ostream;

    ostream = g_memory_output_stream_new(NULL, 0, g_realloc, g_free);

    SchOutputStream *sostream = sch_output_stream_new(ostream);

    sch_drawing_write(privat->drawing, sostream);

    g_debug("Get clipboard target: %d", info);

    mostream = (GMemoryOutputStream*)sostream;

    switch (info) {
      case 1:

        gtk_clipboard_set_text(clipboard,
                               g_memory_output_stream_get_data(mostream),
                               g_memory_output_stream_get_data_size(mostream));
        break;

      case 2:
        gtk_selection_data_set(
          data,
          gdk_atom_intern("application/x-geda-schematic", FALSE),
                               8,
                               g_memory_output_stream_get_data(mostream),
                               g_memory_output_stream_get_data_size(mostream)
        );
        break;
    }

    g_object_unref(ostream);
    g_object_unref(mostream);
  }
}

static void
schgui_clipboard_clear_cb(GtkClipboard *clipboard, void *user_data)
{
    g_debug("clear");
}

SchGUIClipboard*
schgui_clipboard_new(void)
{
    return g_object_new(SCHGUI_TYPE_CLIPBOARD,
                        "clipboard",
                        gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), NULL);
}

