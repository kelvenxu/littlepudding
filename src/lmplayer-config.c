/* vi: set sw=4 ts=4: */
/*
 * lmplayer-config.c
 *
 * This file is part of lmplayer.
 *
 * Copyright (C) 2010 - kelvenxu <kelvenxu@gmail.com>.
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
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 * */

#include "lmplayer-config.h"
#include <glib.h>
#include <stdlib.h>

int
lmplayer_load_config(LmplayerObject *lmplayer)
{
	g_return_val_if_fail(lmplayer != NULL, -1);

	gchar *cfg = g_build_filename(getenv("HOME"), ".lmplayer/lmplayer.cfg", NULL);
	if(!cfg)
		return -1;

	GKeyFile *key_file = g_key_file_new();
	if(!key_file)
	{
		g_free(cfg);
		return -1;
	}

	GError *error = NULL;
	g_key_file_load_from_file(key_file, cfg, G_KEY_FILE_KEEP_COMMENTS, &error);
	if(error)
	{
		g_free(cfg);
		g_key_file_free(key_file);
		g_warning("lmplayer load config: %s\n", error->message);
		return -1;
	}

	lmplayer->database = g_key_file_get_string(key_file, "search", "database", &error);
	if(error)
	{
		g_warning("lmplayer load config: %s\n", error->message);
	}

	lmplayer->monitor_path = g_key_file_get_string(key_file, "monitor", "path", &error);
	if(error)
	{
		g_warning("lmplayer load config: %s\n", error->message);
	}

	return 0;
}

