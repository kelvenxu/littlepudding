/* lmplayer-options.c

   Copyright (C) 2004 Bastien Nocera

   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: Bastien Nocera <hadess@hadess.net>
 */

#include "config.h"

#include <glib/gi18n.h>
#include <string.h>
#include <stdlib.h>

#include "lmplayer-options.h"
#include "lmplayer-uri.h"
#include "lmplayer-debug.h"
#include "lmplayer-private.h"

LmpCmdLineOptions optionstate;	/* Decoded command line options */

const GOptionEntry options[] = {
	{"debug", '\0', 0, G_OPTION_ARG_NONE, &optionstate.debug, N_("Enable debug"), NULL},
	{"play-pause", '\0', 0, G_OPTION_ARG_NONE, &optionstate.playpause, N_("Play/Pause"), NULL},
	{"play", '\0', 0, G_OPTION_ARG_NONE, &optionstate.play, N_("Play"), NULL},
	{"pause", '\0', 0, G_OPTION_ARG_NONE, &optionstate.pause, N_("Pause"), NULL},
	{"next", '\0', 0, G_OPTION_ARG_NONE, &optionstate.next, N_("Next"), NULL},
	{"previous", '\0', 0, G_OPTION_ARG_NONE, &optionstate.previous, N_("Previous"), NULL},
	{"seek-fwd", '\0', 0, G_OPTION_ARG_NONE, &optionstate.seekfwd, N_("Seek Forwards"), NULL},
	{"seek-bwd", '\0', 0, G_OPTION_ARG_NONE, &optionstate.seekbwd, N_("Seek Backwards"), NULL},
	{"volume-up", '\0', 0, G_OPTION_ARG_NONE, &optionstate.volumeup, N_("Volume Up"), NULL},
	{"volume-down", '\0', 0, G_OPTION_ARG_NONE, &optionstate.volumedown, N_("Volume Down"), NULL},
	{"quit", '\0', 0, G_OPTION_ARG_NONE, &optionstate.quit, N_("Quit"), NULL},
	{"enqueue", '\0', 0, G_OPTION_ARG_NONE, &optionstate.enqueue, N_("Enqueue"), NULL},
	{"replace", '\0', 0, G_OPTION_ARG_NONE, &optionstate.replace, N_("Replace"), NULL},
	{"no-existing-session", '\0', 0, G_OPTION_ARG_NONE, &optionstate.notconnectexistingsession, N_("Don't connect to an already running instance"), NULL},
	{"printplaying", '\0', 0, G_OPTION_ARG_NONE, &optionstate.printplaying, N_("Print playing audio"), NULL},
	{"seek", '\0', G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_INT64, &optionstate.seek, N_("Seek"), NULL},
	{"playlist-idx", '\0', G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_DOUBLE, &optionstate.playlistidx, N_("Playlist index"), NULL},
	{G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_FILENAME_ARRAY, &optionstate.filenames, N_("Audio file to play"), NULL},
	{NULL} /* end the list */
};

void
lmplayer_options_process_late (LmplayerObject *lmplayper, const LmpCmdLineOptions* options)
{
	lmplayer_debug(" ");
	/* Handle --playlist-idx */
	lmplayper->index = options->playlistidx;

	lmplayer_debug(" ");
	/* Handle --seek */
	lmplayper->seek_to = options->seek;
}

void
lmplayer_options_process_early (LmplayerObject *lmp, const LmpCmdLineOptions* options)
{
	if (options->quit) 
	{
		/* If --quit is one of the commands, just quit */
		//gdk_notify_startup_complete ();
		exit (0);
	}
}

void
lmplayer_options_register_remote_commands(LmplayerObject *lmplayer)
{
	GEnumClass *klass;
	guint i;

	klass = (GEnumClass *) g_type_class_ref(LMPLAYER_TYPE_REMOTE_COMMAND);
	for (i = LMPLAYER_REMOTE_COMMAND_UNKNOWN + 1; i < klass->n_values; i++) 
	{
		GEnumValue *val;
		val = g_enum_get_value(klass, i);
		unique_app_add_command(lmplayer->uapp, val->value_name, i);
	}
	g_type_class_unref (klass);
}

static void
lmplayer_print_playing_cb (const gchar *msg, gpointer user_data)
{
	//if (strcmp (msg, SHOW_PLAYING_NO_TRACKS) != 0)
	//	g_print ("%s\n", msg);
	exit (0);
}

static char *
lmplayer_option_create_line (int command)
{
	return g_strdup_printf ("%03d ", command);
}

void
lmplayer_options_process_for_server(UniqueApp *app,
		const LmpCmdLineOptions* options)
{
	GList *commands, *l;
	int default_action, i;

	commands = NULL;
	default_action = LMPLAYER_REMOTE_COMMAND_REPLACE;

#if 0
	/* We can only handle "printplaying" on its own */
	if (options->printplaying)
	{
		char *line;
		GMainLoop *loop = g_main_loop_new (NULL, FALSE);

		line = lmplayer_option_create_line (LMPLAYER_REMOTE_COMMAND_SHOW_PLAYING);
		bacon_message_connection_set_callback (conn,
				lmplayer_print_playing_cb, loop);
		//bacon_message_connection_send (conn, line);
		unique_app_send_message(app, line, NULL);
		g_free (line);

		g_main_loop_run (loop);
		return;
	}
#endif

	/* Are we quitting ? */
	if (options->quit) {
		char *line;
		//line = lmplayer_option_create_line (LMPLAYER_REMOTE_COMMAND_QUIT);
		unique_app_send_message (app, LMPLAYER_REMOTE_COMMAND_QUIT, NULL);
		//bacon_message_connection_send (conn, line);
		//unique_app_send_message(app, line, NULL);
		g_free (line);
		return;
	}

	/* Then handle the things that modify the playlist */
	if (options->replace && options->enqueue) {
		/* FIXME translate that */
		g_warning ("Can't enqueue and replace at the same time");
	} else if (options->replace) {
		default_action = LMPLAYER_REMOTE_COMMAND_REPLACE;
	} else if (options->enqueue) {
		default_action = LMPLAYER_REMOTE_COMMAND_ENQUEUE;
	}

	/* Send the files to enqueue */
	for (i = 0; options->filenames && options->filenames[i] != NULL; i++)
	{
		UniqueMessageData *data;
		//char *line; 
		char *full_path;

		data = unique_message_data_new();
		full_path = lmplayer_create_full_path(options->filenames[i]);
		unique_message_data_set_text(data, full_path ? full_path : options->filenames[i], -1);
		full_path = lmplayer_create_full_path(options->filenames[i]);
		//bacon_message_connection_send (conn, line);
		unique_app_send_message(app, default_action, data);
		/* Even if the default action is replace, we only want to replace with the
		   first file.  After that, we enqueue. */
		default_action = LMPLAYER_REMOTE_COMMAND_ENQUEUE;
		g_free (full_path);
	}

	if (options->playpause) {
		commands = g_list_append (commands, lmplayer_option_create_line
					  (LMPLAYER_REMOTE_COMMAND_PLAYPAUSE));
	}

	if (options->play) {
		commands = g_list_append (commands, lmplayer_option_create_line
					  (LMPLAYER_REMOTE_COMMAND_PLAY));
	}

	if (options->pause) {
		commands = g_list_append (commands, lmplayer_option_create_line
					  (LMPLAYER_REMOTE_COMMAND_PAUSE));
	}

	if (options->next) {
		commands = g_list_append (commands, lmplayer_option_create_line
					  (LMPLAYER_REMOTE_COMMAND_NEXT));
	}

	if (options->previous) {
		commands = g_list_append (commands, lmplayer_option_create_line
					  (LMPLAYER_REMOTE_COMMAND_PREVIOUS));
	}

	if (options->seekfwd) {
		commands = g_list_append (commands, lmplayer_option_create_line
					  (LMPLAYER_REMOTE_COMMAND_SEEK_FORWARD));
	}

	if (options->seekbwd) {
		commands = g_list_append (commands, lmplayer_option_create_line
					  (LMPLAYER_REMOTE_COMMAND_SEEK_BACKWARD));
	}

	if (options->volumeup) {
		commands = g_list_append (commands, lmplayer_option_create_line
					  (LMPLAYER_REMOTE_COMMAND_VOLUME_UP));
	}

	if (options->volumedown) {
		commands = g_list_append (commands, lmplayer_option_create_line
					  (LMPLAYER_REMOTE_COMMAND_VOLUME_DOWN));
	}

	/* No commands, no files, show ourselves */
	if (commands == NULL && options->filenames == NULL) {
		unique_app_send_message (app, LMPLAYER_REMOTE_COMMAND_SHOW, NULL);
		return;
	}

	/* Send commands */
	for (l = commands; l != NULL; l = l->next) {
		int command = GPOINTER_TO_INT (l->data);
		unique_app_send_message (app, command, NULL);
	}
	g_list_free (commands);
}

