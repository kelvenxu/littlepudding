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

void lmplayer_action_load_lyric(LmplayerObject *lmplayer);
#endif /*__LMPLAYER_H__ */
