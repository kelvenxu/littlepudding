/*
 * lmplayer-lyric.c
 *
 * This file is part of lyric plugin.
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
#include "lyric-downloader.h"
#include "lyric-downloader-tt.h"
#include "lyric-widget.h"
#include "lyric-widget-da.h"
#include "lyric-widget-text.h"

#define LMPLAYER_TYPE_LYRIC_PLUGIN	(lmplayer_lyric_plugin_get_type())
#define LMPLAYER_LYRIC_PLUGIN(o)		(G_TYPE_CHECK_INSTANCE_CAST((o), LMPLAYER_TYPE_LYRIC_PLUGIN, LmplayerLyricPlugin))
#define LMPLAYER_LYRIC_PLUGIN_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), LMPLAYER_TYPE_LYRIC_PLUGIN, LmplayerLyricPluginClass))
#define LMPLAYER_IS_LYRIC_PLUGIN(o) (G_TYPE_CHECK_INSTANCE_TYPE((o), LMPLAYER_TYPE_LYRIC_PLUGIN))
#define LMPLAYER_IS_LYRIC_PLUGIN_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE((k), LMPLAYER_TYPE_LYRIC_PLUGIN))
#define LMPLAYER_LYRIC_PLUGIN_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS((o), LMPLAYER_TYPE_LYRIC_PLUGIN, LmplayerLyricPluginClass))

typedef struct 
{
	LmplayerPlugin parent;

	Lmplayer *lmplayer;

	GtkWidget *nb;
	GtkWidget *lyric_widget;

	LmplayerLyricDownloader *downloader;

	gboolean has_lyric;
	gchar *lyric_filename;

	gchar *title;
	gchar *artist;

	gboolean activated;

	/* plugin object members */
} LmplayerLyricPlugin;

typedef struct 
{
	LmplayerPluginClass parent_class;
} LmplayerLyricPluginClass;

G_MODULE_EXPORT GType register_lmplayer_plugin(GTypeModule *module);
GType lmplayer_lyric_plugin_get_type(void) G_GNUC_CONST;

static gboolean impl_activate(LmplayerPlugin *plugin, LmplayerObject *lmplayer, GError **error);
static void impl_deactivate(LmplayerPlugin *plugin, LmplayerObject *lmplayer);

LMPLAYER_PLUGIN_REGISTER(LmplayerLyricPlugin, lmplayer_lyric_plugin)

static void
lmplayer_lyric_plugin_class_init (LmplayerLyricPluginClass *klass)
{
	LmplayerPluginClass *plugin_class = LMPLAYER_PLUGIN_CLASS (klass);

	plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;
}

static void
lmplayer_lyric_plugin_init(LmplayerLyricPlugin *plugin)
{
	/* Initialise resources, but only ones which should exist for the entire lifetime of Lmplayer;
	 * those which should only exist for the lifetime of the plugin (which may be short, and may
	 * occur several times during one Lmplayer session) should be created in impl_activate, and destroyed
	 * in impl_deactivate. 
	 */

	g_return_if_fail(LMPLAYER_IS_LYRIC_PLUGIN(plugin));

	plugin->title = NULL;
	plugin->artist = NULL;
	plugin->lyric_filename = NULL;

	plugin->has_lyric = FALSE;
	plugin->activated = FALSE;
}

static gboolean
timer_cb(LmplayerLyricPlugin *self)
{
	g_return_val_if_fail(LMPLAYER_IS_LYRIC_PLUGIN(self), FALSE);

	if(!self->activated)
		return FALSE;

	if(self->has_lyric)
	{
		int sec = lmplayer_get_current_time(self->lmplayer);
	
		lmplayer_lyric_widget_set_current_second(LMPLAYER_LYRIC_WIDGET(self->lyric_widget), sec / 1000.0 + 0.5);
	}

	return TRUE;
}

static gboolean
build_lyric_filename(LmplayerLyricPlugin *self, char *mrl)
{
	gint i;
	gchar *fn;
	gboolean flag;
	GError *error = NULL;

	g_return_val_if_fail(LMPLAYER_IS_LYRIC_PLUGIN(self), FALSE);

	fn = g_filename_from_uri(mrl, NULL, &error);
	if(fn == NULL)
	{
		fprintf(stderr, _("Not found lyric file: %s\n"), error->message);
		g_error_free(error);
		return FALSE;
	}

	for(i = strlen(fn) - 1; i >=0; --i)
	{
		if(fn[i] == '.')
		{
			fn[i] = '\0';
			flag = TRUE;
			break;
		}
	}

	if(!flag)
	{
		self->has_lyric = FALSE;
		g_free(fn);
		return FALSE;
	}
	
	if(self->lyric_filename)
		g_free(self->lyric_filename);

	self->lyric_filename = g_strdup_printf("%s.lrc", fn);
	g_free(fn);
	
	return TRUE;
}

static gboolean
load_local_lyric(LmplayerLyricPlugin *self)
{
	g_return_val_if_fail(LMPLAYER_IS_LYRIC_PLUGIN(self), FALSE);

	self->has_lyric = FALSE;

	if(g_file_test(self->lyric_filename, G_FILE_TEST_EXISTS))
	{
		lmplayer_lyric_widget_add_file(LMPLAYER_LYRIC_WIDGET(self->lyric_widget), self->lyric_filename);
		self->has_lyric = TRUE;
	}

	return self->has_lyric;
}

static void
load_lyric(LmplayerLyricPlugin *self, gchar *mrl)
{
	g_return_if_fail(LMPLAYER_IS_LYRIC_PLUGIN(self));
	g_return_if_fail(mrl);

	if(!build_lyric_filename(self, mrl))
		return;

	if(!load_local_lyric(self))
	{
		if(self->title)
		{
			g_free(self->title);
			self->title = NULL;
		}

		if(self->artist)
		{
			g_free(self->artist);
			self->artist = NULL;
		}

		self->title = lmplayer_get_current_title(self->lmplayer);
		self->artist = lmplayer_get_current_artist(self->lmplayer);

		if(self->artist && self->title && self->lyric_filename)
		{
			lmplayer_lyric_downloader_download(self->downloader, 
																			self->title, 
																			self->artist, 
																			self->lyric_filename);
		}
	}
}

static void
start_playing_cb(LmplayerObject *lmplayer, gchar *mrl, LmplayerLyricPlugin *self)
{
	load_lyric(self, mrl);
}

static void
lyric_downloader_finished_cb(LmplayerLyricDownloader *downloader, LmplayerLyricPlugin *self)
{
	g_print("download finished \n");
	// add file to lyric widget ? which file ?
	
	load_local_lyric(self);
}

static gboolean            
lyric_box_resize_cb(GtkWidget *widget, GtkAllocation *allocation, LmplayerLyricPlugin *self)
{
	static gint width;
	static gint height;
	if(width != allocation->width || height != allocation->height)
	{
		g_print("lyric_box configure cb: w: %d h: %d\n", allocation->width, allocation->height);
		//lmplayer_lyric_widget_set_size(LMPLAYER_LYRIC_WIDGET(self->lyric_widget), 
		//															allocation->width, 
		//															allocation->height);
		width = allocation->width;
		height = allocation->height;
	}
	return FALSE;
}

static void 
create_lyric_widget(LmplayerLyricPlugin *self)
{
	g_return_if_fail(LMPLAYER_IS_LYRIC_PLUGIN(self));

	self->nb = gtk_notebook_new();

	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(self->nb), FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(self->nb), TRUE);

	//self->lyric_widget = lmplayer_lyric_widget_da_new();
	self->lyric_widget = lmplayer_lyric_widget_text_new();

	gtk_notebook_append_page(GTK_NOTEBOOK(self->nb), self->lyric_widget, NULL);

	GtkStyle *style = gtk_widget_get_default_style();

	lmplayer_lyric_widget_set_color(LMPLAYER_LYRIC_WIDGET(self->lyric_widget), 
																	&(style->white),
																	&(style->black), 
																	&(style->bg[GTK_STATE_SELECTED]));

	lmplayer_lyric_widget_set_size(LMPLAYER_LYRIC_WIDGET(self->lyric_widget), 200, 400);

	g_signal_connect(G_OBJECT(self->nb), "size-allocate", G_CALLBACK(lyric_box_resize_cb), self);

	gtk_widget_show_all(GTK_WIDGET(self->nb));

	self->downloader = g_object_new(LMPLAYER_TYPE_LYRIC_DOWNLOADER_TT, NULL);

	g_signal_connect(G_OBJECT(self->downloader), "download_finished", 
			G_CALLBACK(lyric_downloader_finished_cb), self);
}

static gboolean
impl_activate (LmplayerPlugin *plugin, LmplayerObject *lmplayer, GError **error)
{
	LmplayerLyricPlugin *self = LMPLAYER_LYRIC_PLUGIN (plugin);

	self->lmplayer = g_object_ref(lmplayer);

	create_lyric_widget(self);

	g_assert(LMPLAYER_IS_LYRIC_WIDGET(self->lyric_widget));
	lmplayer_add_extra_widget(lmplayer, self->nb);

	self->activated = TRUE;

	if(lmplayer_is_playing(self->lmplayer))
	{
		gchar *mrl = lmplayer_get_current_mrl(self->lmplayer);
		if(mrl)
		{
			load_lyric(self, mrl);
			g_free(mrl);
		}
	}

	g_timeout_add_seconds(1, (GSourceFunc)timer_cb, self);

	g_signal_connect(G_OBJECT(lmplayer), "start-playing", G_CALLBACK(start_playing_cb), self);

	return TRUE;
}

static void
impl_deactivate(LmplayerPlugin *plugin, LmplayerObject *lmplayer)
{
	LmplayerLyricPlugin *self = LMPLAYER_LYRIC_PLUGIN (plugin);

	g_return_if_fail(LMPLAYER_IS_LYRIC_PLUGIN(self));

	self->activated = FALSE;

	if(self->title)
	{
		g_free(self->title);
		self->title = NULL;
	}

	if(self->artist)
	{
		g_free(self->artist);
		self->artist = NULL;
	}
	
	if(self->lyric_filename)
	{
		g_free(self->lyric_filename);
		self->lyric_filename = NULL;
	}

	self->has_lyric = FALSE;

	lmplayer_remove_extra_widget(self->lmplayer, self->nb);
	g_object_unref(self->lmplayer);

	/* Destroy resources created in impl_activate here. e.g. Disconnect from signals
	 * and remove menu entries and UI. */
}

