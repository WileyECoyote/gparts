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

/*! \file miscgui-color.c
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <glib-object.h>
#include <gdk/gdk.h>

#include "miscgui-color.h"


MiscGUIColor*
miscgui_color_copy(const MiscGUIColor *color)
{
    return MISCGUI_COLOR(g_memdup(color, sizeof(MiscGUIColor)));
}

GType
miscgui_color_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        type = g_boxed_type_register_static(
            "MiscGUIColor",
            (GBoxedCopyFunc) miscgui_color_copy,
            (GBoxedFreeFunc) miscgui_color_free
            );
    }

    return type;
}

void
miscgui_color_free(MiscGUIColor *color)
{
    g_free(color);
}

void
miscgui_color_init(MiscGUIColor *color)
{
    memset(color, 0, sizeof(MiscGUIColor));
}

MiscGUIColor*
miscgui_color_new(void)
{
    MiscGUIColor *color = g_new(MiscGUIColor, 1);

    miscgui_color_init(color);

    return color;
}

MiscGUIColor*
miscgui_color_new_from_name(const gchar *name)
{
  MiscGUIColor *color = NULL;

  if (name != NULL) {

    GdkColor value;

    if (gdk_color_parse(name, &value)) {

      color = miscgui_color_new();

      color->red   = (gdouble) value.red   / 65535.0;
      color->green = (gdouble) value.green / 65535.0;
      color->blue  = (gdouble) value.blue  / 65535.0;
      color->alpha = 1.0;
    }
  }

  return color;
}
