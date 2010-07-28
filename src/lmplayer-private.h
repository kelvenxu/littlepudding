/* vi: set sw=4 ts=4: */
/*
 * lmplayer-private.h
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

#ifndef __LMPLAYER_PRIVATE_H__
#define __LMPLAYER_PRIVATE_H__  1

#include <glib-object.h>
#include <glib.h>
#include <gconf/gconf-client.h>
#include <gtk/gtk.h>
#include <unique/uniqueapp.h>

#include "bacon-video-widget.h"
#include "lmplayer-playlist.h"
#include "lmplayer-open-location.h"
#include "lmplayer-notebook.h"
#include "lmplayer.h"

G_BEGIN_DECLS

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

	GtkBuilder *builder;
	GtkWidget *win;

	GtkWidget *view;
	GtkWidget *view_switch;
	GtkWidget *order_switch_button;

	LmplayerViewType view_type;

	GtkWidget *about_button;

	gboolean repeat;
	gboolean repeat_one;

	BaconVideoWidget *bvw;

	GConfClient *gc;

	LmplayerPlaylist *playing_playlist; // playing playlist
	LmplayerPlaylist *current_playlist;
	LmplayerNotebook *playlist_notebook;

	GtkWidget *playlist_view_button;

	GtkWidget *search_box_box;
	GtkWidget *search_box;
	GtkWidget *search_view;
	GtkWidget *search_view_button;
	
	gboolean minimode;

	/* Volume */
	GtkWidget *volume; //FIXME:
	gboolean volume_sensitive;

	//GtkActionGroup *main_action_group;
	GtkUIManager *menus;

	UniqueApp *uapp;
	GVolumeMonitor* monitor;
	gboolean drives_changed;

	/* Seek */
	GtkWidget *seek;
	GtkAdjustment *seekadj;
	gboolean seek_lock;
	gboolean seekable;

	GtkWidget *statusbar;
	GtkWidget *statusbar_box;

	GtkWidget *current_time_label;
	GtkWidget *total_time_label;

	gint64 stream_length;

	gint index;
	gint seek_to;

	gboolean autoload_subs;
	gchar *mrl;
	gchar *pls; //notebook播放列表的配置文件
	//LmplayerObjectPrivate *priv;

	GtkWidget *prefs_button;
	GtkWidget *prefs;

	GtkWidget *plugin_button;
	GtkWidget *plugins_box;
	GtkWidget *plugins_manager_dialog;

	gchar *skinfile;

	LmplayerStates state;

	LmplayerOpenLocation *open_location;
	GtkWidget *extra_widget_box;
};

#define SEEK_FORWARD_OFFSET 60
#define SEEK_BACKWARD_OFFSET -15

#define VOLUME_DOWN_OFFSET (-0.08)
#define VOLUME_UP_OFFSET (0.08)

#define ZOOM_IN_OFFSET 1
#define ZOOM_OUT_OFFSET -1

void lmplayer_action_open(LmplayerObject *lmplayer);
void lmplayer_action_open_location(LmplayerObject *lmplayer);

void lmplayer_action_error_and_exit(const char *title, 
																		const char *reason, 
																		LmplayerObject *lmplayer);

gboolean lmplayer_action_open_files (LmplayerObject *lmplayer, char **list);

void video_widget_create (LmplayerObject *lmplayer);
void playlist_widget_setup(LmplayerObject *lmplayer);
GtkWidget *lmplayer_create_playlist_widget(LmplayerObject *lmplayer, const char *filename);
void lmplayer_setup_statusbar(LmplayerObject *lmplayer);

G_END_DECLS

#endif /*__LMPLAYER_PRIVATE_H__ */
