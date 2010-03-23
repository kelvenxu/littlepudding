/*
 * This is a based on rb-module.h from Rhythmbox, which is based on 
 * gedit-module.h from gedit, which is based on Epiphany source code.
 *
 * Copyright (C) 2003 Marco Pesenti Gritti
 * Copyright (C) 2003, 2004 Christian Persch
 * Copyright (C) 2005 - Paolo Maggi
 * Copyright (C) 2007 - Bastien Nocera <hadess@hadess.net>
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

#ifndef LMPLAYER_MODULE_H
#define LMPLAYER_MODULE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define LMPLAYER_TYPE_MODULE		(lmplayer_module_get_type ())
#define LMPLAYER_MODULE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), LMPLAYER_TYPE_MODULE, LmplayerModule))
#define LMPLAYER_MODULE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), LMPLAYER_TYPE_MODULE, LmplayerModuleClass))
#define LMPLAYER_IS_MODULE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), LMPLAYER_TYPE_MODULE))
#define LMPLAYER_IS_MODULE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((obj), LMPLAYER_TYPE_MODULE))
#define LMPLAYER_MODULE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), LMPLAYER_TYPE_MODULE, LmplayerModuleClass))

typedef struct _LmplayerModule	LmplayerModule;

GType		 lmplayer_module_get_type		(void) G_GNUC_CONST;;

LmplayerModule	*lmplayer_module_new		(const gchar *path, const char *module);

const gchar	*lmplayer_module_get_path		(LmplayerModule *module);

GObject		*lmplayer_module_new_object	(LmplayerModule *module);

G_END_DECLS

#endif
