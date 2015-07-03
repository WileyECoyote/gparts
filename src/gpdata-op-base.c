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

/*! \file gpdata-op-base.c
 */

#include <glib.h>
#include <glib-object.h>
#include <libxml/xmlreader.h>

#include "misc-object.h"

#include "gpdata.h"

#define GPDATA_OP_BASE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),GPDATA_TYPE_OP_BASE,GPDataOPBasePrivate))

enum
{
    GPDATA_OP_BASE_XML_NAME = 1
};

typedef struct _GPDataOPBasePrivate GPDataOPBasePrivate;

struct _GPDataOPBasePrivate
{
    char *xml_name;
};



static void
gpdata_op_base_class_init(gpointer g_class, gpointer g_class_data);

static void
gpdata_op_base_dispose(GObject *object);

static void
gpdata_op_base_finalize(GObject *object);

static void
gpdata_op_base_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
gpdata_op_base_init(GTypeInstance *instance, gpointer g_class);

static void
gpdata_op_base_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

/*
static void
gpdata_op_base_check_cb(GPDataOPBase *base, xmlTextReader *reader, GError **error);
*/

/*
static void
gpdata_op_base_parse_cb(GPDataOPBase *base, xmlTextReader *reader, GPDataOPBuildable *object, GError **error);
*/

void
gpdata_op_base_check(GPDataOPBase *base, xmlTextReader *reader, GError **error)
{
    if (base != NULL) {

        GPDataOPBaseClass *klasse = GPDATA_OP_BASE_CLASS(base);

        if (klasse == NULL)
        {
            g_critical("Unable to get GPDataOPBaseClass from parameter");
        }
        else if (klasse->check == NULL)
        {
            g_critical("GPDataOPBaseClass contains NULL check() function pointer");
        }
        else
        {
            klasse->check(base, reader, error);
        }
    }
}

/*
static void
gpdata_op_base_check_cb(GPDataOPBase *base, xmlTextReader *reader, GError **error)
{
    if (reader == NULL) {

    }
    else if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT)
    {

    }
    else {

        GPDataOPBasePrivate *privat = GPDATA_OP_BASE_GET_PRIVATE(base);

        if (privat != NULL) {

//            const xmlChar *a = xmlTextReaderConstValue(reader);
//            xmlChar *b = xmlCharStrdup(privat->xml_name);
//
//            if (!xmlStrEqual(a,b))
//            {
//            }
//
//            free(b);
        }
    }
}
*/

static void
gpdata_op_base_class_init(void *g_class, void *g_class_data)
{
    GObjectClass *klasse = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(klasse, sizeof(GPDataOPBasePrivate));

    klasse->dispose  = gpdata_op_base_dispose;
    klasse->finalize = gpdata_op_base_finalize;

    klasse->get_property = gpdata_op_base_get_property;
    klasse->set_property = gpdata_op_base_set_property;

    g_object_class_install_property(
        klasse,
        GPDATA_OP_BASE_XML_NAME,
        g_param_spec_string(
            "xml-name",
            "XML Name",
            "XML Name",
            NULL,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );
}

static void
gpdata_op_base_dispose(GObject *object)
{
    misc_object_chain_dispose(object);
}

static void
gpdata_op_base_finalize(GObject *object)
{
    GPDataOPBasePrivate *privat = GPDATA_OP_BASE_GET_PRIVATE(object);

    if (privat != NULL) {

        g_free(privat->xml_name);
        privat->xml_name = NULL;
    }

    misc_object_chain_finalize(object);
}

static void
gpdata_op_base_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    GPDataOPBase *factory = GPDATA_OP_BASE(object);

    if (factory != NULL) {

        switch (property_id) {

            case GPDATA_OP_BASE_XML_NAME:
                g_value_take_string(value, gpdata_op_base_get_xml_name(factory));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
gpdata_op_base_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {

        static const GTypeInfo tinfo = {
            sizeof(GPDataOPBaseClass),    /* class_size */
            NULL,                         /* base_init */
            NULL,                         /* base_finalize */
            gpdata_op_base_class_init,    /* class_init */
            NULL,                         /* class_finalize */
            NULL,                         /* class_data */
            sizeof(GPDataOPBase),         /* instance_size */
            0,                            /* n_preallocs */
            gpdata_op_base_init,          /* instance_init */
            NULL                          /* value_table */
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "GPDataOPBase",
            &tinfo,
            G_TYPE_FLAG_ABSTRACT
            );
    }

    return type;
}

char*
gpdata_op_base_get_xml_name(const GPDataOPBase *factory)
{
    GPDataOPBasePrivate *privat = GPDATA_OP_BASE_GET_PRIVATE(factory);
    char *xml_name = NULL;

    if (privat != NULL) {

        xml_name = g_strdup(privat->xml_name);
    }

    return xml_name;
}


static void
gpdata_op_base_init(GTypeInstance *instance, gpointer g_class)
{
    GPDataOPBasePrivate *privat = GPDATA_OP_BASE_GET_PRIVATE(instance);

    if (privat != NULL) {

        /* \todo Use install location for deployed version */

        privat->xml_name = g_build_path(
            G_DIR_SEPARATOR_S,
            ".",
            /* DATADIR, */
            /* PACKAGE_NAME, */
            /* "xml", */
            /* "models", */
            NULL
            );
    }
}

void
gpdata_op_base_parse(GPDataOPBase *base, xmlTextReader *reader, GPDataOPBuildable *object, GError **error)
{
    if (base != NULL) {

        GPDataOPBaseClass *klasse = GPDATA_OP_BASE_CLASS(base);

        if (klasse == NULL) {
            g_critical("Unable to get GPDataOPBaseClass from parameter");
        }
        else if (klasse->parse == NULL) {
            g_critical("GPDataOPBaseClass contains NULL parse() function pointer");
        }
        else {
            klasse->parse(base, reader, object, error);
        }
    }
}

/*
static void
gpdata_op_base_parse_cb(GPDataOPBase *base, xmlTextReader *reader, GPDataOPBuildable *object, GError **error)
{
    if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {

    }

}
*/

static void
gpdata_op_base_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GPDataOPBase *factory = GPDATA_OP_BASE(object);

    if (factory != NULL) {

        switch (property_id) {

            case GPDATA_OP_BASE_XML_NAME:
                gpdata_op_base_set_xml_name(factory, g_value_get_string(value));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

void
gpdata_op_base_set_xml_name(GPDataOPBase *factory, const gchar *xml_name)
{
    GPDataOPBasePrivate *privat = GPDATA_OP_BASE_GET_PRIVATE(factory);

    if (privat != NULL) {

        g_free(privat->xml_name);

        privat->xml_name = g_strdup(xml_name);

        g_object_notify(G_OBJECT(factory), "xml-name");
    }
}
