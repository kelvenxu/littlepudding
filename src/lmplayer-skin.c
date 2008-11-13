/* vi: set sw=4 ts=4: */
/*
 * lmplayer-skin.c
 *
 * This file is part of ________.
 *
 * Copyright (C) 2008 - kelvenxu <kelvenxu@gmail.com>.
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

#include "lmplayer-skin.h"
#include <glib.h>

char *lmplayer_skin_get_full_path(const char *name)
{
	char *filename;

#ifdef LMPLAYER_RUN_IN_SOURCE_TREE
	/* Try the file in the source tree first */
	filename = g_build_filename("..", "data", name, NULL);
	if(g_file_test(filename, G_FILE_TEST_EXISTS) == FALSE)
	{
		g_free(filename);
		/* Try the local file */
		filename = g_build_filename(DATADIR, "lmplayer", name, NULL);

		if(g_file_test(filename, G_FILE_TEST_EXISTS) == FALSE)
		{
			g_free (filename);
			return NULL;
		}
	}
#else
	const char *home = g_getenv("HOME");
	filename = g_build_filename(home, ".lmplayer/skins", name, NULL);

	if(g_file_test(filename, G_FILE_TEST_EXISTS) == FALSE)
	{
		g_free(filename);
		filename = g_build_filename(DATADIR, "lmplayer/skins", name, NULL);
		if(g_file_test(filename, G_FILE_TEST_EXISTS) == FALSE)
		{
			g_free (filename);
			return NULL;
		}
	}
#endif

	return filename;
}

