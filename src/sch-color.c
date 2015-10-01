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

/*! \file sch-color.c
 */

#include <stdlib.h>

#define SCH_COLOR_COUNT 24

#define SCH_COLOR_ENTRY(r,g,b) {(r/255.0),(g/255.0),(b/255.0)}

typedef struct _SchColorEntry SchColorEntry;

struct _SchColorEntry
{
    double red;
    double green;
    double blue;
};

static const char *sch_color_name_table[SCH_COLOR_COUNT] =
{
    /*  0 */ "background",
    /*  1 */ "pin",
    /*  2 */ "net-endpoint",
    /*  3 */ "graphic",
    /*  4 */ "net",
    /*  5 */ "attribute",
    /*  6 */ "logic-bubble",
    /*  7 */ "dots-grid",
    /*  8 */ "detached-attribute",
    /*  9 */ "text",
    /* 10 */ "bus",
    /* 11 */ "select",
    /* 12 */ "bounding-box",
    /* 13 */ "zoom-box",
    /* 14 */ "stroke",
    /* 15 */ "lock",
    /* 16 */ "output-background",
    /* 17 */ "junction",
    /* 18 */ "freestyle1",
    /* 19 */ "freestyle2",
    /* 20 */ "freestyle3",
    /* 21 */ "freestyle4",
    /* 22 */ "mesh-grid-major",
    /* 23 */ "mesh-grid-minor"
};

static const SchColorEntry sch_color_dark_table[SCH_COLOR_COUNT] =
{
    /*  0  background         */ SCH_COLOR_ENTRY(0x00, 0x00, 0x00),
    /*  1  pin                */ SCH_COLOR_ENTRY(0xFF, 0xFF, 0xFF),
    /*  2  net-endpoint       */ SCH_COLOR_ENTRY(0xFF, 0x00, 0x00),
    /*  3  graphic            */ SCH_COLOR_ENTRY(0x00, 0xFF, 0x00),
    /*  4  net                */ SCH_COLOR_ENTRY(0x00, 0x00, 0xFF),
    /*  5  attribute          */ SCH_COLOR_ENTRY(0xFF, 0xFF, 0x00),
    /*  6  logic-bubble       */ SCH_COLOR_ENTRY(0x00, 0xFF, 0xFF),
    /*  7  dots-grid          */ SCH_COLOR_ENTRY(0xBE, 0xBE, 0xBE),
    /*  8  detached-attribute */ SCH_COLOR_ENTRY(0xFF, 0x00, 0x00),
    /*  9  text               */ SCH_COLOR_ENTRY(0x00, 0xFF, 0x00),
    /* 10  bus                */ SCH_COLOR_ENTRY(0x00, 0xFF, 0x00),
    /* 11  select             */ SCH_COLOR_ENTRY(0xFF, 0xA5, 0x00),
    /* 12  bounding-box       */ SCH_COLOR_ENTRY(0xFF, 0xA5, 0x00),
    /* 13  zoom-box           */ SCH_COLOR_ENTRY(0x00, 0xFF, 0xFF),
    /* 14  stroke             */ SCH_COLOR_ENTRY(0xE5, 0xE5, 0xE5),
    /* 15  lock               */ SCH_COLOR_ENTRY(0xBE, 0xBE, 0xBE),
    /* 16  output-background  */ SCH_COLOR_ENTRY(0x00, 0xFF, 0x00), /* originally false */
    /* 17  junction           */ SCH_COLOR_ENTRY(0xFF, 0xFF, 0x00),
    /* 18  freestyle1         */ SCH_COLOR_ENTRY(0x00, 0xFF, 0x00), /* originally false */
    /* 19  freestyle2         */ SCH_COLOR_ENTRY(0x00, 0xFF, 0x00), /* originally false */
    /* 20  freestyle3         */ SCH_COLOR_ENTRY(0x00, 0xFF, 0x00), /* originally false */
    /* 21  freestyle4         */ SCH_COLOR_ENTRY(0x00, 0xFF, 0x00), /* originally false */
    /* 22  mesh-grid-major    */ SCH_COLOR_ENTRY(0x1E, 0x1E, 0x1E),
    /* 23  mesh-grid-minor    */ SCH_COLOR_ENTRY(0x17, 0x17, 0x17)
};

static const SchColorEntry sch_color_light_table[SCH_COLOR_COUNT] =
{
    /*  0  background         */ SCH_COLOR_ENTRY(0xF0, 0xF0, 0xF0),
    /*  1  pin                */ SCH_COLOR_ENTRY(0x00, 0x00, 0x00),
    /*  2  net-endpoint       */ SCH_COLOR_ENTRY(0xFF, 0x00, 0x00),
    /*  3  graphic            */ SCH_COLOR_ENTRY(0x00, 0x8B, 0x00),
    /*  4  net                */ SCH_COLOR_ENTRY(0x00, 0x00, 0xEE),
    /*  5  attribute          */ SCH_COLOR_ENTRY(0x00, 0x00, 0x00),
    /*  6  logic-bubble       */ SCH_COLOR_ENTRY(0x00, 0x8B, 0x8B),
    /*  7  dots-grid          */ SCH_COLOR_ENTRY(0x7F, 0x7F, 0x7F),
    /*  8  detached-attribute */ SCH_COLOR_ENTRY(0xFF, 0x00, 0x00),
    /*  9  text               */ SCH_COLOR_ENTRY(0x00, 0x8B, 0x00),
    /* 10  bus                */ SCH_COLOR_ENTRY(0x00, 0xEE, 0x00),
    /* 11  select             */ SCH_COLOR_ENTRY(0xB2, 0x22, 0x22),
    /* 12  bounding-box       */ SCH_COLOR_ENTRY(0xFF, 0xA5, 0x00),
    /* 13  stroke             */ SCH_COLOR_ENTRY(0xA0, 0x20, 0xF0),
    /* 14  zoom-box           */ SCH_COLOR_ENTRY(0x00, 0x8B, 0x8B),
    /* 15  lock               */ SCH_COLOR_ENTRY(0x66, 0x66, 0x66),
    /* 16  output-background  */ SCH_COLOR_ENTRY(0x00, 0x8B, 0x00), /* originally false */
    /* 17  junction           */ SCH_COLOR_ENTRY(0xA0, 0x20, 0xF0),
    /* 18  freestyle1         */ SCH_COLOR_ENTRY(0x00, 0x8B, 0x00), /* originally false */
    /* 19  freestyle2         */ SCH_COLOR_ENTRY(0x00, 0x8B, 0x00), /* originally false */
    /* 20  freestyle3         */ SCH_COLOR_ENTRY(0x00, 0x8B, 0x00), /* originally false */
    /* 21  freestyle4         */ SCH_COLOR_ENTRY(0x00, 0x8B, 0x00), /* originally false */
    /* 22  mesh-grid-major    */ SCH_COLOR_ENTRY(0xE1, 0xE1, 0xE1),
    /* 23  mesh-grid-minor    */ SCH_COLOR_ENTRY(0xE8, 0xE8, 0xE8)
};

void
sch_color_get_default_dark(int index, double *red, double *green, double *blue)
{
    if ((index < 0) || (index >= SCH_COLOR_COUNT))
    {
        index = 3;
    }

    if (red != NULL)
    {
        *red = sch_color_dark_table[index].red;
    }

    if (green != NULL)
    {
        *green = sch_color_dark_table[index].green;
    }

    if (blue != NULL)
    {
        *blue = sch_color_dark_table[index].blue;
    }
}

void
sch_color_get_default(int index, double *red, double *green, double *blue)
{
    sch_color_get_default_dark(index, red, green, blue);
}

void
sch_color_get_default_light(int index, double *red, double *green, double *blue)
{
    if ((index < 0) || (index >= SCH_COLOR_COUNT))
    {
        index = 3;
    }

    if (red != NULL)
    {
        *red = sch_color_light_table[index].red;
    }

    if (green != NULL)
    {
        *green = sch_color_light_table[index].green;
    }

    if (blue != NULL)
    {
        *blue = sch_color_light_table[index].blue;
    }
}

const char*
sch_color_get_name(int index)
{
    const char *name = NULL;

    if ((index >= 0) && (index < SCH_COLOR_COUNT))
    {
        name = sch_color_name_table[index];
    }

    return name;
}

