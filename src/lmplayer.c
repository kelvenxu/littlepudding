/* vi: set sw=4 ts=4: */
/*
 * lmplayer.c
 *
 * This file is part of lmplayer.
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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <totem-disc.h>
#include "lmplayer.h"
#include "lmplayer-options.h"
#include "lmplayer-interface.h"
#include "lmplayer-menu.h"
#include "lmplayer-uri.h"
#include "lmplayer-dvb-setup.h"
#include "lmplayer-encode.h"
#include "lmplayer-utils.h"
#include "lmplayer-debug.h"
//#include "lmplayer-statusbar.h"
#include "lmplayer-skin.h"

#define REWIND_OR_PREVIOUS 4000

#define SEEK_FORWARD_SHORT_OFFSET 15
#define SEEK_BACKWARD_SHORT_OFFSET -5

#define SEEK_FORWARD_LONG_OFFSET 10*60
#define SEEK_BACKWARD_LONG_OFFSET -3*60

#define VOLUME_EPSILON (1e-10)

static void video_widget_create (LmplayerObject *lmplayer);
static void lmplayer_message_connection_receive_cb (const char *msg, LmplayerObject *lmp);
static void lmplayer_action_remote (LmplayerObject *lmp, LmpRemoteCommand cmd, const char *url);
static gboolean lmplayer_action_open_files (LmplayerObject *lmplayer, char **list);
static gboolean lmplayer_action_open_dialog (LmplayerObject *lmplayer, const char *path, gboolean play);
static gboolean lmplayer_action_open_files_list (LmplayerObject *lmplayer, GSList *list);

gboolean seek_slider_pressed_cb (GtkWidget *widget, GdkEventButton *event, LmplayerObject *lmplayer);
void seek_slider_changed_cb (GtkAdjustment *adj, LmplayerObject *lmplayer);
gboolean seek_slider_released_cb(GtkWidget *widget, GdkEventButton *event, LmplayerObject *lmplayer);
static void lmplayer_volume_value_changed_cb(SkinHScale *hscale, LmplayerObject *lmplayer);

static void
reset_seek_status (LmplayerObject *lmplayer)
{
	/* Release the lock and reset everything so that we
	 * avoid being "stuck" seeking on errors */

	if (lmplayer->seek_lock != FALSE) 
	{
		//lmplayer_statusbar_set_seeking (LMPLAYER_STATUSBAR (lmplayer->statusbar), FALSE);
		//lmplayer_time_label_set_seeking (TOTEM_TIME_LABEL (totem->fs->time_label), FALSE);
		lmplayer->seek_lock = FALSE;
		bacon_video_widget_seek (lmplayer->bvw, 0, NULL);
		lmplayer_action_stop (lmplayer);
	}
}

/*
static void
lmplayer_action_save_state (LmplayerObject *lmplayer, const char *page_id)
{
	GKeyFile *keyfile;
	char *contents, *filename;

	//if (lmplayer->win == NULL)
	//	return;

	keyfile = g_key_file_new ();

	contents = g_key_file_to_data (keyfile, NULL, NULL);
	g_key_file_free (keyfile);
	filename = g_build_filename (lmplayer_dot_dir (), "state.ini", NULL);
	g_file_set_contents (filename, contents, -1, NULL);

	g_free (filename);
	g_free (contents);
}
*/

static void
lmplayer_action_wait_force_exit (gpointer user_data)
{
	g_usleep (5 * G_USEC_PER_SEC);
	exit (1);
}

/*
static void
lmplayer_dvb_setup_result (int result, const char *device, gpointer user_data)
{
	LmplayerObject *lmplayer = (LmplayerObject*) user_data;

	lmplayer_action_play_media (lmplayer, MEDIA_TYPE_DVB, device);
}
*/

static gboolean
lmplayer_action_load_media (LmplayerObject *lmplayer, TotemDiscMediaType type, const char *device)
{
	char **mrls, *msg;
	GError *error = NULL;
	const char *link, *link_text, *secondary;
	gboolean retval;

	mrls = bacon_video_widget_get_mrls (lmplayer->bvw, type, device, &error);
	if (mrls == NULL) {
		/* No errors? Weird */
		if (error == NULL) {
			msg = g_strdup_printf (_("Lmplayer could not play this media (%s) although a plugin is present to handle it."), _(totem_cd_get_human_readable_name (type)));
			lmplayer_action_error (msg, _("You might want to check that a disc is present in the drive and that it is correctly configured."), lmplayer);
			g_free (msg);
			return FALSE;
		}
		/* No plugin for the media type */
		if (g_error_matches (error, BVW_ERROR, BVW_ERROR_NO_PLUGIN_FOR_FILE) != FALSE) {
			link = "http://www.gnome.org/projects/lmplayer/#codecs";
			link_text = _("More information about media plugins");
			secondary = _("Please install the necessary plugins and restart Lmplayer to be able to play this media.");
			if (type == MEDIA_TYPE_DVD || type == MEDIA_TYPE_VCD)
				msg = g_strdup_printf (_("Lmplayer cannot play this type of media (%s) because it does not have the appropriate plugins to be able to read from the disc."), _(totem_cd_get_human_readable_name (type)));
			else
				msg = g_strdup_printf (_("Lmplayer cannot play this type of media (%s) because you do not have the appropriate plugins to handle it."), _(totem_cd_get_human_readable_name (type)));
		/* Device doesn't exist */
		} else if (g_error_matches (error, BVW_ERROR, BVW_ERROR_INVALID_DEVICE) != FALSE) {
			g_assert (type == MEDIA_TYPE_DVB);
			msg = N_("Lmplayer cannot play TV, because no TV adapters are present or they are not supported.");
			lmplayer_action_error (_(msg), _("Please insert a supported TV adapter."), lmplayer);
			return FALSE;
		/* No channels.conf file */
		} else if (g_error_matches (error, BVW_ERROR, BVW_ERROR_FILE_NOT_FOUND) != FALSE) {
			g_assert (type == MEDIA_TYPE_DVB);

			//if (lmplayer_dvb_setup_device (device, GTK_WINDOW (lmplayer->win), lmplayer_dvb_setup_result, lmplayer) == LMPLAYER_DVB_SETUP_STARTED_OK)
			//	return FALSE;

			link = "http://www.gnome.org/projects/lmplayer/#dvb";
			link_text = _("More information about watching TV");
			msg = g_strdup (_("Lmplayer is missing a channels listing to be able to tune the receiver."));
			secondary = _("Please follow the instructions provided in the link to create a channels listing.");
		} else if (g_error_matches (error, BVW_ERROR, BVW_ERROR_DEVICE_BUSY) != FALSE) {
			g_assert (type == MEDIA_TYPE_DVB);
			msg = g_strdup_printf(_("Lmplayer cannot play this type of media (%s) because the TV device is busy."), _(totem_cd_get_human_readable_name (type)));
			lmplayer_action_error (msg, _("Please try again later."), lmplayer);
			g_free (msg);
			return FALSE;
		/* Unsupported type (ie. CDDA) */
		} else if (g_error_matches (error, BVW_ERROR, BVW_ERROR_UNVALID_LOCATION) != FALSE) {
			msg = g_strdup_printf(_("Lmplayer cannot play this type of media (%s) because it is not supported."), _(totem_cd_get_human_readable_name (type)));
			lmplayer_action_error (msg, _("Please insert another disc to play back."), lmplayer);
			g_free (msg);
			return FALSE;
		} else {
			g_assert_not_reached ();
		}
		//lmplayer_interface_error_with_link (msg, secondary, link, link_text, GTK_WINDOW (lmplayer->win), lmplayer);
		g_free (msg);
		return FALSE;
	}

	retval = lmplayer_action_open_files (lmplayer, mrls);
	g_strfreev (mrls);

	return retval;
}

static gboolean
lmplayer_action_load_media_device (LmplayerObject *lmplayer, const char *device)
{
	TotemDiscMediaType type;
	GError *error = NULL;
	char *device_path, *url;
	gboolean retval;

	if (g_str_has_prefix (device, "file://") != FALSE)
		device_path = g_filename_from_uri (device, NULL, NULL);
	else
		device_path = g_strdup (device);

	type = totem_cd_detect_type_with_url (device_path, &url, &error);

	switch (type) {
		case MEDIA_TYPE_ERROR:
			lmplayer_action_error (_("Totem was not able to play this disc."),
					    error ? error->message : _("No reason."),
					    lmplayer);
			retval = FALSE;
			break;
		case MEDIA_TYPE_DATA:
			/* Set default location to the mountpoint of
			 * this device */
			retval = lmplayer_action_open_dialog (lmplayer, url, FALSE);
			break;
		case MEDIA_TYPE_DVD:
		case MEDIA_TYPE_VCD:
			retval = lmplayer_action_load_media (lmplayer, type, device_path);
			break;
		case MEDIA_TYPE_CDDA:
			lmplayer_action_error (_("Lmplayer does not support playback of Audio CDs"),
					    _("Please consider using a music player or a CD extractor to play this CD"),
					    lmplayer);
			retval = FALSE;
			break;
		default:
			g_assert_not_reached ();
	}

	g_free (url);
	g_free (device_path);

	return retval;
}

static void 
lmplayer_statusbar_set_text(LmplayerObject *lmplayer, gchar *text)
{
	SkinStatusBar *sb;
	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	sb = (SkinStatusBar*)skin_builder_get_object(lmplayer->builder, "player-statusbar");

	skin_status_bar_set_text(sb, text);
}

static void 
lmplayer_info_update(LmplayerObject *lmplayer)
{
	SkinDynamicText *info;
	gchar *title, *artist, *album, *duration;
	gchar *title_utf8 = NULL;
	gchar *artist_utf8 = NULL;
	gchar *album_utf8 = NULL;
	int length;
	GValue value = { 0, };

	bacon_video_widget_get_metadata(lmplayer->bvw, BVW_INFO_TITLE, &value);
	title = g_value_dup_string (&value);
	g_value_unset (&value);
	if(title)
	{
		title_utf8 = lmplayer_encode_convert_to_utf8(title);
		g_free (title);
	}
	title = g_strdup_printf (_("Title: %s"), title_utf8);
	g_free (title_utf8);

	bacon_video_widget_get_metadata(lmplayer->bvw, BVW_INFO_ARTIST, &value);
	artist = g_value_dup_string(&value);
	g_value_unset (&value);

	if(artist)
	{
		artist_utf8 = lmplayer_encode_convert_to_utf8(artist);
		g_free (artist);
	}
	artist = g_strdup_printf(_("Artist: %s"), artist_utf8);
	g_free (artist_utf8);

	bacon_video_widget_get_metadata(lmplayer->bvw, BVW_INFO_ALBUM, &value);
	album = g_value_dup_string(&value);
	g_value_unset (&value);
	if(album)
	{
		album_utf8 = lmplayer_encode_convert_to_utf8(album);
		g_free (album);
	}
	album = g_strdup_printf(_("Album: %s"), album_utf8);
	g_free (album_utf8);


	bacon_video_widget_get_metadata(lmplayer->bvw, BVW_INFO_DURATION, &value);
	length = g_value_get_int(&value);
	g_value_unset(&value);

	duration = g_strdup_printf(_("Length: %02d:%02d"), length / 60, length % 60);

	info = (SkinDynamicText *)skin_builder_get_object(lmplayer->builder, "player-info");
	g_object_set(G_OBJECT(info), 
			"title", title,
			"artist", artist,
			"album", album,
			"format", duration,
			NULL);

	info = (SkinDynamicText *)skin_builder_get_object(lmplayer->builder, "mini-info");
	g_object_set(G_OBJECT(info), 
			"title", title,
			"artist", artist,
			"album", album,
			"format", duration,
			NULL);

	g_free (artist);
	g_free (title);
	g_free (album);
	g_free (duration);
}

static void
play_pause_set_label(LmplayerObject *lmplayer, LmplayerStates state)
{
	SkinButton *play, *stop, *pause;

	lmplayer_debug(" ");
	if(lmplayer->minimode)
	{
		play = (SkinButton*)skin_builder_get_object(lmplayer->builder, "mini-play");
		stop = (SkinButton*)skin_builder_get_object(lmplayer->builder, "mini-stop");
		pause = (SkinButton*)skin_builder_get_object(lmplayer->builder, "mini-pause");
	}
	else
	{
		play = (SkinButton*)skin_builder_get_object(lmplayer->builder, "player-play");
		stop = (SkinButton*)skin_builder_get_object(lmplayer->builder, "player-stop");
		pause = (SkinButton*)skin_builder_get_object(lmplayer->builder, "player-pause");
	}

	//if (state == lmplayer->state)
	//	return;

	switch (state)
	{
	case STATE_PLAYING:
		lmplayer_debug("playing");
		lmplayer_statusbar_set_text(lmplayer, _("Status: Playing"));
		lmplayer_playlist_set_playing(lmplayer->playlist, LMPLAYER_PLAYLIST_STATUS_PLAYING);
		skin_button_hide(play);
		skin_button_show(pause);
		skin_button_set_sensitive(stop, TRUE);
		break;
	case STATE_PAUSED:
		lmplayer_statusbar_set_text(lmplayer, _("Status: Paused"));
		lmplayer_playlist_set_playing(lmplayer->playlist, LMPLAYER_PLAYLIST_STATUS_PAUSED);
		skin_button_hide(pause);
		skin_button_show(play);
		skin_button_set_sensitive(stop, TRUE);
		break;
	case STATE_STOPPED:
		lmplayer_statusbar_set_text(lmplayer, _("Status: Stopped"));
		lmplayer_playlist_set_playing(lmplayer->playlist, LMPLAYER_PLAYLIST_STATUS_NONE);
		skin_button_hide(pause);
		skin_button_show(play);
		skin_button_set_sensitive(stop, FALSE);
		break;
	default:
		g_assert_not_reached();
		return;
	}

	lmplayer->state = state;

	lmplayer_debug(" ");
	g_object_notify(G_OBJECT(lmplayer), "playing");
}

void
lmplayer_action_play (LmplayerObject *lmplayer)
{
	GError *err = NULL;
	int retval;
	char *msg, *disp;

	if (lmplayer->mrl == NULL)
		return;

	if (bacon_video_widget_is_playing (lmplayer->bvw) != FALSE)
		return;

	
	retval = bacon_video_widget_play (lmplayer->bvw,  &err);
	play_pause_set_label(lmplayer, retval ? STATE_PLAYING : STATE_STOPPED);

	if (retval != FALSE)
		return;

	disp = lmplayer_uri_escape_for_display (lmplayer->mrl);
	msg = g_strdup_printf(_("LmplayerObject could not play '%s'."), disp);
	g_free (disp);

	lmplayer_action_error (msg, err->message, lmplayer);
	lmplayer_action_stop (lmplayer);
	g_free (msg);
	g_error_free (err);
}

void
lmplayer_action_play_media (LmplayerObject *lmplayer, TotemDiscMediaType type, const char *device)
{
	char *mrl;

	if (lmplayer_action_load_media (lmplayer, type, device) != FALSE) 
	{
		mrl = lmplayer_playlist_get_current_mrl (lmplayer->playlist, NULL);
		lmplayer_action_set_mrl_and_play (lmplayer, mrl, NULL);
		g_free (mrl);
	}
}

void
lmplayer_action_seek (LmplayerObject *lmplayer, double pos)
{
	GError *err = NULL;
	int retval;

	if (lmplayer->mrl == NULL)
		return;
	if (bacon_video_widget_is_seekable (lmplayer->bvw) == FALSE)
		return;

	retval = bacon_video_widget_seek (lmplayer->bvw, pos, &err);

	if (retval == FALSE)
	{
		char *msg, *disp;

		disp = lmplayer_uri_escape_for_display (lmplayer->mrl);
		msg = g_strdup_printf(_("Linux Music Player could not play '%s'."), disp);
		g_free (disp);

		reset_seek_status (lmplayer);

		lmplayer_action_error (msg, err->message, lmplayer);
		g_free (msg);
		g_error_free (err);
	}
}

/**
 * lmplayer_action_stop:
 * @lmplayer: a #LmplayerObjectObject
 *
 * Stops the current stream.
 **/
void
lmplayer_action_stop (LmplayerObject *lmplayer)
{
	bacon_video_widget_stop (lmplayer->bvw);
	play_pause_set_label (lmplayer, STATE_STOPPED);
}

/**
 * lmplayer_action_play_pause:
 * @lmplayer: a #LmplayerObjectObject
 *
 * Gets the current MRL from the playlist and attempts to play it.
 * If the stream is already playing, playback is paused.
 **/
void
lmplayer_action_play_pause (LmplayerObject *lmplayer)
{
	if (lmplayer->mrl == NULL)
	{
		char *mrl, *subtitle;

		/* Try to pull an mrl from the playlist */
		mrl = lmplayer_playlist_get_current_mrl (lmplayer->playlist, &subtitle);
		if (mrl == NULL) 
		{
			play_pause_set_label (lmplayer, STATE_STOPPED);
			return;
		} 
		else 
		{
			lmplayer_action_set_mrl_and_play (lmplayer, mrl, subtitle);
			g_free (mrl);
			g_free (subtitle);
			return;
		}
	}

	if (bacon_video_widget_is_playing (lmplayer->bvw) == FALSE)
	{
		bacon_video_widget_play (lmplayer->bvw, NULL);
		play_pause_set_label (lmplayer, STATE_PLAYING);
	} 
	else 
	{
		bacon_video_widget_pause (lmplayer->bvw);
		play_pause_set_label (lmplayer, STATE_PAUSED);
	}
}

/**
 * lmplayer_action_pause:
 * @lmplayer: a #LmplayerObjectObject
 *
 * Pauses the current stream. If LmplayerObject is already paused, it continues
 * to be paused.
 **/
void
lmplayer_action_pause(LmplayerObject *lmplayer)
{
	if (bacon_video_widget_is_playing(lmplayer->bvw) != FALSE) 
	{
		bacon_video_widget_pause(lmplayer->bvw);
		play_pause_set_label(lmplayer, STATE_PAUSED);
	}
}

void
lmplayer_action_exit(LmplayerObject *lmplayer)
{
	//GdkDisplay *display = NULL;

	lmplayer_debug(" ");
	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	if(lmplayer->playlist && lmplayer->pls)
	{
		lmplayer_playlist_save_current_playlist(lmplayer->playlist, lmplayer->pls);
		g_free(lmplayer->pls);
	}

	/* Exit forcefully if we can't do the shutdown in 10 seconds */
	g_thread_create ((GThreadFunc) lmplayer_action_wait_force_exit,
			 NULL, FALSE, NULL);
	
	/*
	if (gtk_main_level () > 0)
	{
		lmplayer_debug(" gtk main quit");
		gtk_main_quit ();
	}

	if (lmplayer == NULL)
		exit (0);

	if (lmplayer->pl_win != NULL) 
	{
		gtk_widget_hide(GTK_WIDGET(lmplayer->pl_win));
		display = gtk_widget_get_display(GTK_WIDGET(lmplayer->pl_win));
	}

	if (display != NULL)
		gdk_display_sync (display);

	if (lmplayer->lyric_win != NULL) 
	{
		gtk_widget_hide(GTK_WIDGET(lmplayer->lyric_win));
		display = gtk_widget_get_display(GTK_WIDGET(lmplayer->lyric_win));
	}

	if (display != NULL)
		gdk_display_sync (display);

	if (lmplayer->eq_win != NULL) 
	{
		gtk_widget_hide(GTK_WIDGET(lmplayer->eq_win));
		display = gtk_widget_get_display(GTK_WIDGET(lmplayer->eq_win));
	}

	if (display != NULL)
		gdk_display_sync (display);

	if (lmplayer->mini_win != NULL) 
	{
		gtk_widget_hide(GTK_WIDGET(lmplayer->mini_win));
		display = gtk_widget_get_display(GTK_WIDGET(lmplayer->mini_win));
	}

	if (display != NULL)
		gdk_display_sync (display);

	if (lmplayer->win != NULL) 
	{
		gtk_widget_hide(GTK_WIDGET(lmplayer->win));
		display = gtk_widget_get_display(GTK_WIDGET(lmplayer->win));
	}

	if (display != NULL)
		gdk_display_sync (display);
	*/

	if (lmplayer->bvw) 
	{
		int vol;
		lmplayer_action_stop (lmplayer);

		vol = bacon_video_widget_get_volume (lmplayer->bvw) * 100.0 + 0.5;

		/* FIXME move the volume to the static file? */
		gconf_client_add_dir (lmplayer->gc, GCONF_PREFIX,
				GCONF_CLIENT_PRELOAD_ONELEVEL, NULL);
		gconf_client_set_int (lmplayer->gc,
				GCONF_PREFIX"/volume",
				CLAMP (vol, 0, 100),
				NULL);
	}

	if (lmplayer->conn != NULL)
		bacon_message_connection_free (lmplayer->conn);

	//lmplayer_sublang_exit (lmplayer);
	lmplayer_destroy_file_filters ();

	if (lmplayer->gc)
		g_object_unref (G_OBJECT (lmplayer->gc));

	/*
	if (lmplayer->pl_win)
		gtk_widget_destroy (GTK_WIDGET (lmplayer->pl_win));

	if (lmplayer->lyric_win)
		gtk_widget_destroy (GTK_WIDGET (lmplayer->lyric_win));

	lmplayer_debug(" ");
	if (lmplayer->eq_win)
		gtk_widget_destroy (GTK_WIDGET (lmplayer->eq_win));

	lmplayer_debug(" ");
	if (lmplayer->mini_win)
		gtk_widget_destroy (GTK_WIDGET (lmplayer->mini_win));

	lmplayer_debug(" ");
	if (lmplayer->win)
	{
		//FIXME:
		gtk_widget_destroy (GTK_WIDGET (lmplayer->win));
		//gtk_main_quit ();
	}
	*/

	if (gtk_main_level () > 0)
		gtk_main_quit();

	g_object_unref (lmplayer);
	exit (0);
}

void
lmplayer_action_error (const char *title, const char *reason, LmplayerObject *lmplayer)
{
}

void 
lmplayer_action_minimize(LmplayerObject *lmplayer)
{
	gtk_window_iconify(GTK_WINDOW(lmplayer->pl_win));
	gtk_window_iconify(GTK_WINDOW(lmplayer->lyric_win));
	gtk_window_iconify(GTK_WINDOW(lmplayer->eq_win));
	gtk_window_iconify(GTK_WINDOW(lmplayer->mini_win));
	gtk_window_iconify(GTK_WINDOW(lmplayer->win));
}

void 
lmplayer_action_minimode(LmplayerObject *lmplayer, gboolean minimode)
{
	SkinCheckButton *button;

	lmplayer->minimode = minimode;

	if(minimode)
	{
		gtk_widget_hide(GTK_WIDGET(lmplayer->pl_win));
		gtk_widget_hide(GTK_WIDGET(lmplayer->lyric_win));
		gtk_widget_hide(GTK_WIDGET(lmplayer->eq_win));
		gtk_widget_hide(GTK_WIDGET(lmplayer->win));
		gtk_widget_show_all(GTK_WIDGET(lmplayer->mini_win));
	}
	else
	{
		gtk_widget_hide(GTK_WIDGET(lmplayer->mini_win));
		gtk_widget_show_all(GTK_WIDGET(lmplayer->win));

		button = (SkinCheckButton *)skin_builder_get_object(lmplayer->builder, "player-playlist");
		if(skin_check_button_get_active(button))
			gtk_widget_show_all(GTK_WIDGET(lmplayer->pl_win));

		button = (SkinCheckButton *)skin_builder_get_object(lmplayer->builder, "player-lyric");
		if(skin_check_button_get_active(button))
			gtk_widget_show_all(GTK_WIDGET(lmplayer->lyric_win));

		button = (SkinCheckButton *)skin_builder_get_object(lmplayer->builder, "player-equalizer");
		if(skin_check_button_get_active(button))
			gtk_widget_show_all(GTK_WIDGET(lmplayer->eq_win));
	}

	lmplayer_debug("play_pause_set_label");
	play_pause_set_label(lmplayer, lmplayer->state);
}

static void
update_mrl_label (LmplayerObject *lmplayer, const char *name)
{
	if (name != NULL)
	{
		/* Update the mrl label */
		//totem_fullscreen_set_title (totem->fs, name);

		/* Title */
		gtk_window_set_title(GTK_WINDOW(lmplayer->win), name);
	} 
	else 
	{
		//lmplayer_statusbar_set_time_and_length(LMPLAYER_STATUSBAR
		//		(lmplayer->statusbar), 0, 0);
		lmplayer_statusbar_set_text(lmplayer, _("Stopped"));

		g_object_notify(G_OBJECT(lmplayer), "stream-length");

		/* Update the mrl label */
		//totem_fullscreen_set_title (totem->fs, NULL);

		/* Title */
		gtk_window_set_title(GTK_WINDOW(lmplayer->win), _("Linux Music Player"));
	}
}

gboolean
lmplayer_action_set_mrl_with_warning (LmplayerObject *lmplayer,
				   const char *mrl, 
				   const char *subtitle,
				   gboolean warn)
{
	gboolean retval = TRUE;

	lmplayer_debug(" ");
	if (lmplayer->mrl != NULL)
	{
		lmplayer_debug(" ");
		g_free (lmplayer->mrl);
		lmplayer->mrl = NULL;
		bacon_video_widget_close (lmplayer->bvw);
		lmplayer_file_closed(lmplayer);
		play_pause_set_label(lmplayer, LMPLAYER_PLAYLIST_STATUS_NONE);
	}

	if (mrl == NULL)
	{
		lmplayer_debug(" ");

		retval = FALSE;

		play_pause_set_label (lmplayer, LMPLAYER_PLAYLIST_STATUS_NONE);

		/* Play/Pause */
		//lmplayer_action_set_sensitivity ("play", FALSE);

		/* Volume */
		//lmplayer_main_set_sensitivity ("tmw_volume_button", FALSE);
		//lmplayer_action_set_sensitivity ("volume-up", FALSE);
		//lmplayer_action_set_sensitivity ("volume-down", FALSE);
		//lmplayer->volume_sensitive = FALSE;

		/* Control popup */
		//lmplayer_fullscreen_set_can_set_volume (lmplayer->fs, FALSE);
		//lmplayer_fullscreen_set_seekable (lmplayer->fs, FALSE);
		//lmplayer_action_set_sensitivity ("next-chapter", FALSE);
		//lmplayer_action_set_sensitivity ("previous-chapter", FALSE);

		/* Take a screenshot */
		//lmplayer_action_set_sensitivity ("take-screenshot", FALSE);

		/* Clear the playlist */
		//lmplayer_action_set_sensitivity ("clear-playlist", FALSE);

		/* Set the logo */
		//bacon_video_widget_set_logo_mode (lmplayer->bvw, TRUE);
		update_mrl_label (lmplayer, NULL);
	} 
	else 
	{
		gboolean caps;
		gdouble volume;
		char *autoload_sub = NULL;
		GError *err = NULL;

		//bacon_video_widget_set_logo_mode (lmplayer->bvw, FALSE);

		if (subtitle == NULL && lmplayer->autoload_subs != FALSE)
			autoload_sub = lmplayer_uri_get_subtitle_uri (mrl);

		lmplayer_debug("bacon_video_widget_open_with_subtitle: mrl=%s", mrl);
		lmplayer_window_set_waiting_cursor (GTK_WIDGET(lmplayer->win)->window);
		retval = bacon_video_widget_open_with_subtitle (lmplayer->bvw, mrl,
								subtitle ? subtitle : autoload_sub, &err);
		g_free (autoload_sub);
		gdk_window_set_cursor (GTK_WIDGET(lmplayer->win)->window, NULL);
		lmplayer->mrl = g_strdup (mrl);

		/* Play/Pause */
		//lmplayer_action_set_sensitivity ("play", TRUE);

		/* Volume */
		caps = bacon_video_widget_can_set_volume (lmplayer->bvw);
		//lmplayer_main_set_sensitivity ("tmw_volume_button", caps);
		//lmplayer_fullscreen_set_can_set_volume (lmplayer->fs, caps);
		volume = bacon_video_widget_get_volume (lmplayer->bvw);
		//lmplayer_action_set_sensitivity ("volume-up", caps && volume < (1.0 - VOLUME_EPSILON));
		//lmplayer_action_set_sensitivity ("volume-down", caps && volume > VOLUME_EPSILON);
		//lmplayer->volume_sensitive = caps;

		/* Take a screenshot */
		//lmplayer_action_set_sensitivity ("take-screenshot", retval);

		/* Clear the playlist */
		//lmplayer_action_set_sensitivity ("clear-playlist", retval);

		/* Set the playlist */
		play_pause_set_label(lmplayer, retval ? STATE_PAUSED : STATE_STOPPED);

		lmplayer_debug(" ");
		if (retval == FALSE && warn != FALSE)
		{
			char *msg, *disp;

			lmplayer_debug(" ");
			lmplayer_debug(" ");
			disp = lmplayer_uri_escape_for_display (lmplayer->mrl);
			msg = g_strdup_printf(_("Lmplayer could not play '%s'."), disp);
			g_free (disp);
			if (err && err->message) 
			{
				lmplayer_action_error (msg, err->message, lmplayer);
			}
			else 
			{
				lmplayer_action_error (msg, _("No error message"), lmplayer);
			}
			g_free (msg);
		}

		if (retval == FALSE)
		{
			lmplayer_debug(" ");
			if (err)
				g_error_free (err);
			g_free (lmplayer->mrl);
			lmplayer->mrl = NULL;
			//bacon_video_widget_set_logo_mode (lmplayer->bvw, TRUE);
		} 
		else 
		{
			lmplayer_debug(" ");
			lmplayer_file_opened (lmplayer, lmplayer->mrl);
			lmplayer_debug(" ");
		}
	}
	//update_buttons (lmplayer);
	//update_media_menu_items (lmplayer);

	return retval;
}

static gboolean
lmplayer_time_within_seconds (LmplayerObject *lmplayer)
{
	gint64 time;

	time = bacon_video_widget_get_current_time (lmplayer->bvw);

	return (time < REWIND_OR_PREVIOUS);
}

static void
lmplayer_action_direction (LmplayerObject *lmplayer, LmplayerPlaylistDirection dir)
{
	if (lmplayer_playing_dvd (lmplayer->mrl) == FALSE &&
		lmplayer_playlist_has_direction (lmplayer->playlist, dir) == FALSE
		&& lmplayer_playlist_get_repeat (lmplayer->playlist) == FALSE)
		return;

	if (lmplayer_playing_dvd (lmplayer->mrl) != FALSE)
	{
		bacon_video_widget_dvd_event (lmplayer->bvw,
				dir == LMPLAYER_PLAYLIST_DIRECTION_NEXT ?
				BVW_DVD_NEXT_CHAPTER :
				BVW_DVD_PREV_CHAPTER);
		return;
	}
	
	if (dir == LMPLAYER_PLAYLIST_DIRECTION_NEXT
			|| bacon_video_widget_is_seekable (lmplayer->bvw) == FALSE
			|| lmplayer_time_within_seconds (lmplayer) != FALSE)
	{
		char *mrl, *subtitle;

		lmplayer_playlist_set_direction (lmplayer->playlist, dir);
		mrl = lmplayer_playlist_get_current_mrl (lmplayer->playlist, &subtitle);
		lmplayer_action_set_mrl_and_play (lmplayer, mrl, subtitle);

		g_free (subtitle);
		g_free (mrl);
	} else {
		lmplayer_action_seek (lmplayer, 0);
	}
}

/**
 * lmplayer_action_previous:
 * @lmplayer: a #LmplayerObject
 *
 * If a DVD is being played, goes to the previous chapter. If a normal stream
 * is being played, goes to the start of the stream if possible. If seeking is
 * not possible, plays the previous entry in the playlist.
 **/
void
lmplayer_action_previous (LmplayerObject *lmplayer)
{
	lmplayer_action_direction (lmplayer, LMPLAYER_PLAYLIST_DIRECTION_PREVIOUS);
}

/**
 * lmplayer_action_next:
 * @lmplayer: a #LmplayerObject
 *
 * If a DVD is being played, goes to the next chapter. If a normal stream
 * is being played, plays the next entry in the playlist.
 **/
void
lmplayer_action_next (LmplayerObject *lmplayer)
{
	lmplayer_action_direction (lmplayer, LMPLAYER_PLAYLIST_DIRECTION_NEXT);
}

static gboolean
lmplayer_action_open_dialog (LmplayerObject *lmplayer, const char *path, gboolean play)
{
	GSList *filenames;
	gboolean playlist_modified;

	g_return_val_if_fail(LMPLAYER_IS_OBJECT(lmplayer), FALSE);

	filenames = lmplayer_add_files (GTK_WINDOW (lmplayer->win), path);

	if (filenames == NULL)
		return FALSE;

	playlist_modified = lmplayer_action_open_files_list (lmplayer, filenames);

	if (playlist_modified == FALSE) 
	{
		g_slist_foreach (filenames, (GFunc) g_free, NULL);
		g_slist_free (filenames);
		return FALSE;
	}

	g_slist_foreach (filenames, (GFunc) g_free, NULL);
	g_slist_free (filenames);

	if (play != FALSE) {
		char *mrl, *subtitle;

		mrl = lmplayer_playlist_get_current_mrl (lmplayer->playlist, &subtitle);
		lmplayer_action_set_mrl_and_play (lmplayer, mrl, subtitle);
		g_free (mrl);
		g_free (subtitle);
	}

	return TRUE;
}

void 
lmplayer_action_open(LmplayerObject *lmplayer)
{
	lmplayer_action_open_dialog(lmplayer, NULL, TRUE);
}

gboolean
lmplayer_action_set_mrl (LmplayerObject *lmplayer, const char *mrl, const char *subtitle)
{
	return lmplayer_action_set_mrl_with_warning (lmplayer, mrl, subtitle, TRUE);
}

void
lmplayer_action_set_mrl_and_play (LmplayerObject *lmplayer, const char *mrl, const char *subtitle)
{
	if (lmplayer_action_set_mrl (lmplayer, mrl, subtitle) != FALSE)
		lmplayer_action_play (lmplayer);
}

static void
playlist_changed_cb (GtkWidget *playlist, LmplayerObject *lmplayer)
{
	char *mrl, *subtitle;

	mrl = lmplayer_playlist_get_current_mrl(lmplayer->playlist, &subtitle);

	if (mrl == NULL)
		return;

	if (lmplayer_playlist_get_playing (lmplayer->playlist) == LMPLAYER_PLAYLIST_STATUS_NONE)
		lmplayer_action_set_mrl_and_play (lmplayer, mrl, subtitle);

	g_free (mrl);
	g_free (subtitle);
}

static gboolean
lmplayer_action_open_files_list (LmplayerObject *lmplayer, GSList *list)
{
	GSList *l;
	gboolean changed;
	gboolean cleared;

	changed = FALSE;
	cleared = FALSE;

	lmplayer_debug(" ");
	if (list == NULL)
		return changed;

	lmplayer_debug(" ");
	lmplayer_window_set_waiting_cursor(GTK_WIDGET(lmplayer->win)->window);

	for (l = list ; l != NULL; l = l->next)
	{
		char *filename;
		char *data = l->data;

		if (data == NULL)
			continue;

		/* Ignore relatives paths that start with "--", tough luck */
		if (data[0] == '-' && data[1] == '-')
			continue;

		/* Get the subtitle part out for our tests */
		filename = lmplayer_create_full_path (data);
		if (filename == NULL)
			filename = g_strdup (data);

		if (g_file_test (filename, G_FILE_TEST_IS_REGULAR)
				|| strstr (filename, "#") != NULL
				|| strstr (filename, "://") != NULL
				|| g_str_has_prefix (filename, "dvd:") != FALSE
				|| g_str_has_prefix (filename, "vcd:") != FALSE
				|| g_str_has_prefix (filename, "dvb:") != FALSE)
		{
			if (cleared == FALSE)
			{
				lmplayer_debug(" ");
				/* The function that calls us knows better
				 * if we should be doing something with the 
				 * changed playlist ... */
				g_signal_handlers_disconnect_by_func
					(G_OBJECT (lmplayer->playlist),
					 playlist_changed_cb, lmplayer);

				lmplayer_debug(" ");
				changed = lmplayer_playlist_clear (lmplayer->playlist);
				lmplayer_debug(" ");
				bacon_video_widget_close (lmplayer->bvw);
				lmplayer_debug(" ");
				lmplayer_file_closed (lmplayer);
				lmplayer_debug(" ");
				cleared = TRUE;
			}

			if (lmplayer_is_block_device (filename) != FALSE) 
			{
				lmplayer_debug(" ");
				lmplayer_action_load_media_device (lmplayer, data);
				changed = TRUE;
			} 
			else if (g_str_has_prefix (filename, "dvb:/") != FALSE) 
			{
				lmplayer_debug(" ");
				lmplayer_playlist_add_mrl (lmplayer->playlist, data, NULL);
				changed = TRUE;
			} 
			else if (g_str_equal (filename, "dvb:") != FALSE) 
			{
				lmplayer_debug(" ");
				lmplayer_action_load_media (lmplayer, MEDIA_TYPE_DVB, "0");
				changed = TRUE;
			} 
			else if (lmplayer_playlist_add_mrl (lmplayer->playlist, filename, NULL) != FALSE) 
			{
				lmplayer_debug(" ");
				//lmplayer_action_add_recent (lmplayer, filename);
				changed = TRUE;
			}
		}

		g_free (filename);
	}

	lmplayer_debug(" ");
	gdk_window_set_cursor (GTK_WIDGET(lmplayer->win)->window, NULL);

	/* ... and reconnect because we're nice people */
	if (cleared != FALSE)
	{
		g_signal_connect (G_OBJECT (lmplayer->playlist),
				"changed", G_CALLBACK (playlist_changed_cb),
				lmplayer);
	}

	return changed;
}
static gboolean
lmplayer_action_open_files (LmplayerObject *lmplayer, char **list)
{
	GSList *slist = NULL;
	int i, retval;

	for (i = 0 ; list[i] != NULL; i++)
		slist = g_slist_prepend (slist, list[i]);

	slist = g_slist_reverse (slist);
	retval = lmplayer_action_open_files_list (lmplayer, slist);
	g_slist_free (slist);

	return retval;
}

gboolean
seek_slider_pressed_cb (GtkWidget *widget, GdkEventButton *event, LmplayerObject *lmplayer)
{
	lmplayer->seek_lock = TRUE;
	//lmplayer_statusbar_set_seeking (LMPLAYER_STATUSBAR(lmplayer->statusbar), TRUE);
	//totem_time_label_set_seeking (TOTEM_TIME_LABEL (totem->fs->time_label), TRUE);

	return FALSE;
}

void
seek_slider_changed_cb (GtkAdjustment *adj, LmplayerObject *lmplayer)
{
	double pos;
	//gint time;

	//if (lmplayer->seek_lock == FALSE)
	//	return;

	//pos = gtk_adjustment_get_value (adj) / 65535;
	//time = bacon_video_widget_get_stream_length(lmplayer->bvw);

	//lmplayer_statusbar_set_time_and_length (LMPLAYER_STATUSBAR(lmplayer->statusbar),
	//		(int) (pos * time / 1000), time / 1000);
	/*
	lmplayer_time_label_set_time
			(LMPLAYER_TIME_LABEL (totem->fs->time_label),
			 (int) (pos * time), time);
	*/

	pos = skin_hscale_get_value(lmplayer->seek);
	if(bacon_video_widget_can_direct_seek(lmplayer->bvw) != FALSE)
		lmplayer_action_seek(lmplayer, pos);
}

gboolean
seek_slider_released_cb(GtkWidget *widget, GdkEventButton *event, LmplayerObject *lmplayer)
{
	//GtkAdjustment *adj;
	gdouble val;

	/* set to FALSE here to avoid triggering a final seek when
	 * syncing the adjustments while being in direct seek mode */
	lmplayer->seek_lock = FALSE;

	/* sync both adjustments */
	//adj = gtk_range_get_adjustment (GTK_RANGE (widget));
	//val = gtk_adjustment_get_value (adj);
	val = skin_hscale_get_value(lmplayer->seek);

	if(bacon_video_widget_can_direct_seek(lmplayer->bvw) == FALSE)
		lmplayer_action_seek(lmplayer, val / 65535.0);

	//lmplayer_statusbar_set_seeking(LMPLAYER_STATUSBAR(lmplayer->statusbar), FALSE);
	//totem_time_label_set_seeking (TOTEM_TIME_LABEL (totem->fs->time_label),
	//		FALSE);
	return FALSE;
}

static char *
lmplayer_get_nice_name_for_stream(LmplayerObject *lmplayer)
{
	gchar *title, *artist, *retval;
	gchar *title_utf8, *artist_utf8;
	int tracknum;
	GValue value = { 0, };

	bacon_video_widget_get_metadata(lmplayer->bvw, BVW_INFO_TITLE, &value);
	title = g_value_dup_string (&value);
	g_value_unset (&value);

	if (title == NULL)
		return NULL;

	title_utf8 = lmplayer_encode_convert_to_utf8(title);

	bacon_video_widget_get_metadata(lmplayer->bvw, BVW_INFO_ARTIST, &value);
	artist = g_value_dup_string(&value);
	g_value_unset (&value);

	if (artist == NULL)
		return title_utf8;

	artist_utf8 = lmplayer_encode_convert_to_utf8(artist);

	bacon_video_widget_get_metadata(lmplayer->bvw,
					 BVW_INFO_TRACK_NUMBER,
					 &value);
	tracknum = g_value_get_int(&value);
	g_value_unset(&value);

	if (tracknum != 0) 
	{
		retval = g_strdup_printf ("%02d. %s - %s",
				tracknum, artist_utf8, title_utf8);
	} 
	else 
	{
		retval = g_strdup_printf ("%s - %s", artist_utf8, title_utf8);
	}
	g_free (artist);
	g_free (title);

	g_free (artist_utf8);
	g_free (title_utf8);

	return retval;
}

static void
playlist_active_name_changed_cb (LmplayerPlaylist *playlist, LmplayerObject *lmplayer)
{
	char *name;
	gboolean cur;

	if ((name = lmplayer_playlist_get_current_title (playlist, &cur)) != NULL) 
	{
		update_mrl_label(lmplayer, name);
		g_free (name);
	}
}

static void
playlist_item_activated_cb (GtkWidget *playlist, LmplayerObject *lmplayer)
{
	lmplayer_action_seek(lmplayer, 0);
}

static void
playlist_current_removed_cb (GtkWidget *playlist, LmplayerObject *lmplayer)
{
	char *mrl, *subtitle;

	lmplayer_debug("playlist_current_removed_cb");

	/* Set play button status */
	play_pause_set_label (lmplayer, STATE_STOPPED);
	mrl = lmplayer_playlist_get_current_mrl (lmplayer->playlist, &subtitle);

	if (mrl == NULL) 
	{
		g_free (subtitle);
		subtitle = NULL;
		lmplayer_playlist_set_at_start (lmplayer->playlist);
		//update_buttons (lmplayer);
		mrl = lmplayer_playlist_get_current_mrl (lmplayer->playlist, &subtitle);
	} 
	else 
	{
		//update_buttons (totem);
	}

	lmplayer_action_set_mrl_and_play (lmplayer, mrl, subtitle);
	g_free (mrl);
	g_free (subtitle);
}

static void
playlist_subtitle_changed_cb (GtkWidget *playlist, LmplayerObject *lmplayer)
{
	char *mrl, *subtitle;

	lmplayer_action_stop (lmplayer);
	mrl = lmplayer_playlist_get_current_mrl (lmplayer->playlist, &subtitle);
	lmplayer_action_set_mrl_and_play (lmplayer, mrl, subtitle);

	g_free (mrl);
	g_free (subtitle);
}

static void
playlist_repeat_toggled_cb (LmplayerPlaylist *playlist, gboolean repeat, LmplayerObject *lmplayer)
{
	lmplayer_debug("playlist_repeat_toggled_cb");
	//GtkAction *action;

	//action = gtk_action_group_get_action (totem->main_action_group, "repeat-mode");

	//g_signal_handlers_block_matched (G_OBJECT (action), G_SIGNAL_MATCH_DATA, 0, 0,
	//		NULL, NULL, totem);

	//gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), repeat);

	//g_signal_handlers_unblock_matched (G_OBJECT (action), G_SIGNAL_MATCH_DATA, 0, 0,
	//		NULL, NULL, totem);
}

static void
playlist_shuffle_toggled_cb (LmplayerPlaylist *playlist, gboolean shuffle, LmplayerObject *lmplayer)
{
	lmplayer_debug("playlist_shuffle_toggled_cb");
	//GtkAction *action;

	//action = gtk_action_group_get_action (totem->main_action_group, "shuffle-mode");

	//g_signal_handlers_block_matched (G_OBJECT (action), G_SIGNAL_MATCH_DATA, 0, 0,
	//		NULL, NULL, totem);

	//gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), shuffle);

	//g_signal_handlers_unblock_matched (G_OBJECT (action), G_SIGNAL_MATCH_DATA, 0, 0,
	//		NULL, NULL, totem);
}

static void playlist_widget_setup(LmplayerObject *lmplayer)
{
	GnomeCanvasItem *item;
	LmplayerPlaylist *playlist = NULL;

	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	playlist = LMPLAYER_PLAYLIST(lmplayer_playlist_new());
	lmplayer->playlist = playlist;

	if(lmplayer->playlist == NULL)
		lmplayer_action_exit(lmplayer);

	gtk_widget_modify_text(GTK_WIDGET(playlist), 
			GTK_STATE_NORMAL, 
			&lmplayer->ar->playlist->attr.color_text);
	gtk_widget_modify_text(GTK_WIDGET(playlist), 
			GTK_STATE_ACTIVE, 
			&lmplayer->ar->playlist->attr.color_hilight);
	gtk_widget_modify_text(GTK_WIDGET(playlist), 
			GTK_STATE_SELECTED, 
			&lmplayer->ar->playlist->attr.color_select);
	gtk_widget_modify_base(GTK_WIDGET(playlist), 
			GTK_STATE_NORMAL, 
			&(lmplayer->ar->playlist->attr.color_bg));
	
	item = (GnomeCanvasItem*)skin_builder_get_object(lmplayer->builder, "playlist-playlistbox");
	
	gnome_canvas_item_set(item, 
			"widget", lmplayer->playlist,
			NULL);

	gtk_widget_show_all(GTK_WIDGET(lmplayer->playlist));

	g_signal_connect (G_OBJECT (playlist), "active-name-changed",
			G_CALLBACK (playlist_active_name_changed_cb), lmplayer);
	g_signal_connect (G_OBJECT (playlist), "item-activated",
			G_CALLBACK (playlist_item_activated_cb), lmplayer);
	g_signal_connect (G_OBJECT (playlist), "changed",
			G_CALLBACK (playlist_changed_cb), lmplayer);
	g_signal_connect (G_OBJECT (playlist), "current-removed",
			G_CALLBACK (playlist_current_removed_cb), lmplayer);
	g_signal_connect (G_OBJECT (playlist), "repeat-toggled",
			G_CALLBACK (playlist_repeat_toggled_cb), lmplayer);
	g_signal_connect (G_OBJECT (playlist), "shuffle-toggled",
			G_CALLBACK (playlist_shuffle_toggled_cb), lmplayer);
	g_signal_connect (G_OBJECT (playlist), "subtitle-changed",
			G_CALLBACK (playlist_subtitle_changed_cb), lmplayer);
}

static void lmplayer_action_error_and_exit(const char *title,
		const char *reason, LmplayerObject *lmplayer)
{
	//reset_seek_status (lmplayer);
	//lmplayer_interface_error_blocking (title, reason, GTK_WINDOW (lmplayer->win));
	lmplayer_action_stop (lmplayer);
	lmplayer_action_exit (lmplayer);
}


static gboolean 
main_window_destroy_cb (GtkWidget *widget, GdkEvent *event, LmplayerObject *lmplayer)
{
	lmplayer_debug("window destroy");
	//g_assert(LMPLAYER_IS_OBJECT(lmplayer));
	
	lmplayer_debug("window destroy");
	lmplayer_action_exit (lmplayer);
	lmplayer_debug("window destroy");

	return FALSE;
}

static gboolean 
main_window_state_changed_cb (GtkWidget *widget, GdkEventWindowState *event, LmplayerObject *lmplayer)
{
	switch(event->changed_mask)
	{
		case GDK_WINDOW_STATE_WITHDRAWN:
			lmplayer_debug("withdrawn");
			break;
		case GDK_WINDOW_STATE_ICONIFIED:
			if(gdk_window_get_state(widget->window) == 2)
			{
				gtk_window_iconify(GTK_WINDOW(lmplayer->pl_win));
				gtk_window_iconify(GTK_WINDOW(lmplayer->lyric_win));
				gtk_window_iconify(GTK_WINDOW(lmplayer->eq_win));
				gtk_window_iconify(GTK_WINDOW(lmplayer->win));
			}
			else if(gdk_window_get_state(widget->window) == 0)
			{
				gtk_window_deiconify(GTK_WINDOW(lmplayer->pl_win));
				gtk_window_deiconify(GTK_WINDOW(lmplayer->lyric_win));
				gtk_window_deiconify(GTK_WINDOW(lmplayer->eq_win));
				gtk_window_deiconify(GTK_WINDOW(lmplayer->win));
			}
			break;
		case GDK_WINDOW_STATE_MAXIMIZED:
			lmplayer_debug("maximized");
			break;
		case GDK_WINDOW_STATE_STICKY:
			lmplayer_debug("sticky");
			break;
		case GDK_WINDOW_STATE_FULLSCREEN:
			lmplayer_debug("fullscreen");
			break;
		case GDK_WINDOW_STATE_ABOVE:
			lmplayer_debug("above");
			break;
		case GDK_WINDOW_STATE_BELOW:
			lmplayer_debug("below");
			break;
		default:
			lmplayer_debug("default");
			break;
	}

	return FALSE;
}

static void 
main_window_right_button_press_cb(SkinWindow *win, LmplayerObject *lmplayer)
{
	GtkWidget *menu;

	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));
	g_return_if_fail(GTK_IS_UI_MANAGER(lmplayer->menus));

	menu = gtk_ui_manager_get_widget(lmplayer->menus, "/MainMenu");
	g_return_if_fail(GTK_IS_MENU(menu));

	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
			1, gtk_get_current_event_time());

}

static void lmplayer_callback_connect(LmplayerObject *lmplayer)
{
	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	g_signal_connect(G_OBJECT(lmplayer->win), "right-button-press", 
			G_CALLBACK(main_window_right_button_press_cb), lmplayer);

	g_signal_connect(G_OBJECT(lmplayer->mini_win), "right-button-press", 
			G_CALLBACK(main_window_right_button_press_cb), lmplayer);

	//g_signal_connect(G_OBJECT(lmplayer->win), "destroy", 
	//		G_CALLBACK(main_window_destroy_cb), lmplayer);
	
	//FIXME: 为什么用destroy这个信号LMPLAYER_IS_OBJECT(lmplayer)通不过呢?
	g_signal_connect(G_OBJECT(lmplayer->win), "delete-event", 
			G_CALLBACK(main_window_destroy_cb), lmplayer);

	g_signal_connect(G_OBJECT(lmplayer->mini_win), "delete-event", 
			G_CALLBACK(main_window_destroy_cb), lmplayer);

	g_signal_connect(G_OBJECT(lmplayer->win), "window-state-event",
			G_CALLBACK(main_window_state_changed_cb), lmplayer);

	g_signal_connect(G_OBJECT(lmplayer->volume), "value-changed",
			G_CALLBACK(lmplayer_volume_value_changed_cb), lmplayer);

	g_signal_connect(G_OBJECT(lmplayer->seek), "value-changed",
			G_CALLBACK(seek_slider_changed_cb), lmplayer);
}

static gboolean
on_eos_event (GtkWidget *widget, LmplayerObject *lmplayer)
{
	if (lmplayer_playlist_has_next_mrl (lmplayer->playlist) == FALSE
			&& lmplayer_playlist_get_repeat (lmplayer->playlist) == FALSE)
	{
		char *mrl, *subtitle;

		lmplayer_playlist_set_at_start (lmplayer->playlist);
		//update_buttons (lmplayer);
		lmplayer_action_stop (lmplayer);
		mrl = lmplayer_playlist_get_current_mrl (lmplayer->playlist, &subtitle);
		lmplayer_action_set_mrl_with_warning (lmplayer, mrl, subtitle, FALSE);
		bacon_video_widget_pause (lmplayer->bvw);
		g_free (mrl);
		g_free (subtitle);
	} 
	else 
	{
		lmplayer_action_next (lmplayer);
	}

	return FALSE;
}

static void
on_got_redirect (BaconVideoWidget *bvw, const char *mrl, LmplayerObject *lmplayer)
{
}

static void
on_title_change_event (BaconVideoWidget *bvw, const char *string, LmplayerObject *lmplayer)
{
}

static void
on_channels_change_event (BaconVideoWidget *bvw, LmplayerObject *lmplayer)
{
}

static void
on_got_metadata_event (BaconVideoWidget *bvw, LmplayerObject *lmplayer)
{
	char *name = NULL;
	
	lmplayer_metadata_updated(lmplayer, NULL, NULL, NULL);

	name = lmplayer_get_nice_name_for_stream(lmplayer);

	if (name != NULL) {
		lmplayer_playlist_set_title
			(LMPLAYER_PLAYLIST(lmplayer->playlist), name, FALSE);
		g_free (name);
	}
	
	lmplayer_info_update(lmplayer);
	playlist_active_name_changed_cb (LMPLAYER_PLAYLIST (lmplayer->playlist), lmplayer);
}

static void
on_error_event (BaconVideoWidget *bvw, char *message,
                gboolean playback_stopped, gboolean fatal, LmplayerObject *lmplayer)
{
}

static void
on_buffering_event (BaconVideoWidget *bvw, int percentage, LmplayerObject *lmplayer)
{
}

/*
static void
update_seekable (LmplayerObject *lmplayer)
{
}
*/

static void
update_current_time (BaconVideoWidget *bvw,
		gint64 current_time,
		gint64 stream_length,
		double current_position,
		gboolean seekable, LmplayerObject *lmplayer)
{
	if(lmplayer->seek_lock == FALSE)
	{
		//gtk_adjustment_set_value(lmplayer->seekadj,
		//		current_position * 65535);

		//skin_hscale_set_value(lmplayer->seek, (gdouble)current_position * 65535.0);
		//skin_digital_time_set_value(lmplayer->led, (gdouble)current_position * 65535.0);
		if(stream_length == 0 && lmplayer->mrl != NULL)
		{
			//lmplayer_statusbar_set_time_and_length
			//	(LMPLAYER_STATUSBAR(lmplayer->statusbar),
			//	(int) (current_time / 1000), -1);
		} 
		else 
		{
			//lmplayer_statusbar_set_time_and_length
			//	(LMPLAYER_STATUSBAR(lmplayer->statusbar),
			//	(int) (current_time / 1000),
			//	(int) (stream_length / 1000));

			//lmplayer_debug(" ");
			//skin_hscale_set_range(lmplayer->seek, 0.0, (gdouble)stream_length / 1000.0);
			//skin_hscale_set_value(lmplayer->seek, (gdouble)current_time / 1000.0);
			skin_hscale_set_range_and_value(lmplayer->seek, 0.0, 
					(gdouble)stream_length / 1000.0,
					(gdouble)current_time / 1000.0);
			skin_digital_time_set_value(lmplayer->led, (gdouble)current_time / 1000.0);
		}

		//totem_time_label_set_time
		//	(TOTEM_TIME_LABEL (totem->fs->time_label),
		//	 current_time, stream_length);
	}

	if (lmplayer->stream_length != stream_length) 
	{
		g_object_notify (G_OBJECT (lmplayer), "stream-length");
		lmplayer->stream_length = stream_length;
	}
}

void
lmplayer_action_volume_relative (LmplayerObject *lmplayer, double off_pct)
{
	double vol;

	if (bacon_video_widget_can_set_volume (lmplayer->bvw) == FALSE)
		return;

	vol = bacon_video_widget_get_volume (lmplayer->bvw);
	bacon_video_widget_set_volume (lmplayer->bvw, vol + off_pct);
}

static void 
lmplayer_volume_value_changed_cb(SkinHScale *hscale, LmplayerObject *lmplayer)
{
	double volume;
	volume = skin_hscale_get_value(lmplayer->volume);
	bacon_video_widget_set_volume(lmplayer->bvw, volume / 100.0);
}


static void
update_volume_slider(LmplayerObject *lmplayer)
{
	double volume;

	volume = bacon_video_widget_get_volume(lmplayer->bvw);

	skin_hscale_set_value(lmplayer->volume, volume * 100.0);
}

static void
property_notify_cb_seekable (BaconVideoWidget *bvw, GParamSpec *spec, LmplayerObject *lmplayer)
{
}

static void
property_notify_cb_volume (BaconVideoWidget *bvw, GParamSpec *spec, LmplayerObject *lmplayer)
{
	update_volume_slider(lmplayer);
}

static void
video_widget_create (LmplayerObject *lmplayer) 
{
	GnomeCanvasItem *item;
	BaconVideoWidget **bvw;
	GError *err = NULL;

	/*
	const GtkTargetEntry source_table[] = {
		{ "text/uri-list", 0, 0 }
	};
	*/

	lmplayer->bvw = BACON_VIDEO_WIDGET
		(bacon_video_widget_new (-1, -1, BVW_USE_TYPE_VIDEO, &err));

	if (lmplayer->bvw == NULL) {
		lmplayer_action_error_and_exit (_("Lmplayer could not startup."), err != NULL ? err->message : _("No reason."), lmplayer);
		if (err != NULL)
			g_error_free (err);
	}

	g_signal_connect (G_OBJECT (lmplayer->bvw),
			"eos",
			G_CALLBACK (on_eos_event),
			lmplayer);//FIXME: for eos
	g_signal_connect (G_OBJECT (lmplayer->bvw),
			"got-redirect",
			G_CALLBACK (on_got_redirect),
			lmplayer);//FIXME: for redirect
	g_signal_connect (G_OBJECT(lmplayer->bvw),
			"title-change",
			G_CALLBACK (on_title_change_event),
			lmplayer);
	g_signal_connect (G_OBJECT(lmplayer->bvw),
			"channels-change",
			G_CALLBACK (on_channels_change_event),
			lmplayer);
	g_signal_connect (G_OBJECT (lmplayer->bvw),
			"tick",
			G_CALLBACK (update_current_time),
			lmplayer); //FIXME: what is tick?
	g_signal_connect (G_OBJECT (lmplayer->bvw),
			"got-metadata",
			G_CALLBACK (on_got_metadata_event),
			lmplayer);
	g_signal_connect (G_OBJECT (lmplayer->bvw),
			"buffering",
			G_CALLBACK (on_buffering_event),
			lmplayer);
	g_signal_connect (G_OBJECT (lmplayer->bvw),
			"error",
			G_CALLBACK (on_error_event),
			lmplayer);

	item = (GnomeCanvasItem*)skin_builder_get_object(lmplayer->builder, "player-visualbox");
	gnome_canvas_item_set(item,
			"widget", lmplayer->bvw,
			NULL);

	bvw = &(lmplayer->bvw);
	g_object_add_weak_pointer(G_OBJECT(lmplayer->bvw), (gpointer *)bvw);

	gtk_widget_realize(GTK_WIDGET(lmplayer->bvw));
	gtk_widget_show(GTK_WIDGET(lmplayer->bvw));

	bacon_video_widget_set_visuals(lmplayer->bvw, "GOOM:what a GOOM!");

	bacon_video_widget_set_volume (lmplayer->bvw,
			((double) gconf_client_get_int (lmplayer->gc,
				GCONF_PREFIX"/volume", NULL)) / 100.0);

	g_signal_connect (G_OBJECT (lmplayer->bvw), "notify::volume",
			G_CALLBACK (property_notify_cb_volume), lmplayer);
	g_signal_connect (G_OBJECT (lmplayer->bvw), "notify::seekable",
			G_CALLBACK (property_notify_cb_seekable), lmplayer);

	update_volume_slider (lmplayer); //TODO:
}

static void
lmplayer_message_connection_receive_cb (const char *msg, LmplayerObject *lmp)
{
	char *command_str, *url;
	int command;

	if (strlen (msg) < 4)
		return;

	command_str = g_strndup (msg, 3);
	sscanf (command_str, "%d", &command);
	g_free (command_str);

	if (msg[4] != '\0')
		url = g_strdup (msg + 4);
	else
		url = NULL;

	lmplayer_debug(" ");
	lmplayer_action_remote (lmp, command, url);
	lmplayer_debug(" ");

	g_free (url);
}

static void
lmplayer_action_remote (LmplayerObject *lmplayer, LmpRemoteCommand cmd, const char *url)
{
	gboolean handled = TRUE;

	lmplayer_debug(" ");
	switch (cmd) {
	case LMP_REMOTE_COMMAND_PLAY:
		g_print("remote command: play\n");
		lmplayer_action_play(lmplayer);
		break;
	case LMP_REMOTE_COMMAND_PLAYPAUSE:
		g_print("remote command: playpause\n");
		lmplayer_action_play_pause(lmplayer);
		break;
	case LMP_REMOTE_COMMAND_PAUSE:
		lmplayer_action_pause(lmplayer);
		g_print("remote command: pause\n");
		break;
	case LMP_REMOTE_COMMAND_STOP:
		{
			char *mrl, *subtitle;
			lmplayer_playlist_set_at_start(lmplayer->playlist);
			//update_buttons(lmplayer);
			lmplayer_action_stop(lmplayer);
			mrl = lmplayer_playlist_get_current_mrl(lmplayer->playlist, &subtitle);
			if(mrl != NULL)
			{
				lmplayer_action_set_mrl_with_warning(lmplayer, mrl, subtitle, FALSE);
				bacon_video_widget_pause(lmplayer->bvw);
				g_free(mrl);
				g_free(subtitle);
			}
		};
	case LMP_REMOTE_COMMAND_SEEK_FORWARD:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_SEEK_BACKWARD:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_VOLUME_UP:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_VOLUME_DOWN:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_NEXT:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_PREVIOUS:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_FULLSCREEN:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_QUIT:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_ENQUEUE:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_REPLACE:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_SHOW:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_TOGGLE_CONTROLS:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_SHOW_PLAYING:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_SHOW_VOLUME:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_UP:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_DOWN:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_LEFT:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_RIGHT:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_SELECT:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_DVD_MENU:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_ZOOM_UP:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_ZOOM_DOWN:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_EJECT:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_PLAY_DVD:
		g_print("\n");
		break;
	case LMP_REMOTE_COMMAND_MUTE:
		g_print("\n");
		break;
	default:
		handled = FALSE;
		break;
	}
	lmplayer_debug(" ");
}

int 
main (int argc, char* argv[])
{
	LmplayerObject *lmplayer;
	GOptionContext *context;
	GOptionGroup *baconoptiongroup;
	GConfClient *gc;
	GError* error = NULL;

	bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

	g_thread_init(NULL);
	gtk_init(&argc, &argv);
	g_type_init();

	bacon_video_widget_init_backend(NULL, NULL);

	context = g_option_context_new(N_("[FILES] - Play songs"));
	baconoptiongroup = bacon_video_widget_get_option_group();
	g_option_context_add_main_entries(context, options, GETTEXT_PACKAGE);
	g_option_context_set_translation_domain(context, GETTEXT_PACKAGE);
	g_option_context_add_group (context, baconoptiongroup);

	if(g_option_context_parse (context, &argc, &argv, &error) == FALSE)
	{
		g_print(_("%s\nRun '%s --help' to see a full list of available command line options.\n"), 
				error->message, argv[0]);
		g_error_free (error);
		lmplayer_action_exit (NULL);
	}

	g_set_application_name(_("Linux Music Player"));
	gtk_window_set_default_icon_name ("lmplayer");

	gc = gconf_client_get_default ();
	if (gc == NULL)
	{
		lmplayer_action_error_and_exit (_("Lmplayer could not initialize the configuration engine."), _("Make sure that GNOME is properly installed."), NULL);
	}

	lmplayer = lmplayer_object_new();
	lmplayer->gc = gc;

	if(optionstate.notconnectexistingsession == FALSE)
	{
		lmplayer->conn = bacon_message_connection_new(GETTEXT_PACKAGE);
		if(bacon_message_connection_get_is_server(lmplayer->conn) == FALSE)
		{
			lmplayer_options_process_for_server(lmplayer->conn, &optionstate);
			gdk_notify_startup_complete ();
			lmplayer_action_exit (lmplayer);
		}
		else
		{
			lmplayer_options_process_early(lmplayer, &optionstate);
		}
	}
	else
	{
		lmplayer_options_process_early (lmplayer, &optionstate);
	}

	lmplayer->ar = skin_archive_new();
	if(lmplayer->ar == NULL)
	{
		lmplayer_debug("lmplayer->ar is NULL");
		lmplayer_action_exit(NULL);
	}

	gchar *filename = lmplayer_skin_get_full_path("tt.zip");
	lmplayer_debug("skin file: %s", filename);
	if(filename == NULL)
	{
		//FIXME: search other skins
		lmplayer_action_exit(NULL);
	}

	skin_archive_load(lmplayer->ar, filename);
	lmplayer->builder = skin_builder_new();
	skin_builder_add_from_archive(lmplayer->builder, lmplayer->ar);

	lmplayer->win = SKIN_WINDOW(skin_builder_get_object(lmplayer->builder, "player-window"));
	if(lmplayer->win == NULL)
	{
		lmplayer_action_exit(NULL);
	}

	lmplayer->pl_win = SKIN_WINDOW(skin_builder_get_object(lmplayer->builder, "playlist-window"));
	if(lmplayer->pl_win == NULL)
	{
		lmplayer_action_exit(NULL);
	}
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(lmplayer->pl_win), TRUE);

	lmplayer->lyric_win = SKIN_WINDOW(skin_builder_get_object(lmplayer->builder, "lyric-window"));
	if(lmplayer->lyric_win == NULL)
	{
		lmplayer_action_exit(NULL);
	}
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(lmplayer->lyric_win), TRUE);

	lmplayer->eq_win = SKIN_WINDOW(skin_builder_get_object(lmplayer->builder, "equalizer-window"));
	if(lmplayer->eq_win == NULL)
	{
		lmplayer_action_exit(NULL);
	}
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(lmplayer->eq_win), TRUE);

	lmplayer->mini_win = SKIN_WINDOW(skin_builder_get_object(lmplayer->builder, "mini-window"));
	if(lmplayer->mini_win == NULL)
	{
		g_warning("No mini mode window");
	}

	lmplayer->minimode = FALSE;
	gtk_widget_show_all(GTK_WIDGET(lmplayer->win));
	gtk_widget_show_all(GTK_WIDGET(lmplayer->pl_win));
	gtk_widget_show_all(GTK_WIDGET(lmplayer->lyric_win));
	gtk_widget_show_all(GTK_WIDGET(lmplayer->eq_win));

	lmplayer_ui_manager_setup(lmplayer);
	playlist_widget_setup(lmplayer);
	video_widget_create(lmplayer);
	
	//TODO: 安装其它如会话管理、cd播放、文件监视等功能
	lmplayer->state = STATE_STOPPED;
	lmplayer->seek = (SkinHScale *)skin_builder_get_object(lmplayer->builder, "player-progressbar");
	lmplayer->led = (SkinDigitalTime *)skin_builder_get_object(lmplayer->builder, "player-led");
	lmplayer->volume = (SkinHScale *)skin_builder_get_object(lmplayer->builder, "player-volume");
	//lmplayer->statusbar = GTK_WIDGET(gtk_builder_get_object(lmplayer->xml, "tmw_statusbar"));
	lmplayer->seek_lock = FALSE;
	lmplayer_setup_file_monitoring(lmplayer);
	lmplayer_setup_file_filters();
	lmplayer_callback_connect(lmplayer);
	lmplayer_setup_toolbar(lmplayer);

	lmplayer_options_process_late(lmplayer, &optionstate);

	if(optionstate.filenames != NULL && lmplayer_action_open_files(lmplayer, optionstate.filenames))
	{
		lmplayer_action_play_pause(lmplayer);
	}
	else
	{
		lmplayer_action_set_mrl(lmplayer, NULL, NULL);
	}

	if(lmplayer->conn != NULL && bacon_message_connection_get_is_server (lmplayer->conn) != FALSE)
	{
		bacon_message_connection_set_callback (lmplayer->conn,
				(BaconMessageReceivedFunc)
				lmplayer_message_connection_receive_cb, lmplayer);
	}

	
	const gchar *home = g_getenv("HOME");
	gchar *cfg_path = g_build_path(G_DIR_SEPARATOR_S, home, ".lmplayer", NULL);
	if(cfg_path != NULL)
	{
		lmplayer->pls = g_build_filename(cfg_path, "default_playlist.pls", NULL);
	}

	gchar *uri = g_filename_to_uri(lmplayer->pls, NULL, NULL);
	lmplayer_playlist_add_mrl(lmplayer->playlist, uri, NULL);
	g_free(uri);
	gtk_widget_show_all(GTK_WIDGET(lmplayer->win));
	gtk_widget_show_all(GTK_WIDGET(lmplayer->pl_win));
	gtk_widget_show_all(GTK_WIDGET(lmplayer->lyric_win));
	gtk_widget_show_all(GTK_WIDGET(lmplayer->eq_win));
	
	SkinCheckButton *button;
	button = (SkinCheckButton*)skin_builder_get_object(lmplayer->builder, "player-playlist");
	skin_check_button_set_active(button, TRUE);
	button = (SkinCheckButton*)skin_builder_get_object(lmplayer->builder, "player-lyric");
	skin_check_button_set_active(button, TRUE);
	button = (SkinCheckButton*)skin_builder_get_object(lmplayer->builder, "player-equalizer");
	skin_check_button_set_active(button, TRUE);

	gtk_main();
	return 0;
}

