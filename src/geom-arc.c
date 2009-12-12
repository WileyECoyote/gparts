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

#include <math.h>
#include <stdlib.h>
#include "geom-angle.h"
#include "geom-bounds.h"
#include "geom-arc.h"

void
geom_arc_bounds(const GeomArc *arc, GeomBounds *bounds)
{
    double radians0 = geom_angle_radians(arc->start);
    double radians1 = geom_angle_radians(arc->start + arc->sweep);

    double x0 = arc->center_x + arc->radius * cos(radians0);    
    double y0 = arc->center_y + arc->radius * sin(radians0);    

    double x1 = arc->center_x + arc->radius * cos(radians1);    
    double y1 = arc->center_y + arc->radius * sin(radians1);

    double start;
    double end;

    if (x0 < x1)
    {
        bounds->min_x = floor(x0);
        bounds->max_x = ceil(x1);
    }
    else
    {
        bounds->min_x = floor(x1);
        bounds->max_x = ceil(x0);
    }

    if (y0 < y1)
    {
        bounds->min_y = floor(y0);
        bounds->max_y = ceil(y1);
    }
    else
    {
        bounds->min_y = floor(y1);
        bounds->max_y = ceil(y0);
    }

    if (arc->sweep >= 0)
    {
        start = geom_angle_normalize(arc->start);
        end = start + arc->sweep;
    }
    else
    {
        start = geom_angle_normalize(arc->start + arc->sweep);
        end = start - arc->sweep;
    }

    if (start < 90 && end > 90)
    {
        bounds->max_y = arc->center_y + arc->radius;
    }

    if (start < 180 && end > 180)
    {
        bounds->min_x = arc->center_x - arc->radius;
    }

    if (start < 270 && end > 270)
    {
        bounds->min_y = arc->center_y - arc->radius;
    }

    if (start < 360 && end > 360)
    {
        bounds->max_x = arc->center_x + arc->radius;
    }
}

void
geom_arc_translate(GeomArc *arc, int dx, int dy)
{
    if (arc != NULL)
    {
        arc->center_x += dx;
        arc->center_y += dy;
    }
}

