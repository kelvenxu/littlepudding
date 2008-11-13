/* vi: set sw=4 ts=4: */
/*
 * lmplayer-menu.c
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

#include "lmplayer-menu.h"
#include "lmplayer.h"

void play_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void pause_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void next_music_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void prev_music_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void stop_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void open_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void quit_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void minimode_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void minimize_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void lyric_close_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void lyric_ontop_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void lyric_show_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void playlist_show_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void playlist_close_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void eq_show_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void eq_close_action_callback(GtkAction *action, LmplayerObject *lmplayer);

void play_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_play_pause(lmplayer);
}

void pause_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_play_pause(lmplayer);
}

void next_music_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_next(lmplayer);
}

void prev_music_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_previous(lmplayer);
}

void stop_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_play_pause(lmplayer);
}

void open_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_open(lmplayer);
}

void quit_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_exit(lmplayer);
}

void minimode_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
}

void minimize_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
}

void lyric_close_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
}

void lyric_ontop_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
}

void lyric_show_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
}

void playlist_show_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
}

void playlist_close_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
}

void eq_show_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
}

void eq_close_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
}

void lmplayer_ui_manager_setup (LmplayerObject *lmplayer)
{
	SkinButton *button;
	SkinBuilder *builder;

	builder = lmplayer->builder;

	// player window
	button = (SkinButton*)skin_builder_get_object(builder, "player-open");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(open_action_callback), lmplayer);
	
	button = (SkinButton*)skin_builder_get_object(builder, "player-play");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(play_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-pause");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(pause_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-next");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(next_music_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-prev");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(prev_music_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-stop");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(stop_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-minimode");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(minimode_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-minimize");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(minimize_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-exit");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(quit_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-playlist");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(playlist_show_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-lyric");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(lyric_show_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-equalizer");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(eq_show_action_callback), lmplayer);

	// lyric window
	button = (SkinButton*)skin_builder_get_object(builder, "lyric-ontop");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(lyric_ontop_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "lyric-close");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(lyric_close_action_callback), lmplayer);
	
	// playlist window
	button = (SkinButton*)skin_builder_get_object(builder, "playlist-close");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(playlist_close_action_callback), lmplayer);

	// equalizer window
	button = (SkinButton*)skin_builder_get_object(builder, "equalizer-close");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(eq_close_action_callback), lmplayer);
	
	// mini mode
	button = (SkinButton*)skin_builder_get_object(builder, "mini-open");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(open_action_callback), lmplayer);
	
	button = (SkinButton*)skin_builder_get_object(builder, "mini-play");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(play_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-pause");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(pause_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-next");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(next_music_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-prev");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(prev_music_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-stop");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(stop_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-exit");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(quit_action_callback), lmplayer);
}

