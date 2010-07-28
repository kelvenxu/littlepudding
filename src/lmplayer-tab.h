/* vi: set sw=4 ts=4: */
/*
 * lmplayer-tab.h
 *
 * This file is part of lmplayer.
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

#ifndef __LMPLAYER_TAB_H__
#define __LMPLAYER_TAB_H__  1

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define LMPLAYER_TYPE_TAB (lmplayer_tab_get_type ())
#define LMPLAYER_TAB(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), LMPLAYER_TYPE_TAB, LmplayerTab))
#define LMPLAYER_TAB_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), LMPLAYER_TYPE_TAB, LmplayerTabClass))
#define LMPLAYER_IS_TAB(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LMPLAYER_TYPE_TAB))
#define LMPLAYER_IS_TAB_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LMPLAYER_TYPE_TAB))
#define LMPLAYER_TAB_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), LMPLAYER_TYPE_TAB, LmplayerTabClass))

typedef struct _LmplayerTabPrivate LmplayerTabPrivate;

typedef struct 
{
	GtkHBox parent;

	LmplayerTabPrivate *priv;
} LmplayerTab;

typedef struct
{
	GtkHBoxClass parent;

	void (* tab_close_request) (LmplayerTab *tab);
} LmplayerTabClass;

GType lmplayer_tab_get_type(void) G_GNUC_CONST;
GtkWidget *lmplayer_tab_new();

void lmplayer_tab_set_page(LmplayerTab *tab, GtkWidget *page);
GtkWidget *lmplayer_tab_get_page(LmplayerTab *tab);
void lmplayer_tab_set_name(LmplayerTab *tab, const char *new_name);
const char *lmplayer_tab_get_name(LmplayerTab *tab);

G_END_DECLS

#endif /*__LMPLAYER_TAB_H__ */
