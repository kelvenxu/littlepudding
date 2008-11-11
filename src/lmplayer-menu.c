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
void next_music_action_callback(GtkAction *action, LmplayerObject *lmplayer);

void play_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_play_pause(lmplayer);
}

void next_music_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_next(lmplayer);
}

void previous_music_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_previous(lmplayer);
}

void lmplayer_ui_manager_setup (LmplayerObject *lmplayer)
{
	lmplayer->main_action_group = GTK_ACTION_GROUP (gtk_builder_get_object (lmplayer->xml, "main-action-group"));

	lmplayer->ui_manager = GTK_UI_MANAGER (gtk_builder_get_object (lmplayer->xml, "lmplayer-ui-manager"));
}

