/* vi: set sw=4 ts=4: */
/*
 * lmplayer-object.c
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

#include "lmplayer-private.h"
#include "lmplayerobject-marshal.h"
#include "lmplayer-debug.h"
#include "lmplayer-uri.h"
#include "lmplayer-utils.h"
#include "lmplayer-interface.h"
#include "lmplayer-encode.h"
#include "lmplayer-plugins-engine.h"
#include <glib/gi18n.h>
#include <glib.h>
#include <stdlib.h>

#define REWIND_OR_PREVIOUS 4000

#define SEEK_FORWARD_SHORT_OFFSET 15
#define SEEK_BACKWARD_SHORT_OFFSET -5

#define SEEK_FORWARD_LONG_OFFSET 10*60
#define SEEK_BACKWARD_LONG_OFFSET -3*60

#define VOLUME_EPSILON (1e-10)

enum {
	PROP_0,
	PROP_FULLSCREEN,
	PROP_PLAYING,
	PROP_STREAM_LENGTH,
	PROP_SEEKABLE,
	PROP_CURRENT_TIME,
	PROP_ERROR_SHOWN,
	PROP_CURRENT_MRL
};

enum {
	FILE_OPENED,
	FILE_CLOSED,
	METADATA_UPDATED,
	LAST_SIGNAL
};

static void lmplayer_object_set_property		(GObject *object,
						 guint property_id,
						 const GValue *value,
						 GParamSpec *pspec);
static void lmplayer_object_get_property		(GObject *object,
						 guint property_id,
						 GValue *value,
						 GParamSpec *pspec);
static void lmplayer_object_finalize(GObject *lmplayer);

static int lmplayer_table_signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(LmplayerObject, lmplayer_object, G_TYPE_OBJECT)

/*
#define LMPLAYER_OBJECT_GET_PRIVATE(o)\
	(G_TYPE_INSTANCE_GET_PRIVATE ((o), LMPLAYER_TYPE_OBJECT, LmplayerObjectPrivate))


struct _LmplayerObjectPrivate 
{
};
*/

static gboolean lmplayer_action_open_files_list(LmplayerObject *lmplayer, GSList *list);
static gboolean lmplayer_action_open_dialog (LmplayerObject *lmplayer, const char *path, gboolean play);

static void
lmplayer_object_dispose (LmplayerObject *self)
{
}

static void
lmplayer_object_finalize(GObject *object)
{
	G_OBJECT_CLASS(lmplayer_object_parent_class)->finalize(object);
}

static void
lmplayer_object_init (LmplayerObject *self)
{
	//LmplayerObjectPrivate *priv;

	//priv = LMPLAYER_OBJECT_GET_PRIVATE (self);
}

static void
lmplayer_object_set_property (GObject *object,
			   guint property_id,
			   const GValue *value,
			   GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
}

static void
lmplayer_object_get_property (GObject *object,
			   guint property_id,
			   GValue *value,
			   GParamSpec *pspec)
{
	LmplayerObject *lmplayer;

	lmplayer = LMPLAYER_OBJECT(object);

	switch (property_id)
	{
	case PROP_FULLSCREEN:
		g_value_set_boolean(value, lmplayer_is_fullscreen (lmplayer));
		break;
	case PROP_PLAYING:
		g_value_set_boolean(value, lmplayer_is_playing (lmplayer));
		break;
	case PROP_STREAM_LENGTH:
		g_value_set_int64(value, bacon_video_widget_get_stream_length(lmplayer->bvw));
		break;
	case PROP_CURRENT_TIME:
		g_value_set_int64(value, bacon_video_widget_get_current_time(lmplayer->bvw));
		break;
	case PROP_SEEKABLE:
		g_value_set_boolean(value, lmplayer_is_seekable(lmplayer));
		break;
	case PROP_ERROR_SHOWN:
		//g_value_set_boolean (value, XXX);
		break;
	case PROP_CURRENT_MRL:
		g_value_set_string (value, lmplayer->mrl);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
lmplayer_object_class_init (LmplayerObjectClass *self_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (self_class);

	//g_type_class_add_private (self_class, sizeof (LmplayerObjectPrivate));
	object_class->set_property = lmplayer_object_set_property;
	object_class->get_property = lmplayer_object_get_property;

	object_class->dispose = (void (*) (GObject *object)) lmplayer_object_dispose;
	object_class->finalize = (void (*) (GObject *object)) lmplayer_object_finalize;

	g_object_class_install_property (object_class, PROP_FULLSCREEN,
					 g_param_spec_boolean ("fullscreen", NULL, NULL,
							       FALSE, G_PARAM_READABLE));
	g_object_class_install_property (object_class, PROP_PLAYING,
					 g_param_spec_boolean ("playing", NULL, NULL,
							       FALSE, G_PARAM_READABLE));
	g_object_class_install_property (object_class, PROP_STREAM_LENGTH,
					 g_param_spec_int64 ("stream-length", NULL, NULL,
							     G_MININT64, G_MAXINT64, 0,
							     G_PARAM_READABLE));
	g_object_class_install_property (object_class, PROP_CURRENT_TIME,
					 g_param_spec_int64 ("current-time", NULL, NULL,
							     G_MININT64, G_MAXINT64, 0,
							     G_PARAM_READABLE));
	g_object_class_install_property (object_class, PROP_SEEKABLE,
					 g_param_spec_boolean ("seekable", NULL, NULL,
							       FALSE, G_PARAM_READABLE));
	g_object_class_install_property (object_class, PROP_ERROR_SHOWN,
					 g_param_spec_boolean ("error-shown", NULL, NULL,
							       FALSE, G_PARAM_READABLE));
	g_object_class_install_property (object_class, PROP_CURRENT_MRL,
					 g_param_spec_string ("current-mrl", NULL, NULL,
							      NULL, G_PARAM_READABLE));

	lmplayer_table_signals[FILE_OPENED] =
		g_signal_new ("file-opened",
				G_TYPE_FROM_CLASS (object_class),
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET (LmplayerObjectClass, file_opened),
				NULL, NULL,
				g_cclosure_marshal_VOID__STRING,
				G_TYPE_NONE, 1, G_TYPE_STRING);

	lmplayer_table_signals[FILE_CLOSED] =
		g_signal_new ("file-closed",
				G_TYPE_FROM_CLASS (object_class),
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET (LmplayerObjectClass, file_closed),
				NULL, NULL,
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE, 0, G_TYPE_NONE);

	lmplayer_table_signals[METADATA_UPDATED] =
		g_signal_new ("metadata-updated",
				G_TYPE_FROM_CLASS (object_class),
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET (LmplayerObjectClass, metadata_updated),
				NULL, NULL,
				lmplayerobject_marshal_VOID__STRING_STRING_STRING,
				G_TYPE_NONE, 3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
}

/**
 * lmplayer_object_plugins_init:
 * @lmplayer: a #LmplayerObject
 *
 * Initialises the plugin engine and activates all the
 * enabled plugins.
 **/
void
lmplayer_object_plugins_init(LmplayerObject *lmplayer)
{
	lmplayer_plugins_engine_init(lmplayer);
}

/**
 * lmplayer_object_plugins_shutdown:
 *
 * Shuts down the plugin engine and deactivates all the
 * plugins.
 **/
void
lmplayer_object_plugins_shutdown(void)
{
	lmplayer_plugins_engine_shutdown ();
}

void
lmplayer_add_tools_button(LmplayerObject *lmplayer, GtkWidget *button)
{
	g_return_if_fail(lmplayer);

	gtk_box_pack_start(GTK_BOX(lmplayer->plugins_box), button, FALSE, FALSE, 0);
	//g_object_set(lmplayer->plugins_box, id, button, NULL);
}

void 
lmplayer_remove_tools_button(LmplayerObject *lmplayer, GtkWidget *button)
{
	g_return_if_fail(lmplayer);

	if(button)
		gtk_container_remove(GTK_CONTAINER(lmplayer->plugins_box), button);
}

LmplayerObject* 
lmplayer_object_new()
{
	return g_object_new(LMPLAYER_TYPE_OBJECT, NULL);
}

void
lmplayer_file_opened (LmplayerObject *lmplayer,
		   const char *mrl)
{
	lmplayer_debug(" ");
	g_signal_emit (G_OBJECT (lmplayer),
		       lmplayer_table_signals[FILE_OPENED],
		       0, mrl);
}

void
lmplayer_file_closed (LmplayerObject *lmplayer)
{
	g_signal_emit (G_OBJECT (lmplayer),
		       lmplayer_table_signals[FILE_CLOSED],
		       0);

}

void
lmplayer_metadata_updated (LmplayerObject *lmplayer,
			const char *artist,
			const char *title,
			const char *album)
{
	g_signal_emit (G_OBJECT (lmplayer),
		       lmplayer_table_signals[METADATA_UPDATED],
		       0,
		       artist,
		       title,
		       album);
}

GtkWindow *
lmplayer_get_main_window(LmplayerObject *lmplayer)
{
	g_return_val_if_fail(LMPLAYER_IS_OBJECT(lmplayer), NULL);

	g_object_ref(G_OBJECT(lmplayer->win));

	return GTK_WINDOW(lmplayer->win);
}

GtkUIManager *
lmplayer_get_ui_manager(LmplayerObject *lmplayer)
{
	g_return_val_if_fail(LMPLAYER_IS_OBJECT(lmplayer), NULL);

	//return lmplayer->ui_manager;
	return NULL;
}

GtkWidget *
lmplayer_get_video_widget(LmplayerObject *lmplayer)
{
	g_return_val_if_fail(LMPLAYER_IS_OBJECT(lmplayer), NULL);

	g_object_ref(G_OBJECT(lmplayer->bvw));

	return GTK_WIDGET(lmplayer->bvw);
}

char *
lmplayer_get_video_widget_backend_name(LmplayerObject *lmplayer)
{
	return bacon_video_widget_get_backend_name(lmplayer->bvw);
}

gint64
lmplayer_get_current_time(LmplayerObject *lmplayer)
{
	g_return_val_if_fail(LMPLAYER_IS_OBJECT(lmplayer), 0);

	return bacon_video_widget_get_current_time(lmplayer->bvw);
}

gint64
lmplayer_get_stream_length(LmplayerObject *lmplayer)
{
	g_return_val_if_fail(LMPLAYER_IS_OBJECT(lmplayer), 0);

	return bacon_video_widget_get_stream_length(lmplayer->bvw);
}

gboolean
lmplayer_is_fullscreen(LmplayerObject *lmplayer)
{
	return FALSE;
}

/**
 * lmplayer_is_playing:
 * @lmplayer: a #LmplayerObject
 *
 * Returns %TRUE if Lmplayer is playing a stream.
 *
 * Return value: %TRUE if Lmplayer is playing a stream
 **/
gboolean
lmplayer_is_playing (LmplayerObject *lmplayer)
{
	g_return_val_if_fail (LMPLAYER_IS_OBJECT (lmplayer), FALSE);

	if (lmplayer->bvw == NULL)
		return FALSE;

	return bacon_video_widget_is_playing (lmplayer->bvw) != FALSE;
}

gboolean
lmplayer_is_paused (LmplayerObject *lmplayer)
{
	g_return_val_if_fail (LMPLAYER_IS_OBJECT (lmplayer), FALSE);

	return lmplayer->state == STATE_PAUSED;
}

/**
 * lmplayer_is_seekable:
 * @lmplayer: a #LmplayerObject
 *
 * Returns %TRUE if the current stream is seekable.
 *
 * Return value: %TRUE if the current stream is seekable
 **/
gboolean
lmplayer_is_seekable (LmplayerObject *lmplayer)
{
	g_return_val_if_fail (LMPLAYER_IS_OBJECT (lmplayer), FALSE);

	if (lmplayer->bvw == NULL)
		return FALSE;

	return bacon_video_widget_is_seekable (lmplayer->bvw) != FALSE;
}

static void 
update_buttons(LmplayerObject *lmplayer)
{
	GtkImage *play_image;

	g_return_if_fail(lmplayer != NULL);

	play_image = (GtkImage *)gtk_builder_get_object(lmplayer->builder, "player-play-image");
	g_return_if_fail(play_image != NULL);

	switch(lmplayer->state)
	{
	case STATE_PLAYING:
		gtk_image_set_from_stock(play_image, "gtk-media-pause", GTK_ICON_SIZE_BUTTON);
		break;
	case STATE_PAUSED:
		gtk_image_set_from_stock(play_image, "gtk-media-play", GTK_ICON_SIZE_BUTTON);
		break;
	case STATE_STOPPED:
		gtk_image_set_from_stock(play_image, "gtk-media-play", GTK_ICON_SIZE_BUTTON);
		break;
	default:
		g_assert_not_reached();
		break;
	}
}

static void 
lmplayer_statusbar_set_text(LmplayerObject *lmplayer, gchar *text)
{
#if 0
	SkinStatusBar *sb;
	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	sb = (SkinStatusBar*)skin_builder_get_object(lmplayer->builder, "player-statusbar");

	skin_status_bar_set_text(sb, text);
#endif
}

static void
play_pause_set_label(LmplayerObject *lmplayer, LmplayerStates state)
{
	GtkWidget *play;

	play = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-play");

	if(state == lmplayer->state)
		return;

	switch (state)
	{
	case STATE_PLAYING:
		lmplayer_debug("playing");
		lmplayer_statusbar_set_text(lmplayer, _("Status: Playing"));
		lmplayer_playlist_set_playing(lmplayer->playlist, LMPLAYER_PLAYLIST_STATUS_PLAYING);
		break;
	case STATE_PAUSED:
		lmplayer_debug("paused");
		lmplayer_statusbar_set_text(lmplayer, _("Status: Paused"));
		lmplayer_playlist_set_playing(lmplayer->playlist, LMPLAYER_PLAYLIST_STATUS_PAUSED);
		break;
	case STATE_STOPPED:
		lmplayer_debug("stop");
		lmplayer_statusbar_set_text(lmplayer, _("Status: Stopped"));
		lmplayer_playlist_set_playing(lmplayer->playlist, LMPLAYER_PLAYLIST_STATUS_NONE);
		break;
	default:
		g_assert_not_reached();
		return;
	}

	lmplayer->state = state;

	update_buttons(lmplayer);

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

	lmplayer_debug("retval: %d", retval);
	if (retval != FALSE)
	{
		//lmplayer->has_lyric = FALSE;
		//lmplayer->lyric_downloaded = FALSE;
		//lmplayer_build_lyric_name(lmplayer);
		//lmplayer_debug("lyric: %s\n", lmplayer->lyric_filename);
		//lmplayer_load_local_lyric(lmplayer);
		return;
	}

	disp = lmplayer_uri_escape_for_display (lmplayer->mrl);
	msg = g_strdup_printf(_("LmplayerObject could not play '%s'."), disp);
	g_free (disp);

	lmplayer_action_error (msg, err->message, lmplayer);
	lmplayer_action_stop (lmplayer);
	g_free (msg);
	g_error_free (err);
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

void
lmplayer_action_volume_relative (LmplayerObject *lmplayer, double off_pct)
{
	double vol;

	lmplayer_debug(" ");
	g_return_if_fail(lmplayer != NULL);

	if (bacon_video_widget_can_set_volume (lmplayer->bvw) == FALSE)
		return;

	vol = bacon_video_widget_get_volume (lmplayer->bvw);
	bacon_video_widget_set_volume (lmplayer->bvw, vol + off_pct);
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
lmplayer_action_open_files_list(LmplayerObject *lmplayer, GSList *list)
{
	GSList *l;
	gboolean changed;
	gboolean cleared;

	changed = FALSE;
	cleared = FALSE;

	if (list == NULL)
		return changed;

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
				/* The function that calls us knows better
				 * if we should be doing something with the 
				 * changed playlist ... */
				g_signal_handlers_disconnect_by_func
					(G_OBJECT (lmplayer->playlist),
					 playlist_changed_cb, lmplayer);

				changed = lmplayer_playlist_clear (lmplayer->playlist);
				bacon_video_widget_close (lmplayer->bvw);
				lmplayer_file_closed (lmplayer);
				cleared = TRUE;
			}

			if (lmplayer_is_block_device (filename) != FALSE) 
			{
				lmplayer_action_load_media_device (lmplayer, data);
				changed = TRUE;
			} 
			else if (g_str_has_prefix (filename, "dvb:/") != FALSE) 
			{
				lmplayer_playlist_add_mrl (lmplayer->playlist, data, NULL);
				changed = TRUE;
			} 
			else if (g_str_equal (filename, "dvb:") != FALSE) 
			{
				lmplayer_action_load_media (lmplayer, MEDIA_TYPE_DVB, "0");
				changed = TRUE;
			} 
			else if (lmplayer_playlist_add_mrl (lmplayer->playlist, filename, NULL) != FALSE) 
			{
				//lmplayer_action_add_recent (lmplayer, filename);
				changed = TRUE;
			}
		}

		g_free (filename);
	}

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

gboolean
lmplayer_action_open_files (LmplayerObject *lmplayer, char **list)
{
	GSList *slist = NULL;
	int i, retval;

	for (i = 0; list[i] != NULL; i++)
	{
		lmplayer_debug("file: %s", list[i]);
		slist = g_slist_prepend(slist, list[i]);
	}

	slist = g_slist_reverse(slist);
	retval = lmplayer_action_open_files_list(lmplayer, slist);
	g_slist_free(slist);

	return retval;
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
		gtk_window_set_title(GTK_WINDOW(lmplayer->win), _("Little Pudding Music Player"));
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

		update_mrl_label (lmplayer, NULL);
	} 
	else 
	{
		gboolean caps;
		gdouble volume;
		char *autoload_sub = NULL;
		GError *err = NULL;

		if (subtitle == NULL && lmplayer->autoload_subs != FALSE)
			autoload_sub = lmplayer_uri_get_subtitle_uri (mrl);

		lmplayer_debug("bacon_video_widget_open_with_subtitle: mrl=%s", mrl);
		lmplayer_window_set_waiting_cursor (GTK_WIDGET(lmplayer->win)->window);
		retval = bacon_video_widget_open_with_subtitle (lmplayer->bvw, mrl,
								subtitle ? subtitle : autoload_sub, &err);
		g_free (autoload_sub);
		gdk_window_set_cursor (GTK_WIDGET(lmplayer->win)->window, NULL);
		lmplayer->mrl = g_strdup (mrl);

		/* Volume */
		caps = bacon_video_widget_can_set_volume (lmplayer->bvw);
		volume = bacon_video_widget_get_volume (lmplayer->bvw);

		/* Set the playlist */
		play_pause_set_label(lmplayer, retval ? STATE_PAUSED : STATE_STOPPED);

		if (retval == FALSE && warn != FALSE)
		{
			char *msg, *disp;

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
		} 
		else 
		{
			lmplayer_file_opened (lmplayer, lmplayer->mrl);
		}
	}
	update_buttons (lmplayer);

	return retval;
}

static void
lmplayer_open_location_destroy (LmplayerObject *lmplayer)
{
	if (lmplayer->open_location != NULL) {
		g_object_remove_weak_pointer (G_OBJECT (lmplayer->open_location), (gpointer *)&(lmplayer->open_location));
		gtk_widget_destroy (GTK_WIDGET (lmplayer->open_location));
		lmplayer->open_location = NULL;
	}
}

static void
lmplayer_open_location_response_cb (GtkDialog *dialog, gint response, LmplayerObject *lmplayer)
{
	char *uri;

	if (response != GTK_RESPONSE_OK) {
		lmplayer_open_location_destroy (lmplayer);
		return;
	}

	gtk_widget_hide (GTK_WIDGET (dialog));

	/* Open the specified URI */
	uri = lmplayer_open_location_get_uri (lmplayer->open_location);

	if (uri != NULL)
	{
		char *mrl, *subtitle;
		const char *filenames[2];

		filenames[0] = uri;
		filenames[1] = NULL;
		lmplayer_action_open_files (lmplayer, (char **) filenames);

		mrl = lmplayer_playlist_get_current_mrl (lmplayer->playlist, &subtitle);
		lmplayer_action_set_mrl_and_play (lmplayer, mrl, subtitle);
		g_free (mrl);
		g_free (subtitle);
	}
 	g_free (uri);

	lmplayer_open_location_destroy (lmplayer);
}

void 
lmplayer_action_open_location(LmplayerObject *lmplayer)
{
	if(lmplayer->open_location != NULL)
	{
		gtk_window_present(GTK_WINDOW(lmplayer->open_location));
	}

	lmplayer->open_location = LMPLAYER_OPEN_LOCATION(lmplayer_open_location_new(lmplayer));

	g_signal_connect(G_OBJECT(lmplayer->open_location), "delete-event",
			G_CALLBACK(gtk_widget_destroy), NULL);
	g_signal_connect(G_OBJECT(lmplayer->open_location), "response",
			G_CALLBACK(lmplayer_open_location_response_cb), lmplayer);

	g_object_add_weak_pointer(G_OBJECT(lmplayer->open_location), (gpointer *)&(lmplayer->open_location));

	gtk_window_set_transient_for(GTK_WINDOW(lmplayer->open_location), GTK_WINDOW(lmplayer->win));
	gtk_widget_show(GTK_WIDGET(lmplayer->open_location));
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
	if(lmplayer_action_set_mrl(lmplayer, mrl, subtitle) != FALSE)
		lmplayer_action_play(lmplayer);
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

/**
 * lmplayer_action_stop:
 * @lmplayer: a #LmplayerObjectObject
 *
 * Stops the current stream.
 **/
void
lmplayer_action_stop (LmplayerObject *lmplayer)
{
	lmplayer_debug(" ");
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
	lmplayer_debug(" ");
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
	if(bacon_video_widget_is_playing(lmplayer->bvw) != FALSE) 
	{
		bacon_video_widget_pause(lmplayer->bvw);
		play_pause_set_label(lmplayer, STATE_PAUSED);
	}
}

static void
lmplayer_action_wait_force_exit (gpointer user_data)
{
	g_usleep (5 * G_USEC_PER_SEC);
	exit (1);
}

void
lmplayer_action_exit(LmplayerObject *lmplayer)
{
	/* Exit forcefully if we can't do the shutdown in 10 seconds */
	g_thread_create ((GThreadFunc) lmplayer_action_wait_force_exit,
			 NULL, FALSE, NULL);
	
	if (gtk_main_level () > 0)
	{
		lmplayer_debug(" gtk main quit");
		gtk_main_quit ();
	}

	if (lmplayer == NULL)
		exit (0);

	if(lmplayer->playlist && lmplayer->pls)
	{
		lmplayer_playlist_save_current_playlist(lmplayer->playlist, lmplayer->pls);
		g_free(lmplayer->pls);
		lmplayer->pls = NULL;
	}

	if(lmplayer->lyric_filename)
		g_free(lmplayer->lyric_filename);

	if(lmplayer->bvw) 
	{
		int vol;
		lmplayer_action_stop(lmplayer);

		vol = bacon_video_widget_get_volume(lmplayer->bvw) * 100.0 + 0.5;

		/* FIXME move the volume to the static file? */
		gconf_client_add_dir(lmplayer->gc, GCONF_PREFIX, GCONF_CLIENT_PRELOAD_ONELEVEL, NULL);
		gconf_client_set_int(lmplayer->gc, GCONF_PREFIX"/volume", CLAMP(vol, 0, 100), NULL);
	}

	if(lmplayer->conn != NULL)
		bacon_message_connection_free(lmplayer->conn);

	lmplayer_destroy_file_filters();

	lmplayer_object_plugins_shutdown();

	if(lmplayer->gc)
		g_object_unref(G_OBJECT(lmplayer->gc));

	if(gtk_main_level () > 0)
		gtk_main_quit();

	g_object_unref(lmplayer);
	exit(0);
}

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
static void
lmplayer_seek_time_rel(LmplayerObject *lmplayer, gint64 time, gboolean relative)
{
	GError *err = NULL;
	gint64 sec;

	if (lmplayer->mrl == NULL)
		return;
	if (bacon_video_widget_is_seekable (lmplayer->bvw) == FALSE)
		return;

	//lmplayer_statusbar_set_seeking (TOTEM_STATUSBAR (totem->statusbar), TRUE);
	//lmplayer_time_label_set_seeking (TOTEM_TIME_LABEL (totem->fs->time_label), TRUE);

	if (relative != FALSE) {
		gint64 oldmsec;
		oldmsec = bacon_video_widget_get_current_time (lmplayer->bvw);
		sec = MAX (0, oldmsec + time);
	} else {
		sec = time;
	}

	bacon_video_widget_seek_time (lmplayer->bvw, sec, &err);

	//lmplayer_statusbar_set_seeking (TOTEM_STATUSBAR (totem->statusbar), FALSE);
	//lmplayer_time_label_set_seeking (TOTEM_TIME_LABEL (totem->fs->time_label), FALSE);

	if (err != NULL)
	{
		char *msg, *disp;

		disp = lmplayer_uri_escape_for_display (lmplayer->mrl);
		msg = g_strdup_printf(_("Totem could not play '%s'."), disp);
		g_free (disp);

		lmplayer_action_stop (lmplayer);
		lmplayer_action_error (msg, err->message, lmplayer);
		g_free (msg);
		g_error_free (err);
	}
}

void
lmplayer_action_seek_relative(LmplayerObject *lmplayer, gint64 offset)
{
	lmplayer_seek_time_rel(lmplayer, offset, TRUE);
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
		msg = g_strdup_printf(_("Player could not play '%s'."), disp);
		g_free (disp);

		reset_seek_status (lmplayer);

		lmplayer_action_error (msg, err->message, lmplayer);
		g_free (msg);
		g_error_free (err);
	}
}


void
lmplayer_action_error (const char *title, const char *reason, LmplayerObject *lmplayer)
{
	reset_seek_status(lmplayer);
	lmplayer_interface_error(title, reason, GTK_WINDOW(lmplayer->win));
}

void lmplayer_action_error_and_exit(const char *title, const char *reason, LmplayerObject *lmplayer)
{
	reset_seek_status (lmplayer);
	lmplayer_interface_error_blocking (title, reason, GTK_WINDOW (lmplayer->win));
	lmplayer_action_stop (lmplayer);
	lmplayer_action_exit (lmplayer);
}

GQuark
lmplayer_remote_command_quark (void)
{
	static GQuark quark = 0;
	if (!quark)
		quark = g_quark_from_static_string ("lmplayer_remote_command");

	return quark;
}

/* This should really be standard. */
#define ENUM_ENTRY(NAME, DESC) { NAME, "" #NAME "", DESC }

GType
lmplayer_remote_command_get_type (void)
{
	static GType etype = 0;

	if (etype == 0) {
		static const GEnumValue values[] = {
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_UNKNOWN, "Unknown command"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_PLAY, "Play"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_PAUSE, "Pause"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_STOP, "Stop"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_PLAYPAUSE, "Play or pause"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_NEXT, "Next file"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_PREVIOUS, "Previous file"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_SEEK_FORWARD, "Seek forward"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_SEEK_BACKWARD, "Seek backward"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_VOLUME_UP, "Volume up"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_VOLUME_DOWN, "Volume down"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_FULLSCREEN, "Fullscreen"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_QUIT, "Quit"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_ENQUEUE, "Enqueue"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_REPLACE, "Replace"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_SHOW, "Show"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_TOGGLE_CONTROLS, "Toggle controls"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_UP, "Up"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_DOWN, "Down"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_LEFT, "Left"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_RIGHT, "Right"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_SELECT, "Select"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_DVD_MENU, "DVD menu"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_ZOOM_UP, "Zoom up"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_ZOOM_DOWN, "Zoom down"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_EJECT, "Eject"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_PLAY_DVD, "Play DVD"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_MUTE, "Mute"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_TOGGLE_ASPECT, "Toggle Aspect Ratio"),
			{ 0, NULL, NULL }
		};

		etype = g_enum_register_static ("LmplayerRemoteCommand", values);
	}

	return etype;
}

void
lmplayer_action_remote (LmplayerObject *lmplayer, LmplayerRemoteCommand cmd, const char *url)
{
	gboolean handled = TRUE;

	switch (cmd) {
	case LMPLAYER_REMOTE_COMMAND_PLAY:
		lmplayer_action_play(lmplayer);
		break;
	case LMPLAYER_REMOTE_COMMAND_PLAYPAUSE:
		lmplayer_action_play_pause(lmplayer);
		break;
	case LMPLAYER_REMOTE_COMMAND_PAUSE:
		lmplayer_action_pause(lmplayer);
		break;
	case LMPLAYER_REMOTE_COMMAND_STOP:
		{
			char *mrl, *subtitle;
			lmplayer_playlist_set_at_start(lmplayer->playlist);
			update_buttons(lmplayer);
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
	case LMPLAYER_REMOTE_COMMAND_SEEK_FORWARD:
		{
			double offset = 0;

			if (url != NULL)
				offset = g_ascii_strtod(url, NULL);
			if (offset == 0)
				lmplayer_action_seek_relative(lmplayer, SEEK_FORWARD_OFFSET * 1000);
			else
				lmplayer_action_seek_relative(lmplayer,  offset * 1000);
			break;
		}
	case LMPLAYER_REMOTE_COMMAND_SEEK_BACKWARD:
		{
			double offset = 0;

			if (url != NULL)
				offset = g_ascii_strtod(url, NULL);
			if (offset == 0)
				lmplayer_action_seek_relative(lmplayer, SEEK_BACKWARD_OFFSET * 1000);
			else
				lmplayer_action_seek_relative(lmplayer,  - (offset * 1000));
			break;
		}
	case LMPLAYER_REMOTE_COMMAND_VOLUME_UP:
		lmplayer_action_volume_relative(lmplayer, VOLUME_UP_OFFSET);
		break;
	case LMPLAYER_REMOTE_COMMAND_VOLUME_DOWN:
		lmplayer_action_volume_relative(lmplayer, VOLUME_DOWN_OFFSET);
		break;
	case LMPLAYER_REMOTE_COMMAND_NEXT:
		lmplayer_action_next(lmplayer);
		break;
	case LMPLAYER_REMOTE_COMMAND_PREVIOUS:
		lmplayer_action_previous(lmplayer);
		break;
	case LMPLAYER_REMOTE_COMMAND_FULLSCREEN:
		g_print(_("Do nothing!\n"));
		break;
	case LMPLAYER_REMOTE_COMMAND_QUIT:
		lmplayer_action_exit(lmplayer);
		break;
	case LMPLAYER_REMOTE_COMMAND_ENQUEUE:
		g_assert(url != NULL);
		lmplayer_playlist_add_mrl_with_cursor(lmplayer->playlist, url, NULL);
		break;
	case LMPLAYER_REMOTE_COMMAND_REPLACE:
		lmplayer_playlist_clear(lmplayer->playlist);
		if(url == NULL)
		{
			bacon_video_widget_close(lmplayer->bvw);
			lmplayer_file_closed(lmplayer);
			lmplayer_action_set_mrl(lmplayer, NULL, NULL);
			break;
		}
		if(strcmp(url, "dvd:") == 0)
		{
			lmplayer_action_play_media(lmplayer, MEDIA_TYPE_DVD, NULL);
		}
		else if(strcmp(url, "vcd:") == 0)
		{
			lmplayer_action_play_media(lmplayer, MEDIA_TYPE_VCD, NULL);
		}
		else if(g_str_has_prefix(url, "dvb:") != FALSE)
		{
			lmplayer_action_load_media(lmplayer, MEDIA_TYPE_DVB, "0");
		}
		else
			lmplayer_playlist_add_mrl_with_cursor(lmplayer->playlist, url, NULL);
		break;
	case LMPLAYER_REMOTE_COMMAND_SHOW:
		gtk_window_present(GTK_WINDOW(lmplayer->win));
		break;
	case LMPLAYER_REMOTE_COMMAND_TOGGLE_CONTROLS:
		g_print(_("Do nothing!\n"));
		break;
	case LMPLAYER_REMOTE_COMMAND_SHOW_PLAYING:
		{
			char *title;
			gboolean custom;

			title = lmplayer_playlist_get_current_title(lmplayer->playlist, &custom);
			bacon_message_connection_send(lmplayer->conn,
					title ? title : SHOW_PLAYING_NO_TRACKS);
			g_free (title);
		}
		break;
	case LMPLAYER_REMOTE_COMMAND_SHOW_VOLUME:
		{
			char *vol_str;
			int vol;

			if (bacon_video_widget_can_set_volume(lmplayer->bvw) == FALSE)
				vol = 0;
			else
				vol = bacon_video_widget_get_volume(lmplayer->bvw);
			vol_str = g_strdup_printf ("%d", vol);
			bacon_message_connection_send(lmplayer->conn, vol_str);
			g_free (vol_str);
		}
		break;
	case LMPLAYER_REMOTE_COMMAND_UP:
		g_print(_("Do nothing!\n"));
		break;
	case LMPLAYER_REMOTE_COMMAND_DOWN:
		g_print(_("Do nothing!\n"));
		break;
	case LMPLAYER_REMOTE_COMMAND_LEFT:
		g_print(_("Do nothing!\n"));
		break;
	case LMPLAYER_REMOTE_COMMAND_RIGHT:
		g_print(_("Do nothing!\n"));
		break;
	case LMPLAYER_REMOTE_COMMAND_SELECT:
		g_print(_("Do nothing!\n"));
		break;
	case LMPLAYER_REMOTE_COMMAND_DVD_MENU:
		g_print(_("Do nothing!\n"));
		break;
	case LMPLAYER_REMOTE_COMMAND_ZOOM_UP:
		g_print(_("Do nothing!\n"));
		break;
	case LMPLAYER_REMOTE_COMMAND_ZOOM_DOWN:
		g_print(_("Do nothing!\n"));
		break;
	case LMPLAYER_REMOTE_COMMAND_EJECT:
		g_print(_("Do nothing!\n"));
		break;
	case LMPLAYER_REMOTE_COMMAND_PLAY_DVD:
		g_print(_("Do nothing!\n"));
		break;
	case LMPLAYER_REMOTE_COMMAND_MUTE:
		lmplayer_action_volume_relative(lmplayer, -1.0);
		break;
	default:
		handled = FALSE;
		break;
	}
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
update_volume_slider(LmplayerObject *lmplayer)
{
	double volume;

	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));
	volume = bacon_video_widget_get_volume(lmplayer->bvw);
	gtk_scale_button_set_value(GTK_SCALE_BUTTON(lmplayer->volume), volume);
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
lmplayer_info_update(LmplayerObject *lmplayer)
{
#if 0
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

#if 0
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
#endif

	g_free (artist);
	g_free (title);
	g_free (album);
	g_free (duration);
#endif
}


static gboolean
on_eos_event (GtkWidget *widget, LmplayerObject *lmplayer)
{
	lmplayer_debug(" ");

	if(lmplayer->repeat_one)
	{
		lmplayer_action_stop(lmplayer);
		lmplayer_action_play(lmplayer);
		return FALSE;
	}

	if(lmplayer_playlist_has_next_mrl(lmplayer->playlist) == FALSE
			&& lmplayer_playlist_get_repeat(lmplayer->playlist) == FALSE)
	{
		char *mrl, *subtitle;

		lmplayer_playlist_set_at_start(lmplayer->playlist);
		lmplayer_action_stop (lmplayer);
		mrl = lmplayer_playlist_get_current_mrl(lmplayer->playlist, &subtitle);
		lmplayer_action_set_mrl_with_warning(lmplayer, mrl, subtitle, FALSE);
		bacon_video_widget_pause(lmplayer->bvw);
		g_free(mrl);
		g_free(subtitle);
	} 
	else 
	{
		lmplayer_action_next(lmplayer);
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
	
	lmplayer_playlist_set_stream_length(LMPLAYER_PLAYLIST(lmplayer->playlist), lmplayer->stream_length);
	lmplayer_info_update(lmplayer);
	playlist_active_name_changed_cb (LMPLAYER_PLAYLIST (lmplayer->playlist), lmplayer);

#if 0
	if(lmplayer->has_lyric == FALSE && lmplayer->lyric_downloaded == FALSE)
	{
		lmplayer_load_net_lyric(lmplayer);
	}
#endif
}

static void
on_error_event (BaconVideoWidget *bvw, char *message,
                gboolean playback_stopped, gboolean fatal, LmplayerObject *lmplayer)
{
	lmplayer->seek_to = 0;
	
	if(playback_stopped)
		play_pause_set_label(lmplayer, STATE_STOPPED);

	if(fatal == FALSE)
	{
		lmplayer_action_error(_("An error occurred"), message, lmplayer);
	}
	else
	{
		lmplayer_action_error_and_exit(_("An error occurred"),
				message, lmplayer);
	}
}

static void
on_buffering_event (BaconVideoWidget *bvw, int percentage, LmplayerObject *lmplayer)
{
}

static void
update_current_time (BaconVideoWidget *bvw,
		gint64 current_time,
		gint64 stream_length,
		double current_position,
		gboolean seekable, LmplayerObject *lmplayer)
{
	if(lmplayer->seek_lock == FALSE)
	{
		if(stream_length == 0 && lmplayer->mrl != NULL)
		{
		} 
		else 
		{
			//lmplayer_statusbar_set_time_and_length
			//	(LMPLAYER_STATUSBAR(lmplayer->statusbar),
			//	(int) (current_time / 1000),
			//	(int) (stream_length / 1000));

			gtk_range_set_range(GTK_RANGE(lmplayer->seek), 0.0, (gdouble)stream_length);
			gtk_range_set_value(GTK_RANGE(lmplayer->seek), (gdouble)current_time);

			//if(lmplayer->has_lyric)
			//	skin_lyric_set_current_second(lmplayer->lyricview, current_time / 1000);
		}
	}

	if (lmplayer->stream_length != stream_length) 
	{
		g_object_notify (G_OBJECT (lmplayer), "stream-length");
		lmplayer->stream_length = stream_length;
	}
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

void
video_widget_create (LmplayerObject *lmplayer) 
{
	BaconVideoWidget **bvw;
	GError *err = NULL;
	GtkWidget *box;

	lmplayer->bvw = BACON_VIDEO_WIDGET(bacon_video_widget_new(-1, -1, BVW_USE_TYPE_VIDEO, &err));

	if (lmplayer->bvw == NULL) 
	{
		lmplayer_action_error_and_exit(_("Lmplayer could not startup."), 
				err != NULL ? err->message : _("No reason."), 
				lmplayer);
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

	box = (GtkWidget*)gtk_builder_get_object(lmplayer->builder, "player-visualbox");
	gtk_container_add(GTK_CONTAINER(box), GTK_WIDGET(lmplayer->bvw));

	bvw = &(lmplayer->bvw);
	g_object_add_weak_pointer(G_OBJECT(lmplayer->bvw), (gpointer *)bvw);

	gtk_widget_show(GTK_WIDGET(lmplayer->bvw));

	//bacon_video_widget_set_visuals(lmplayer->bvw, "GOOM:what a GOOM!");

	bacon_video_widget_set_volume (lmplayer->bvw,
			((double) gconf_client_get_int (lmplayer->gc,
				GCONF_PREFIX"/volume", NULL)) / 100.0);

	g_signal_connect (G_OBJECT (lmplayer->bvw), "notify::volume",
			G_CALLBACK (property_notify_cb_volume), lmplayer);
	g_signal_connect (G_OBJECT (lmplayer->bvw), "notify::seekable",
			G_CALLBACK (property_notify_cb_seekable), lmplayer);

	update_volume_slider(lmplayer); //TODO:
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
	play_pause_set_label(lmplayer, STATE_STOPPED);
	mrl = lmplayer_playlist_get_current_mrl(lmplayer->playlist, &subtitle);

	if (mrl == NULL) 
	{
		g_free (subtitle);
		subtitle = NULL;
		lmplayer_playlist_set_at_start(lmplayer->playlist);
		update_buttons(lmplayer);
		mrl = lmplayer_playlist_get_current_mrl(lmplayer->playlist, &subtitle);
	} 
	else 
	{
		update_buttons(lmplayer);
	}

	lmplayer_action_set_mrl_and_play(lmplayer, mrl, subtitle);
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
}

static void
playlist_shuffle_toggled_cb (LmplayerPlaylist *playlist, gboolean shuffle, LmplayerObject *lmplayer)
{
	lmplayer_debug("playlist_shuffle_toggled_cb");
}

void 
playlist_widget_setup(LmplayerObject *lmplayer)
{
	GtkWidget *box;
	LmplayerPlaylist *playlist = NULL;

	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	playlist = LMPLAYER_PLAYLIST(lmplayer_playlist_new());
	lmplayer->playlist = playlist;

	if(lmplayer->playlist == NULL)
		lmplayer_action_exit(lmplayer);
	
	box = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-playlistbox");
	gtk_container_add(GTK_CONTAINER(box), GTK_WIDGET(lmplayer->playlist));

	gtk_widget_show_all(GTK_WIDGET(lmplayer->playlist));
	gtk_widget_show(box);

	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(lmplayer->playlist), TRUE);
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

