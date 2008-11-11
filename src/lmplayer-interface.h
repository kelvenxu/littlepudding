/* lmplayer-interface.h

   Copyright (C) 2005,2007 Bastien Nocera <hadess@hadess.net>

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
   write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301  USA.

   Author: Bastien Nocera <hadess@hadess.net>
 */

#ifndef LMPLAYER_INTERFACE_H
#define LMPLAYER_INTERFACE_H

#include <gtk/gtk.h>
#include "lmplayer.h"

G_BEGIN_DECLS

GdkPixbuf	*lmplayer_interface_load_pixbuf	(const char *name);
char		*lmplayer_interface_get_full_path	(const char *name);
GtkBuilder	*lmplayer_interface_load		(const char *name,
						 gboolean fatal,
						 GtkWindow *parent,
						 gpointer user_data);
GtkBuilder      *lmplayer_interface_load_with_full_path (const char *filename, 
						      gboolean fatal, 
						      GtkWindow *parent,
						      gpointer user_data);
void		 lmplayer_interface_error		(const char *title,
						 const char *reason,
						 GtkWindow *parent);
void		 lmplayer_interface_error_blocking	(const char *title,
						 const char *reason,
						 GtkWindow *parent);
void		 lmplayer_interface_error_with_link (const char *title,
						  const char *reason,
						  const char *uri,
						  const char *label,
						  GtkWindow *parent,
						  LmplayerObject *lmplayer);
void		 lmplayer_interface_set_transient_for (GtkWindow *window,
						    GtkWindow *parent);
char *		 lmplayer_interface_get_license	(void);
void		 lmplayer_interface_boldify_label	(GtkBuilder *xml,
						 const char *label);
void		 lmplayer_interface_italicise_label(GtkBuilder *xml,
						 const char *label);

G_END_DECLS

#endif /* LMPLAYER_INTERFACE_H */
