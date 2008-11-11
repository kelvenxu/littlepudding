/* vi: set sw=4 ts=4: */
/*
 * lmplayer-remote-command.h
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

#ifndef __LMPLAYER_REMOTE_COMMAND_H__
#define __LMPLAYER_REMOTE_COMMAND_H__  1

G_BEGIN_DECLS

typedef enum {
	LMP_REMOTE_COMMAND_UNKNOWN,
	LMP_REMOTE_COMMAND_PLAY,
	LMP_REMOTE_COMMAND_PAUSE,
	LMP_REMOTE_COMMAND_STOP,
	LMP_REMOTE_COMMAND_PLAYPAUSE,
	LMP_REMOTE_COMMAND_NEXT,
	LMP_REMOTE_COMMAND_PREVIOUS,
	LMP_REMOTE_COMMAND_SEEK_FORWARD,
	LMP_REMOTE_COMMAND_SEEK_BACKWARD,
	LMP_REMOTE_COMMAND_VOLUME_UP,
	LMP_REMOTE_COMMAND_VOLUME_DOWN,
	LMP_REMOTE_COMMAND_FULLSCREEN,
	LMP_REMOTE_COMMAND_QUIT,
	LMP_REMOTE_COMMAND_ENQUEUE,
	LMP_REMOTE_COMMAND_REPLACE,
	LMP_REMOTE_COMMAND_SHOW,
	LMP_REMOTE_COMMAND_TOGGLE_CONTROLS,
	LMP_REMOTE_COMMAND_SHOW_PLAYING,
	LMP_REMOTE_COMMAND_SHOW_VOLUME,
	LMP_REMOTE_COMMAND_UP,
	LMP_REMOTE_COMMAND_DOWN,
	LMP_REMOTE_COMMAND_LEFT,
	LMP_REMOTE_COMMAND_RIGHT,
	LMP_REMOTE_COMMAND_SELECT,
	LMP_REMOTE_COMMAND_DVD_MENU,
	LMP_REMOTE_COMMAND_ZOOM_UP,
	LMP_REMOTE_COMMAND_ZOOM_DOWN,
	LMP_REMOTE_COMMAND_EJECT,
	LMP_REMOTE_COMMAND_PLAY_DVD,
	LMP_REMOTE_COMMAND_MUTE
} LmpRemoteCommand;


#endif /*__LMPLAYER_REMOTE_COMMAND_H__ */
