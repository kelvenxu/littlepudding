/* vi: set sw=4 ts=4: */
/*
 * lmplayer.h
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

#ifndef __LMPLAYER_H__
#define __LMPLAYER_H__  1

G_BEGIN_DECLS

#include <glib-object.h>
#include <gtk/gtk.h>
#include <totem-disc.h>

#define SHOW_PLAYING_NO_TRACKS "NONE"
#define LMPLAYER_GCONF_PREFIX "/apps/lmplayer"

typedef enum {
	LMPLAYER_REMOTE_COMMAND_UNKNOWN,
	LMPLAYER_REMOTE_COMMAND_PLAY,
	LMPLAYER_REMOTE_COMMAND_PAUSE,
	LMPLAYER_REMOTE_COMMAND_STOP,
	LMPLAYER_REMOTE_COMMAND_PLAYPAUSE,
	LMPLAYER_REMOTE_COMMAND_NEXT,
	LMPLAYER_REMOTE_COMMAND_PREVIOUS,
	LMPLAYER_REMOTE_COMMAND_SEEK_FORWARD,
	LMPLAYER_REMOTE_COMMAND_SEEK_BACKWARD,
	LMPLAYER_REMOTE_COMMAND_VOLUME_UP,
	LMPLAYER_REMOTE_COMMAND_VOLUME_DOWN,
	LMPLAYER_REMOTE_COMMAND_FULLSCREEN,
	LMPLAYER_REMOTE_COMMAND_QUIT,
	LMPLAYER_REMOTE_COMMAND_ENQUEUE,
	LMPLAYER_REMOTE_COMMAND_REPLACE,
	LMPLAYER_REMOTE_COMMAND_SHOW,
	LMPLAYER_REMOTE_COMMAND_TOGGLE_CONTROLS,
	LMPLAYER_REMOTE_COMMAND_UP,
	LMPLAYER_REMOTE_COMMAND_DOWN,
	LMPLAYER_REMOTE_COMMAND_LEFT,
	LMPLAYER_REMOTE_COMMAND_RIGHT,
	LMPLAYER_REMOTE_COMMAND_SELECT,
	LMPLAYER_REMOTE_COMMAND_DVD_MENU,
	LMPLAYER_REMOTE_COMMAND_ZOOM_UP,
	LMPLAYER_REMOTE_COMMAND_ZOOM_DOWN,
	LMPLAYER_REMOTE_COMMAND_EJECT,
	LMPLAYER_REMOTE_COMMAND_PLAY_DVD,
	LMPLAYER_REMOTE_COMMAND_MUTE,
	LMPLAYER_REMOTE_COMMAND_TOGGLE_ASPECT
} LmplayerRemoteCommand;

GType lmplayer_remote_command_get_type(void);
GQuark lmplayer_remote_command_quark(void);

#define LMPLAYER_TYPE_REMOTE_COMMAND	(lmplayer_remote_command_get_type())
#define LMPLAYER_REMOTE_COMMAND		lmplayer_remote_command_quark()

//#if 0
/**
 * LmplayerRemoteSetting:
 * @LMPLAYER_REMOTE_SETTING_SHUFFLE: whether shuffle is enabled
 * @LMPLAYER_REMOTE_SETTING_REPEAT: whether repeat is enabled
 *
 * Represents a boolean setting or preference on a remote Lmplayer instance.
 **/

//typedef enum 
//{
//	LMPLAYER_REMOTE_SETTING_SHUFFLE,
//	LMPLAYER_REMOTE_SETTING_REPEAT
//} LmplayerRemoteSetting;


//GType lmplayer_remote_setting_get_type	(void);
//GQuark lmplayer_remote_setting_quark	(void);

//#define LMPLAYER_TYPE_REMOTE_SETTING	(lmplayer_remote_setting_get_type())
//#define LMPLAYER_REMOTE_SETTING		lmplayer_remote_setting_quark ()
//#endif

#define LMPLAYER_TYPE_OBJECT (lmplayer_object_get_type ())
#define LMPLAYER_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), LMPLAYER_TYPE_OBJECT, LmplayerObject))
#define LMPLAYER_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), LMPLAYER_TYPE_OBJECT, LmplayerObjectClass))
#define LMPLAYER_IS_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LMPLAYER_TYPE_OBJECT))
#define LMPLAYER_IS_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LMPLAYER_TYPE_OBJECT))
#define LMPLAYER_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), LMPLAYER_TYPE_OBJECT, LmplayerObjectClass))

typedef struct _LmplayerObject			LmplayerObject;
typedef struct _LmplayerObject			Lmplayer;
typedef struct _LmplayerObjectClass		LmplayerObjectClass;
typedef struct _LmplayerObjectPrivate	LmplayerObjectPrivate;

struct _LmplayerObjectClass
{
	GObjectClass parent;

	void (*file_opened)			(LmplayerObject *lmplayer, const char *mrl);
	void (*file_closed)			(LmplayerObject *lmplayer);
	void (*metadata_updated) (LmplayerObject *lmplayer, const char *artist, const char *title, const char *album);
};


GType lmplayer_object_get_type();

LmplayerObject* lmplayer_object_new();

void lmplayer_file_opened (LmplayerObject *lmplayer, const char *mrl);
void lmplayer_file_closed (LmplayerObject *lmplayer);

void lmplayer_metadata_updated (LmplayerObject *lmplayer, 
																const char *artist, 
																const char *title, 
																const char *album);

gboolean lmplayer_is_fullscreen(LmplayerObject *lmplayer);
gboolean lmplayer_is_playing (LmplayerObject *lmplayer);
gboolean lmplayer_is_paused (LmplayerObject *lmplayer);
gboolean lmplayer_is_seekable (LmplayerObject *lmplayer);

void lmplayer_action_play (LmplayerObject *lmplayer);

void lmplayer_action_play_media (LmplayerObject *lmplayer, 
																	TotemDiscMediaType type, 
																	const char *device);

void lmplayer_action_stop (LmplayerObject *lmplayer);
void lmplayer_action_play_pause (LmplayerObject *lmplayer);
void lmplayer_action_pause (LmplayerObject *lmplayer);
void lmplayer_action_seek (LmplayerObject *lmplayer, double pos);
void lmplayer_action_exit (LmplayerObject *lmplayer);

void lmplayer_action_error (const char *title, 
														const char *reason, 
														LmplayerObject *lmplayer);

gboolean lmplayer_action_set_mrl_with_warning (LmplayerObject *lmplayer,
																							 const char *mrl, 
																							 const char *subtitle, 
																							 gboolean warn);

gboolean lmplayer_action_set_mrl (LmplayerObject *lmplayer, 
																	const char *mrl, 
																	const char *subtitle);

void lmplayer_action_set_mrl_and_play (LmplayerObject *lmplayer, 
																				const char *mrl, 
																				const char *subtitle);

void lmplayer_action_previous (LmplayerObject *lmplayer);
void lmplayer_action_next (LmplayerObject *lmplayer);

void lmplayer_action_minimize(LmplayerObject *lmplayer);
void lmplayer_action_minimode(LmplayerObject *lmplayer, gboolean minimode);

void lmplayer_action_volume_relative (LmplayerObject *lmplayer, double off_pct);

//void lmplayer_action_load_lyric(LmplayerObject *lmplayer);
void lmplayer_action_load_default_playlist(LmplayerObject *lmplayer);
void lmplayer_action_seek_relative(LmplayerObject *lmplayer, gint64 offset);

// FIXME:
void lmplayer_load_net_lyric(LmplayerObject *lmplayer);

// FIXME:
void lmplayer_action_change_skin(LmplayerObject *lmplayer);

void lmplayer_object_plugins_init(LmplayerObject *lmplayer);
void lmplayer_object_plugins_shutdown (void);

void lmplayer_add_tools_button(LmplayerObject *lmplayer, GtkWidget *button);
void lmplayer_remove_tools_button(LmplayerObject *lmplayer, GtkWidget *button);

GtkWindow * lmplayer_get_main_window(LmplayerObject *lmplayer);
GtkUIManager *lmplayer_get_ui_manager(LmplayerObject *lmplayer);
GtkWidget *lmplayer_get_video_widget(LmplayerObject *lmplayer);
char *lmplayer_get_video_widget_backend_name(LmplayerObject *lmplayer);
gint64 lmplayer_get_current_time(LmplayerObject *lmplayer);
gint64 lmplayer_get_stream_length(LmplayerObject *lmplayer);

gboolean lmplayer_action_set_mrl_with_warning (LmplayerObject *lmplayer,
																							 const char *mrl, 
																							 const char *subtitle,
																							 gboolean warn);

void lmplayer_action_remote (LmplayerObject *lmplayer, 
															LmplayerRemoteCommand cmd, 
															const char *url);

G_END_DECLS

#endif /*__LMPLAYER_H__ */
