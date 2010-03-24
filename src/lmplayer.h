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

#include "lmplayer-object.h"

#define SHOW_PLAYING_NO_TRACKS "NONE"

void lmplayer_action_play (LmplayerObject *lmplayer);
void lmplayer_action_play_media (LmplayerObject *lmplayer, TotemDiscMediaType type, const char *device);
void lmplayer_action_stop (LmplayerObject *lmplayer);
void lmplayer_action_play_pause (LmplayerObject *lmplayer);
void lmplayer_action_pause (LmplayerObject *lmplayer);
void lmplayer_action_seek (LmplayerObject *lmplayer, double pos);
void lmplayer_action_exit (LmplayerObject *lmplayer);
void lmplayer_action_error (const char *title, const char *reason, LmplayerObject *lmplayer);
gboolean lmplayer_action_set_mrl_with_warning (LmplayerObject *lmplayer,
				   const char *mrl, 
				   const char *subtitle,
				   gboolean warn);
gboolean lmplayer_action_set_mrl (LmplayerObject *lmplayer, const char *mrl, const char *subtitle);
void lmplayer_action_set_mrl_and_play (LmplayerObject *lmplayer, const char *mrl, const char *subtitle);
void lmplayer_action_play_pause (LmplayerObject *lmplayer);

void lmplayer_action_previous (LmplayerObject *lmplayer);
void lmplayer_action_next (LmplayerObject *lmplayer);

void lmplayer_action_minimize(LmplayerObject *lmplayer);
void lmplayer_action_minimode(LmplayerObject *lmplayer, gboolean minimode);

void lmplayer_action_volume_relative (LmplayerObject *lmplayer, double off_pct);

//void lmplayer_action_load_lyric(LmplayerObject *lmplayer);
void lmplayer_action_load_default_playlist(LmplayerObject *lmplayer);
void lmplayer_action_seek_relative(LmplayerObject *lmplayer, gint64 offset);

void lmplayer_load_net_lyric(LmplayerObject *lmplayer);

void lmplayer_action_change_skin(LmplayerObject *lmplayer);

void lmplayer_object_plugins_init(LmplayerObject *lmplayer);
void lmplayer_object_plugins_shutdown (void);

void lmplayer_add_tools_button(LmplayerObject *lmplayer, GtkWidget *button, const char *id);
void lmplayer_remove_tools_button(LmplayerObject *lmplayer, const char *id);

GtkWindow * lmplayer_get_main_window(LmplayerObject *lmplayer);
GtkUIManager *lmplayer_get_ui_manager(LmplayerObject *lmplayer);
GtkWidget *lmplayer_get_video_widget(LmplayerObject *lmplayer);
char *lmplayer_get_video_widget_backend_name(LmplayerObject *lmplayer);
gint64 lmplayer_get_current_time(LmplayerObject *lmplayer);

#endif /*__LMPLAYER_H__ */
