/* gEDA - GPL Electronic Design Automation
 * gparts - gEDA Parts Manager
 * Copyright (C) 2011 Edward C. Hennessy
 * Copyright (C) 2011 gEDA Contributors (see ChangeLog for details)
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

/*! \file gpform-ui-image.c
 */

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "misc-object.h"

#include "gpform.h"

#define GPFORM_UI_IMAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),GPFORM_TYPE_UI_IMAGE,GPFormUIImagePrivate))

enum
{
    GPFORM_UI_IMAGE_FILENAME = 1
};

typedef struct _GPFormUIImagePrivate GPFormUIImagePrivate;

struct _GPFormUIImagePrivate
{
    gint dummy;
};



static void
gpform_ui_image_class_init(gpointer g_class, gpointer g_class_data);

static void
gpform_ui_image_dispose(GObject *object);

static void
gpform_ui_image_finalize(GObject *object);

static void
gpform_ui_image_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
gpform_ui_image_init(GTypeInstance *instance, gpointer g_class);

static void
gpform_ui_image_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);



static void
gpform_ui_image_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *klasse = G_OBJECT_CLASS(g_class);
    /*GPFormUIImageClass *klasse1 = GPFORM_UI_IMAGE_CLASS(g_class); */

    g_type_class_add_private(klasse, sizeof(GPFormUIImagePrivate));

    klasse->dispose  = gpform_ui_image_dispose;
    klasse->finalize = gpform_ui_image_finalize;

    klasse->get_property = gpform_ui_image_get_property;
    klasse->set_property = gpform_ui_image_set_property;

    g_object_class_install_property(
        klasse,
        GPFORM_UI_IMAGE_FILENAME,
        g_param_spec_string(
            "filename",
            "Filename",
            "Filename",
            NULL,
            G_PARAM_LAX_VALIDATION | G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS
            )
        );
}

static void
gpform_ui_image_dispose(GObject *object)
{
    misc_object_chain_dispose(object);
}

static void
gpform_ui_image_finalize(GObject *object)
{
    GPFormUIImagePrivate *privat = GPFORM_UI_IMAGE_GET_PRIVATE(object);

    if (privat != NULL)
    {
    }

    misc_object_chain_finalize(object);
}

static void
gpform_ui_image_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    GPFormUIImage *image = GPFORM_UI_IMAGE(object);

    if (image != NULL)
    {
        switch (property_id)
        {
            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
gpform_ui_image_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(GPFormUIImageClass),    /* class_size */
            NULL,                          /* base_init */
            NULL,                          /* base_finalize */
            gpform_ui_image_class_init,    /* class_init */
            NULL,                          /* class_finalize */
            NULL,                          /* class_data */
            sizeof(GPFormUIImage),         /* instance_size */
            0,                             /* n_preallocs */
            gpform_ui_image_init,          /* instance_init */
            NULL                           /* value_table */
            };

        type = g_type_register_static(
            GTK_TYPE_IMAGE,
            "GPFormUIImage",
            &tinfo,
            0
            );
    }

    return type;
}

static void
gpform_ui_image_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GPFormUIImage *image = GPFORM_UI_IMAGE(object);

    if (image != NULL)
    {
        switch (property_id)
        {
            case GPFORM_UI_IMAGE_FILENAME:
                gpform_ui_image_set_from_file(image, g_value_get_string(value));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

static void
gpform_ui_image_init(GTypeInstance *instance, gpointer g_class)
{
    GPFormUIImagePrivate *privat = GPFORM_UI_IMAGE_GET_PRIVATE(instance);

    if (privat != NULL)
    {
    }
}

GPFormUIImage*
gpform_ui_image_new(void)
{
    return GPFORM_UI_IMAGE(g_object_new(GPFORM_TYPE_UI_IMAGE, NULL));
}

void
gpform_ui_image_set_from_file(GPFormUIImage *image, const gchar *filename)
{
    GdkPixbufAnimation *animation = NULL;

    /*! \todo Move to a factory */

    if (filename != NULL)
    {
        gchar *pathname = g_build_path(
            G_DIR_SEPARATOR_S,
            ".",
            /* DATADIR, */
            /* PACKAGE_NAME, */
            /* "image", */
            filename,
            NULL
            );

        animation = gdk_pixbuf_animation_new_from_file(pathname, NULL);

        g_free(pathname);
    }

    if (animation != NULL)
    {
        if (gdk_pixbuf_animation_is_static_image(animation))
        {
            gtk_image_set_from_pixbuf(
                GTK_IMAGE(image),
                gdk_pixbuf_animation_get_static_image(animation)
                );
        }
        else
        {
            gtk_image_set_from_animation(GTK_IMAGE(image), animation);
        }

        g_object_unref(G_OBJECT(animation));
    }
    else
    {
        gtk_image_set_from_stock(
            GTK_IMAGE(image),
            GTK_STOCK_MISSING_IMAGE,
            GTK_ICON_SIZE_BUTTON
            );
    }
}

