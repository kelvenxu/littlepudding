/* vi: set sw=4 ts=4: */
/*
 * lmplayer-object.h
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

#ifndef __LMPLAYER_OBJECT_H__
#define __LMPLAYER_OBJECT_H__  1

#include <glib-object.h>
#include <gio/gio.h>
#include <gconf/gconf-client.h>
#include <gtk/gtk.h>
#include "bacon-message-connection.h"
#include <totem-pl-parser.h>
#include "bacon-video-widget.h"
#include "lmplayer-notebook.h"
#include "lmplayer-playlist.h"

G_BEGIN_DECLS

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

struct _LmplayerObject
{
	GObject parent;

	//gpointer player;
	//gpointer ui;

	GtkBuilder *xml;
	GtkWidget *win;
	LmplayerNotebook *notebook;
	BaconVideoWidget *bvw;
	GtkWidget *statusbar;

	GConfClient *gc;

	LmplayerPlaylist *playlist;
	//GtkWidget *
	
	/* Volume */
	GtkWidget *volume;
	gboolean volume_sensitive;

	GtkActionGroup *main_action_group;
	GtkUIManager *ui_manager;

	BaconMessageConnection* conn;
	GVolumeMonitor* monitor;
	gboolean drives_changed;

	/* Seek */
	GtkWidget *seek;
	GtkAdjustment *seekadj;
	gboolean seek_lock;
	gboolean seekable;

	gint index;
	gint seek_to;

	gboolean autoload_subs;
	gchar *mrl;
	gchar *pls; //notebook播放列表的配置文件
	//LmplayerObjectPrivate *priv;

	LmplayerStates state;
	gint64 stream_length;
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

void lmplayer_file_opened (LmplayerObject *lmplayer, const char *mrl);
void lmplayer_file_closed (LmplayerObject *lmplayer);
void lmplayer_metadata_updated (LmplayerObject *lmplayer,
			const char *artist,
			const char *title,
			const char *album);
void lmplayer_metadata_updated (LmplayerObject *lmplayer, const char *artist, const char *title, const char *album);

GtkWindow *lmplayer_get_main_window(LmplayerObject *lmplayer);
GtkUIManager *lmplayer_get_ui_manager(LmplayerObject *lmplayer);
GtkWidget *lmplayer_get_video_widget(LmplayerObject *lmplayer);
char *lmplayer_get_video_widget_backend_name(LmplayerObject *lmplayer);
gint64 lmplayer_get_current_time(LmplayerObject *lmplayer);
gboolean lmplayer_is_fullscreen(LmplayerObject *lmplayer);
gboolean lmplayer_is_playing (LmplayerObject *lmplayer);
gboolean lmplayer_is_paused (LmplayerObject *lmplayer);
gboolean lmplayer_is_seekable (LmplayerObject *lmplayer);


G_END_DECLS
#endif /*__LMPLAYER_OBJECT_H__ */
