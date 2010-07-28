/* lmplayer-playlist.h: Simple playlist dialog

   Copyright (C) 2002, 2003, 2004, 2005 Bastien Nocera <hadess@hadess.net>

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

#ifndef LMPLAYER_PLAYLIST_H
#define LMPLAYER_PLAYLIST_H

#include <gtk/gtk.h>
#include <totem-pl-parser.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define LMPLAYER_TYPE_PLAYLIST            (lmplayer_playlist_get_type ())
#define LMPLAYER_PLAYLIST(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), LMPLAYER_TYPE_PLAYLIST, LmplayerPlaylist))
#define LMPLAYER_PLAYLIST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), LMPLAYER_TYPE_PLAYLIST, LmplayerPlaylistClass))
#define LMPLAYER_IS_PLAYLIST(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LMPLAYER_TYPE_PLAYLIST))
#define LMPLAYER_IS_PLAYLIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LMPLAYER_TYPE_PLAYLIST))

typedef enum {
	LMPLAYER_PLAYLIST_STATUS_NONE,
	LMPLAYER_PLAYLIST_STATUS_PLAYING,
	LMPLAYER_PLAYLIST_STATUS_PAUSED
} LmplayerPlaylistStatus;

typedef enum {
	LMPLAYER_PLAYLIST_DIRECTION_NEXT,
	LMPLAYER_PLAYLIST_DIRECTION_PREVIOUS
} LmplayerPlaylistDirection;

typedef struct LmplayerPlaylist	       LmplayerPlaylist;
typedef struct LmplayerPlaylistClass      LmplayerPlaylistClass;
typedef struct LmplayerPlaylistPrivate    LmplayerPlaylistPrivate;

typedef void (*LmplayerPlaylistForeachFunc) (LmplayerPlaylist *playlist,
					  const gchar   *filename,
					  const gchar   *uri,
					  gpointer       user_data);

struct LmplayerPlaylist {
	GtkScrolledWindow parent;
	//GtkTreeView parent;
	LmplayerPlaylistPrivate *priv;
};

struct LmplayerPlaylistClass {
	//GtkTreeViewClass parent_class;
	GtkScrolledWindowClass parent_class;

	void (*changed) (LmplayerPlaylist *playlist);
	void (*item_activated) (LmplayerPlaylist *playlist);
	void (*active_name_changed) (LmplayerPlaylist *playlist);
	void (*current_removed) (LmplayerPlaylist *playlist);
	void (*repeat_toggled) (LmplayerPlaylist *playlist, gboolean repeat);
	void (*shuffle_toggled) (LmplayerPlaylist *playlist, gboolean toggled);
	void (*subtitle_changed) (LmplayerPlaylist *playlist);
	void (*item_added) (LmplayerPlaylist *playlist, const gchar *filename, const gchar *uri);
	void (*item_removed) (LmplayerPlaylist *playlist, const gchar *filename, const gchar *uri);
};

GType    lmplayer_playlist_get_type (void);
GtkWidget *lmplayer_playlist_new      (void);

/* The application is responsible for checking that the mrl is correct
 * @display_name is if you have a preferred display string for the mrl,
 * NULL otherwise
 */
gboolean lmplayer_playlist_add_mrl  (LmplayerPlaylist *playlist,
				  const char *mrl,
				  const char *display_name);
gboolean lmplayer_playlist_add_mrl_with_cursor (LmplayerPlaylist *playlist,
					     const char *mrl,
					     const char *display_name);

void lmplayer_playlist_save_current_playlist (LmplayerPlaylist *playlist,
					   const char *output);
void lmplayer_playlist_save_current_playlist_ext (LmplayerPlaylist *playlist,
					   const char *output, TotemPlParserType type);

/* lmplayer_playlist_clear doesn't emit the current_removed signal, even if it does
 * because the caller should know what to do after it's done with clearing */
gboolean   lmplayer_playlist_clear (LmplayerPlaylist *playlist);
void       lmplayer_playlist_clear_with_g_mount (LmplayerPlaylist *playlist,
					      GMount *mount);
char      *lmplayer_playlist_get_current_mrl (LmplayerPlaylist *playlist,
					   char **subtitle);
char      *lmplayer_playlist_get_current_title (LmplayerPlaylist *playlist,
					     gboolean *custom);
char      *lmplayer_playlist_get_title (LmplayerPlaylist *playlist,
				     guint index);

gboolean   lmplayer_playlist_set_title (LmplayerPlaylist *playlist,
				     const char *title,
				     gboolean force);

gboolean   lmplayer_playlist_set_stream_length(LmplayerPlaylist *playlist, gint64 stream_length);
#define    lmplayer_playlist_has_direction(playlist, direction) (direction == LMPLAYER_PLAYLIST_DIRECTION_NEXT ? lmplayer_playlist_has_next_mrl (playlist) : lmplayer_playlist_has_previous_mrl (playlist))
gboolean   lmplayer_playlist_has_previous_mrl (LmplayerPlaylist *playlist);
gboolean   lmplayer_playlist_has_next_mrl (LmplayerPlaylist *playlist);

#define    lmplayer_playlist_set_direction(playlist, direction) (direction == LMPLAYER_PLAYLIST_DIRECTION_NEXT ? lmplayer_playlist_set_next (playlist) : lmplayer_playlist_set_previous (playlist))
void       lmplayer_playlist_set_previous (LmplayerPlaylist *playlist);
void       lmplayer_playlist_set_next (LmplayerPlaylist *playlist);

gboolean   lmplayer_playlist_get_repeat (LmplayerPlaylist *playlist);
void       lmplayer_playlist_set_repeat (LmplayerPlaylist *playlist, gboolean repeat);

gboolean   lmplayer_playlist_get_shuffle (LmplayerPlaylist *playlist);
void       lmplayer_playlist_set_shuffle (LmplayerPlaylist *playlist,
				       gboolean shuffle);

gboolean   lmplayer_playlist_set_playing (LmplayerPlaylist *playlist, LmplayerPlaylistStatus state);
LmplayerPlaylistStatus lmplayer_playlist_get_playing (LmplayerPlaylist *playlist);

void       lmplayer_playlist_set_at_start (LmplayerPlaylist *playlist);
void       lmplayer_playlist_set_at_end (LmplayerPlaylist *playlist);

guint      lmplayer_playlist_get_current (LmplayerPlaylist *playlist);
guint      lmplayer_playlist_get_last (LmplayerPlaylist *playlist);
void       lmplayer_playlist_set_current (LmplayerPlaylist *playlist, guint index);

void       lmplayer_playlist_foreach (LmplayerPlaylist *playlist,
				   LmplayerPlaylistForeachFunc callback,
				   gpointer user_data);

void		lmplayer_playlist_save_files (LmplayerPlaylist *playlist);
void		lmplayer_playlist_add_files (LmplayerPlaylist *playlist);
void		lmplayer_playlist_remove_files (LmplayerPlaylist *playlist);
void		lmplayer_playlist_up_files (LmplayerPlaylist *playlist);
void		lmplayer_playlist_down_files (LmplayerPlaylist *playlist);

void lmplayer_playlist_set_color(LmplayerPlaylist *playlist, GdkColor *playing_color, GdkColor *noplaying_color);
G_END_DECLS

#endif /* LMPLAYER_PLAYLIST_H */
