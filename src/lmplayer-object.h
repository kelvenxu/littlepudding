/* vi: set sw=4 ts=4: */
/*
 * lmplayer-object.h
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

#ifndef __LMPLAYER_OBJECT_H__
#define __LMPLAYER_OBJECT_H__  1

#include <glib-object.h>
#include <gio/gio.h>
#include <gconf/gconf-client.h>
#include <gtk/gtk.h>
//#include <libskin/skin.h>

#include "bacon-message-connection.h"
#include <totem-pl-parser.h>
#include "bacon-video-widget.h"
#include "lmplayer-notebook.h"
#include "lmplayer-playlist.h"
#include "lmplayer-open-location.h"

G_BEGIN_DECLS

#define SEEK_FORWARD_OFFSET 60
#define SEEK_BACKWARD_OFFSET -15

#define VOLUME_DOWN_OFFSET (-0.08)
#define VOLUME_UP_OFFSET (0.08)

#define ZOOM_IN_OFFSET 1
#define ZOOM_OUT_OFFSET -1


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

typedef enum
{
	STATE_PLAYING,
	STATE_PAUSED,
	STATE_STOPPED
} LmplayerStates;

typedef enum
{
	LMPLAYER_VIEW_TYPE_PLAYLIST,
	LMPLAYER_VIEW_TYPE_VISUAL,
	LMPLAYER_VIEW_TYPE_SEARCH
} LmplayerViewType;

typedef enum
{
	LMPLAYER_ORDER_NORMAL,
	LMPLAYER_ORDER_REPEAT,
	LMPLAYER_ORDER_REPEAT_ONE,
	LMPLAYER_ORDER_NUMBER
} LmplayerOrder;

struct _LmplayerObject
{
	GObject parent;

	//gpointer player;
	//gpointer ui;

	//SkinArchive *ar;
	//SkinBuilder *builder;

	GtkBuilder *builder;
	//SkinWindow *win;
	GtkWidget *win;

	GtkWidget *view;
	LmplayerViewType view_type;
	GtkWidget *view_switch;
	//GtkWidget *order_model;
	GtkWidget *order_switch_button;

	GtkWidget *about_button;

	gboolean repeat;
	gboolean repeat_one;

	//SkinWindow *pl_win;
	//SkinWindow *lyric_win;
	//SkinWindow *eq_win;
	//SkinWindow *mini_win;

	//SkinLyric *lyricview;

	LmplayerNotebook *notebook;
	BaconVideoWidget *bvw;
	GtkWidget *statusbar;

	GConfClient *gc;

	LmplayerPlaylist *playlist;
	GtkWidget *playlist_view_button;

	GtkWidget *search_box_box;
	GtkWidget *search_box;
	GtkWidget *search_view;
	GtkWidget *search_view_button;
	
	gboolean minimode;

	/* Volume */
	GtkWidget *volume; //FIXME:
	//SkinHScale *volume;
	gboolean volume_sensitive;

	//GtkActionGroup *main_action_group;
	GtkUIManager *menus;

	BaconMessageConnection* conn;
	GVolumeMonitor* monitor;
	gboolean drives_changed;

	/* Seek */
	GtkWidget *seek;
	GtkAdjustment *seekadj;
	//SkinHScale *seek;
	gboolean seek_lock;
	gboolean seekable;

	gint64 stream_length;

	//SkinDigitalTime *led;
	//SkinDigitalTime *mini_led;

	gint index;
	gint seek_to;

	gboolean autoload_subs;
	gchar *mrl;
	gchar *pls; //notebook播放列表的配置文件
	//LmplayerObjectPrivate *priv;
	gchar *lyric_filename;
	gboolean has_lyric;
	gboolean lyric_downloaded;
	//gchar *artist;
	//gchar *title;

	GtkWidget *prefs_button;
	GtkWidget *prefs;

	GtkWidget *plugin_button;
	GtkWidget *plugins_box;
	GtkWidget *plugins_manager_dialog;

	gchar *skinfile;

	LmplayerStates state;

	LmplayerOpenLocation *open_location;

	//gchar *database;
	//gchar *monitor_path;
};

struct _LmplayerObjectClass
{
	GObjectClass parent;

	void (*file_opened)			(LmplayerObject *lmplayer, const char *mrl);
	void (*file_closed)			(LmplayerObject *lmplayer);
	void (*metadata_updated)		(LmplayerObject *lmplayer,
						 const char *artist,
						 const char *title,
						 const char *album);
};


GType lmplayer_object_get_type();

LmplayerObject* lmplayer_object_new();

void lmplayer_action_open(LmplayerObject *lmplayer);
void lmplayer_action_open_location(LmplayerObject *lmplayer);

void lmplayer_file_opened (LmplayerObject *lmplayer, const char *mrl);
void lmplayer_file_closed (LmplayerObject *lmplayer);
void lmplayer_metadata_updated (LmplayerObject *lmplayer,
			const char *artist,
			const char *title,
			const char *album);
void lmplayer_metadata_updated (LmplayerObject *lmplayer, const char *artist, const char *title, const char *album);

gboolean lmplayer_is_fullscreen(LmplayerObject *lmplayer);
gboolean lmplayer_is_playing (LmplayerObject *lmplayer);
gboolean lmplayer_is_paused (LmplayerObject *lmplayer);
gboolean lmplayer_is_seekable (LmplayerObject *lmplayer);


G_END_DECLS
#endif /*__LMPLAYER_OBJECT_H__ */
