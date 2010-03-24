/* totem-options.h

   Copyright (C) 2004,2007 Bastien Nocera <hadess@hadess.net>

   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: Bastien Nocera <hadess@hadess.net>
 */

#ifndef _LMPLAYER_OPTIONS_H_
#define _LMPLAYER_OPTIONS_H_

#include "bacon-message-connection.h"
#include "lmplayer-remote-command.h"
#include "lmplayer.h"

G_BEGIN_DECLS

/* Stores the state of the command line options */
typedef struct 
{
	gboolean debug;
	gboolean playpause;
	gboolean play;
	gboolean pause;
	gboolean next;
	gboolean previous;
	gboolean seekfwd;
	gboolean seekbwd;
	gboolean volumeup;
	gboolean volumedown;
	gboolean quit;
	gboolean enqueue;
	gboolean replace;
	gboolean printplaying;
	gboolean notconnectexistingsession;
	gdouble playlistidx;
	gint64 seek;
	gchar **filenames;
} LmpCmdLineOptions;

extern const GOptionEntry options[];
extern LmpCmdLineOptions optionstate;

void lmplayer_options_process_early (LmplayerObject* lmp,
				const LmpCmdLineOptions* options);

void lmplayer_options_process_late (LmplayerObject *lmp, 
				const LmpCmdLineOptions* options);

void lmplayer_options_process_for_server (BaconMessageConnection *conn, 
				const LmpCmdLineOptions* options);

G_END_DECLS

#endif /* _LMPLAYER_OPTIONS_H_ */
