/* vi: set sw=4 ts=4: */
/*
 * lmplayer-magnetic.c
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

// 窗口磁性化：这里的代码应该是临时的，以后应改进之。

#include "lmplayer-magnetic.h"

#define MAGNET_POWER 32
#define BORDER 2

// 指示窗口是否被吸引
static gboolean lyric_magnetic;
static gboolean playlist_magnetic;
static gboolean eq_magnetic;

// 主窗口的位置
static GdkRectangle win_rect;

// 主窗口的移动前的初始位置
static gint old_x;
static gint old_y;

static void gtk_window_get_rect(GtkWindow *win, GdkRectangle *rect);
static gboolean do_magnetize_to(GtkWindow *win, GdkRectangle *my, GdkRectangle *target);
static gboolean main_window_move_cb(GtkWindow *win, LmplayerObject *lmplayer);
static gboolean pl_window_move_cb(GtkWindow *win, LmplayerObject *lmplayer);
static gboolean lyric_window_move_cb(GtkWindow *win, LmplayerObject *lmplayer);
static gboolean eq_window_move_cb(GtkWindow *win, LmplayerObject *lmplayer);

void 
lmplayer_magnetic_activate(LmplayerObject *lmplayer)
{
	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	lyric_magnetic = FALSE;
	playlist_magnetic = FALSE;
	eq_magnetic = FALSE;

	gtk_window_get_rect(GTK_WINDOW(lmplayer->win), &win_rect);

	old_x = win_rect.x;
	old_y = win_rect.y;

	g_signal_connect(G_OBJECT(lmplayer->win), "move-event",
			G_CALLBACK(main_window_move_cb), lmplayer);

	g_signal_connect(G_OBJECT(lmplayer->pl_win), "move-event",
			G_CALLBACK(pl_window_move_cb), lmplayer);

	g_signal_connect(G_OBJECT(lmplayer->lyric_win), "move-event",
			G_CALLBACK(lyric_window_move_cb), lmplayer);

	g_signal_connect(G_OBJECT(lmplayer->eq_win), "move-event",
			G_CALLBACK(eq_window_move_cb), lmplayer);
}

static void 
gtk_window_get_rect(GtkWindow *win, GdkRectangle *rect)
{
	gtk_window_get_position(win, &(rect->x), &(rect->y));
	gtk_window_get_size(win, &(rect->width), &(rect->height));
}

static gboolean 
do_magnetize_to(GtkWindow *win, GdkRectangle *my, GdkRectangle *target)
{
	gint rx, ry;
	gboolean  flag = FALSE;
	
	// right side
	rx = my->x - (target->x + target->width);
	if(rx > 0 && rx < MAGNET_POWER)
	{
		gtk_window_move(win, target->x + target->width + BORDER, my->y);
		flag = TRUE;
	}

	// left side
	rx = target->x - (my->x + my->width);
	if(rx > 0 && rx < MAGNET_POWER)
	{
		gtk_window_move(win, target->x - my->width - BORDER, my->y);
		flag = TRUE;
	}

	// top side
	ry = target->y - (my->y + my->height);
	if(ry > 0 && ry < MAGNET_POWER)
	{
		gtk_window_move(win, my->x, target->y - my->height - BORDER);
		flag = TRUE;
	}

	// bottom side
	ry = my->y - (target->y + target->height);
	if(ry > 0 && ry < MAGNET_POWER)
	{
		gtk_window_move(win, my->x, target->y + target->height + BORDER);
		flag = TRUE;
	}

	return flag;
}

static gboolean
main_window_move_cb(GtkWindow *win, LmplayerObject *lmplayer)
{
	gint new_x, new_y;
	gint star_x, star_y;
	gint rx, ry;
	
	gtk_window_get_position(win, &new_x, &new_y);

	rx = new_x - old_x;
	ry = new_y - old_y;

	if(playlist_magnetic)
	{
		gtk_window_get_position(GTK_WINDOW(lmplayer->pl_win), &star_x, &star_y);
		gtk_window_move(GTK_WINDOW(lmplayer->pl_win), star_x + rx, star_y + ry);
	}
	if(lyric_magnetic)
	{
		gtk_window_get_position(GTK_WINDOW(lmplayer->lyric_win), &star_x, &star_y);
		gtk_window_move(GTK_WINDOW(lmplayer->lyric_win), star_x + rx, star_y + ry);
	}
	if(eq_magnetic)
	{
		gtk_window_get_position(GTK_WINDOW(lmplayer->eq_win), &star_x, &star_y);
		gtk_window_move(GTK_WINDOW(lmplayer->eq_win), star_x + rx, star_y + ry);
	}

	old_x = new_x;
	old_y = new_y;

	return FALSE;
}

static gboolean
pl_window_move_cb(GtkWindow *win, LmplayerObject *lmplayer)
{
	GdkRectangle my;
	GdkRectangle target;
	
	gtk_window_get_rect(GTK_WINDOW(lmplayer->pl_win), &my);
	gtk_window_get_rect(GTK_WINDOW(lmplayer->win), &target);

	playlist_magnetic = do_magnetize_to(GTK_WINDOW(lmplayer->pl_win), &my, &target);

	return FALSE;
}

static gboolean
lyric_window_move_cb(GtkWindow *win, LmplayerObject *lmplayer)
{
	GdkRectangle my;
	GdkRectangle target;
	
	gtk_window_get_rect(GTK_WINDOW(lmplayer->lyric_win), &my);
	gtk_window_get_rect(GTK_WINDOW(lmplayer->win), &target);

	lyric_magnetic = do_magnetize_to(GTK_WINDOW(lmplayer->lyric_win), &my, &target);

	return FALSE;
}

static gboolean
eq_window_move_cb(GtkWindow *win, LmplayerObject *lmplayer)
{
	GdkRectangle my;
	GdkRectangle target;
	
	gtk_window_get_rect(GTK_WINDOW(lmplayer->eq_win), &my);
	gtk_window_get_rect(GTK_WINDOW(lmplayer->win), &target);

	eq_magnetic = do_magnetize_to(GTK_WINDOW(lmplayer->eq_win), &my, &target);

	return FALSE;
}

