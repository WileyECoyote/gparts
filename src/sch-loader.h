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

/*! \file sch-loader.h
 */

#define SCH_TYPE_LOADER (sch_loader_get_type())
#define SCH_LOADER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SCH_TYPE_LOADER,SchLoader))
#define SCH_LOADER_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),SCH_TYPE_LOADER,SchLoaderClass))
#define SCH_IS_LOADER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SCH_TYPE_LOADER))
#define SCH_IS_LOADER_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),SCH_TYPE_LOADER))
#define SCH_LOADER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),SCH_TYPE_LOADER,SchLoaderClass))

struct _SchLoaderStyle;

typedef struct _SchLoader SchLoader;
typedef struct _SchLoaderClass SchLoaderClass;

struct _SchLoader
{
    GObject parent;
};

struct _SchLoaderClass
{
    GObjectClass parent;
};

void
sch_loader_add_component_library(SchLoader *loader, char *library);

SchLoader*
sch_loader_get_default(void);

char**
sch_loader_get_component_libraries(GObject *object);

GType
sch_loader_get_type(void);

SchDrawing*
sch_loader_load_drawing(SchLoader *loader, const char *filename, GError **error);

SchDrawing*
sch_loader_load_symbol(SchLoader *loader, const char *filename, GError **error);

char*
sch_loader_find_symbol_file(SchLoader *loader, const char *filename);

FILE*
sch_loader_open_symbol_file(SchLoader *loader, const char *filename, const char *mode);

void
sch_loader_set_component_libraries(GObject *object, char **name);

