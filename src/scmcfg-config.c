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

/*! \file scmcfg-config.c
 */

#include <glib.h>
#include <glib-object.h>
#include <libguile.h>
#include <gdk/gdk.h>

#include "schgui.h"

#include "scmcfg-dirs.h"

#include "gparts.h"

#define ENABLED "enabled"

static void*
scmcfg_config_init_inner(void* data);

static void*
scmcfg_config_load_inner(void* data);

static SCM
scmcfg_config_load_inner_body(void * data);

static SCM
scmcfg_config_load_inner_handler(void *data, SCM key, SCM args);


SCM_DEFINE(always_promote_attributes, "always-promote-attributes", 1, 0, 0, (SCM a), "")
{
    SchConfig *config = sch_config_new();

    if (config != NULL) {

        int    index;
        GArray *list = g_array_new(TRUE, FALSE, sizeof(char*));
        SCM    temp = a;

        while (temp != SCM_EOL) {

            char *name = scm_to_locale_string(scm_car(temp));

            g_array_append_val(list, name);

            temp = scm_cdr(temp);
        }

        sch_config_set_promote_attributes(config, (char**) list->data);

        for (index=0; index<list->len; index++)
        {
            free(g_array_index(list, char*, index));
        }

        g_array_free(list, TRUE);
    }

    return SCM_BOOL_F;
}

SCM_DEFINE(attribute_promotion, "attribute-promotion", 1, 0, 0, (SCM a), "")
{
    SchConfig *config = sch_config_new();

    if (config != NULL)
    {
        char *value = scm_to_locale_string(a);

        if (strcmp(value, ENABLED) == 0)
        {
            sch_config_set_attribute_promotion(config, TRUE);
        }
        else
        {
            sch_config_set_attribute_promotion(config, FALSE);
        }

        free(value);
    }

    return SCM_BOOL_F;
}

SCM_DEFINE(bitmap_directory, "bitmap-directory", 1, 0, 0, (SCM a), "")
{
    char *value = scm_to_locale_string(a);

    g_debug("bitmap-directory %s", value);

    free(value);

    return SCM_BOOL_F;
}

SCM_DEFINE(bus_style, "bus-style", 1, 0, 0, (SCM a), "")
{
    return SCM_BOOL_F;
}

SCM_DEFINE(component_library, "component-library", 1, 1, 0, (SCM a, SCM b), "")
{
    SchLoader *loader = sch_loader_get_default();

    if (loader != NULL)
    {
        char *library = scm_to_locale_string(a);

        sch_loader_add_component_library(loader, library);

        free(library);
    }

    return SCM_BOOL_F;
}

SCM_DEFINE(database_interface, "database-interface", 1, 0, 0, (SCM a), "")
{
    GPartsConfig *config = gparts_config_new();

    if (config != NULL)
    {
        GPartsDatabaseType *type = gparts_config_get_database_types(config);

        if (type != NULL)
        {
            char *name = scm_to_locale_string(a);

            gparts_database_type_load_module(type, name, NULL);

            free(name);
            g_object_unref(type);
        }
    }

    return SCM_BOOL_F;
}

SCM_DEFINE(display_color_map, "display-color-map", 0, 1, 0, (SCM a), "")
{
    SCM b = SCM_UNDEFINED;
    SchGUIDrawingCfg *config = schgui_drawing_cfg_get_default_display();

    if (config != NULL)
    {
        if (a == SCM_UNDEFINED)
        {
            /* TODO */

            b = SCM_BOOL_F;
        }
        else
        {
            SCM temp = a;

            while (temp != SCM_EOL)
            {
                SCM color = scm_cadar(temp);
                int index = scm_to_int(scm_caar(temp));

                if (color == SCM_BOOL_F)
                {
                    g_debug("display %2i none", index);
                    schgui_drawing_cfg_disable_color(config, index);
                }
                else
                {
                    char *name = scm_to_locale_string(color);

                    g_debug("display %2i %s", index, name);
                    schgui_drawing_cfg_set_color_by_name(config, index, name);
                    free(name);
                }

                temp = scm_cdr(temp);
            }

        b = SCM_BOOL_F;
        }

    }

    return b;
}

SCM_DEFINE(display_outline_color_map, "display-outline-color-map", 0, 1, 0, (SCM a), "")
{
    return SCM_BOOL_F;
}

SCM_DEFINE(eval_protected, "eval-protected", 1, 1, 0, (SCM a, SCM b), "")
{
    SCM c;

    if (b == SCM_UNDEFINED)
    {
        c = scm_primitive_eval(a);
    }
    else
    {
         c = scm_eval(a, b);
    }

    return c;
}

SCM_DEFINE(keep_invisible, "keep-invisible", 1, 0, 0, (SCM a), "")
{
    SchConfig *config = sch_config_new();

    if (config != NULL)
    {
        char *value = scm_to_locale_string(a);

        if (strcmp(value, ENABLED) == 0)
        {
            sch_config_set_keep_invisible(config, TRUE);
        }
        else
        {
            sch_config_set_keep_invisible(config, FALSE);
        }

        free(value);
    }

    return SCM_BOOL_F;
}

SCM_DEFINE(net_style, "net-style", 1, 0, 0, (SCM a), "")
{
    return SCM_BOOL_F;
}

SCM_DEFINE(pin_style, "pin-style", 1, 0, 0, (SCM a), "")
{
    return SCM_BOOL_F;
}

SCM_DEFINE(postscript_prolog, "postscript-prolog", 1, 0, 0, (SCM a), "")
{
    return SCM_BOOL_F;
}

SCM_DEFINE(print_color_map, "print-color-map", 0, 1, 0, (SCM a), "")
{
    SCM b = SCM_UNDEFINED;
    SchGUIDrawingCfg *config = schgui_drawing_cfg_get_default_print();

    if (config != NULL)
    {
        if (a == SCM_UNDEFINED)
        {
            /*! \todo */

            b = SCM_BOOL_F;
        }
        else
        {
            SCM temp = a;

            while (temp != SCM_EOL)
            {
                SCM color = scm_cadar(temp);
                int index = scm_to_int(scm_caar(temp));

                if (color == SCM_BOOL_F)
                {
                    g_debug("print %2i none", index);
                    schgui_drawing_cfg_disable_color(config, index);
                }
                else
                {
                    char *name = scm_to_locale_string(color);

                    g_debug("print %2i %s", index, name);
                    schgui_drawing_cfg_set_color_by_name(config, index, name);
                    free(name);
                }

                temp = scm_cdr(temp);
            }

        b = SCM_BOOL_F;
        }

    }

    return b;
}

SCM_DEFINE(promote_invisible, "promote-invisible", 1, 0, 0, (SCM a), "")
{
    SchConfig *config = sch_config_new();

    if (config != NULL)
    {
        char *value = scm_to_locale_string(a);

        if (strcmp(value, ENABLED) == 0)
        {
            sch_config_set_promote_invisible(config, TRUE);
        }
        else
        {
            sch_config_set_promote_invisible(config, FALSE);
        }

        free(value);
    }

    return SCM_BOOL_F;
}

SCM_DEFINE(scheme_directory, "scheme-directory", 1, 0, 0, (SCM a), "")
{
    char *value = scm_to_locale_string(a);

    g_debug("scheme-directory %s", value);

    free(value);

    return SCM_BOOL_F;
}

void
scmcfg_config_init(void)
{
    scm_with_guile(scmcfg_config_init_inner, NULL);
}

static void*
scmcfg_config_init_inner(void* data)
{
    char *configdir;
    char *datadir;
    char *filename;

    SCM scm1;

    scm_dynwind_begin(0);

    configdir = scmcfg_dirs_find_geda_config();
    g_debug("gEDA configdir found %s", configdir);
    scm_dynwind_unwind_handler(g_free, configdir, SCM_F_WIND_EXPLICITLY);

    datadir = scmcfg_dirs_find_geda_data();
    g_debug("gEDA datadir found %s", datadir);
    scm_dynwind_unwind_handler(g_free, datadir, SCM_F_WIND_EXPLICITLY);

    scm_c_define("geda-data-path", scm_from_locale_string(datadir));
    scm_c_define("geda-rc-path", scm_from_locale_string(configdir));
    scm_c_define("path-sep", scm_from_locale_string(G_DIR_SEPARATOR_S));

    g_debug("Here");

    scm1 = scm_c_lookup("%load-path");

    g_debug("Here");

    filename = g_build_filename(datadir, "scheme", NULL);

    scm_variable_set_x (scm1,
                        scm_cons (scm_from_locale_string(filename),
                                  scm_variable_ref(scm1)));

    g_debug("Here");

#include "scmcfg-config.x"

    scm_dynwind_end();

    return NULL;
}

void
scmcfg_config_load(void)
{
    scm_with_guile(scmcfg_config_load_inner, NULL);
}

/*! \todo Refactor this function */
/*! \todo Make exception handler only catch file not found */

static void*
scmcfg_config_load_inner(void* data)
{
    char *datapath = scmcfg_dirs_find_geda_config();
    SCM   loaded   = SCM_BOOL_F;

    if (datapath != NULL) {

        char *pathname = g_build_filename(datapath, "system-gafrc", NULL);

        g_debug("Attempting to load %s\n", pathname);

        loaded = scm_internal_catch(
            SCM_BOOL_T,
            scmcfg_config_load_inner_body,
            pathname,
            scmcfg_config_load_inner_handler,
            NULL
            );

        g_free(datapath);
        g_free(pathname);
    }

    if (loaded == SCM_BOOL_F) {

        g_warning("Unable to locate a system-gafrc");

        /* should an error be thrown here? */
    }

    loaded = SCM_BOOL_F;

    datapath = scmcfg_dirs_find_gparts_data();

    if (datapath != NULL) {

        char *pathname = g_build_filename(datapath, "system-gpartsrc", NULL);

        g_debug("Attempting to load %s\n", pathname);

        g_free(datapath);

        loaded = scm_internal_catch(
            SCM_BOOL_T,
            scmcfg_config_load_inner_body,
            pathname,
            scmcfg_config_load_inner_handler,
            NULL
            );

        g_free(pathname);
    }

    if (loaded == SCM_BOOL_F) {

        g_warning("Unable to locate a system-gpartsrc");

        /* should an error be thrown here? */
    }

    return NULL;
}

static SCM
scmcfg_config_load_inner_body(void *data)
{
    g_debug("Before Load %s", (char*) data);

    scm_c_primitive_load((char*) data);

    g_debug("Loaded %s", (char*) data);

    return SCM_BOOL_T;
}

static SCM
scmcfg_config_load_inner_handler(void *data, SCM key, SCM args)
{
    scm_handle_by_message(NULL, key, args);

    return SCM_BOOL_F;
}

