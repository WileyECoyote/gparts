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

/*! \file gparts-connect-model.c
 */

#include "gpartsui.h"

#define GPARTSUI_CONNECT_MODEL_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE(object,GPARTSUI_TYPE_CONNECT_MODEL,GPartsUIConnectModelPrivate)

enum
{
    GPARTSUI_CONNECT_MODEL_CONNECT_SENSITIVE =1,
    GPARTSUI_CONNECT_MODEL_DATABASE_FACTORY,
    GPARTSUI_CONNECT_MODEL_DATABASE_MODEL,
    GPARTSUI_CONNECT_MODEL_DATABASE_NAME,
    GPARTSUI_CONNECT_MODEL_DATABASE_NAME_SENSITIVE,
    GPARTSUI_CONNECT_MODEL_DATABASE_NAME_VALID,
    GPARTSUI_CONNECT_MODEL_DATABASE_TYPE,
    GPARTSUI_CONNECT_MODEL_DATABASE_TYPES,
    GPARTSUI_CONNECT_MODEL_FILENAME,
    GPARTSUI_CONNECT_MODEL_FILENAME_SENSITIVE,
    GPARTSUI_CONNECT_MODEL_FILENAME_VALID,
    GPARTSUI_CONNECT_MODEL_HOSTNAME,
    GPARTSUI_CONNECT_MODEL_HOSTNAME_SENSITIVE,
    GPARTSUI_CONNECT_MODEL_HOSTNAME_VALID,
    GPARTSUI_CONNECT_MODEL_PASSWORD,
    GPARTSUI_CONNECT_MODEL_PASSWORD_SENSITIVE,
    GPARTSUI_CONNECT_MODEL_PASSWORD_VALID,
    GPARTSUI_CONNECT_MODEL_USERNAME,
    GPARTSUI_CONNECT_MODEL_USERNAME_SENSITIVE,
    GPARTSUI_CONNECT_MODEL_USERNAME_VALID,
};

typedef struct _GPartsUIConnectModelPrivate GPartsUIConnectModelPrivate;

struct _GPartsUIConnectModelPrivate
{
    GPartsDatabaseFactory *database_factory;
    GPartsUIDatabaseModel *database_model;

    gchar                 *database;
    gchar                 *filename;
    gchar                 *hostname;
    gchar                 *password;
    gchar                 *username;
};

static void
gpartsui_connect_model_class_init(gpointer g_class, gpointer g_class_data);

static void
gpartsui_connect_model_dispose(GObject *object);

static void
gpartsui_connect_model_finalize(GObject *object);

static void
gpartsui_connect_model_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
gpartsui_connect_model_instance_init(GTypeInstance *instance, gpointer g_class);

static void
gpartsui_connect_model_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);



static void
gpartsui_connect_model_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass* klasse = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(g_class, sizeof(GPartsUIConnectModelPrivate));

    klasse->dispose      = gpartsui_connect_model_dispose;
    klasse->finalize     = gpartsui_connect_model_finalize;
    klasse->get_property = gpartsui_connect_model_get_property;
    klasse->set_property = gpartsui_connect_model_set_property;

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_CONNECT_SENSITIVE,
        g_param_spec_boolean(
            "connect-sensitive",
            "Connect Sensitive",
            "Connect Sensitive",
            FALSE,
            G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_DATABASE_FACTORY,
        g_param_spec_object(
            "database-factory",
            "Database Factory",
            "Database Factory",
            GPARTS_TYPE_DATABASE_FACTORY,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_DATABASE_MODEL,
        g_param_spec_object(
            "database-model",
            "Database Model",
            "Database Model",
            GPARTSUI_TYPE_DATABASE_MODEL,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_DATABASE_NAME,
        g_param_spec_string(
            "database-name",
            "Database Name",
            "Database Name",
            NULL,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_DATABASE_NAME_SENSITIVE,
        g_param_spec_boolean(
            "database-name-sensitive",
            "Database Name Sensitive",
            "Database Name Sensitive",
            FALSE,
            G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_DATABASE_NAME_VALID,
        g_param_spec_boolean(
            "database-name-valid",
            "Database Name Valid",
            "Database Name Valid",
            FALSE,
            G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_DATABASE_TYPE,
        g_param_spec_string(
            "database-type",
            "Database Type",
            "Database Type",
            NULL,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_DATABASE_TYPES,
        g_param_spec_boxed(
            "database-types",
            "Database Types",
            "Database Types",
            G_TYPE_STRV,
            G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_FILENAME,
        g_param_spec_string(
            "filename",
            "Filename",
            "Filename",
            NULL,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_FILENAME_SENSITIVE,
        g_param_spec_boolean(
            "filename-sensitive",
            "Filename Sensitive",
            "Filename Sensitive",
            FALSE,
            G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_FILENAME_VALID,
        g_param_spec_boolean(
            "filename-valid",
            "Filename Valid",
            "Filename Valid",
            FALSE,
            G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_HOSTNAME,
        g_param_spec_string(
            "hostname",
            "Hostname",
            "Hostname",
            NULL,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_HOSTNAME_SENSITIVE,
        g_param_spec_boolean(
            "hostname-sensitive",
            "hostname Sensitive",
            "Hostname Sensitive",
            FALSE,
            G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_HOSTNAME_VALID,
        g_param_spec_boolean(
            "hostname-valid",
            "hostname Valid",
            "Hostname Valid",
            FALSE,
            G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_PASSWORD,
        g_param_spec_string(
            "password",
            "Password",
            "Password",
            NULL,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_PASSWORD_SENSITIVE,
        g_param_spec_boolean(
            "password-sensitive",
            "Password Sensitive",
            "Password Sensitive",
            FALSE,
            G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_PASSWORD_VALID,
        g_param_spec_boolean(
            "password-valid",
            "Password Valid",
            "Password Valid",
            FALSE,
            G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_USERNAME,
        g_param_spec_string(
            "username",
            "Username",
            "Username",
            NULL,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_USERNAME_SENSITIVE,
        g_param_spec_boolean(
            "username-sensitive",
            "Username Sensitive",
            "Username Sensitive",
            FALSE,
            G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        GPARTSUI_CONNECT_MODEL_USERNAME_VALID,
        g_param_spec_boolean(
            "username-valid",
            "Username Valid",
            "Username Valid",
            FALSE,
            G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
            )
        );
}

void
gpartsui_connect_model_connect(GPartsUIConnectModel *model, GError **error)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);

    if (privat != NULL)
    {
        GPartsDatabase *database = gparts_database_factory_create_database(privat->database_factory, NULL);

        if (database != NULL)
        {
            GPartsConnectData connect_data;

            connect_data.database = privat->database;
            connect_data.filename = privat->filename;
            connect_data.hostname = privat->hostname;
            connect_data.password = privat->password;
            connect_data.username = privat->username;

            gparts_database_connect(database, &connect_data, error);
        }

        /* a current defect in the code: the database must be connected before setting it in the model */

        gpartsui_database_model_set_database(privat->database_model, database);

        if (database != NULL)
        {
            g_object_unref(database);
        }
    }
}

static void
gpartsui_connect_model_dispose(GObject *object)
{
    misc_object_chain_dispose(object);
}

static void
gpartsui_connect_model_finalize(GObject *object)
{
    misc_object_chain_finalize(object);
}

gboolean
gpartsui_connect_model_get_connect_sensitive(const GPartsUIConnectModel *model)
{
    return TRUE;
}

GPartsDatabaseFactory*
gpartsui_connect_model_get_database_factory(const GPartsUIConnectModel *model)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);
    GPartsDatabaseFactory *factory = NULL;

    if (privat != NULL)
    {
        factory = privat->database_factory;

        if (factory != NULL)
        {
            g_object_ref(factory);
        }
    }

    return factory;
}

GPartsUIDatabaseModel*
gpartsui_connect_model_get_database_model(const GPartsUIConnectModel *model)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);
    GPartsUIDatabaseModel *dmodel = NULL;

    if (privat != NULL)
    {
        dmodel = privat->database_model;

        if (dmodel != NULL)
        {
            g_object_ref(dmodel);
        }
    }

    return dmodel;
}

gchar*
gpartsui_connect_model_get_database_name(const GPartsUIConnectModel *model)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);
    gchar *database = NULL;

    if (privat != NULL)
    {
        database = g_strdup(privat->database);
    }

    return database;
}

gboolean
gpartsui_connect_model_get_database_name_sensitive(const GPartsUIConnectModel *model)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);
    gboolean sensitive = FALSE;

    if (privat != NULL)
    {
        gint flags = gparts_database_factory_get_flags(privat->database_factory);

        sensitive = (flags & GPARTS_DATABASE_TYPE_FLAGS_USES_DATABASE);
    }

    return sensitive;
}

gboolean
gpartsui_connect_model_get_database_name_valid(const GPartsUIConnectModel *model)
{
    return TRUE;
}


gchar*
gpartsui_connect_model_get_database_type(const GPartsUIConnectModel *model)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);
    gchar *type = NULL;

    if (privat != NULL)
    {
        type = gparts_database_factory_get_name(privat->database_factory);
    }

    return type;
}

GStrv
gpartsui_connect_model_get_database_types(const GPartsUIConnectModel *model)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);
    GStrv types = NULL;

    if (privat != NULL) {

        GPartsConfig *config = gparts_config_new();

        GPartsDatabaseType *type = gparts_config_get_database_types(config);

        types = gparts_database_type_get_type_names(type);
    }

    return types;
}


gchar*
gpartsui_connect_model_get_filename(const GPartsUIConnectModel *model)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);
    gchar *filename = NULL;

    if (privat != NULL)
    {
        filename = g_strdup(privat->filename);
    }

    return filename;
}

gboolean
gpartsui_connect_model_get_filename_sensitive(const GPartsUIConnectModel *model)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);
    gboolean sensitive = FALSE;

    if (privat != NULL)
    {
        gint flags = gparts_database_factory_get_flags(privat->database_factory);

        sensitive = (flags & GPARTS_DATABASE_TYPE_FLAGS_USES_FILENAME);
    }

    return sensitive;
}

gboolean
gpartsui_connect_model_get_filename_valid(const GPartsUIConnectModel *model)
{
    return TRUE;
}

gchar*
gpartsui_connect_model_get_hostname(const GPartsUIConnectModel *model)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);
    gchar *hostname = NULL;

    if (privat != NULL)
    {
        hostname = g_strdup(privat->hostname);
    }

    return hostname;
}

gboolean
gpartsui_connect_model_get_hostname_sensitive(const GPartsUIConnectModel *model)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);
    gboolean sensitive = FALSE;

    if (privat != NULL)
    {
        gint flags = gparts_database_factory_get_flags(privat->database_factory);

        sensitive = (flags & GPARTS_DATABASE_TYPE_FLAGS_USES_HOSTNAME);
    }

    return sensitive;
}

gboolean
gpartsui_connect_model_get_hostname_valid(const GPartsUIConnectModel *model)
{
    return TRUE;
}

gchar*
gpartsui_connect_model_get_password(const GPartsUIConnectModel *model)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);
    gchar *password = NULL;

    if (privat != NULL)
    {
        password = g_strdup(privat->password);
    }

    return password;
}

gboolean
gpartsui_connect_model_get_password_sensitive(const GPartsUIConnectModel *model)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);
    gboolean sensitive = FALSE;

    if (privat != NULL)
    {
        gint flags = gparts_database_factory_get_flags(privat->database_factory);

        sensitive = (flags & GPARTS_DATABASE_TYPE_FLAGS_USES_PASSWORD);
    }

    return sensitive;
}

gboolean
gpartsui_connect_model_get_password_valid(const GPartsUIConnectModel *model)
{
    return TRUE;
}

static void
gpartsui_connect_model_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    GPartsUIConnectModel *model = GPARTSUI_CONNECT_MODEL(object);

    if (model != NULL)
    {
        switch ( property_id )
        {
            case GPARTSUI_CONNECT_MODEL_CONNECT_SENSITIVE:
                g_value_set_boolean(value, gpartsui_connect_model_get_connect_sensitive(model));
                break;

            case GPARTSUI_CONNECT_MODEL_DATABASE_FACTORY:
                g_value_take_object(value, gpartsui_connect_model_get_database_factory(model));
                break;

            case GPARTSUI_CONNECT_MODEL_DATABASE_MODEL:
                g_value_take_object(value, gpartsui_connect_model_get_database_model(model));
                break;

            case GPARTSUI_CONNECT_MODEL_DATABASE_NAME:
                g_value_take_string(value, gpartsui_connect_model_get_database_name(model));
                break;

            case GPARTSUI_CONNECT_MODEL_DATABASE_NAME_SENSITIVE:
                g_value_set_boolean(value, gpartsui_connect_model_get_database_name_sensitive(model));
                break;

            case GPARTSUI_CONNECT_MODEL_DATABASE_NAME_VALID:
                g_value_set_boolean(value, gpartsui_connect_model_get_database_name_valid(model));
                break;

            case GPARTSUI_CONNECT_MODEL_DATABASE_TYPE:
                g_value_take_string(value, gpartsui_connect_model_get_database_type(model));
                break;

            case GPARTSUI_CONNECT_MODEL_DATABASE_TYPES:
                g_value_take_boxed(value, gpartsui_connect_model_get_database_types(model));
                break;

            case GPARTSUI_CONNECT_MODEL_FILENAME:
                g_value_take_string(value, gpartsui_connect_model_get_filename(model));
                break;

            case GPARTSUI_CONNECT_MODEL_FILENAME_SENSITIVE:
                g_value_set_boolean(value, gpartsui_connect_model_get_filename_sensitive(model));
                break;

            case GPARTSUI_CONNECT_MODEL_FILENAME_VALID:
                g_value_set_boolean(value, gpartsui_connect_model_get_filename_valid(model));
                break;

            case GPARTSUI_CONNECT_MODEL_HOSTNAME:
                g_value_take_string(value, gpartsui_connect_model_get_hostname(model));
                break;

            case GPARTSUI_CONNECT_MODEL_HOSTNAME_SENSITIVE:
                g_value_set_boolean(value, gpartsui_connect_model_get_hostname_sensitive(model));
                break;

            case GPARTSUI_CONNECT_MODEL_HOSTNAME_VALID:
                g_value_set_boolean(value, gpartsui_connect_model_get_hostname_valid(model));
                break;

            case GPARTSUI_CONNECT_MODEL_PASSWORD:
                g_value_take_string(value, gpartsui_connect_model_get_password(model));
                break;

            case GPARTSUI_CONNECT_MODEL_PASSWORD_SENSITIVE:
                g_value_set_boolean(value, gpartsui_connect_model_get_password_sensitive(model));
                break;

            case GPARTSUI_CONNECT_MODEL_PASSWORD_VALID:
                g_value_set_boolean(value, gpartsui_connect_model_get_password_valid(model));
                break;

            case GPARTSUI_CONNECT_MODEL_USERNAME:
                g_value_take_string(value, gpartsui_connect_model_get_username(model));
                break;

            case GPARTSUI_CONNECT_MODEL_USERNAME_SENSITIVE:
                g_value_set_boolean(value, gpartsui_connect_model_get_username_sensitive(model));
                break;

            case GPARTSUI_CONNECT_MODEL_USERNAME_VALID:
                g_value_set_boolean(value, gpartsui_connect_model_get_username_valid(model));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
gpartsui_connect_model_get_type( void )
{
    static GType type = 0;

    if ( type == 0 )
    {
        static const GTypeInfo tinfo = {
            sizeof(GPartsUIConnectModelClass),
            NULL,
            NULL,
            gpartsui_connect_model_class_init,
            NULL,
            NULL,
            sizeof(GPartsUIConnectModel),
            0,
            gpartsui_connect_model_instance_init,
            NULL
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "gpartsui-connect-model",
            &tinfo,
            0
            );
    }

    return type;
}

gchar*
gpartsui_connect_model_get_username(const GPartsUIConnectModel *model)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);
    gchar *username = NULL;

    if (privat != NULL)
    {
        username = g_strdup(privat->username);
    }

    return username;
}

gboolean
gpartsui_connect_model_get_username_sensitive(const GPartsUIConnectModel *model)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);
    gboolean sensitive = FALSE;

    if (privat != NULL)
    {
        gint flags = gparts_database_factory_get_flags(privat->database_factory);

        sensitive = (flags & GPARTS_DATABASE_TYPE_FLAGS_USES_USERNAME);
    }

    return sensitive;
}

gboolean
gpartsui_connect_model_get_username_valid(const GPartsUIConnectModel *model)
{
    return TRUE;
}

static void
gpartsui_connect_model_instance_init(GTypeInstance* instance, gpointer g_class)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(instance);
}

GPartsUIConnectModel*
gpartsui_connect_model_new(void)
{
    return GPARTSUI_CONNECT_MODEL(g_object_new(GPARTSUI_TYPE_CONNECT_MODEL, NULL));
}

void
gpartsui_connect_model_set_database_name(GPartsUIConnectModel *model, const gchar *database)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);

    if (privat != NULL)
    {
        g_free(privat->database);
        privat->database = g_strdup(database);

        g_object_notify(G_OBJECT(model), "database-name");
        g_object_notify(G_OBJECT(model), "database-name-valid");
    }
}


void
gpartsui_connect_model_set_database_factory(GPartsUIConnectModel *model, GPartsDatabaseFactory *factory)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);

    if (privat != NULL)
    {
        if (privat->database_factory != NULL)
        {
            g_object_unref(privat->database_factory);
        }

        privat->database_factory = factory;

        if (privat->database_factory != NULL)
        {
            g_object_ref(privat->database_factory);
        }

        g_object_notify(G_OBJECT(model), "database-factory");
        g_object_notify(G_OBJECT(model), "database-name-sensitive");
        g_object_notify(G_OBJECT(model), "database-name-valid");
        g_object_notify(G_OBJECT(model), "database-type");
        g_object_notify(G_OBJECT(model), "filename-sensitive");
        g_object_notify(G_OBJECT(model), "filename-valid");
        g_object_notify(G_OBJECT(model), "hostname-sensitive");
        g_object_notify(G_OBJECT(model), "hostname-valid");
        g_object_notify(G_OBJECT(model), "password-sensitive");
        g_object_notify(G_OBJECT(model), "password-valid");
        g_object_notify(G_OBJECT(model), "username-sensitive");
        g_object_notify(G_OBJECT(model), "username-valid");
    }
}

void
gpartsui_connect_model_set_database_model(GPartsUIConnectModel *cmodel, GPartsUIDatabaseModel *dmodel)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(cmodel);

    if (privat != NULL)
    {
        if (privat->database_model != NULL)
        {
            g_object_unref(privat->database_model);
        }

        privat->database_model = dmodel;

        if (privat->database_model != NULL)
        {
            g_object_ref(privat->database_model);
        }

        g_object_notify(G_OBJECT(cmodel), "database-model");
        g_object_notify(G_OBJECT(cmodel), "database-type");
        g_object_notify(G_OBJECT(cmodel), "filename-valid");
        g_object_notify(G_OBJECT(cmodel), "hostname-valid");
        g_object_notify(G_OBJECT(cmodel), "password-valid");
        g_object_notify(G_OBJECT(cmodel), "username-valid");
    }
}

void
gpartsui_connect_model_set_database_type(GPartsUIConnectModel *model, const gchar *name)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);

    if (privat != NULL)
    {
        GPartsConfig *config = gparts_config_new();
        GPartsDatabaseType *type = gparts_config_get_database_types(config);

        GPartsDatabaseFactory *factory = gparts_database_type_get_factory(type, name);

        gpartsui_connect_model_set_database_factory(model, factory);

        g_object_unref(factory);
        g_object_unref(type);
    }
}

void
gpartsui_connect_model_set_filename(GPartsUIConnectModel *model, const gchar *filename)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);

    if (privat != NULL)
    {
        g_free(privat->filename);
        privat->filename = g_strdup(filename);

        g_object_notify(G_OBJECT(model), "filename");
        g_object_notify(G_OBJECT(model), "filename-valid");
    }
}

void
gpartsui_connect_model_set_hostname(GPartsUIConnectModel *model, const gchar *hostname)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);

    if (privat != NULL)
    {
        g_free(privat->hostname);
        privat->hostname = g_strdup(hostname);

        g_object_notify(G_OBJECT(model), "hostname");
        g_object_notify(G_OBJECT(model), "hostname-valid");
    }
}

void
gpartsui_connect_model_set_password(GPartsUIConnectModel *model, const gchar *password)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);

    if (privat != NULL)
    {
        g_free(privat->password);
        privat->password = g_strdup(password);

        g_object_notify(G_OBJECT(model), "password");
        g_object_notify(G_OBJECT(model), "password-valid");
    }
}

static void
gpartsui_connect_model_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GPartsUIConnectModel *model = GPARTSUI_CONNECT_MODEL(object);

    if (model != NULL)
    {
        switch ( property_id )
        {
            case GPARTSUI_CONNECT_MODEL_DATABASE_FACTORY:
                gpartsui_connect_model_set_database_factory(model, g_value_get_object(value));
                break;

            case GPARTSUI_CONNECT_MODEL_DATABASE_MODEL:
                gpartsui_connect_model_set_database_model(model, g_value_get_object(value));
                break;

            case GPARTSUI_CONNECT_MODEL_DATABASE_NAME:
                gpartsui_connect_model_set_database_name(model, g_value_get_string(value));
                break;

            case GPARTSUI_CONNECT_MODEL_DATABASE_TYPE:
                gpartsui_connect_model_set_database_type(model, g_value_get_string(value));
                break;

            case GPARTSUI_CONNECT_MODEL_FILENAME:
                gpartsui_connect_model_set_filename(model, g_value_get_string(value));
                break;

            case GPARTSUI_CONNECT_MODEL_HOSTNAME:
                gpartsui_connect_model_set_hostname(model, g_value_get_string(value));
                break;

            case GPARTSUI_CONNECT_MODEL_PASSWORD:
                gpartsui_connect_model_set_password(model, g_value_get_string(value));
                break;

            case GPARTSUI_CONNECT_MODEL_USERNAME:
                gpartsui_connect_model_set_username(model, g_value_get_string(value));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

void
gpartsui_connect_model_set_username(GPartsUIConnectModel *model, const gchar *username)
{
    GPartsUIConnectModelPrivate *privat = GPARTSUI_CONNECT_MODEL_GET_PRIVATE(model);

    if (privat != NULL)
    {
        g_free(privat->username);
        privat->username = g_strdup(username);

        g_object_notify(G_OBJECT(model), "username");
        g_object_notify(G_OBJECT(model), "username-valid");
    }
}

