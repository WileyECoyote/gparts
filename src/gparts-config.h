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

/*! \file gparts-config.h
 *
 *  \brief Stores configuration values for the part manager
 */

#define GPARTS_TYPE_CONFIG (gparts_config_get_type())
#define GPARTS_CONFIG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GPARTS_TYPE_CONFIG,GPartsConfig))
#define GPARTS_CONFIG_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls),GPARTS_TYPE_CONFIG,GPartsConfigClass))
#define GPARTS_IS_CONFIG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GPARTS_TYPE_CONFIG))
#define GPARTS_IS_CONFIG_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls),GPARTS_TYPE_CONFIG))
#define GPARTS_CONFIG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj),GPARTS_TYPE_CONFIG,GPartsConfigClass))

struct _GPartsConfig
{
    GObject parent;
};
/*! \private */
struct _GPartsConfigClass
{
    GObjectClass parent;
};

/*! \private */
GType
gparts_config_get_type(void);

/*! \brief Create a new part manager configuration
 *
 *  The config object is a singleton. This function will return the same
 *  instance each call.
 *
 *  \return A pointer to the configuration.
 */
GPartsConfig       * gparts_config_new                (void);
GPartsDatabaseType * gparts_config_get_database_types (const GPartsConfig *config);
