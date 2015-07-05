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

/*! \file gpform-factory.c
 */

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "misc-object.h"

#include "gpform.h"

#define GPFORM_FACTORY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),GPFORM_TYPE_FACTORY,GPFormFactoryPrivate))

enum
{
    GPFORM_FACTORY_DIALOG_PARENT = 1,
    GPFORM_FACTORY_FORM_ID,
    GPFORM_FACTORY_IMAGE_PATH,
    GPFORM_FACTORY_XML_PATH
};

typedef struct _GPFormFactoryPrivate GPFormFactoryPrivate;

struct _GPFormFactoryPrivate
{
    GtkWindow *dialog_parent;
    char      *form_id;
    char      *image_path;
    char      *xml_path;
};



static void
gpform_factory_class_init(gpointer g_class, gpointer g_class_data);

static void
gpform_factory_dispose(GObject *object);

static void
gpform_factory_finalize(GObject *object);

static void
gpform_factory_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
gpform_factory_init(GTypeInstance *instance, gpointer g_class);

static void
gpform_factory_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);



static void
gpform_factory_class_init(gpointer g_class, gpointer g_class_data)
{
  GObjectClass *class = G_OBJECT_CLASS(g_class);

  g_type_class_add_private(class, sizeof(GPFormFactoryPrivate));

  class->dispose  = gpform_factory_dispose;
  class->finalize = gpform_factory_finalize;

  class->get_property = gpform_factory_get_property;
  class->set_property = gpform_factory_set_property;

  g_object_class_install_property(class,
                                  GPFORM_FACTORY_DIALOG_PARENT,
                                  g_param_spec_object( "dialog-parent",
                                                       "Dialog Parent",
                                                       "Dialog Parent",
                                                       GTK_TYPE_WINDOW,
                                                       G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
                                  )
  );


  g_object_class_install_property(class,
                                  GPFORM_FACTORY_FORM_ID,
                                  g_param_spec_string("form-id",
                                                      "Form ID",
                                                      "Form ID",
                                                      NULL,
                                                      G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
                                  )
  );

  g_object_class_install_property(class,
                                  GPFORM_FACTORY_IMAGE_PATH,
                                  g_param_spec_string("image-path",
                                                      "Image Path",
                                                      "Image Path",
                                                      NULL,
                                                      G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
                                  )
  );

  g_object_class_install_property(class,
                                  GPFORM_FACTORY_XML_PATH,
                                  g_param_spec_string("xml-path",
                                                      "XML path",
                                                      "XML path",
                                                      NULL,
                                                      G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
                                  )
  );

  /* Ensure other classes are loaded into the type system */
  /* to allow dynamic instantiation.                      */

  GPFORM_TYPE_UI_COMBO;
  GPFORM_TYPE_UI_DIALOG;
  GPFORM_TYPE_UI_ENTRY;
  GPFORM_TYPE_UI_IMAGE;
  GPFORM_TYPE_UI_LABEL;
}

GPFormUIDialog*
gpform_factory_create_form(const GPFormFactory *factory, const char *filename)
{
  GPFormUIDialog *dialog = NULL;

  GPFormFactoryPrivate *privat = GPFORM_FACTORY_GET_PRIVATE(factory);

  if (privat != NULL) {

    GtkBuilder *builder  = gtk_builder_new();
    char       *filepath = g_build_filename(privat->xml_path, filename, NULL);

    if ((builder != NULL) && (filepath != NULL)) {

      GError *err = NULL;

      if (!gtk_builder_add_from_file(builder, filepath, &err)) {
        fprintf(stderr, "gtk_builder encounted an error: '%s'", err->message);
        g_clear_error (&err);
      }

      dialog = GPFORM_UI_DIALOG(gtk_builder_get_object(builder, privat->form_id));
    }

    if (builder != NULL) {

      g_object_unref(builder);
    }

    builder = NULL;

    g_free(filepath);

    gtk_window_set_transient_for(GTK_WINDOW(dialog),
                                 privat->dialog_parent);
  }

  return dialog;
}

static void
gpform_factory_dispose(GObject *object)
{

    GPFormFactoryPrivate *privat = GPFORM_FACTORY_GET_PRIVATE(object);

    if (privat != NULL) {

        misc_object_unref(privat->dialog_parent);
        privat->dialog_parent = NULL;
    }

    misc_object_chain_dispose(object);
}

static void
gpform_factory_finalize(GObject *object)
{
    GPFormFactoryPrivate *privat = GPFORM_FACTORY_GET_PRIVATE(object);

    if (privat != NULL) {

        g_free(privat->form_id);
        privat->form_id = NULL;

        g_free(privat->image_path);
        privat->image_path = NULL;

        g_free(privat->xml_path);
        privat->xml_path = NULL;
    }

    misc_object_chain_finalize(object);
}

GtkWindow*
gpform_factory_get_dialog_parent(const GPFormFactory *factory)
{
    GtkWindow *parent = NULL;

    if (factory != NULL) {

        GPFormFactoryPrivate *privat = GPFORM_FACTORY_GET_PRIVATE(factory);

        if (privat != NULL) {

            parent = privat->dialog_parent;

            if (parent != NULL) {

                g_object_ref(parent);
            }
        }
    }

    return parent;
}

static void
gpform_factory_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    GPFormFactory *factory = GPFORM_FACTORY(object);

    if (factory != NULL) {

        switch (property_id) {

            case GPFORM_FACTORY_DIALOG_PARENT:
                g_value_take_object(value, gpform_factory_get_dialog_parent(factory));
                break;

            case GPFORM_FACTORY_FORM_ID:
                g_value_take_string(value, gpform_factory_get_form_id(factory));
                break;

            case GPFORM_FACTORY_IMAGE_PATH:
                g_value_take_string(value, gpform_factory_get_image_path(factory));
                break;

            case GPFORM_FACTORY_XML_PATH:
                g_value_take_string(value, gpform_factory_get_xml_path(factory));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

unsigned int
gpform_factory_get_type(void)
{
  static unsigned int type = G_TYPE_INVALID;

  if (type == G_TYPE_INVALID) {

    static const GTypeInfo tinfo = {
      sizeof(GPFormFactoryClass),    /* class_size */
      NULL,                          /* base_init */
      NULL,                          /* base_finalize */
      gpform_factory_class_init,     /* class_init */
      NULL,                          /* class_finalize */
      NULL,                          /* class_data */
      sizeof(GPFormFactory),         /* instance_size */
      0,                             /* n_preallocs */
      gpform_factory_init,           /* instance_init */
      NULL                           /* value_table */
    };

    type = g_type_register_static(G_TYPE_OBJECT,
                                  "GPFormFactory",
                                  &tinfo,
                                  0
    );
  }

  return type;
}

void
gpform_factory_set_dialog_parent(GPFormFactory *factory, GtkWindow *parent)
{
    GPFormFactoryPrivate *privat = GPFORM_FACTORY_GET_PRIVATE(factory);

    if (privat != NULL) {

        if (privat->dialog_parent != NULL) {

            g_object_unref(privat->dialog_parent);
        }

        privat->dialog_parent = parent;

        if (privat->dialog_parent != NULL) {

            g_object_ref(privat->dialog_parent);
        }
    }
}

static void
gpform_factory_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GPFormFactory *factory = GPFORM_FACTORY(object);

    if (factory != NULL) {

        switch (property_id) {

            case GPFORM_FACTORY_DIALOG_PARENT:
                gpform_factory_set_dialog_parent(factory, g_value_get_object(value));
                break;

            case GPFORM_FACTORY_FORM_ID:
                gpform_factory_set_form_id(factory, g_value_get_string(value));
                break;

            case GPFORM_FACTORY_IMAGE_PATH:
                gpform_factory_set_image_path(factory, g_value_get_string(value));
                break;

            case GPFORM_FACTORY_XML_PATH:
                gpform_factory_set_xml_path(factory, g_value_get_string(value));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

char*
gpform_factory_get_form_id(const GPFormFactory *factory)
{
    char *form_id = NULL;
    GPFormFactoryPrivate *privat = GPFORM_FACTORY_GET_PRIVATE(factory);

    if (privat != NULL) {

        form_id = g_strdup(privat->form_id);
    }

    return form_id;
}

char*
gpform_factory_get_image_path(const GPFormFactory *factory)
{
    GPFormFactoryPrivate *privat = GPFORM_FACTORY_GET_PRIVATE(factory);
    char *image_path = NULL;

    if (privat != NULL) {

        image_path = g_strdup(privat->image_path);
    }

    return image_path;
}

char*
gpform_factory_get_xml_path(const GPFormFactory *factory)
{
    GPFormFactoryPrivate *privat = GPFORM_FACTORY_GET_PRIVATE(factory);
    char *xml_path = NULL;

    if (privat != NULL) {

        xml_path = g_strdup(privat->xml_path);
    }

    return xml_path;
}

static void
gpform_factory_init(GTypeInstance *instance, gpointer g_class)
{
  GPFormFactoryPrivate *privat = GPFORM_FACTORY_GET_PRIVATE(instance);

  if (privat != NULL) {

    privat->form_id = g_strdup("main");

    /* \todo Use install location for deployed version */

    privat->image_path = g_build_path(G_DIR_SEPARATOR_S,
                                      ".",
                                      /* DATADIR, */
                                      /* PACKAGE_NAME, */
                                      /* "image", */
                                      NULL);

    privat->xml_path = g_build_path(G_DIR_SEPARATOR_S,
                                    "..",
                                    "xml",
                                    "forms",
                                    /* DATADIR, */
                                    /* PACKAGE_NAME, */
                                    /* "xml", */
                                    /* "forms", */
                                    NULL);
  }
}

GPFormFactory*
gpform_factory_new(void)
{
    return GPFORM_FACTORY(g_object_new(GPFORM_TYPE_FACTORY, NULL));
}

void
gpform_factory_set_form_id(GPFormFactory *factory, const char *form_id)
{
  GPFormFactoryPrivate *privat = GPFORM_FACTORY_GET_PRIVATE(factory);

  if (privat != NULL) {

    g_free(privat->form_id);

    privat->form_id = g_strdup(form_id);

    g_object_notify(G_OBJECT(factory), "form-id");
  }
}

void
gpform_factory_set_image_path(GPFormFactory *factory, const char *image_path)
{
  GPFormFactoryPrivate *privat = GPFORM_FACTORY_GET_PRIVATE(factory);

  if (privat != NULL) {

    g_free(privat->image_path);

    privat->image_path = g_strdup(image_path);

    g_object_notify(G_OBJECT(factory), "image-path");
  }
}

void
gpform_factory_set_xml_path(GPFormFactory *factory, const char *xml_path)
{
    GPFormFactoryPrivate *privat = GPFORM_FACTORY_GET_PRIVATE(factory);

    if (privat != NULL) {

        g_free(privat->xml_path);

        privat->xml_path = g_strdup(xml_path);

        g_object_notify(G_OBJECT(factory), "xml-path");
    }
}

