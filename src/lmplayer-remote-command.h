/* vi: set sw=4 ts=4: */
/*
 * lmplayer-remote-command.h
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

#ifndef __LMPLAYER_REMOTE_COMMAND_H__
#define __LMPLAYER_REMOTE_COMMAND_H__  1

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

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
	LMPLAYER_REMOTE_COMMAND_SHOW_PLAYING,
	LMPLAYER_REMOTE_COMMAND_SHOW_VOLUME,
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

/**
 * LmplayerRemoteSetting:
 * @LMPLAYER_REMOTE_SETTING_SHUFFLE: whether shuffle is enabled
 * @LMPLAYER_REMOTE_SETTING_REPEAT: whether repeat is enabled
 *
 * Represents a boolean setting or preference on a remote Lmplayer instance.
 **/
typedef enum {
	LMPLAYER_REMOTE_SETTING_SHUFFLE,
	LMPLAYER_REMOTE_SETTING_REPEAT
} LmplayerRemoteSetting;

GType lmplayer_remote_command_get_type(void);
GQuark lmplayer_remote_command_quark(void);

#define LMPLAYER_TYPE_REMOTE_COMMAND	(lmplayer_remote_command_get_type())
#define LMPLAYER_REMOTE_COMMAND		lmplayer_remote_command_quark ()

#endif /*__LMPLAYER_REMOTE_COMMAND_H__ */
