/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Heavily based on code from Rhythmbox and Gedit.
 *
 * Copyright (C) 2005 Raphael Slinckx
 * Copyright (C) 2007 Philip Withnall
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
 * Saturday 19th May 2007: Philip Withnall: Add exception clause.
 * See license_change file for details.
 */

#ifndef LMPLAYER_PYTHON_MODULE_H
#define LMPLAYER_PYTHON_MODULE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define LMPLAYER_TYPE_PYTHON_MODULE		(lmplayer_python_module_get_type ())
#define LMPLAYER_PYTHON_MODULE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), LMPLAYER_TYPE_PYTHON_MODULE, LmplayerPythonModule))
#define LMPLAYER_PYTHON_MODULE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), LMPLAYER_TYPE_PYTHON_MODULE, LmplayerPythonModuleClass))
#define LMPLAYER_IS_PYTHON_MODULE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), LMPLAYER_TYPE_PYTHON_MODULE))
#define LMPLAYER_IS_PYTHON_MODULE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((obj), LMPLAYER_TYPE_PYTHON_MODULE))
#define LMPLAYER_PYTHON_MODULE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), LMPLAYER_TYPE_PYTHON_MODULE, LmplayerPythonModuleClass))

typedef struct
{
	GTypeModuleClass parent_class;
} LmplayerPythonModuleClass;

typedef struct
{
	GTypeModule parent_instance;
} LmplayerPythonModule;

GType			lmplayer_python_module_get_type		(void);
LmplayerPythonModule	*lmplayer_python_module_new		(const gchar* path, const gchar *module);
GObject			*lmplayer_python_module_new_object		(LmplayerPythonModule *module);

/* --- python utils --- */
void			lmplayer_python_garbage_collect		(void);
void			lmplayer_python_shutdown			(void);

G_END_DECLS

#endif
