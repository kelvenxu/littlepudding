/* lmplayer-uri.h

   Copyright (C) 2004 Bastien Nocera <hadess@hadess.net>

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

#ifndef _LMPLAYER_URI_H_
#define _LMPLAYER_URI_H_

#include "lmplayer-object.h"

G_BEGIN_DECLS

const char *	lmplayer_dot_dir			(void);
const char *	lmplayer_data_dot_dir			(void);
char *		lmplayer_pictures_dir		(void);
char *		lmplayer_create_full_path		(const char *path);
GMount *	lmplayer_get_mount_for_media	(const char *uri);
gboolean	lmplayer_playing_dvd		(const char *uri);
gboolean	lmplayer_is_block_device		(const char *uri);
void		lmplayer_setup_file_monitoring	(LmplayerObject *lmplayer);
void		lmplayer_setup_file_filters	(void);
void		lmplayer_destroy_file_filters	(void);
char *		lmplayer_uri_get_subtitle_uri	(const char *uri);
char *		lmplayer_uri_escape_for_display	(const char *uri);
GSList *	lmplayer_add_files			(GtkWindow *parent,
						 const char *path);
char *		lmplayer_add_subtitle		(GtkWindow *parent, 
						 const char *path);
void		lmplayer_add_pictures_dir		(GtkWidget *chooser);

G_END_DECLS

#endif /* _LMPLAYER_URI_H_ */
