/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 * LmplayerStatusbar Copyright (C) 1998 Shawn T. Amundson
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301  USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

#ifndef __LMPLAYER_STATUSBAR_H__
#define __LMPLAYER_STATUSBAR_H__

#include <gtk/gtkwidget.h>
#include <gtk/gtkstatusbar.h>

G_BEGIN_DECLS

#define LMPLAYER_TYPE_STATUSBAR            (lmplayer_statusbar_get_type ())
#define LMPLAYER_STATUSBAR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), LMPLAYER_TYPE_STATUSBAR, LmplayerStatusbar))
#define LMPLAYER_STATUSBAR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), LMPLAYER_TYPE_STATUSBAR, LmplayerStatusbarClass))
#define LMPLAYER_IS_STATUSBAR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LMPLAYER_TYPE_STATUSBAR))
#define LMPLAYER_IS_STATUSBAR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LMPLAYER_TYPE_STATUSBAR))
#define LMPLAYER_STATUSBAR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), LMPLAYER_TYPE_STATUSBAR, LmplayerStatusbarClass))


typedef struct _LmplayerStatusbar      LmplayerStatusbar;

struct _LmplayerStatusbar
{
  GtkStatusbar parent_instance;

  GtkWidget *progress;
  GtkWidget *time_label;

  gint time;
  gint length;
  guint timeout;
  guint percentage;

  guint pushed : 1;
  guint seeking : 1;
  guint timeout_ticks : 2;
};

typedef GtkStatusbarClass LmplayerStatusbarClass;

GType      lmplayer_statusbar_get_type     	(void) G_GNUC_CONST;
GtkWidget* lmplayer_statusbar_new          	(void);

void       lmplayer_statusbar_set_time		(LmplayerStatusbar *statusbar,
						 gint time);
void       lmplayer_statusbar_set_time_and_length	(LmplayerStatusbar *statusbar,
						 gint time, gint length);
void       lmplayer_statusbar_set_seeking          (LmplayerStatusbar *statusbar,
						 gboolean seeking);

void       lmplayer_statusbar_set_text             (LmplayerStatusbar *statusbar,
						 const char *label);
void	   lmplayer_statusbar_push			(LmplayerStatusbar *statusbar,
						 guint percentage);
void       lmplayer_statusbar_pop			(LmplayerStatusbar *statusbar);

G_END_DECLS

#endif /* __LMPLAYER_STATUSBAR_H__ */
