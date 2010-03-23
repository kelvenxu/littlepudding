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

#ifndef LMPLAYER_PYTHON_OBJECT_H
#define LMPLAYER_PYTHON_OBJECT_H

#include <Python.h>
#include <glib-object.h>
#include "lmplayer-plugin.h"

G_BEGIN_DECLS

typedef struct
{
	LmplayerPlugin parent_slot;
	PyObject *instance;
} LmplayerPythonObject;

typedef struct
{
	LmplayerPluginClass parent_slot;
	PyObject *type;
} LmplayerPythonObjectClass;

GType lmplayer_python_object_get_type (GTypeModule *module, PyObject *type);

G_END_DECLS

#endif
