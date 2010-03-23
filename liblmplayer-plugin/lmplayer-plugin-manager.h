/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * heavily based on code from Rhythmbox and Gedit
 *
 * Copyright (C) 2002 Paolo Maggi and James Willcox
 * Copyright (C) 2003-2005 Paolo Maggi
 * Copyright (C) 2007 Bastien Nocera <hadess@hadess.net>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301  USA.
 *
 * Sunday 13th May 2007: Bastien Nocera: Add exception clause.
 * See license_change file for details.
 *
 */

#ifndef __LMPLAYER_PLUGIN_MANAGER_H__
#define __LMPLAYER_PLUGIN_MANAGER_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define LMPLAYER_TYPE_PLUGIN_MANAGER              (lmplayer_plugin_manager_get_type())
#define LMPLAYER_PLUGIN_MANAGER(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LMPLAYER_TYPE_PLUGIN_MANAGER, LmplayerPluginManager))
#define LMPLAYER_PLUGIN_MANAGER_CONST(obj)        (G_TYPE_CHECK_INSTANCE_CAST((obj), LMPLAYER_TYPE_PLUGIN_MANAGER, LmplayerPluginManager const))
#define LMPLAYER_PLUGIN_MANAGER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LMPLAYER_TYPE_PLUGIN_MANAGER, LmplayerPluginManagerClass))
#define LMPLAYER_IS_PLUGIN_MANAGER(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LMPLAYER_TYPE_PLUGIN_MANAGER))
#define LMPLAYER_IS_PLUGIN_MANAGER_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LMPLAYER_TYPE_PLUGIN_MANAGER))
#define LMPLAYER_PLUGIN_MANAGER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LMPLAYER_TYPE_PLUGIN_MANAGER, LmplayerPluginManagerClass))

/* Private structure type */
typedef struct _LmplayerPluginManagerPrivate LmplayerPluginManagerPrivate;

/*
 * Main object structure
 */
typedef struct
{
	GtkVBox vbox;

	/*< private > */
	LmplayerPluginManagerPrivate *priv;
} LmplayerPluginManager;

/*
 * Class definition
 */
typedef struct
{
	GtkVBoxClass parent_class;
} LmplayerPluginManagerClass;

/*
 * Public methods
 */
GType		 lmplayer_plugin_manager_get_type		(void) G_GNUC_CONST;

GtkWidget	*lmplayer_plugin_manager_new		(void);

G_END_DECLS

#endif  /* __LMPLAYER_PLUGIN_MANAGER_H__  */
