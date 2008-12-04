/* vi: set sw=4 ts=4: */
/*
 * main.c
 *
 * This file is part of littlepudding.
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


#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <stdio.h>
#include <stdlib.h>
#include "lyric_downloader.h"
enum {
	ID_COL,
	INFO_COL,
	DATA_COL,
	NUM_COLS
};

typedef struct Options
{
	gchar *title;
	gchar *artist;
	gchar *output;
} Options;

static Options opt;

const GOptionEntry options[] = {
	{"title", 't', 0, G_OPTION_ARG_STRING, &opt.title, N_("Song's title"), NULL},
	{"artist", 'a', 0, G_OPTION_ARG_STRING, &opt.artist, N_("Song's artist"), NULL},
	{"output", 'o', 0, G_OPTION_ARG_STRING, &opt.output, N_("Save lyric to this file"), NULL}, 
};

void row_activated_callback(GtkTreeView *treeview, 
						GtkTreePath *path, 
						GtkTreeViewColumn *column, 
						gpointer user_data);
void download_callback(GtkButton *button, GtkBuilder *builder);
void cancel_callback(GtkButton *button, gpointer user_data);
void about_callback(GtkButton *button, GtkBuilder *builder);

static void _do_download(GtkTreeView *treeview);
static GtkBuilder* lyric_downloader_ui_builder_new();
static void lyric_downloader_dialog_show(GtkBuilder *builder);
static void lyric_downloader_set_lyric_list(GtkBuilder *builder, GSList *list);


void
row_activated_callback(GtkTreeView *treeview, 
						GtkTreePath *path, 
						GtkTreeViewColumn *column, 
						gpointer user_data)
{
	_do_download(treeview);
	gtk_main_quit();
}

void
download_callback(GtkButton *button, GtkBuilder *builder)
{
	GtkTreeView *treeview;
	treeview = (GtkTreeView*)gtk_builder_get_object(builder, "items-list");	
	_do_download(treeview);
	gtk_main_quit();
}

void
cancel_callback(GtkButton *button, gpointer user_data)
{
	gtk_main_quit();
}

void
about_callback(GtkButton *button, GtkBuilder *builder)
{
	gtk_main_quit();
}

static void
_do_download(GtkTreeView *treeview)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model = NULL;
	GtkTreeIter iter;
	LyricId *item;
	selection = gtk_tree_view_get_selection(treeview);
	g_return_if_fail(GTK_IS_TREE_SELECTION(selection));
	if(gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gtk_tree_model_get(model, &iter,
				DATA_COL, &item,
				-1);

		unsigned int id = atoi(item->id);
		tt_get_lyrics_content_and_save(id, item->artist, item->title, opt.output);
	}
}

static GtkTreeModel* 
create_and_fill_model()
{
	GtkListStore  *store;

	store = gtk_list_store_new (NUM_COLS, 
		  G_TYPE_UINT, 
		  G_TYPE_STRING,
		  G_TYPE_POINTER);

	return GTK_TREE_MODEL (store);
}

static void
init_items_list(GtkTreeView *lists)
{
	GtkCellRenderer *renderer;

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(lists,
			-1,
			_("ID"),
			renderer,
			"text", ID_COL,
			NULL);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(lists,
			-1,
			_("Name"),
			renderer,
			"text", INFO_COL,
			NULL);

	GtkTreeSelection *selection;
	selection = gtk_tree_view_get_selection(lists);
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_BROWSE);
}

static GtkBuilder *
lyric_downloader_ui_builder_new()
{
	GtkBuilder *builder;
	GError *err = NULL;

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, DATADIR"lyricselectdlg.ui", &err);
	if(err)
	{
		printf("Error: %s\n", err->message);
		g_error_free(err);
		return NULL;
	}

	gtk_builder_connect_signals(builder, builder);

	return builder;
}

static void
lyric_downloader_dialog_show(GtkBuilder *builder)
{
	GtkWidget *window = (GtkWidget*)gtk_builder_get_object(builder, "lyricselectdlg");
	GtkWidget *lists = (GtkWidget*)gtk_builder_get_object(builder, "items-list");

	init_items_list(GTK_TREE_VIEW(lists));

	GtkTreeModel *model = create_and_fill_model();
	gtk_tree_view_set_model(GTK_TREE_VIEW(lists), model);
	g_object_unref(model);

	gtk_widget_show_all(window);
}

static void
lyric_downloader_set_lyric_list(GtkBuilder *builder, GSList *list)
{
	GtkTreeIter tree_iter;
	GtkTreeView *view;
	GtkTreeModel *model;
	GtkListStore *store; 

	GSList *iter = NULL;
	gint i = 0;

	g_return_if_fail(GTK_IS_BUILDER(builder));
	g_return_if_fail(list != NULL);

	view = (GtkTreeView*)gtk_builder_get_object(builder, "items-list");
	model = gtk_tree_view_get_model(view);

	store = GTK_LIST_STORE(model);
	g_return_if_fail(GTK_IS_LIST_STORE(store));

	for(iter = list; iter; iter = iter->next)
	{
		LyricId *id = (LyricId*)iter->data;
		gchar *info = g_strdup_printf("%s - %s", id->artist, id->title);

		gtk_list_store_append(store, &tree_iter);
		gtk_list_store_set(store, &tree_iter,
				ID_COL, ++i,
				INFO_COL, info,
				DATA_COL, id,
				-1);
	}
}


static gboolean
timeout_quit(void)
{
	gtk_main_quit();
	return FALSE;
}

int main(int argc, char *argv[])
{
	GtkBuilder *builder;
	GError *err = NULL;
	GOptionContext *context;

	gtk_init(&argc, &argv);

	context = g_option_context_new("- download lyric");
	g_option_context_add_main_entries(context, options, NULL);
	g_option_context_add_group(context, gtk_get_option_group(TRUE));
	
	if(!g_option_context_parse(context, &argc, &argv, &err))
	{
		printf("option parsing failed: %s\n", err->message);
		return -1;
	}

	if(!opt.title || !opt.artist || !opt.output)
	{
		printf(_("Run '%s --help' to see a full list of available command line options.\n"), argv[0]);
		return -1;
	}

	guint num = 0;
	GSList *list = NULL;

	//FIXME:
	g_timeout_add_seconds(24, timeout_quit, NULL);

	gchar *xml = tt_get_lyrics_list(opt.artist, opt.title);
	if(xml == NULL)
		return -1;

	list = tt_parse_lyricslist(xml);
	g_free(xml);
	if(list == NULL)
		return -1;

	num = g_slist_length(list);
	if(num <= 0)
		return -1;

	if(num == 1)
	{
		LyricId *item = g_slist_nth_data(list, 0);
		unsigned int id = atoi(item->id);
		tt_get_lyrics_content_and_save(id, item->artist, item->title, opt.output);
		g_slist_free(list);
		return 0;
	}

	builder = lyric_downloader_ui_builder_new();
	lyric_downloader_dialog_show(builder);

	lyric_downloader_set_lyric_list(builder, list);

	gtk_main();

	g_slist_foreach(list, (GFunc)g_free, NULL);
	g_slist_free(list);
	return 0;
}

