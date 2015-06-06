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

/*! \file sch-output-stream.c
 */

#include <string.h>

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

#include "sch.h"

#define SCH_OUTPUT_STREAM_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SCH_TYPE_OUTPUT_STREAM,SchOutputStreamPrivate))

#define SCH_OUTPUT_STREAM_EOL "\n"

enum
{
    SCH_OUTPUT_STREAM_BASE_STREAM = 1
};

typedef struct _SchOutputStreamPrivate SchOutputStreamPrivate;

struct _SchOutputStreamPrivate
{
    GOutputStream *base_stream;
};

static void
sch_output_stream_class_init(void *g_class, void *g_class_data);

static void
sch_output_stream_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
sch_output_stream_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void
sch_output_stream_write_path_2_proc(SchPathCommand *command, void *user_data);


static void
sch_output_stream_class_init(void *g_class, void *g_class_data)
{
    GObjectClass *klasse = G_OBJECT_CLASS(g_class);

    g_type_class_add_private(klasse, sizeof(SchOutputStreamPrivate));

    klasse->get_property = sch_output_stream_get_property;
    klasse->set_property = sch_output_stream_set_property;

    g_object_class_install_property(
        klasse,
        SCH_OUTPUT_STREAM_BASE_STREAM,
        g_param_spec_object(
            "base-stream",
            "Base Stream",
            "Base Stream",
            G_TYPE_OUTPUT_STREAM,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

#if 0
    g_object_class_install_property(
        klasse,
        SCH_OUTPUT_STREAM_Y1,
        g_param_spec_int(
            "y1",
            "Y1",
            "Y1",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCH_OUTPUT_STREAM_X2,
        g_param_spec_int(
            "x2",
            "X2",
            "X2",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCH_OUTPUT_STREAM_Y2,
        g_param_spec_int(
            "y2",
            "Y2",
            "Y2",
            G_MININT,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCH_OUTPUT_STREAM_COLOR,
        g_param_spec_int(
            "color",
            "Color",
            "Color",
            0, /*COLOR_MIN,*/
            31, /*COLOR_MAX,*/
            3, /*COLOR_GRAPHIC,*/
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCH_OUTPUT_STREAM_WIDTH,
        g_param_spec_int(
            "line-width",
            "Line Width",
            "Line Width",
            0,
            G_MAXINT,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCH_OUTPUT_STREAM_CAP_STYLE,
        g_param_spec_int(
            "line-cap-style",
            "Line Cap Style",
            "Line Cap Style",
            0,
            2,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCH_OUTPUT_STREAM_DASH_STYLE,
        g_param_spec_int(
            "line-dash-style",
            "Line dash Style",
            "Line dash Style",
            0,
            4,
            0,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCH_OUTPUT_STREAM_DASH_LENGTH,
        g_param_spec_int(
            "line-dash-length",
            "Line Dash Length",
            "Line Dash Length",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    g_object_class_install_property(
        klasse,
        SCH_OUTPUT_STREAM_DASH_SPACE,
        g_param_spec_int(
            "line-dash-space",
            "Line Dash Space",
            "Line Dash Space",
            -1,
            G_MAXINT,
            -1,
            G_PARAM_LAX_VALIDATION | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );
#endif
}

static void
sch_output_stream_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(object);

    if (privat != NULL)
    {
        switch (property_id)
        {
            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

GType
sch_output_stream_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        static const GTypeInfo tinfo = {
            sizeof(SchOutputStreamClass),    /* class_size */
            NULL,                            /* base_init */
            NULL,                            /* base_finalize */
            sch_output_stream_class_init,    /* class_init */
            NULL,                            /* class_finalize */
            NULL,                            /* class_data */
            sizeof(SchOutputStream),         /* instance_size */
            0,                               /* n_preallocs */
            NULL,                            /* instance_init */
            NULL                             /* value_table */
            };

        type = g_type_register_static(
            G_TYPE_OBJECT,
            "SchOutputStream",
            &tinfo,
            0
            );
    }

    return type;
}

static void
sch_output_stream_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(object);

    if (privat != NULL)
    {
        switch (property_id)
        {
            case SCH_OUTPUT_STREAM_BASE_STREAM:
                /* construction only */
                privat->base_stream = g_value_dup_object(value);
                break;

            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

SchOutputStream*
sch_output_stream_new(GOutputStream *base_stream)
{
    return g_object_new(
        SCH_TYPE_OUTPUT_STREAM,
        "base-stream", base_stream,
        NULL
        );
}

void
sch_output_stream_write_arc_2(SchOutputStream *stream, const SchArc *shape, GError **error)
{
    SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(stream);

    if (privat != NULL && shape != NULL)
    {
        char *str;
        int   x;
        int   y;
        int   radius;
        int   start;
        int   sweep;
        int   color;
        int   width;
        int   capstyle;
        int   dashstyle;
        int   dashlength;
        int   dashspace;

        g_object_get(
            (gpointer) shape,
            "center-x",          &x,
            "center-y",          &y,
            "radius",            &radius,
            "start",             &start,
            "sweep",             &sweep,
            "color",             &color,
            "line-width",        &width,
            "line-cap-style",    &capstyle,
            "line-dash-style",   &dashstyle,
            "line-dash-length",  &dashlength,
            "line-dash-space",   &dashspace,
            NULL
            );

        str = g_strdup_printf(
            "%s %d %d %d %d %d %d %d %d %d %d %d%s",
            "A",
            x,
            y,
            radius,
            start,
            sweep,
            color,
            width,
            capstyle,
            dashstyle,
            dashlength,
            dashspace,
            SCH_OUTPUT_STREAM_EOL
            );

        g_output_stream_write(
            privat->base_stream,
            str,
            strlen(str),
            NULL,
            error
            );

        g_free(str);
    }

}

void
sch_output_stream_write_attributes_begin(SchOutputStream *stream, GError **error)
{
    SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(stream);

    if (privat != NULL)
    {
        char *str = g_strdup_printf(
            "%s%s",
            "{",
            SCH_OUTPUT_STREAM_EOL
            );

        g_output_stream_write(
            privat->base_stream,
            str,
            strlen(str),
            NULL,
            error
            );

        g_free(str);
    }


}

void
sch_output_stream_write_attributes_end(SchOutputStream *stream, GError **error)
{
    SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(stream);

    if (privat != NULL)
    {
        char *str = g_strdup_printf(
            "%s%s",
            "}",
            SCH_OUTPUT_STREAM_EOL
            );

        g_output_stream_write(
            privat->base_stream,
            str,
            strlen(str),
            NULL,
            error
            );

        g_free(str);
    }
}

void
sch_output_stream_write_box_2(SchOutputStream *stream, const SchBox *shape, GError **error)
{
    SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(stream);

    if (privat != NULL && shape != NULL)
    {
        char *str;
        int   x;
        int   y;
        int   width;
        int   height;
        int   color;
        int   line_width;
        int   capstyle;
        int   dashstyle;
        int   dashlength;
        int   dashspace;
        int   fill_type;
        int   fill_width;
        int   fill_angle1;
        int   fill_pitch1;
        int   fill_angle2;
        int   fill_pitch2;

        g_object_get(
            (gpointer) shape,
            "x",                &x,
            "y",                &y,
            "width",            &width,
            "height",           &height,
            "color",            &color,
            "line-width",       &line_width,
            "line-cap-style",   &capstyle,
            "line-dash-style",  &dashstyle,
            "line-dash-length", &dashlength,
            "line-dash-space",  &dashspace,
            "fill-type",        &fill_type,
            "fill-width",       &fill_width,
            "fill-angle1",      &fill_angle1,
            "fill-pitch1",      &fill_pitch1,
            "fill-angle2",      &fill_angle2,
            "fill-pitch2",      &fill_pitch2,
            NULL
            );

        str = g_strdup_printf(
            "%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d%s",
            "B",
            x,
            y,
            width,
            height,
            color,
            line_width,
            capstyle,
            dashstyle,
            dashlength,
            dashspace,
            fill_type,
            fill_width,
            fill_angle1,
            fill_pitch1,
            fill_angle2,
            fill_pitch2,
            SCH_OUTPUT_STREAM_EOL
            );

        g_output_stream_write(
            privat->base_stream,
            str,
            strlen(str),
            NULL,
            error
            );

        g_free(str);
    }
}

void
sch_output_stream_write_bus_2(SchOutputStream *stream, const SchBus *shape, GError **error)
{
    SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(stream);

    if (privat != NULL && shape != NULL)
    {
        char *str;
        int   x1;
        int   y1;
        int   x2;
        int   y2;
        int   color;
        int   ripperdir;

        g_object_get(
            (gpointer) shape,
            "x1",         &x1,
            "y1",         &y1,
            "x2",         &x2,
            "y2",         &y2,
            "color",      &color,
            "ripper-dir", &ripperdir,
            NULL
            );

        str = g_strdup_printf(
            "%s %d %d %d %d %d %d%s",
            "B",
            x1,
            y1,
            x2,
            y2,
            color,
            ripperdir,
            SCH_OUTPUT_STREAM_EOL
            );

        g_output_stream_write(
            privat->base_stream,
            str,
            strlen(str),
            NULL,
            error
            );

        g_free(str);
    }
}

void
sch_output_stream_write_circle_2(SchOutputStream *stream, const SchCircle *shape, GError **error)
{
    SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(stream);

    if (privat != NULL && shape != NULL)
    {
        char *str;
        int   x;
        int   y;
        int   radius;
        int   color;
        int   line_width;
        int   capstyle;
        int   dashstyle;
        int   dashlength;
        int   dashspace;
        int   fill_type;
        int   fill_width;
        int   fill_angle1;
        int   fill_pitch1;
        int   fill_angle2;
        int   fill_pitch2;


        g_object_get(
            (gpointer) shape,
            "center-x",       &x,
            "center-y",       &y,
            "radius",         &radius,
            "color",            &color,
            "line-width",       &line_width,
            "line-cap-style",   &capstyle,
            "line-dash-style",  &dashstyle,
            "line-dash-length", &dashlength,
            "line-dash-space",  &dashspace,
            "fill-type",        &fill_type,
            "fill-width",       &fill_width,
            "fill-angle1",      &fill_angle1,
            "fill-pitch1",      &fill_pitch1,
            "fill-angle2",      &fill_angle2,
            "fill-pitch2",      &fill_pitch2,
            NULL
            );

        str = g_strdup_printf(
            "%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d%s",
            "V",
            x,
            y,
            radius,
            color,
            line_width,
            capstyle,
            dashstyle,
            dashlength,
            dashspace,
            fill_type,
            fill_width,
            fill_angle1,
            fill_pitch1,
            fill_angle2,
            fill_pitch2,
            SCH_OUTPUT_STREAM_EOL
            );

        g_output_stream_write(
            privat->base_stream,
            str,
            strlen(str),
            NULL,
            error
            );

        g_free(str);
    }

}

void
sch_output_stream_write_component_2(SchOutputStream *stream, const SchComponent *shape, GError **error)
{
    SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(stream);

    if (privat != NULL && shape != NULL)
    {
        char *str;
        int   x;
        int   y;
        int   selectable;
        int   angle;
        int   mirror;
        char  *basename;

        g_object_get(
            (gpointer) shape,
            "x",          &x,
            "y",          &y,
            "selectable", &selectable,
            "angle",      &angle,
            "mirror",     &mirror,
            "basename",   &basename,
            NULL
            );

        str = g_strdup_printf(
            "%s %d %d %d %d %d %s%s",
            "C",
            x,
            y,
            selectable,
            angle,
            mirror,
            basename,
            SCH_OUTPUT_STREAM_EOL
            );

        g_free(basename);

        g_debug("Output : %s", str);

        g_output_stream_write(
            privat->base_stream,
            str,
            strlen(str),
            NULL,
            error
            );

        g_free(str);
    }

}

void
sch_output_stream_write_line_2(SchOutputStream *stream, const SchLine *shape, GError **error)
{
    SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(stream);

    if (privat != NULL && shape != NULL)
    {
        char *str;
        int   x1;
        int   y1;
        int   x2;
        int   y2;
        int   color;
        int   width;
        int   capstyle;
        int   dashstyle;
        int   dashlength;
        int   dashspace;

        g_object_get(
            (gpointer) shape,
            "x1",               &x1,
            "y1",               &y1,
            "x2",               &x2,
            "y2",               &y2,
            "color",            &color,
            "line-width",       &width,
            "line-cap-style",   &capstyle,
            "line-dash-style",  &dashstyle,
            "line-dash-length", &dashlength,
            "line-dash-space",  &dashspace,
            NULL
            );

        str = g_strdup_printf(
            "%s %d %d %d %d %d %d %d %d %d %d%s",
            "L",
            x1,
            y1,
            x2,
            y2,
            color,
            width,
            capstyle,
            dashstyle,
            dashlength,
            dashspace,
            SCH_OUTPUT_STREAM_EOL
            );

        g_output_stream_write(
            privat->base_stream,
            str,
            strlen(str),
            NULL,
            error
            );

        g_free(str);
    }

}

void
sch_output_stream_write_net_2(SchOutputStream *stream, const SchNet *shape, GError **error)
{
    SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(stream);

    if (privat != NULL && shape != NULL)
    {
        char *str;
        int   x1;
        int   y1;
        int   x2;
        int   y2;
        int   color;
        int   type;
        int   end;

        g_object_get(
            (gpointer) shape,
            "x1",       &x1,
            "y1",       &y1,
            "x2",       &x2,
            "y2",       &y2,
            "color",    &color,
            "pin-type", &type,
            "pin-end",  &end,
            NULL
            );

        str = g_strdup_printf(
            "%s %d %d %d %d %d %d %d%s",
            "N",
            x1,
            y1,
            x2,
            y2,
            color,
            type,
            end,
            SCH_OUTPUT_STREAM_EOL
            );

        g_output_stream_write(
            privat->base_stream,
            str,
            strlen(str),
            NULL,
            error
            );

        g_free(str);
    }

}

void
sch_output_stream_write_path_2(SchOutputStream *stream, SchPath *shape, GError **error)
{
  SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(stream);

  if (privat != NULL && shape != NULL) {

    char *str;
    //        int   x;
    //        int   y;
    //        int   radius;
    int   color;
    int   line_width;
    int   capstyle;
    int   dashstyle;
    int   dashlength;
    int   dashspace;
    int   fill_type;
    int   fill_width;
    int   fill_angle1;
    int   fill_pitch1;
    int   fill_angle2;
    int   fill_pitch2;
    int   count = sch_path_count(shape);

    g_object_get(shape,
                 "color",            &color,
                 "line-width",       &line_width,
                 "line-cap-style",   &capstyle,
                 "line-dash-style",  &dashstyle,
                 "line-dash-length", &dashlength,
                 "line-dash-space",  &dashspace,
                 "fill-type",        &fill_type,
                 "fill-width",       &fill_width,
                 "fill-angle1",      &fill_angle1,
                 "fill-pitch1",      &fill_pitch1,
                 "fill-angle2",      &fill_angle2,
                 "fill-pitch2",      &fill_pitch2,
                 NULL);

    str = g_strdup_printf(
      "%s %d %d %d %d %d %d %d %d %d %d %d %d %d%s",
      "H",
      color,
      line_width,
      capstyle,
      dashstyle,
      dashlength,
      dashspace,
      fill_type,
      fill_width,
      fill_angle1,
      fill_pitch1,
      fill_angle2,
      fill_pitch2,
      count,
      SCH_OUTPUT_STREAM_EOL
    );

    g_output_stream_write(
      privat->base_stream,
      str,
      strlen(str),
                          NULL,
                          error
    );

    sch_path_foreach(shape, (GFunc)sch_output_stream_write_path_2_proc, stream);

    g_free(str);
  }
}

static void
sch_output_stream_write_path_2_proc(SchPathCommand *command, void *user_data)
{
    SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(user_data);

    if ((privat != NULL) && (command != NULL)) {

        char *str = NULL;

        switch (command->type) {

            case SCH_PATH_COMMAND_INVALID:
               g_debug("SchGUICairoPath: invalid path command");
                break;

            case SCH_PATH_COMMAND_CLOSEPATH:
                str = g_strdup_printf(
                    "%s%s",
                    "z",
                    SCH_OUTPUT_STREAM_EOL
                    );
                break;

            case SCH_PATH_COMMAND_CURVETO_ABS:
                str = g_strdup_printf(
                    "%s %d %d %d %d %d %d%s",
                    "C",
                    command->curveto.x[0],
                    command->curveto.y[0],
                    command->curveto.x[1],
                    command->curveto.y[1],
                    command->curveto.x[2],
                    command->curveto.y[2],
                    SCH_OUTPUT_STREAM_EOL
                    );
                break;

            case SCH_PATH_COMMAND_CURVETO_REL:
                str = g_strdup_printf(
                    "%s %d %d %d %d %d %d%s",
                    "c",
                    command->curveto.x[0],
                    command->curveto.y[0],
                    command->curveto.x[1],
                    command->curveto.y[1],
                    command->curveto.x[2],
                    command->curveto.y[2],
                    SCH_OUTPUT_STREAM_EOL
                    );
                break;

            case SCH_PATH_COMMAND_LINETO_ABS:
                str = g_strdup_printf(
                    "%s %d %d%s",
                    "L",
                    command->lineto.x,
                    command->lineto.y,
                    SCH_OUTPUT_STREAM_EOL
                    );
                break;

            case SCH_PATH_COMMAND_LINETO_REL:
                str = g_strdup_printf(
                    "%s %d %d%s",
                    "l",
                    command->lineto.x,
                    command->lineto.y,
                    SCH_OUTPUT_STREAM_EOL
                    );
                break;

            case SCH_PATH_COMMAND_MOVETO_ABS:
                str = g_strdup_printf(
                    "%s %d %d%s",
                    "M",
                    command->moveto.x,
                    command->moveto.y,
                    SCH_OUTPUT_STREAM_EOL
                    );
                break;

            case SCH_PATH_COMMAND_MOVETO_REL:
                str = g_strdup_printf(
                    "%s %d %d%s",
                    "m",
                    command->moveto.x,
                    command->moveto.y,
                    SCH_OUTPUT_STREAM_EOL
                    );
                break;

            default:
                g_debug("SchGUICairoPath: unknown path command");
        }

        g_output_stream_write(
            privat->base_stream,
            str,
            strlen(str),
            NULL,
            NULL /*! \todo FIXME error */
            );

        g_free(str);
    }
}

void
sch_output_stream_write_picture_2(SchOutputStream *stream, SchPicture *shape, GError **error)
{
    SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(stream);

    if (privat != NULL && shape != NULL) {

        char *str;
        int   x1;
        int   y1;
        int   x2;
        int   y2;
        int   color;
        int   type;
        int   end;

        g_object_get(shape,
            "x1",       &x1,
            "y1",       &y1,
            "x2",       &x2,
            "y2",       &y2,
            "color",    &color,
            "pin-type", &type,
            "pin-end",  &end,
            NULL
            );

        str = g_strdup_printf(
            "%s %d %d %d %d %d %d %d%s",
            "G",
            x1,
            y1,
            x2,
            y2,
            color,
            type,
            end,
            SCH_OUTPUT_STREAM_EOL
            );

        g_output_stream_write(
            privat->base_stream,
            str,
            strlen(str),
            NULL,
            error
            );

        g_free(str);
    }

}

void
sch_output_stream_write_pin_2(SchOutputStream *stream, SchPin *shape, GError **error)
{
    SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(stream);

    if (privat != NULL && shape != NULL) {

        char *str;
        int   x1;
        int   y1;
        int   x2;
        int   y2;
        int   color;
        int   type;
        int   end;

        g_object_get(shape,
            "x1",       &x1,
            "y1",       &y1,
            "x2",       &x2,
            "y2",       &y2,
            "color",    &color,
            "pin-type", &type,
            "pin-end",  &end,
            NULL
            );

        str = g_strdup_printf(
            "%s %d %d %d %d %d %d %d%s",
            "P",
            x1,
            y1,
            x2,
            y2,
            color,
            type,
            end,
            SCH_OUTPUT_STREAM_EOL
            );

        g_output_stream_write(
            privat->base_stream,
            str,
            strlen(str),
            NULL,
            error
            );

        g_free(str);
    }
}

void
sch_output_stream_write_string(SchOutputStream *stream, const char *string, GError **error)
{
    SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(stream);

    if (privat != NULL && string != NULL) {

        g_output_stream_write(
            privat->base_stream,
            string,
            strlen(string),
            NULL,
            error
            );

        g_output_stream_write(
            privat->base_stream,
            SCH_OUTPUT_STREAM_EOL,
            strlen(SCH_OUTPUT_STREAM_EOL),
            NULL,
            error
            );
    }
}

void
sch_output_stream_write_text_2(SchOutputStream *stream, SchText *shape, GError **error)
{
    SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(stream);

    if (privat != NULL && shape != NULL) {

        char *str;
        int   x;
        int   y;
        int   color;
        int   size;
        int   visibility;
        int   name_value;
        int   angle;
        int   alignment;
        int   num_lines;

        g_object_get(shape,
            "x",               &x,
            "y",               &y,
            "color",           &color,
            "size",            &size,
            "visibility",      &visibility,
            "show-name-value", &name_value,
            "angle",           &angle,
            "alignment",       &alignment,
            "num-lines",       &num_lines,
            NULL
            );

        str = g_strdup_printf(
            "%s %d %d %d %d %d %d %d %d %d%s",
            "T",
            x,
            y,
            color,
            size,
            visibility,
            name_value,
            angle,
            alignment,
            num_lines,
            SCH_OUTPUT_STREAM_EOL
            );

        g_output_stream_write(
            privat->base_stream,
            str,
            strlen(str),
            NULL,
            error
            );

        g_free(str);
    }

}

void
sch_output_stream_write_version(SchOutputStream *stream, const char *package_date, int format_version, GError **error)
{
    SchOutputStreamPrivate *privat = SCH_OUTPUT_STREAM_GET_PRIVATE(stream);

    if (privat != NULL) {

        char *str = g_strdup_printf(
            "%s %s %d%s",
            "v",
            package_date,
            format_version,
            SCH_OUTPUT_STREAM_EOL
            );

        g_output_stream_write(
            privat->base_stream,
            str,
            strlen(str),
            NULL,
            error
            );

        g_free(str);
    }
}
