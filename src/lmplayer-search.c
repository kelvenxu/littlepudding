/* vi: set sw=4 ts=4: */
/*
 * lmplayer-search.c
 *
 * This file is part of ________.
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

#include "lmplayer-search.h"
#include "search-library.h"
static void 
search_view_row_activated_cb(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, LmplayerObject *lmplayer)
{
	GtkTreeModel *model;
	GtkTreeIter iter;

	model = gtk_tree_view_get_model(treeview);
	if(gtk_tree_model_get_iter(model, &iter, path))
	{
		gchar *filename;
		gtk_tree_model_get(model, &iter, 0, &filename, -1);
		gchar *uri = g_filename_to_uri(filename, NULL, NULL);
		g_free(filename);
		lmplayer_playlist_add_mrl(lmplayer->playlist, uri, NULL);
		g_print("select uri %s\n", uri);
		g_free(uri);
	}
}

static void
lmplayer_action_search_view(LmplayerObject *lmplayer)
{
	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	if(lmplayer->view_type != LMPLAYER_VIEW_TYPE_SEARCH)
	{
		gtk_notebook_set_current_page(GTK_NOTEBOOK(lmplayer->view), LMPLAYER_VIEW_TYPE_SEARCH);
		lmplayer->view_type = LMPLAYER_VIEW_TYPE_SEARCH;
	}
}

static void
search_view_button_clicked_cb(GtkButton *button, LmplayerObject *lmplayer)
{
	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	lmplayer_action_search_view(lmplayer);
}

void
lmplayer_search_view_setup(LmplayerObject *lmplayer)
{
	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	// search view setup
	lmplayer->search_view = search_view_create();
	lmplayer->search_box = search_box_create();

	gtk_widget_set_size_request(lmplayer->search_box, 256, -1);

	lmplayer->search_box_box = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "search-box-box");
	gtk_box_pack_start(GTK_BOX(lmplayer->search_box_box), lmplayer->search_box, TRUE, TRUE, 0);

	GtkWidget *scrolled_win = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolled_win), GTK_WIDGET(lmplayer->search_view));

	GtkWidget *box = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-search-view-box");
	gtk_container_add(GTK_CONTAINER(box), GTK_WIDGET(scrolled_win));

	lmplayer->search_view_button = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-search-view-button");

	g_signal_connect(G_OBJECT(lmplayer->search_view_button), "clicked", G_CALLBACK(search_view_button_clicked_cb), lmplayer);

	g_signal_connect(lmplayer->search_view, "row-activated", (GCallback)search_view_row_activated_cb, lmplayer);

}

