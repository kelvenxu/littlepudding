/* vi: set sw=4 ts=4: */
/*
 * lmplayer-remote-command.c
 *
 * This file is part of ________.
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

#include "config.h"
#include "lmplayer-remote-command.h"

#include <glib-object.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>

GQuark
lmplayer_remote_command_quark (void)
{
	static GQuark quark = 0;
	if (!quark)
		quark = g_quark_from_static_string ("lmplayer_remote_command");

	return quark;
}

/* This should really be standard. */
#define ENUM_ENTRY(NAME, DESC) { NAME, "" #NAME "", DESC }

GType
lmplayer_remote_command_get_type (void)
{
	static GType etype = 0;

	if (etype == 0) {
		static const GEnumValue values[] = {
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_UNKNOWN, "Unknown command"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_PLAY, "Play"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_PAUSE, "Pause"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_STOP, "Stop"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_PLAYPAUSE, "Play or pause"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_NEXT, "Next file"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_PREVIOUS, "Previous file"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_SEEK_FORWARD, "Seek forward"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_SEEK_BACKWARD, "Seek backward"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_VOLUME_UP, "Volume up"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_VOLUME_DOWN, "Volume down"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_FULLSCREEN, "Fullscreen"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_QUIT, "Quit"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_ENQUEUE, "Enqueue"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_REPLACE, "Replace"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_SHOW, "Show"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_TOGGLE_CONTROLS, "Toggle controls"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_UP, "Up"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_DOWN, "Down"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_LEFT, "Left"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_RIGHT, "Right"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_SELECT, "Select"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_DVD_MENU, "DVD menu"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_ZOOM_UP, "Zoom up"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_ZOOM_DOWN, "Zoom down"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_EJECT, "Eject"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_PLAY_DVD, "Play DVD"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_MUTE, "Mute"),
			ENUM_ENTRY (LMPLAYER_REMOTE_COMMAND_TOGGLE_ASPECT, "Toggle Aspect Ratio"),
			{ 0, NULL, NULL }
		};

		etype = g_enum_register_static ("LmplayerRemoteCommand", values);
	}

	return etype;
}
