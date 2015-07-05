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

/*! \file misc-macro.c
 */

#include <glib.h>

typedef struct _MiscMacroExpandData MiscMacroExpandData;

struct _MiscMacroExpandData
{
    GHashTable *table;
};

static gboolean
misc_macro_expand_cb(const GMatchInfo *info, GString *result, gpointer data);


char*
misc_macro_expand(const GRegex* regex, const char *string, const GHashTable *table)
{
  //MiscMacroExpandData data;

  //data.table = table;

  return g_regex_replace_eval(regex,                   /* regex */
                              string,                  /* string */
                              -1,                      /* string_len */
                              0,                       /* start_position */
                              0,                       /* match_options */
                              misc_macro_expand_cb,    /* eval */
                              &table,                  /* user_data */
                              NULL                     /* error */
  );
}

static gboolean
misc_macro_expand_cb(const GMatchInfo *info, GString *result, void *user_data)
{
    char *value = NULL;

    if (user_data != NULL) {

        MiscMacroExpandData *data = (MiscMacroExpandData*) user_data;

        if (data->table != NULL) {

            char *name = g_match_info_fetch(info, 1);

            if (name != NULL) {

                value = g_hash_table_lookup(data->table, name);

                g_free(name);
            }
        }
    }

    if (value != NULL) {

        g_string_append(result, value);
    }
    else {

        char *macro = g_match_info_fetch(info, 0);

        g_string_append(result, macro);

        g_free(macro);
    }

    return FALSE;
}

void
misc_macro_find(const GRegex *regex, const char *string, GHashTable *table)
{
    GMatchInfo *info;

    g_regex_match(regex, string, 0, &info);

    while (g_match_info_matches(info))
    {
        char *name = g_match_info_fetch(info, 1);

        if (!g_hash_table_lookup_extended(table, name, NULL, NULL))
        {
            g_hash_table_insert(table, name, NULL);
        }

        g_free(name);

        g_match_info_next(info, NULL);
    }

    g_match_info_free(info);
}

GRegex*
misc_macro_new_regex(void)
{
    static GRegex *regex = NULL;

    if (regex == NULL)
    {
        regex = g_regex_new(
            "\\$\\((\\w+)\\)",    /* pattern */
            0,                    /* compile_options */
            0,                    /* match_options */
            NULL                  /* error */
            );
    }

    if (regex != NULL)
    {
        g_regex_ref(regex);
    }

    return regex;
}

GHashTable*
misc_macro_new_table(void)
{
    return g_hash_table_new_full(
        g_str_hash,     /* hash_func */
        g_str_equal,    /* key_equal_func */
        g_free,         /* key_destroy_func */
        g_free          /* value_destroy_func */
        );
}

