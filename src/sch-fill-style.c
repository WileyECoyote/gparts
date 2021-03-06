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

/*! \file sch-fill-style.c
 */

#include <glib.h>
#include <glib-object.h>

#include "sch-fill-style.h"

SchFillStyle*
sch_fill_style_copy(const SchFillStyle *style)
{
    return SCH_FILL_STYLE(g_memdup(style, sizeof(SchFillStyle)));
}

GType
sch_fill_style_get_type(void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID)
    {
        type = g_boxed_type_register_static(
            "SchFillStyle",
            (GBoxedCopyFunc) sch_fill_style_copy,
            (GBoxedFreeFunc) sch_fill_style_free
            );
    }

    return type;
}

void
sch_fill_style_free(SchFillStyle *style)
{
    g_free(style);
}

void
sch_fill_style_init(SchFillStyle *style)
{
    if (style != NULL)
    {
        style->type   = SCH_FILL_STYLE_FILL_TYPE_HOLLOW;
        style->width  = -1;
        style->angle1 = -1;
        style->pitch1 = -1;
        style->angle2 = -1;
        style->pitch2 = -1;
    }
}

SchFillStyle*
sch_fill_style_new(void)
{
    SchFillStyle *style = (SchFillStyle*) g_malloc(sizeof(SchFillStyle));

    sch_fill_style_init(style);

    return style;
}

