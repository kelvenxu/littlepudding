/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA.
 *
 */

#ifndef LMPLAYER_OPEN_LOCATION_H
#define LMPLAYER_OPEN_LOCATION_H

#include <gtk/gtkdialog.h>

G_BEGIN_DECLS

#define LMPLAYER_TYPE_OPEN_LOCATION		(lmplayer_open_location_get_type ())
#define LMPLAYER_OPEN_LOCATION(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), LMPLAYER_TYPE_OPEN_LOCATION, LmplayerOpenLocation))
#define LMPLAYER_OPEN_LOCATION_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), LMPLAYER_TYPE_OPEN_LOCATION, LmplayerOpenLocationClass))
#define LMPLAYER_IS_OPEN_LOCATION(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), LMPLAYER_TYPE_OPEN_LOCATION))
#define LMPLAYER_IS_OPEN_LOCATION_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), LMPLAYER_TYPE_OPEN_LOCATION))

typedef struct LmplayerOpenLocation		LmplayerOpenLocation;
typedef struct LmplayerOpenLocationClass		LmplayerOpenLocationClass;
typedef struct LmplayerOpenLocationPrivate		LmplayerOpenLocationPrivate;

struct LmplayerOpenLocation {
	GtkDialog parent;
	LmplayerOpenLocationPrivate *priv;
};

struct LmplayerOpenLocationClass {
	GtkDialogClass parent_class;
};

GType lmplayer_open_location_get_type		(void);
GtkWidget *lmplayer_open_location_new		(void *lmplayer);
char *lmplayer_open_location_get_uri		(LmplayerOpenLocation *open_location);

G_END_DECLS

#endif /* LMPLAYER_OPEN_LOCATION_H */
