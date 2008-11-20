/* vi: set sw=4 ts=4: */
/*
 * lmplayer-menu.c
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

#include "lmplayer-menu.h"
#include "lmplayer.h"
#include "lmplayer-debug.h"

void play_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void pause_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void next_music_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void prev_music_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void stop_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void open_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void open_location_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void quit_action_callback(GtkAction *action, LmplayerObject *lmplayer);

void minimode_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void mini_minimode_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void minimize_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void mini_minimize_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void mute_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void lyric_close_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void lyric_ontop_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void lyric_show_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void playlist_show_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void playlist_close_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void eq_show_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void eq_close_action_callback(GtkAction *action, LmplayerObject *lmplayer);

void toolbar_add_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void toolbar_remove_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void toolbar_list_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void toolbar_sort_action_callback(GtkAction *action, LmplayerObject *lmplayer);

void add_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void add_direction_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void remove_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void up_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void down_action_callback(GtkAction *action, LmplayerObject *lmplayer);
void save_action_callback(GtkAction *action, LmplayerObject *lmplayer);

void play_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_play_pause(lmplayer);
}

void pause_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_play_pause(lmplayer);
}

void next_music_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_next(lmplayer);
}

void prev_music_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_previous(lmplayer);
}

void stop_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_stop(lmplayer);
}

void open_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_debug(" ");
	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));
	lmplayer_action_open(lmplayer);
}

void open_location_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
}

void quit_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_exit(lmplayer);
}

void minimode_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_minimode(lmplayer, TRUE);
}

void mini_minimode_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_minimode(lmplayer, FALSE);
}

void minimize_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_minimize(lmplayer);
}

void mini_minimize_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	//lmplayer_action_minimize(lmplayer, FALSE);
	gtk_window_iconify(GTK_WINDOW(lmplayer->mini_win));
}

void mute_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	// FIXME: how to mute?
	SkinCheckButton *button;
	static double old_volume;
	button = (SkinCheckButton*)skin_builder_get_object(lmplayer->builder, "player-mute");
	if(skin_check_button_get_active(button))
	{
		old_volume = bacon_video_widget_get_volume(lmplayer->bvw);
		lmplayer_action_volume_relative(lmplayer, -1.0);
	}
	else
	{
		//lmplayer_action_volume_relative(lmplayer, old_volume);
		bacon_video_widget_set_volume(lmplayer->bvw, old_volume);
	}
}

void lyric_close_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	SkinCheckButton *button;
	button = (SkinCheckButton*)skin_builder_get_object(lmplayer->builder, "player-lyric");
	skin_window_hide(lmplayer->lyric_win);
	skin_check_button_set_active(button, FALSE);
}

void lyric_ontop_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	SkinCheckButton *button;
	button = (SkinCheckButton*)skin_builder_get_object(lmplayer->builder, "lyric-ontop");

	if(skin_check_button_get_active(button))
	{
		gtk_window_set_keep_above(GTK_WINDOW(lmplayer->lyric_win), TRUE);
	}
	else
	{
		gtk_window_set_keep_above(GTK_WINDOW(lmplayer->lyric_win), FALSE);
	}
}

void lyric_show_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	SkinCheckButton *button;
	button = (SkinCheckButton*)skin_builder_get_object(lmplayer->builder, "player-lyric");

	if(skin_check_button_get_active(button))
	{
		skin_window_show(lmplayer->lyric_win);
	}
	else
	{
		skin_window_hide(lmplayer->lyric_win);
	}
}

void playlist_show_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	SkinCheckButton *button;
	SkinWindow *win;
	button = (SkinCheckButton*)skin_builder_get_object(lmplayer->builder, "player-playlist");
	win = (SkinWindow*)skin_builder_get_object(lmplayer->builder, "playlist-window");

	if(skin_check_button_get_active(button))
	{
		skin_window_show(win);
	}
	else
	{
		skin_window_hide(win);
	}
}

void playlist_close_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	SkinCheckButton *button;
	button = (SkinCheckButton*)skin_builder_get_object(lmplayer->builder, "player-playlist");
	skin_window_hide(lmplayer->pl_win);
	skin_check_button_set_active(button, FALSE);
}

void eq_show_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	SkinCheckButton *button;
	SkinWindow *win;
	button = (SkinCheckButton*)skin_builder_get_object(lmplayer->builder, "player-equalizer");
	win = (SkinWindow*)skin_builder_get_object(lmplayer->builder, "equalizer-window");

	if(skin_check_button_get_active(button))
	{
		skin_window_show(win);
	}
	else
	{
		skin_window_hide(win);
	}
}

void eq_close_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	SkinCheckButton *button;
	button = (SkinCheckButton*)skin_builder_get_object(lmplayer->builder, "player-equalizer");
	skin_window_hide(lmplayer->eq_win);
	skin_check_button_set_active(button, FALSE);
}

void add_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_playlist_add_files(lmplayer->playlist);
}

void add_direction_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
}

void remove_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_playlist_remove_files(lmplayer->playlist);
}

void up_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_playlist_up_files(lmplayer->playlist);
}

void down_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_playlist_down_files(lmplayer->playlist);
}

void save_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_playlist_save_files(lmplayer->playlist);
}

void toolbar_add_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	GtkWidget *menu;

	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));
	g_return_if_fail(GTK_IS_UI_MANAGER(lmplayer->menus));

	menu = gtk_ui_manager_get_widget(lmplayer->menus, "/Add");
	g_return_if_fail(GTK_IS_MENU(menu));

	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
			1, gtk_get_current_event_time());
}

void toolbar_remove_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	GtkWidget *menu;

	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));
	g_return_if_fail(GTK_IS_UI_MANAGER(lmplayer->menus));

	menu = gtk_ui_manager_get_widget(lmplayer->menus, "/Remove");
	g_return_if_fail(GTK_IS_MENU(menu));

	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
			1, gtk_get_current_event_time());
}

void toolbar_list_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	GtkWidget *menu;

	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));
	g_return_if_fail(GTK_IS_UI_MANAGER(lmplayer->menus));

	menu = gtk_ui_manager_get_widget(lmplayer->menus, "/List");
	g_return_if_fail(GTK_IS_MENU(menu));

	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
			1, gtk_get_current_event_time());
}

void toolbar_sort_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	GtkWidget *menu;

	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));
	g_return_if_fail(GTK_IS_UI_MANAGER(lmplayer->menus));

	menu = gtk_ui_manager_get_widget(lmplayer->menus, "/Sort");
	g_return_if_fail(GTK_IS_MENU(menu));

	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
			1, gtk_get_current_event_time());
}
void lmplayer_ui_manager_setup (LmplayerObject *lmplayer)
{
	SkinButton *button;
	SkinBuilder *builder;

	builder = lmplayer->builder;

	// player window
	button = (SkinButton*)skin_builder_get_object(builder, "player-open");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(open_action_callback), lmplayer);
	
	button = (SkinButton*)skin_builder_get_object(builder, "player-play");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(play_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-pause");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(pause_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-next");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(next_music_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-prev");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(prev_music_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-stop");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(stop_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-minimode");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(minimode_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-minimize");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(minimize_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-exit");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(quit_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-playlist");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(playlist_show_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-lyric");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(lyric_show_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-equalizer");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(eq_show_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "player-mute");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(mute_action_callback), lmplayer);

	// lyric window
	button = (SkinButton*)skin_builder_get_object(builder, "lyric-ontop");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(lyric_ontop_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "lyric-close");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(lyric_close_action_callback), lmplayer);
	
	// playlist window
	button = (SkinButton*)skin_builder_get_object(builder, "playlist-close");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(playlist_close_action_callback), lmplayer);

	// equalizer window
	button = (SkinButton*)skin_builder_get_object(builder, "equalizer-close");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(eq_close_action_callback), lmplayer);
	
	// mini mode
	button = (SkinButton*)skin_builder_get_object(builder, "mini-open");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(open_action_callback), lmplayer);
	
	button = (SkinButton*)skin_builder_get_object(builder, "mini-play");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(play_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-pause");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(pause_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-next");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(next_music_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-prev");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(prev_music_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-stop");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(stop_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-minimize");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(mini_minimize_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-minimode");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(mini_minimode_action_callback), lmplayer);

	button = (SkinButton*)skin_builder_get_object(builder, "mini-exit");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(quit_action_callback), lmplayer);

	// toolbar
	SkinToggleButton *tb;
	tb = (SkinToggleButton*)skin_builder_get_object(builder, "toolbar-add");
	g_signal_connect(G_OBJECT(tb), "clicked", G_CALLBACK(toolbar_add_action_callback), lmplayer);

	tb = (SkinToggleButton*)skin_builder_get_object(builder, "toolbar-remove");
	g_signal_connect(G_OBJECT(tb), "clicked", G_CALLBACK(toolbar_remove_action_callback), lmplayer);

	tb = (SkinToggleButton*)skin_builder_get_object(builder, "toolbar-list");
	g_signal_connect(G_OBJECT(tb), "clicked", G_CALLBACK(toolbar_list_action_callback), lmplayer);

	tb = (SkinToggleButton*)skin_builder_get_object(builder, "toolbar-sort");
	g_signal_connect(G_OBJECT(tb), "clicked", G_CALLBACK(toolbar_sort_action_callback), lmplayer);
}

static GtkActionEntry entries[] = {
  { "Add", GTK_STOCK_ADD,
	  "_Add", "<control>A",
	  "Add files to playlist",
	  G_CALLBACK (add_action_callback) },
  { "Remove", GTK_STOCK_REMOVE,
	  "_Remove", "<control>R",
	  "Remove files from playlist",
	  G_CALLBACK (remove_action_callback) },
  { "AddDirection", NULL,
	  "Add _Direction", "<control>D",
	  "Add all files in given direction",
	  G_CALLBACK (add_direction_action_callback) },
  { "Open", GTK_STOCK_OPEN,                    /* name, stock id */
    "_Open","<control>O",                      /* label, accelerator */     
    "Open a file",                             /* tooltip */
    G_CALLBACK (open_action_callback) }, 
  { "OpenLocation", NULL,
	  "Open _Location", "<control>L",
	  "Open url",
	  G_CALLBACK (open_location_action_callback) },
  { "Save", GTK_STOCK_SAVE,                    /* name, stock id */
    "_Save","<control>S",                      /* label, accelerator */     
    "Save current file",                       /* tooltip */
    G_CALLBACK (save_action_callback) },
  { "Up", GTK_STOCK_GO_UP,
	  "Move _Up", NULL,
	  "Move items up",
	  G_CALLBACK (up_action_callback) },
  { "Down", GTK_STOCK_GO_DOWN,
	  "Move _Down", NULL,
	  "Move items down",
	  G_CALLBACK (down_action_callback) },
  { "Quit", GTK_STOCK_QUIT,                    /* name, stock id */
    "_Quit", "<control>Q",                     /* label, accelerator */     
    "Quit",                                    /* tooltip */
    G_CALLBACK (quit_action_callback) },
  { "About", NULL,                             /* name, stock id */
    "_About", "<control>A",                    /* label, accelerator */     
    "About",                                   /* tooltip */  
    G_CALLBACK (quit_action_callback) },
};

static guint n_entries = G_N_ELEMENTS (entries);

enum {
  COLOR_RED,
  COLOR_GREEN,
  COLOR_BLUE
};

static GtkRadioActionEntry color_entries[] = {
  { "Red", NULL,                               /* name, stock id */
    "_Red", "<control>R",                      /* label, accelerator */     
    "Blood", COLOR_RED },                      /* tooltip, value */
  { "Green", NULL,                             /* name, stock id */
    "_Green", "<control>G",                    /* label, accelerator */     
    "Grass", COLOR_GREEN },                    /* tooltip, value */
  { "Blue", NULL,                              /* name, stock id */
    "_Blue", "<control>B",                     /* label, accelerator */     
    "Sky", COLOR_BLUE },                       /* tooltip, value */
};

static guint n_color_entries = G_N_ELEMENTS (color_entries);

static const gchar *ui_info = 
"<ui>"
"  <popup name='Add'>"
"	 <menuitem name='Add' action='Add'/>"
"    <menuitem name='AddDirection' action='AddDirection'/>"
"    <menuitem name='OpenLocation' action='OpenLocation'/>"
"  </popup>"
"  <popup name='Remove'>"
"    <menuitem name='Remove' action='Remove'/>"
"  </popup>"
"  <popup name='Sort'>"
"    <menuitem name='Up' action='Up'/>"
"    <menuitem name='Down' action='Down'/>"
"  </popup>"
"  <popup name='List'>"
"    <menuitem name='Save' action='Save'/>"
"  </popup>"
"</ui>";

void
lmplayer_setup_toolbar(LmplayerObject *lmplayer)
{
	GtkActionGroup *actions;
	
	actions = gtk_action_group_new("ToolbarActions");
	gtk_action_group_add_actions(actions, entries, n_entries, lmplayer);
	/*
	gtk_action_group_add_radio_actions(actions, 
			color_entries, n_color_entries,
			COLOR_RED,
			G_CALLBACK(activate_radio_action),
			NULL);
	*/

	lmplayer->menus = gtk_ui_manager_new();
	gtk_ui_manager_insert_action_group(lmplayer->menus, actions, 0);
	gtk_window_add_accel_group(GTK_WINDOW(lmplayer->win), 
			gtk_ui_manager_get_accel_group(lmplayer->menus));
	g_object_unref(actions);

	gtk_ui_manager_add_ui_from_string(lmplayer->menus, ui_info, -1, NULL);
}

