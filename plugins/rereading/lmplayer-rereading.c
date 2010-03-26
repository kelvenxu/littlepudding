/*
 * rereading.c
 *
 * This file is part of rereading plugin.
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

#define LMPLAYER_TYPE_REREADING_PLUGIN	(lmplayer_rereading_plugin_get_type())
#define LMPLAYER_REREADING_PLUGIN(o)		(G_TYPE_CHECK_INSTANCE_CAST((o), LMPLAYER_TYPE_REREADING_PLUGIN, LmplayerRereadingPlugin))
#define LMPLAYER_REREADING_PLUGIN_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), LMPLAYER_TYPE_REREADING_PLUGIN, LmplayerRereadingPluginClass))
#define LMPLAYER_IS_REREADING_PLUGIN(o) (G_TYPE_CHECK_INSTANCE_TYPE((o), LMPLAYER_TYPE_REREADING_PLUGIN))
#define LMPLAYER_IS_REREADING_PLUGIN_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE((k), LMPLAYER_TYPE_REREADING_PLUGIN))
#define LMPLAYER_REREADING_PLUGIN_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS((o), LMPLAYER_TYPE_REREADING_PLUGIN, LmplayerRereadingPluginClass))

typedef struct 
{
	LmplayerPlugin parent;

	Lmplayer *lmplayer;
	GtkWidget *dialog;

	gint64 start;
	gint64 end;
	gint64 slength;

	/* plugin object members */
} LmplayerRereadingPlugin;

typedef struct 
{
	LmplayerPluginClass parent_class;
} LmplayerRereadingPluginClass;

G_MODULE_EXPORT GType register_lmplayer_plugin(GTypeModule *module);
GType lmplayer_rereading_plugin_get_type(void) G_GNUC_CONST;

static gboolean impl_activate(LmplayerPlugin *plugin, LmplayerObject *lmplayer, GError **error);
static void impl_deactivate(LmplayerPlugin *plugin, LmplayerObject *lmplayer);

LMPLAYER_PLUGIN_REGISTER(LmplayerRereadingPlugin, lmplayer_rereading_plugin)

static void
lmplayer_rereading_plugin_class_init (LmplayerRereadingPluginClass *klass)
{
	LmplayerPluginClass *plugin_class = LMPLAYER_PLUGIN_CLASS (klass);

	plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;
}

static void
lmplayer_rereading_plugin_init(LmplayerRereadingPlugin *plugin)
{
	/* Initialise resources, but only ones which should exist for the entire lifetime of Lmplayer;
	 * those which should only exist for the lifetime of the plugin (which may be short, and may
	 * occur several times during one Lmplayer session) should be created in impl_activate, and destroyed
	 * in impl_deactivate. 
	 */

	plugin->start = -1;
	plugin->end = -1;
	plugin->slength = -1;
}

static void
start_button_clicked_cb(GtkButton *button, LmplayerRereadingPlugin *self)
{
	self->start = lmplayer_get_current_time(self->lmplayer);
	self->slength = lmplayer_get_stream_length(self->lmplayer);
}

static void
end_button_clicked_cb(GtkButton *button, LmplayerRereadingPlugin *self)
{
	if(self->start == -1)
		return;
	
	if(!lmplayer_is_playing(self->lmplayer))
		return;

	self->end = lmplayer_get_current_time(self->lmplayer);
	lmplayer_action_seek(self->lmplayer, (gdouble)self->start / (gdouble)self->slength);
}

static void
stop_button_clicked_cb(GtkButton *button, LmplayerRereadingPlugin *self)
{
	if(self->start == -1)
		return;
	
	if(!lmplayer_is_playing(self->lmplayer))
		return;

	lmplayer_action_seek(self->lmplayer, (gdouble)self->end / (gdouble)self->slength);
	self->start = -1;
	self->end = -1;
}

static void
quit_button_clicked_cb(GtkButton *button, LmplayerRereadingPlugin *self)
{
	gtk_widget_hide(self->dialog);

	if(self->start == -1 || self->end == -1)
		return;

	if(!lmplayer_is_playing(self->lmplayer))
		return;

	lmplayer_action_seek(self->lmplayer, (gdouble)self->end / (gdouble)self->slength);
	self->start = -1;
	self->end = -1;
}

static gboolean
timer_cb(LmplayerRereadingPlugin *self)
{
	if(self->start == -1 || self->end == -1)
		return TRUE;

	if(!lmplayer_is_playing(self->lmplayer))
		return TRUE;

	int cur = lmplayer_get_current_time(self->lmplayer);
	
	if(cur >= self->end)
	{
		lmplayer_action_seek(self->lmplayer, (gdouble)self->start / (gdouble)self->slength);
	}

	return TRUE;
}

static void
active_button_clicked_cb(GtkButton *button, LmplayerRereadingPlugin *self)
{
	gtk_widget_show(self->dialog);
}

static gboolean
impl_activate (LmplayerPlugin *plugin, LmplayerObject *lmplayer, GError **error)
{
	GtkBuilder *builder;
	GtkWidget *start_button;
	GtkWidget *end_button;
	GtkWidget *stop_button;
	GtkWidget *quit_button;

	LmplayerRereadingPlugin *self = LMPLAYER_REREADING_PLUGIN (plugin);

	self->lmplayer = g_object_ref(lmplayer);

	GtkWindow *main_window = lmplayer_get_main_window(lmplayer);
	builder = lmplayer_plugin_load_interface(plugin, "rereading.ui", TRUE, main_window, self);

	self->dialog = GTK_WIDGET(gtk_builder_get_object(builder, "rereading-window"));
	gtk_window_set_transient_for(GTK_WINDOW(self->dialog), main_window);
	gtk_window_set_position(GTK_WINDOW(self->dialog), GTK_WIN_POS_CENTER_ON_PARENT);
	g_object_unref(main_window);

	gtk_window_set_title(GTK_WINDOW(self->dialog), _("Lmplayer rereading"));
	gtk_window_set_resizable(GTK_WINDOW(self->dialog), FALSE);

	g_signal_connect(G_OBJECT(self->dialog), "delete-event", 
			G_CALLBACK(gtk_widget_hide_on_delete), NULL);
	g_signal_connect(G_OBJECT(self->dialog), "destroy", 
			G_CALLBACK(gtk_widget_destroyed), &self->dialog);

	start_button = GTK_WIDGET(gtk_builder_get_object(builder, "mark-start-button"));
	end_button = GTK_WIDGET(gtk_builder_get_object(builder, "mark-end-button"));
	stop_button = GTK_WIDGET(gtk_builder_get_object(builder, "stop-rereading-button"));
	quit_button = GTK_WIDGET(gtk_builder_get_object(builder, "quit-rereading-button"));

	g_signal_connect(G_OBJECT(start_button), "clicked", G_CALLBACK(start_button_clicked_cb), self);
	g_signal_connect(G_OBJECT(end_button), "clicked", G_CALLBACK(end_button_clicked_cb), self);
	g_signal_connect(G_OBJECT(stop_button), "clicked", G_CALLBACK(stop_button_clicked_cb), self);
	g_signal_connect(G_OBJECT(quit_button), "clicked", G_CALLBACK(quit_button_clicked_cb), self);

	g_timeout_add_seconds(1, (GSourceFunc)timer_cb, self);

	// setup ui icon
	GtkWidget *active_button = gtk_button_new();
	GtkWidget *image = gtk_image_new_from_file(LMPLAYER_PLUGIN_DIR"/rereading/rereading.png");
	gtk_container_add(GTK_CONTAINER(active_button), image);
	gtk_button_set_relief(GTK_BUTTON(active_button), GTK_RELIEF_NONE);
	gtk_widget_set_tooltip_text(active_button, _("Rereading"));

	g_signal_connect(active_button, "clicked", 
			G_CALLBACK(active_button_clicked_cb), self);

	lmplayer_add_tools_button(lmplayer, active_button, "rereading");

	/* Initialise resources, connect to events, create menu items and UI, etc., here.
	 * Note that impl_activate and impl_deactivate can be called multiple times in one
	 * Lmplayer instance, though impl_activate will always be followed by impl_deactivate before
	 * it is called again. Similarly, impl_deactivate cannot be called twice in succession. 
	 */

	return TRUE;
}

static void
impl_deactivate(LmplayerPlugin *plugin, LmplayerObject *lmplayer)
{
	LmplayerRereadingPlugin *self = LMPLAYER_REREADING_PLUGIN (plugin);

	g_object_unref(self->lmplayer);

	/* Destroy resources created in impl_activate here. e.g. Disconnect from signals
	 * and remove menu entries and UI. */
}

