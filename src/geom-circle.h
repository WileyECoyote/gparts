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

/*! \file geom-circle.h
 */

typedef struct _GeomCircle GeomCircle;

struct _GeomCircle
{
    int center_x;
    int center_y;
    int radius;
};

/*! \brief Calculate the bounding rectangle of the given circle.
 *
 *  \param [in]  circle
 *  \param [out] bounds
 */
void
geom_circle_bounds(const GeomCircle *circle, GeomBounds *bounds);

void
geom_circle_init(GeomCircle *circle);

void
geom_circle_rotate(GeomCircle *circle, int angle);

void
geom_circle_translate(GeomCircle *circle, int dx, int dy);

