/*
 * status_icon.c
 *
 * This file is part of status_icon plugin.
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
#include "config.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gmodule.h>
#include <gtk/gtk.h>

#include "lmplayer.h"
#include "lmplayer-plugin.h"

#define LMPLAYER_TYPE_STATUS_ICON_PLUGIN	(lmplayer_status_icon_plugin_get_type())
#define LMPLAYER_STATUS_ICON_PLUGIN(o)		(G_TYPE_CHECK_INSTANCE_CAST((o), LMPLAYER_TYPE_STATUS_ICON_PLUGIN, LmplayerStatusIconPlugin))
#define LMPLAYER_STATUS_ICON_PLUGIN_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), LMPLAYER_TYPE_STATUS_ICON_PLUGIN, LmplayerStatusIconPluginClass))
#define LMPLAYER_IS_STATUS_ICON_PLUGIN(o) (G_TYPE_CHECK_INSTANCE_TYPE((o), LMPLAYER_TYPE_STATUS_ICON_PLUGIN))
#define LMPLAYER_IS_STATUS_ICON_PLUGIN_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE((k), LMPLAYER_TYPE_STATUS_ICON_PLUGIN))
#define LMPLAYER_STATUS_ICON_PLUGIN_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS((o), LMPLAYER_TYPE_STATUS_ICON_PLUGIN, LmplayerStatusIconPluginClass))

typedef struct 
{
	LmplayerPlugin parent;

	Lmplayer *lmplayer;
	GtkStatusIcon *tray_icon;
	gboolean display;

	/* plugin object members */
} LmplayerStatusIconPlugin;

typedef struct 
{
	LmplayerPluginClass parent_class;
} LmplayerStatusIconPluginClass;

G_MODULE_EXPORT GType register_lmplayer_plugin(GTypeModule *module);
GType lmplayer_status_icon_plugin_get_type(void) G_GNUC_CONST;

static gboolean impl_activate(LmplayerPlugin *plugin, LmplayerObject *lmplayer, GError **error);
static void impl_deactivate(LmplayerPlugin *plugin, LmplayerObject *lmplayer);

LMPLAYER_PLUGIN_REGISTER(LmplayerStatusIconPlugin, lmplayer_status_icon_plugin)

static void
lmplayer_status_icon_plugin_class_init (LmplayerStatusIconPluginClass *klass)
{
	LmplayerPluginClass *plugin_class = LMPLAYER_PLUGIN_CLASS (klass);

	plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;
}

static void
lmplayer_status_icon_plugin_init(LmplayerStatusIconPlugin *plugin)
{
	/* Initialise resources, but only ones which should exist for the entire lifetime of Lmplayer;
	 * those which should only exist for the lifetime of the plugin (which may be short, and may
	 * occur several times during one Lmplayer session) should be created in impl_activate, and destroyed
	 * in impl_deactivate. 
	 */

	plugin->display = TRUE;
}


static void 
display_action_callback(GtkAction *action, LmplayerStatusIconPlugin *plugin)
{
	GtkWidget *win = lmplayer_get_main_window(plugin->lmplayer);
	
	if(plugin->display)
	{
		gtk_widget_hide(win);
	}
	else
	{
		gtk_widget_show(win);
		gtk_window_deiconify(GTK_WINDOW(win));
	}

	plugin->display = !plugin->display;
}

static void 
play_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_play_pause(lmplayer);
}

static void 
next_music_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_next(lmplayer);
}

static void 
prev_music_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_previous(lmplayer);
}

static void 
quit_action_callback(GtkAction *action, LmplayerObject *lmplayer)
{
	lmplayer_action_exit(lmplayer);
}

static void
lmplayer_tray_menu_show(LmplayerStatusIconPlugin *plugin, guint button, guint32 activate_time)
{
	static GtkWidget *menu = NULL;

	if(menu == NULL)
	{
		menu = gtk_menu_new();

		GtkWidget *item = gtk_menu_item_new_with_label(_("Previous"));
		gtk_widget_show(item);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		g_signal_connect(item, "activate", G_CALLBACK(prev_music_action_callback), plugin->lmplayer);

		item = gtk_menu_item_new_with_label(_("Play/Pause"));
		gtk_widget_show(item);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		g_signal_connect(item, "activate", G_CALLBACK(play_action_callback), plugin->lmplayer);

		item = gtk_menu_item_new_with_label(_("Next"));
		gtk_widget_show(item);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		g_signal_connect(item, "activate", G_CALLBACK(next_music_action_callback), plugin->lmplayer);

		item = gtk_separator_menu_item_new();  
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		gtk_widget_show(item);

		item = gtk_menu_item_new_with_label(_("Display/hide main window"));
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		gtk_widget_show(item);
		g_signal_connect(item, "activate", G_CALLBACK(display_action_callback), plugin);

		item = gtk_separator_menu_item_new();  
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		gtk_widget_show(item);

		item = gtk_menu_item_new_with_label(_("Close"));
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		gtk_widget_show(item);
		g_signal_connect(item, "activate", G_CALLBACK(quit_action_callback), plugin->lmplayer);
	}

	gtk_menu_set_screen(GTK_MENU(menu), gtk_status_icon_get_screen(plugin->tray_icon));
	gtk_menu_popup(GTK_MENU(menu),  NULL, NULL, 
			gtk_status_icon_position_menu, 
			plugin->tray_icon, 
			button, 
			activate_time);
}

static void 
tray_activate_cb(GtkStatusIcon *status_icon, LmplayerStatusIconPlugin *plugin)
{
	GtkWidget *win = lmplayer_get_main_window(plugin->lmplayer);
	
	if(plugin->display)
	{
		gtk_widget_hide(win);
	}
	else
	{
		gtk_widget_show(win);
		gtk_window_deiconify(GTK_WINDOW(win));
	}

	plugin->display = !plugin->display;
}

static void 
tray_menu_cb(GtkStatusIcon *status_icon, guint button, guint activate_time, LmplayerStatusIconPlugin *plugin)
{
	g_return_if_fail(LMPLAYER_IS_OBJECT(plugin->lmplayer));
	lmplayer_tray_menu_show(plugin, 3, activate_time);
}

static GtkStatusIcon *
lmplayer_tray_new(LmplayerStatusIconPlugin *plugin)
{
	plugin->tray_icon = gtk_status_icon_new();

	gtk_status_icon_set_from_file(plugin->tray_icon, DATADIR"/lmplayer/lmplayer-24x24.png");
	gtk_status_icon_set_tooltip(plugin->tray_icon, _("Lmplayer"));
	gtk_status_icon_set_visible(plugin->tray_icon, TRUE);

	g_signal_connect(G_OBJECT(plugin->tray_icon), "activate", G_CALLBACK(tray_activate_cb), plugin);
	g_signal_connect(G_OBJECT(plugin->tray_icon), "popup_menu", G_CALLBACK(tray_menu_cb), plugin);

	return plugin->tray_icon;
}

static gboolean
impl_activate (LmplayerPlugin *plugin, LmplayerObject *lmplayer, GError **error)
{
	LmplayerStatusIconPlugin *self = LMPLAYER_STATUS_ICON_PLUGIN(plugin);

	self->lmplayer = g_object_ref(lmplayer);

	lmplayer_tray_new(self);

	return TRUE;
}

static void
impl_deactivate(LmplayerPlugin *plugin, LmplayerObject *lmplayer)
{
	LmplayerStatusIconPlugin *self = LMPLAYER_STATUS_ICON_PLUGIN (plugin);

	g_object_unref(self->lmplayer);

	/* Destroy resources created in impl_activate here. e.g. Disconnect from signals
	 * and remove menu entries and UI. */
}

