/* vi: set sw=4 ts=4: */
/*
 * lmplayer-tray.c
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

#include <glib/gi18n.h>
#include "lmplayer-tray.h"
#include "lmplayer-interface.h"

#if 0
static GtkWidget *win;

static void
lmplayer_tray_menu_show(LmplayerObject *lmplayer, guint button, guint32 activate_time)
{
	//GtkBuilder *builder = lmplayer_interface_load("lmplayer-mini-control.ui", TRUE, NULL, NULL);
	//win = gtk_builder_get_object(builder, "lmplayer-mini-window");
	static GtkWidget *menu = NULL;

	if(menu == NULL)
	{
		menu = gtk_menu_new();

		GtkWidget *item = gtk_menu_item_new_with_label(_("Previous"));
		gtk_widget_show(item);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		g_signal_connect_swapped(item, "activate", G_CALLBACK(previous_action_callback), lmplayer);

		item = gtk_menu_item_new_with_label(_("Play/Pause"));
		gtk_widget_show(item);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		g_signal_connect_swapped(item, "activate", G_CALLBACK(play_action_callback), lmplayer);

		item = gtk_menu_item_new_with_label(_("Next"));
		gtk_widget_show(item);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		g_signal_connect_swapped(item, "activate", G_CALLBACK(next_action_callback), lmplayer);

		item = gtk_separator_menu_item_new();  
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		gtk_widget_show(item);

		item = gtk_menu_item_new_with_label(_("Display/hide main window"));
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		gtk_widget_show(item);
		g_signal_connect_swapped(item, "activate", G_CALLBACK(display_action_callback), lmplayer);

		item = gtk_separator_menu_item_new();  
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		gtk_widget_show(item);

		item = gtk_menu_item_new_with_label(_("Close"));
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		gtk_widget_show(item);
		g_signal_connect_swapped(item, "activate", G_CALLBACK(notebook_tab_close_cb), lmplayer);
	}

	gtk_menu_popup(GTK_MENU(menu),  NULL, NULL, NULL, NULL, button, activate_time);
}

static void 
tray_activate_cb(GtkStatusIcon *status_icon, LmplayerObject *lmplayer)
{
	printf("Clicked on tray icon\n");
	//gtk_widget_show_all(win);
}
static void 
tray_menu_cb(GtkStatusIcon *status_icon, guint button, guint activate_time, LmplayerObject *lmplayer)
{
	lmplayer_tray_menu_show(lmplayer, button, activate_time);
}

GtkStatusIcon *
lmplayer_tray_new(LmplayerObject *lmplayer)
{
	GtkStatusIcon *tray_icon = gtk_status_icon_new();
	gtk_status_icon_set_from_icon_name(tray_icon, GTK_STOCK_MEDIA_STOP);
	gtk_status_icon_set_tooltip(tray_icon, _("Lmplayer"));
	gtk_status_icon_set_visible(tray_icon, TRUE);

	//g_signal_connect(G_OBJECT(tray_icon), "activate", G_CALLBACK(tray_activate_cb), lmplayer);
	g_signal_connect(G_OBJECT(tray_icon), "popup_menu", G_CALLBACK(tray_menu_cb), lmplayer);
	//g_signal_connect(G_OBJECT(tray_icon), "popup-menu", G_CALLBACK(tray_icon_on_menu), NULL); 

	//build_tray_control_panel(lmplayer);

	return tray_icon;
}
#endif
