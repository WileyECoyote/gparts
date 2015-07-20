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

/*! \file sch-loader.c
 *
 *  \todo Clean up this mess!
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

#include "sch.h"
#include "misc-object.h"

#define SCH_LOADER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCH_TYPE_LOADER,SchLoaderPrivate))

enum
{
    SCH_LOADER_COMPONENT_LIBRARIES = 1
};

typedef struct _SchLoaderPrivate SchLoaderPrivate;

struct _SchLoaderPrivate
{
    GArray *component_libraries;
};


static void
sch_loader_class_init(gpointer g_class, gpointer g_class_data);

static void
sch_loader_dispose(GObject *object);

static void
sch_loader_finalize(GObject *object);

static void
sch_loader_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
sch_loader_init(GTypeInstance *instance, gpointer g_class);

static void
sch_loader_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static FILE*
open_drawing(SchLoader *loader, const char *filename);

static GObject*
process_arc(FILE *file, gchar **tokens);

static void
process_attributes(FILE *file, gchar **tokens, GObject *object);

static GObject*
process_box(FILE *file, gchar **tokens);

static GObject*
process_bus(FILE *file, gchar **tokens);

static GObject*
process_circle(FILE *file, gchar **tokens);

static GObject*
process_component(FILE *file, gchar **tokens);

static void
process_embedded(FILE *file, SchDrawing *drawing);

static GObject*
process_line(FILE *file, gchar **tokens);

static GObject*
process_net(FILE *file, gchar **tokens);

static GObject*
process_object(FILE *file, gchar **tokens);

static GObject*
process_path(FILE *file, gchar **tokens);

static GObject*
process_picture(FILE *file, gchar **tokens);

static GObject*
process_pin(FILE *file, gchar **tokens);

static GObject*
process_text(FILE *file, gchar **tokens);

static void
process_version(FILE *file, gchar **tokens);

static void
read_file(SchDrawing *drawing, FILE *file, GError **error);

static gchar*
read_line(FILE *file);

static gchar*
read_lines(FILE *file, int count);

static void
process_path_commands(SchPath *path, const char* string);


void
sch_loader_add_component_library(SchLoader *loader, gchar *library)
{
    SchLoaderPrivate *privat = SCH_LOADER_GET_PRIVATE(loader);

    if (privat != NULL)
    {
        char *name = g_strdup(library);

        g_debug("New library %s", name);

        g_array_append_val(privat->component_libraries, name);
    }
}

static void
sch_loader_class_init(gpointer g_class, gpointer g_class_data)
{
    GObjectClass *klasse = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(klasse, sizeof(SchLoaderPrivate));

    klasse->dispose  = sch_loader_dispose;
    klasse->finalize = sch_loader_finalize;

    klasse->get_property = sch_loader_get_property;
    klasse->set_property = sch_loader_set_property;

    g_object_class_install_property(
        klasse,
        SCH_LOADER_COMPONENT_LIBRARIES,
        g_param_spec_boxed(
            "component-libraries",
            "component-libraries",
            "component-libraries",
            G_TYPE_STRV,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );
}

static void
sch_loader_dispose(GObject *object)
{
    misc_object_chain_dispose(object);
}

static void
sch_loader_finalize(GObject *object)
{
    SchLoaderPrivate *privat = SCH_LOADER_GET_PRIVATE(object);

    if (privat != NULL)
    {
    }

    misc_object_chain_finalize(object);
}

SchLoader*
sch_loader_get_default(void)
{
    static SchLoader *factory = NULL;

    if (factory == NULL)
    {
        factory = SCH_LOADER(g_object_new(SCH_TYPE_LOADER, NULL));
    }

    return factory;
}

gchar**
sch_loader_get_component_libraries(GObject *object)
{
    char **libraries = NULL;
    SchLoaderPrivate *privat = SCH_LOADER_GET_PRIVATE(object);

    if (privat != NULL)
    {
        libraries = g_strdupv(privat->component_libraries->data);
    }

    return libraries;
}

static void
sch_loader_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    SchLoaderPrivate *privat = SCH_LOADER_GET_PRIVATE(object);

    if (privat != NULL)
    {
        switch (property_id)
        {
            case SCH_LOADER_COMPONENT_LIBRARIES:
                g_value_take_boxed(value, sch_loader_get_component_libraries(object));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
sch_loader_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(SchLoaderClass),    /* class_size */
            NULL,                      /* base_init */
            NULL,                      /* base_finalize */
            sch_loader_class_init,     /* class_init */
            NULL,                      /* class_finalize */
            NULL,                      /* class_data */
            sizeof(SchLoader),         /* instance_size */
            0,                         /* n_preallocs */
            sch_loader_init,           /* instance_init */
            NULL                       /* value_table */
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "SchLoader",
            &tinfo,
            0
            );
    }

    return type;
}


static void
sch_loader_init(GTypeInstance *instance, gpointer g_class)
{
    SchLoaderPrivate *privat = SCH_LOADER_GET_PRIVATE(instance);

    if (privat != NULL)
    {
        privat->component_libraries = g_array_new(TRUE, FALSE, sizeof(gchar*));
    }
}

SchDrawing*
sch_loader_load_drawing(SchLoader *loader, const gchar *filename, GError **error)
{
    SchDrawing *drawing = NULL;
    FILE       *file;

    file = fopen(filename, "r");

    if (file != NULL)
    {
        drawing = g_object_new(SCH_TYPE_DRAWING, NULL);

        sch_drawing_set_filename(drawing, filename);

        read_file(drawing, file, error);

        fclose(file);
    }
    else
    {
        //g_set_error(
        //    error,
        //    GPARTS_MYSQL_DATABASE_ERROR,
        //    mysql_errno( private->mysql ),
        //    "%s",
        //    mysql_error( private->mysql )
        //    );

        g_debug("Could not open drawing '%s'", filename);
    }

    return drawing;
}

SchDrawing*
sch_loader_load_symbol(SchLoader *loader, const gchar *filename, GError **error)
{
    SchDrawing *drawing = NULL;
    FILE       *file;

    file = sch_loader_open_symbol_file(loader, filename, "r");

    if (file != NULL)
    {
        drawing = g_object_new(SCH_TYPE_DRAWING, NULL);

        sch_drawing_set_filename(drawing, filename);

        read_file(drawing, file, error);

        fclose(file);
    }
    else
    {
        //g_set_error(
        //    error,
        //    GPARTS_MYSQL_DATABASE_ERROR,
        //    mysql_errno( private->mysql ),
        //    "%s",
        //    mysql_error( private->mysql )
        //    );

        g_debug("Could not open symbol '%s'", filename);
    }

    return drawing;
}

gchar*
sch_loader_find_symbol_file(SchLoader *loader, const char *filename)
{
    gchar *path = NULL;
    SchLoaderPrivate *privat = SCH_LOADER_GET_PRIVATE(loader);

    if (privat != NULL)
    {
        const char **dir = privat->component_libraries->data;

        while (*dir != NULL)
        {
            path = g_build_filename(*dir, filename, NULL);

            if (g_file_test(path, G_FILE_TEST_IS_REGULAR))
            {
                break;
            }

            g_free(path);
            path = NULL;

            dir++;
        }
    }

    return path;
}


FILE*
sch_loader_open_symbol_file(SchLoader *loader, const char *filename, const char *mode)
{
    FILE *file = NULL;
    SchLoaderPrivate *privat = SCH_LOADER_GET_PRIVATE(loader);

    if (privat != NULL)
    {
        const char **dir = privat->component_libraries->data;

        while (*dir != NULL)
        {
            gchar *path = g_build_filename(*dir, filename, NULL);

            file = fopen(path, mode);
            g_free(path);

            if ((file != NULL) || (errno != ENOENT))
            {
                break;
            }

            dir++;
        }
    }

    return file;
}

void
sch_loader_set_component_libraries(GObject *object, char **libraries)
{
    SchLoaderPrivate *privat = SCH_LOADER_GET_PRIVATE(object);

    if (privat != NULL)
    {
        char **temp = libraries;

        g_strfreev(g_array_free(privat->component_libraries, FALSE));

        privat->component_libraries = g_array_new(TRUE, FALSE, sizeof(gchar*));

        while (*temp != NULL)
        {
            g_array_append_val(privat->component_libraries, *temp);

            temp++;
        }

    }
}

static void
sch_loader_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    SchLoaderPrivate *privat = SCH_LOADER_GET_PRIVATE(object);

    if (privat != NULL)
    {
        switch (property_id)
        {
            case SCH_LOADER_COMPONENT_LIBRARIES:
                sch_loader_set_component_libraries(object, g_value_get_boxed(value));
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

struct PARAM
{
    char *name;
    int  dummy;
};

static const struct PARAM arc_params[] =
{
    { NULL,               1 },
    { "center-x",         1 },
    { "center-y",         1 },
    { "radius",           1 },
    { "start",            1 },
    { "sweep",            1 },
    { "color",            1 },
    { "line-width",       3 },
    { "line-cap-style",   3 },
    { "line-dash-style",  3 },
    { "line-dash-length", 3 },
    { "line-dash-space",  3 },
    { NULL,               0 }
};

static const struct PARAM box_params[] =
{
    { NULL,               1 },
    { "corner-x",         1 },
    { "corner-y",         1 },
    { "width",            1 },
    { "height",           1 },
    { "color",            1 },
    { "line-width",       3 },
    { "line-cap-style",   3 },
    { "line-dash-style",  3 },
    { "line-dash-length", 3 },
    { "line-dash-space",  3 },
    { "fill-type",        3 },
    { "fill-width",       3 },
    { "fill-angle1",      3 },
    { "fill-pitch1",      3 },
    { "fill-angle2",      3 },
    { "fill-pitch2",      3 },
    { NULL,               0 }
};

static const struct PARAM bus_params[] =
{
    { NULL,               1 },
    { "x1",               1 },
    { "y1",               1 },
    { "x2",               1 },
    { "y2",               1 },
    { "color",            1 },
    { "ripper-dir",       4 },
    { NULL,               0 }
};

static const struct PARAM component_params[] =
{
    { NULL,               1 },
    { "x",                1 },
    { "y",                1 },
    { "selectable",       1 },
    { "angle",            1 },
    { "mirror",           1 },
    { "basename",         4 },
    { NULL,               0 }
};

static const struct PARAM circle_params[] =
{
    { NULL,               1 },
    { "center-x",         1 },
    { "center-y",         1 },
    { "radius",           1 },
    { "color",            1 },
    { "line-width",       3 },
    { "line-cap-style",   3 },
    { "line-dash-style",  3 },
    { "line-dash-length", 3 },
    { "line-dash-space",  3 },
    { "fill-type",        3 },
    { "fill-width",       3 },
    { "fill-angle1",      3 },
    { "fill-pitch1",      3 },
    { "fill-angle2",      3 },
    { "fill-pitch2",      3 },
    { NULL,               0 }
};


static const struct PARAM line_params[] =
{
    { NULL,               1 },
    { "x1",               1 },
    { "y1",               1 },
    { "x2",               1 },
    { "y2",               1 },
    { "color",            1 },
    { "line-width",       3 },
    { "line-cap-style",   3 },
    { "line-dash-style",  3 },
    { "line-dash-length", 3 },
    { "line-dash-space",  3 },
    { NULL,               0 }
};

static const struct PARAM net_params[] =
{
    { NULL,               1 },
    { "x1",               1 },
    { "y1",               1 },
    { "x2",               1 },
    { "y2",               1 },
    { "color",            1 },
    { NULL,               0 }
};

static const struct PARAM path_params[] =
{
    { NULL,               1 },
    { "color",            1 },
    { "line-width",       3 },
    { "line-cap-style",   3 },
    { "line-dash-style",  3 },
    { "line-dash-length", 3 },
    { "line-dash-space",  3 },
    { "fill-type",        3 },
    { "fill-width",       3 },
    { "fill-angle1",      3 },
    { "fill-pitch1",      3 },
    { "fill-angle2",      3 },
    { "fill-pitch2",      3 },
    { NULL,               0 }
};



static const struct PARAM pin_params[] =
{
    { NULL,               1 },
    { "x1",               1 },
    { "y1",               1 },
    { "x2",               1 },
    { "y2",               1 },
    { "color",            1 },
    { "pin-type",         4 },
    { "pin-end",          4 },
    { NULL,               0 }
};


static const struct PARAM text_params[] =
{
    { NULL,               1 },
    { "x",                1 },
    { "y",                1 },
    { "color",            1 },
    { "size",             1 },
    { "visibility",       1 },
    { "show-name-value",  1 },
    { "angle",            1 },
    { "alignment",        2 },
    { NULL,               0 }
};






void
process_params(GObject *object, gchar **token, const struct PARAM paramv[], gint paramc);


static GObject*
process_arc(FILE *file, gchar **tokens)
{
    SchArc *arc;

    //g_debug("Process arc");

    arc = sch_arc_new(NULL);

    process_params(G_OBJECT(arc), tokens, arc_params, 13);

    return G_OBJECT(arc);
}

static void
process_attributes(FILE *file, gchar **tokens, GObject *object)
{
    SchAttributes *attrs = sch_shape_get_attributes(object);

    gchar *line = read_line(file);

    while (line != NULL)
    {
        GObject *attribute;
        gchar **token = g_strsplit_set(line, " \t", 0);

        g_free(line);
        line = NULL;

        if ((token != NULL) && (*token != NULL))
        {
            switch (**token)
            {
                case '{':
                    /* error */
                    break;

                case '}':
                    break;

                default:
                    attribute = process_object(file, token);
                    if (attribute != NULL)
                    {
                        if (attrs != NULL)
                        {
                            sch_attributes_append(attrs, attribute);
                        }
                        g_object_unref(attribute);
                    }
                    line = read_line(file);
            }
        }

        g_strfreev(token);
    }

    if (attrs != NULL)
    {
        g_object_unref(attrs);
    }
}

static  GObject*
process_box(FILE *file, gchar **tokens)
{
    SchBox *box;

    //g_debug("Process box");

    box = sch_box_new(NULL);

    process_params(G_OBJECT(box), tokens, box_params, 19);

    return G_OBJECT(box);
}

static GObject*
process_bus(FILE *file, gchar **tokens)
{
    SchBus *bus;

    //g_debug("Process bus");

    bus = sch_bus_new(NULL);

    process_params(G_OBJECT(bus), tokens, bus_params, 8);

    return G_OBJECT(bus);

}

static GObject*
process_circle(FILE *file, gchar **tokens)
{
    SchCircle *circle;

    //g_debug("Process circle");

    circle = sch_circle_new(NULL);

    process_params(G_OBJECT(circle), tokens, circle_params, 17);

    return G_OBJECT(circle);
}

static GObject*
process_component(FILE *file, gchar **tokens)
{
    SchComponent *component = sch_component_new(NULL);

    process_params(G_OBJECT(component), tokens, component_params, 8);

    if (tokens != NULL)
    {
        long lines = 1;
        char *tail;

        if (g_strv_length(tokens) > 6)
        {
            //g_debug( *(tokens + 6) );

            if (strncmp(*(tokens+6), "EMBEDDED", 8) == 0)
            {
                SchDrawing *symbol;
                int x;
                int y;
                int angle;
                int mirror;
                GeomTransform transform;

                symbol = g_object_new(SCH_TYPE_DRAWING, NULL);
                process_embedded(file, symbol);
                //sch_component_get_insertion_point(component, &x, &y);
                //sch_drawing_translate(symbol, -x, -y);
                //sch_component_get_orientation(component, &angle, &mirror);
                //sch_drawing_rotate(symbol, -angle);

                sch_component_get_insertion_point(component, &x, &y);
                sch_component_get_orientation(component, &angle, &mirror);

                geom_transform_init(&transform);
                geom_transform_translate(&transform, -x, -y);
                geom_transform_rotate(&transform, -angle);

                sch_drawing_transform(symbol, &transform);

                sch_component_set_drawing(component, symbol);
                g_object_unref(symbol);
            }
            else
            {
                SchLoader  *loader = sch_loader_get_default();
                SchDrawing *symbol = NULL;

                if (loader != NULL)
                {
                    symbol = sch_loader_load_symbol(loader, *(tokens+6), NULL);;
                }

                sch_component_set_drawing(component, symbol);
                g_object_unref(symbol);

            }
        }
    }

    return component;
}

static void
process_embedded(FILE *file, SchDrawing *drawing)
{
    gchar *line = read_line(file);

    while (line != NULL)
    {
        gchar **token = g_strsplit_set(line, " \t", 0);
        g_free(line);

        if (*token != NULL)
        {
            if (**token != ']')
            {
                GObject *object;

                switch (**token)
                {
                    case '{':
                        process_attributes(file, token, NULL);
                        break;

                    default:
                        //g_debug("Process object (begin)");
                        object = process_object(file, token);
                        if (object != NULL)
                        {
                            sch_drawing_append_shape(drawing, object);
                            g_object_unref(object);
                        }
                        //g_debug("Process object (end)");
                }

                line = read_line(file);
            }
            else
            {
                line = NULL;
            }
        }

        g_strfreev(token);
    }
}


static GObject*
process_line(FILE *file, gchar **tokens)
{
    SchLine *line;

    //g_debug("Process line");

    line = sch_line_new(NULL);

    process_params(G_OBJECT(line), tokens, line_params, 12);

    return G_OBJECT(line);
}

static  GObject*
process_net(FILE *file, gchar **tokens)
{
     SchNet *net;

    //g_debug("Process net");

    net = sch_net_new(NULL);

    process_params(G_OBJECT(net), tokens, net_params, 7);

    return G_OBJECT(net);

}

static GObject*
process_object(FILE *file, gchar **tokens)
{
    GObject *object;

    if (*tokens != NULL)
    {
        switch (**tokens)
        {
            case 'A':
                object = process_arc(file, tokens);
                break;

            case 'B':
                object = process_box(file, tokens);
                break;

            case 'C':
                object = process_component(file, tokens);
                break;

            case 'G':
                object = process_picture(file, tokens);
                break;

            case 'H':
                object = process_path(file, tokens);
                break;

            case 'L':
                object = process_line(file, tokens);
                break;

            case 'N':
                object = process_net(file, tokens);
                break;

            case 'P':
                object = process_pin(file, tokens);
                break;

            case 'T':
                object = process_text(file, tokens);
                break;

            case 'U':
                object = process_bus(file, tokens);
                break;

            case 'V':
                object = process_circle(file, tokens);
                break;

            default:
                object = NULL;
                //g_debug("Unknown object type %c", **tokens);
                break;
        }
    }

    return object;
}

static  GObject*
process_path(FILE *file, gchar **tokens)
{
    SchPath *path = sch_path_new(NULL);

    process_params(G_OBJECT(path), tokens, path_params, 14);

    //g_debug("Process path");
    if ((tokens != NULL) && (g_strv_length(tokens) > 13))
    {
        long lines;
        char *tail;

        lines = strtol(*(tokens + 13), &tail, 10);

        if (*(tokens + 13) == tail)
        {
            /* invalid */
        }
        else if (lines < 1 || lines > 999)
        {
            /* out of range */
        }
        else
        {
            gchar *line = read_lines(file, lines);

            g_debug("Got some path stuff");
            g_debug("%d", lines);
            g_debug("%s", line);

            process_path_commands(path, line);

            g_free(line);
        }
    }


    return path;
}

static GObject*
process_picture(FILE *file, gchar **tokens)
{
    //g_debug("Process picture");

    return NULL;
}

static GObject*
process_pin(FILE *file, gchar **tokens)
{
    SchPin *pin;

    //g_debug("Process pin");

    pin = sch_pin_new(NULL);

    process_params(G_OBJECT(pin), tokens, pin_params, 9);

    return G_OBJECT(pin);
}

static GObject*
process_text(FILE *file, gchar **tokens)
{
    SchText *text;

    //g_debug("Process text");

    text = sch_text_new(NULL);

    process_params(G_OBJECT(text), tokens, text_params, 10);

    if (tokens != NULL)
    {
        long lines = 1;
        char *tail;

        if (g_strv_length(tokens) > 9)
        {
            lines = strtol(*(tokens + 9), &tail, 10);

            if (*(tokens + 9) == tail)
            {
                /* invalid */
            }
            else if (lines < 1 || lines > 999)
            {
                /* out of range */
            }
        }

        {
            long index;
            SchMultiline *multiline = g_object_new(SCH_TYPE_MULTILINE, NULL);

            for (index=0; index<lines; index++)
            {
                gchar *line = read_line(file);
                sch_multiline_append(multiline, line);
                g_free(line);
            }

            sch_text_set_multiline(text, multiline);
            g_object_unref(multiline);
        }
    }

    return G_OBJECT(text);
}

static void
process_version(FILE *file, gchar **tokens)
{
    //g_debug("Process version");
}



/*! \brief Reads a line from a file
 */
static gchar*
read_line(FILE *file)
{
    gchar *result = NULL;

    if (!feof(file))
    {
        gint c = fgetc(file);
        GString *line = g_string_sized_new(10);

        while (!ferror(file) && !feof(file) && (c != '\n'))
        {
            g_string_append_c(line, c);
            c = fgetc(file);
        }

        result = g_string_free(line, ferror(file));
    }

    return result;
}

/*! \brief Reads a line from a file
 */
static gchar*
read_lines(FILE *file, int count)
{
    gchar *result = NULL;

    if (!feof(file) && (count > 0))
    {
        gint c = fgetc(file);
        GString *line = g_string_sized_new(10);

        while (!ferror(file) && !feof(file))
        {
            if (c == '\n')
            {
                count--;

                if (count <= 0)
                {
                    break;
                }
            }

            g_string_append_c(line, c);
            c = fgetc(file);
        }

        result = g_string_free(line, ferror(file));
    }

    return result;
}



static void
read_file(SchDrawing *drawing, FILE *file, GError **error)
{
    gchar *line = read_line(file);
    GObject *object = NULL;

    while (line != NULL)
    {
        //g_debug("Hello inside read file");

        gchar **token = g_strsplit_set(line, " \t", 0);
        g_free(line);

        if (*token != NULL)
        {

            switch (**token)
            {
                case 'v':
                    process_version(file, token);
                    break;

                case '{':
                    process_attributes(file, token, object);
                    break;

                case '}':
                    /* error */
                    break;

                default:
                    //g_debug("Process object (begin)");
                    if (object != NULL)
                    {
                        g_object_unref(object);
                    }
                    object = process_object(file, token);
                    if (object != NULL)
                    {
                        sch_drawing_append_shape(drawing, object);
                    }
                    //g_debug("Process object (end)");
            }
        }

        g_strfreev(token);
        line = read_line(file);
    }

    if (object != NULL)
    {
        g_object_unref(object);
    }
}




void
set_value(GValue *value, const char *string)
{
    if (G_VALUE_TYPE(value) == G_TYPE_STRING)
    {
        g_value_set_string(value, string);
    }
    else if (G_VALUE_TYPE(value) == G_TYPE_INT)
    {
        long number;
        char *tail;

        number = strtol(string, &tail, 10);
        g_value_set_int(value, number);

        //if (*(tokens + 9) == tail)
       // {
       //     /* invalid */
        //}
        //else
       // {
       //     g_value_set_int(value, number);
       // }
    }
    else
    {
        g_warning("Unknown type");
    }
}

void
process_params(GObject *object, gchar **token, const struct PARAM paramv[], gint paramc)
{
    GObjectClass *object_class = G_OBJECT_GET_CLASS(object);

    if (object_class != NULL)
    {
        gint index;

        for (index=0; index<paramc; index++)
        {
            const char *name = paramv[index].name;

            if (name != NULL)
            {
                GParamSpec *pspec = g_object_class_find_property(object_class, name);
                GValue value = {0};

                if (pspec == NULL)
                {
                    g_warning("    Unknown property %s", name);
                    continue;
                }

                if ((pspec->flags & G_PARAM_WRITABLE) == 0)
                {
                    g_warning("    Property not writable %s", name);
                    continue;
                }

                if (token[index] == NULL)
                {
                    g_warning("    Not enough parameters");
                    break;
                }

                g_value_init(&value, G_PARAM_SPEC_VALUE_TYPE(pspec));
                set_value(&value, token[index]);

                //g_debug("    Assigning parameter %s", name);

                g_object_set_property(object, name, &value);
                g_value_unset(&value);
            }
        }
    }
}



static void
process_path_command_closepath(SchPath *path, GMatchInfo *match_info);

static void
process_path_command_curveto(SchPath *path, GMatchInfo *match_info);

static void
process_path_command_lineto(SchPath *path, GMatchInfo *match_info);

static void
process_path_command_moveto(SchPath *path, GMatchInfo *match_info);


static void
process_path_commands(SchPath *path, const char* string)
{
    GRegex *re = g_regex_new(
        "([MmLlCcZz])([[:space:]]*([0-9]+)([[:space:]]*[,]?[[:space:]]*([0-9]+))*)*",
        0,
        0,
        NULL
        );

    if (re != NULL)
    {
        GMatchInfo *match_info;

        g_regex_match(re, string, 0, &match_info);

        while (g_match_info_matches (match_info))
        {
            gchar *word = g_match_info_fetch (match_info, 0);

            switch (*word)
            {
                case 'C':
                    process_path_command_curveto(path, match_info);
                    break;

                case 'L':
                    process_path_command_lineto(path, match_info);
                    break;

                case 'M':
                    process_path_command_moveto(path, match_info);
                    break;

                case 'z':
                    process_path_command_closepath(path, match_info);
                    break;

                deafult:
                    break;
            }

            g_free (word);
            g_match_info_next (match_info, NULL);
        }

        g_match_info_free(match_info);

        g_regex_unref(re);
    }
}

static void
process_path_command_closepath(SchPath *path, GMatchInfo *match_info)
{
    gchar *word = g_match_info_fetch (match_info, 0);
    SchPathCommand command;

    g_debug("Closepath: %s", word);

    command.type = SCH_PATH_COMMAND_CLOSEPATH;

    sch_path_append(path, &command);

    g_free (word);
}

static void
process_path_command_curveto(SchPath *path, GMatchInfo *match_info)
{
    gchar *word = g_match_info_fetch (match_info, 0);
    SchPathCommand command;
    int i;
    int count = g_match_info_get_match_count(match_info);

    g_debug("Curveto: %s", word);

    command.type = SCH_PATH_COMMAND_CURVETO_ABS;


    for (i=0; (i+13)<count; i++)
    {
        gchar *a = g_match_info_fetch (match_info, i+3);
        command.curveto.x[0] = atoi(a);
        g_free (a);

        a = g_match_info_fetch (match_info, i+5);
        command.curveto.y[0] = atoi(a);
        g_free (a);

        a = g_match_info_fetch (match_info, i+7);
        command.curveto.x[1] = atoi(a);
        g_free (a);

        a = g_match_info_fetch (match_info, i+9);
        command.curveto.y[1] = atoi(a);
        g_free (a);

        a = g_match_info_fetch (match_info, i+11);
        command.curveto.x[2] = atoi(a);
        g_free (a);

        a = g_match_info_fetch (match_info, i+13);
        command.curveto.y[2] = atoi(a);
        g_free (a);

        sch_path_append(path, &command);
    }

    g_free (word);
}


static void
process_path_command_lineto(SchPath *path, GMatchInfo *match_info)
{
    gchar *word = g_match_info_fetch (match_info, 0);
    SchPathCommand command;
    int i;
    int count = g_match_info_get_match_count(match_info);

    g_debug("Lineto: %s", word);

    command.type = SCH_PATH_COMMAND_LINETO_ABS;


    for (i=0; (i+5)<count; i++)
    {
        gchar *a = g_match_info_fetch (match_info, i+3);
        command.lineto.x = atoi(a);
        g_free (a);

        a = g_match_info_fetch (match_info, i+5);
        command.lineto.y = atoi(a);
        g_free (a);

        sch_path_append(path, &command);
    }

    g_free (word);
}

static void
process_path_command_moveto(SchPath *path, GMatchInfo *match_info)
{
    gchar *word = g_match_info_fetch (match_info, 0);
    SchPathCommand command;
    int i;
    int count = g_match_info_get_match_count(match_info);

    g_debug("Moveto: %s", word);

    command.type = SCH_PATH_COMMAND_MOVETO_ABS;

    for (i=0; (i+5)<count; i++)
    {
        gchar *a = g_match_info_fetch (match_info, i+3);
        command.lineto.x = atoi(a);
        g_free (a);

        a = g_match_info_fetch (match_info, i+5);
        command.lineto.y = atoi(a);
        g_free (a);

        sch_path_append(path, &command);

        command.type = SCH_PATH_COMMAND_LINETO_ABS;
    }

    g_free (word);
}

