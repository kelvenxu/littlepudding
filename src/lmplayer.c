/* vi: set sw=4 ts=4: */
/*
 * lmplayer.c
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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <totem-disc.h>

#include "lmplayer.h"
#include "lmplayer-options.h"
#include "lmplayer-interface.h"
#include "lmplayer-menu.h"
#include "lmplayer-uri.h"
#include "lmplayer-dvb-setup.h"
#include "lmplayer-encode.h"
#include "lmplayer-utils.h"
#include "search-library.h"
#include "lmplayer-search.h"
#include "lmplayer-config.h"
#include "lmplayer-prefs.h"
#include "lmplayer-statusbar.h"

static UniqueResponse lmplayer_message_received_cb(UniqueApp *app,
			   int                command,
			   UniqueMessageData *message_data,
			   guint              time_,
			   LmplayerObject             *lmplayer);

gboolean seek_slider_pressed_cb (GtkWidget *widget, GdkEventButton *event, LmplayerObject *lmplayer);
void seek_slider_changed_cb (GtkAdjustment *adj, LmplayerObject *lmplayer);
gboolean seek_slider_released_cb(GtkWidget *widget, GdkEventButton *event, LmplayerObject *lmplayer);
static void lmplayer_volume_value_changed_cb(GtkScaleButton *hscale, gdouble value, LmplayerObject *lmplayer);
static gchar * lmplayer_build_default_playlist_filename();


/*
static void
lmplayer_action_save_state (LmplayerObject *lmplayer, const char *page_id)
{
	GKeyFile *keyfile;
	char *contents, *filename;

	//if (lmplayer->win == NULL)
	//	return;

	keyfile = g_key_file_new ();

	contents = g_key_file_to_data (keyfile, NULL, NULL);
	g_key_file_free (keyfile);
	filename = g_build_filename (lmplayer_dot_dir (), "state.ini", NULL);
	g_file_set_contents (filename, contents, -1, NULL);

	g_free (filename);
	g_free (contents);
}
*/

static UniqueResponse
lmplayer_message_received_cb(UniqueApp *app,
			   int                command,
			   UniqueMessageData *message_data,
			   guint              time_,
			   LmplayerObject             *lmplayer)
{
	char *url;

	if (message_data != NULL)
		url = unique_message_data_get_text(message_data);
	else
		url = NULL;

	lmplayer_action_remote(lmplayer, command, url);

	g_free (url);

	return UNIQUE_RESPONSE_OK;
}

/*
static void
lmplayer_dvb_setup_result (int result, const char *device, gpointer user_data)
{
	LmplayerObject *lmplayer = (LmplayerObject*) user_data;

	lmplayer_action_play_media (lmplayer, MEDIA_TYPE_DVB, device);
}
*/

/**
 * lmplayer_build_default_playlist_filename:
 *
 * Returns: a string. Free when no used.
 */
static gchar *
lmplayer_build_default_playlist_filename()
{
	const gchar *home = NULL;
	gchar *cfg_path = NULL;
	gchar *fn = NULL;

	home = g_getenv("HOME");

	if(home != NULL)
		cfg_path = g_build_path(G_DIR_SEPARATOR_S, home, ".lmplayer", NULL);

	if(cfg_path != NULL)
	{
		fn = g_build_filename(cfg_path, "default_playlist.pls", NULL);
		g_free(cfg_path);
	}

	return fn;
}

void 
lmplayer_action_load_default_playlist(LmplayerObject *lmplayer)
{
	gchar *uri;

	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	if(lmplayer->pls == NULL)
		lmplayer->pls = lmplayer_build_default_playlist_filename();
	if(lmplayer->pls == NULL)
		return;

	uri = g_filename_to_uri(lmplayer->pls, NULL, NULL);
	if(uri != NULL)
	{
		lmplayer_playlist_add_mrl(lmplayer->playing_playlist, uri, NULL);
		g_free(uri);
		uri = NULL;
	}
}


#if 0
static gboolean
show_notebook_menu(LmplayerObject *lmplayer, GdkEventButton *event)
{
	static GtkWidget *menu = NULL;

	if(menu == NULL)
	{
		menu = gtk_menu_new();
		GtkWidget *item = gtk_menu_item_new_with_label(_("Rename"));
		gtk_widget_show(item);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		g_signal_connect_swapped(item, "activate", G_CALLBACK(notebook_tab_rename_cb), lmplayer);

		item = gtk_menu_item_new_with_label(_("Close"));
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		gtk_widget_show(item);
		g_signal_connect_swapped(item, "activate", G_CALLBACK(notebook_tab_close_cb), lmplayer);
	}

	gtk_menu_popup(GTK_MENU(menu),  NULL, NULL, NULL, NULL, event->button, event->time);

	return TRUE;
}
#endif

static void 
notebook_switch_page_cb(GtkNotebook *nb, GtkNotebookPage *page, gint page_num, LmplayerObject *lmplayer)
{
	lmplayer->current_playlist = (LmplayerPlaylist *)gtk_notebook_get_nth_page(nb, page_num);
}

static void
lmplayer_action_load_playlists(LmplayerObject *lmplayer)
{
	gboolean find_playlist = FALSE;
	GtkWidget *default_playlist = NULL;

	lmplayer->playlist_notebook = lmplayer_notebook_new();
	g_signal_connect(lmplayer->playlist_notebook, "switch-page", G_CALLBACK(notebook_switch_page_cb), lmplayer);

	GtkWidget *box = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-playlist-box");
	gtk_container_add(GTK_CONTAINER(box), GTK_WIDGET(lmplayer->playlist_notebook));

	gchar *root = g_build_filename(g_getenv("HOME"), ".lmplayer", NULL);
	GDir *dir = g_dir_open(root, 0, NULL);

	if(dir)
	{
		const char *filename = g_dir_read_name(dir);
		while(filename)
		{
			if(g_str_has_suffix(filename, ".pls"))
			{
				char *fullname = g_build_filename(root, filename, NULL);

				GtkWidget *pls = lmplayer_create_playlist_widget(lmplayer, fullname);
				char *label_name = g_path_get_basename(fullname);
				label_name[strlen(label_name) - 4] = '\0'; //delete suffix '.pls'
				lmplayer_notebook_append_page(lmplayer->playlist_notebook, pls, label_name);

				if(strcmp(label_name, "Default") == 0)
					default_playlist = pls;

				g_free(label_name);
				g_free(fullname);
			}
			filename = g_dir_read_name(dir);
		}
	}

	g_dir_close(dir);
	g_free(root);

	gtk_widget_show_all(lmplayer->playlist_notebook);

	if(!default_playlist)
	{
		default_playlist = lmplayer_create_playlist_widget(lmplayer, NULL);
		lmplayer_notebook_append_page(lmplayer->playlist_notebook, default_playlist, "Default");
	}

	//set current playlist and playing playlist
	//int idx = gtk_notebook_get_current_page(lmplayer->playlist_notebook);
	//lmplayer->playing_playlist = gtk_notebook_get_nth_page(lmplayer->playlist_notebook, idx);
	lmplayer->playing_playlist = default_playlist;
	lmplayer->current_playlist = lmplayer->playing_playlist;
}

gboolean
seek_slider_pressed_cb (GtkWidget *widget, GdkEventButton *event, LmplayerObject *lmplayer)
{
	lmplayer->seek_lock = TRUE;

	if(lmplayer->statusbar)
		lmplayer_statusbar_set_seeking(LMPLAYER_STATUSBAR(lmplayer->statusbar), TRUE);

	return FALSE;
}

gboolean
seek_slider_released_cb(GtkWidget *widget, GdkEventButton *event, LmplayerObject *lmplayer)
{
	gdouble val;

	lmplayer->seek_lock = FALSE;

	val = gtk_range_get_value(GTK_RANGE(lmplayer->seek));

	if(lmplayer->stream_length && bacon_video_widget_can_direct_seek(lmplayer->bvw))
		lmplayer_action_seek(lmplayer, (double)val / lmplayer->stream_length); // 需要的是一个比例值

	if(lmplayer->statusbar)
		lmplayer_statusbar_set_seeking(LMPLAYER_STATUSBAR(lmplayer->statusbar), FALSE);

	return FALSE;
}

static gboolean 
main_window_destroy_cb(GtkWidget *widget, LmplayerObject *lmplayer)
{
	lmplayer_action_exit (lmplayer);
	search_library_quit();
	return FALSE;
}

#if 0
static void
lmplayer_action_view_switch(LmplayerObject *lmplayer)
{
	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	if(lmplayer->view_type == LMPLAYER_VIEW_TYPE_PLAYLIST)
	{
		gtk_notebook_set_current_page(GTK_NOTEBOOK(lmplayer->view), LMPLAYER_VIEW_TYPE_VISUAL);
		lmplayer->view_type = LMPLAYER_VIEW_TYPE_VISUAL;
	}
	else
	{
		gtk_notebook_set_current_page(GTK_NOTEBOOK(lmplayer->view), LMPLAYER_VIEW_TYPE_PLAYLIST);
		lmplayer->view_type = LMPLAYER_VIEW_TYPE_PLAYLIST;
	}
}
#endif

static void
lmplayer_action_playlist_view(LmplayerObject *lmplayer)
{
	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	if(lmplayer->view_type != LMPLAYER_VIEW_TYPE_PLAYLIST)
	{
		gtk_notebook_set_current_page(GTK_NOTEBOOK(lmplayer->view), LMPLAYER_VIEW_TYPE_PLAYLIST);
		lmplayer->view_type = LMPLAYER_VIEW_TYPE_PLAYLIST;
	}
}

static void
lmplayer_action_visual_view(LmplayerObject *lmplayer)
{
	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	if(lmplayer->view_type != LMPLAYER_VIEW_TYPE_VISUAL)
	{
		gtk_notebook_set_current_page(GTK_NOTEBOOK(lmplayer->view), LMPLAYER_VIEW_TYPE_VISUAL);
		lmplayer->view_type = LMPLAYER_VIEW_TYPE_VISUAL;
	}
}

static void
playlist_view_button_clicked_cb(GtkButton *button, LmplayerObject *lmplayer)
{
	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	lmplayer_action_playlist_view(lmplayer);
}

static void
order_switch_button_clicked_cb(GtkButton *button, LmplayerObject *lmplayer)
{
	static int click_counter = 0;

	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	click_counter++;

	GtkWidget *image = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-order-switch-image");
	switch(click_counter % LMPLAYER_ORDER_NUMBER)
	{
		case LMPLAYER_ORDER_NORMAL:
			lmplayer_playlist_set_repeat(lmplayer->playing_playlist, FALSE);
			lmplayer->repeat = FALSE;
			lmplayer->repeat_one = FALSE;
			gtk_image_set_from_file(GTK_IMAGE(image), DATADIR"/lmplayer/ui/order.png");
			break;
		case LMPLAYER_ORDER_REPEAT:
			lmplayer_playlist_set_repeat(lmplayer->playing_playlist, TRUE);
			lmplayer->repeat = TRUE;
			lmplayer->repeat_one = FALSE;
			gtk_image_set_from_file(GTK_IMAGE(image), DATADIR"/lmplayer/ui/repeat.png");
			break;
		case LMPLAYER_ORDER_REPEAT_ONE:
			lmplayer_playlist_set_repeat(lmplayer->playing_playlist, FALSE);
			lmplayer->repeat_one = TRUE;
			lmplayer->repeat = FALSE;
			gtk_image_set_from_file(GTK_IMAGE(image), DATADIR"/lmplayer/ui/repeat-one.png");
			break;
	}
}

static void 
prefs_button_clicked_cb(GtkButton *button, LmplayerObject *lmplayer)
{
	lmplayer_prefs_show(lmplayer);
}

static gboolean
lmplayer_plugins_window_delete_cb(GtkWidget *window, GdkEventAny *event, gpointer data)
{
	gtk_widget_hide (window);

	return TRUE;
}

static void
lmplayer_plugins_response_cb(GtkDialog *dialog, int response_id, gpointer data)
{
	if (response_id == GTK_RESPONSE_CLOSE)
		gtk_widget_hide (GTK_WIDGET (dialog));
}

static void 
about_button_clicked_cb(GtkButton *button, LmplayerObject *lmplayer)
{
	char *backend_version, *description;

	const char *authors[] =
	{
		"Kelven Xu <kelvenxu@gmail.com>",
		NULL
	};
	const char *artists[] = { "Kelven Xu <kelvenxu@gmail.com>", NULL };
	const char *documenters[] = { "Kelven Xu <kelvenxu@gmail.com>", NULL };
	char *license = lmplayer_interface_get_license ();

	backend_version = bacon_video_widget_get_backend_name (lmplayer->bvw);
	/* This lists the back-end type and version, such as
	 * Movie Player using GStreamer 0.10.1 */
	description = g_strdup_printf (_("Music Player using %s"), backend_version);

	gtk_show_about_dialog (GTK_WINDOW (lmplayer->win),
				     "version", VERSION,
				     "copyright", _("Copyright \xc2\xa9 2007-2010 Kelven Xu"),
				     "comments", description,
				     "authors", authors,
				     "documenters", documenters,
				     "artists", artists,
				     "translator-credits", _("translator-credits"),
				     "logo-icon-name", "lmplayer",
				     "license", license,
				     "wrap-license", TRUE,
				     "website-label", _("Lmplayer Website"),
				     "website", "http://www.lmplayer.com/",
				     NULL);

	g_free (backend_version);
	g_free (description);
	g_free (license);
}

static void 
lmplayer_callback_connect(LmplayerObject *lmplayer)
{
	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	gtk_widget_add_events(GTK_WIDGET(lmplayer->seek), GDK_SCROLL_MASK);

	g_signal_connect(G_OBJECT(lmplayer->volume), "value-changed",
			G_CALLBACK(lmplayer_volume_value_changed_cb), lmplayer);

	//g_signal_connect(G_OBJECT(lmplayer->seek), "value-changed",
	//		G_CALLBACK(seek_slider_changed_cb), lmplayer);

	g_signal_connect(G_OBJECT(lmplayer->seek), "button-press-event",
			G_CALLBACK(seek_slider_pressed_cb), lmplayer);

	g_signal_connect(G_OBJECT(lmplayer->seek), "button-release-event",
			G_CALLBACK(seek_slider_released_cb), lmplayer);

	g_signal_connect(G_OBJECT(lmplayer->playlist_view_button), "clicked", 
				G_CALLBACK(playlist_view_button_clicked_cb), lmplayer);

	g_signal_connect(G_OBJECT(lmplayer->order_switch_button), "clicked", 
				G_CALLBACK(order_switch_button_clicked_cb), lmplayer);

	g_signal_connect(G_OBJECT(lmplayer->about_button), "clicked", G_CALLBACK(about_button_clicked_cb), lmplayer);
	g_signal_connect(G_OBJECT(lmplayer->prefs_button), "clicked", G_CALLBACK(prefs_button_clicked_cb), lmplayer);
	g_signal_connect(G_OBJECT(lmplayer->win), "destroy", G_CALLBACK(main_window_destroy_cb), lmplayer);
	
}

static void 
lmplayer_volume_value_changed_cb(GtkScaleButton *scale, gdouble value, LmplayerObject *lmplayer)
{
	g_return_if_fail(GTK_IS_SCALE_BUTTON(scale));
	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	bacon_video_widget_set_volume(lmplayer->bvw, value);
}

#if 0
static void
eqfactor_value_changed_cb(SkinVScale *vscale, LmplayerObject *lmplayer)
{
	gdouble *gains;
	gdouble value;
	gchar name[24];
	gint i;

	gains = bacon_video_widget_get_equalizer_gain(lmplayer->bvw);

	for(i = 0; i < 10; ++i)
	{
		SkinVScale *scale;
		sprintf(name, "equalizer-eqfactor%d", i);
		scale = (SkinVScale*)skin_builder_get_object(lmplayer->builder, name);
		g_object_get(G_OBJECT(scale), 
				"value", &value,
				NULL);

		gains[i] = value / 100.0 * (12.0 - (-24.0)) + (-24.0);
	}

	bacon_video_widget_set_equalizer_gain(lmplayer->bvw, gains);
}
#endif

#if 0
static void
lmplayer_equalizer_setup(LmplayerObject *lmplayer)
{
	gchar name[24];
	gdouble *gains;
	gdouble value;
	gint i;

	gains = bacon_video_widget_get_equalizer_gain(lmplayer->bvw);

	for(i = 0; i < 10; ++i)
	{
		SkinVScale *vscale;
		sprintf(name, "equalizer-eqfactor%d", i);
		vscale = (SkinVScale*)skin_builder_get_object(lmplayer->builder, name);

		value = (gains[i] - (-24.0)) / (12.0 - (-24.0)) * 100.0 + 0.0;
		g_object_set(G_OBJECT(vscale), 
				//"min", -24.0,
				//"max", 12.0,
				"min", 0.0,
				"max", 100.0,
				"value", value,
				NULL);
		g_signal_connect(G_OBJECT(vscale), "value_changed", 
				G_CALLBACK(eqfactor_value_changed_cb), lmplayer);
	}
}
#endif

static void
lmplayer_message_connection_receive_cb (const char *msg, LmplayerObject *lmp)
{
	char *command_str, *url;
	int command;

	if (strlen (msg) < 4)
		return;

	command_str = g_strndup (msg, 3);
	sscanf (command_str, "%d", &command);
	g_free (command_str);

	if (msg[4] != '\0')
		url = g_strdup (msg + 4);
	else
		url = NULL;

	lmplayer_action_remote (lmp, command, url);

	g_free (url);
}

static GtkBuilder *
lmplayer_load_ui(LmplayerObject *lmplayer)
{
	gchar *uiname = NULL;
	GtkBuilder *builder = NULL;

	gchar *ui = gconf_client_get_string(lmplayer->gc, GCONF_PREFIX"/ui", NULL);
	if(ui)
	{
		uiname = g_build_filename("ui", ui, NULL);
		g_free(ui);
	}
	else
	{
		gconf_client_set_string(lmplayer->gc, GCONF_PREFIX"/ui", "lmplayer-default.ui", NULL);
		uiname = g_strdup("ui/lmplayer-default.ui");
	}

	if(uiname)
	{
		builder = lmplayer_interface_load(uiname, TRUE, NULL, NULL);
		g_free(uiname);
	}

	return builder;
}

int 
main(int argc, char* argv[])
{
	LmplayerObject *lmplayer;
	GOptionContext *context;
	GOptionGroup *baconoptiongroup;
	GConfClient *gc;
	GError* error = NULL;

	gboolean hasfiles = FALSE;

	bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

	gtk_init(&argc, &argv);
	g_thread_init(NULL);

	bacon_video_widget_init_backend(NULL, NULL);

	context = g_option_context_new(N_("[FILES] - Play songs"));
	baconoptiongroup = bacon_video_widget_get_option_group();
	g_option_context_add_main_entries(context, options, GETTEXT_PACKAGE);
	g_option_context_set_translation_domain(context, GETTEXT_PACKAGE);
	g_option_context_add_group (context, baconoptiongroup);

	if(g_option_context_parse (context, &argc, &argv, &error) == FALSE)
	{
		g_print(_("%s\nRun '%s --help' to see a full list of available command line options.\n"), 
				error->message, argv[0]);
		g_error_free (error);
		lmplayer_action_exit (NULL);
	}

	g_set_application_name(_("Little Pudding Music Player"));
	gtk_window_set_default_icon_name ("lmplayer");

	gc = gconf_client_get_default ();
	if (gc == NULL)
	{
		lmplayer_action_error_and_exit (_("Lmplayer could not initialize the configuration engine."), 
				_("Make sure that GNOME is properly installed."), NULL);
	}

	lmplayer = lmplayer_object_new();

	if(lmplayer == NULL)
		lmplayer_action_exit(NULL);

	lmplayer->gc = gc;
	lmplayer->pls = NULL;
	lmplayer->view_type = LMPLAYER_VIEW_TYPE_PLAYLIST;

	if(optionstate.notconnectexistingsession == FALSE)
	{
		lmplayer->uapp = unique_app_new("com.lmplayer.Lmplayer", NULL);
		lmplayer_options_register_remote_commands(lmplayer);
		if(unique_app_is_running(lmplayer->uapp) != FALSE)
		{
			lmplayer_options_process_for_server(lmplayer->uapp, &optionstate);
			gdk_notify_startup_complete ();
			lmplayer_action_exit (lmplayer);
		}
		else
		{
			lmplayer_options_process_early(lmplayer, &optionstate);
		}
	}
	else
	{
		lmplayer_options_process_early (lmplayer, &optionstate);
	}

	lmplayer->builder = lmplayer_load_ui(lmplayer);
	if(!lmplayer->builder)
	{
		lmplayer_action_exit(NULL);
	}
	
	lmplayer->win = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-window");
	if(lmplayer->win == NULL)
	{
		lmplayer_action_exit(NULL);
	}

	lmplayer->seek = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, 
			"player-progressbar");
	lmplayer->seekadj = gtk_range_get_adjustment(GTK_RANGE(lmplayer->seek));

	lmplayer->volume = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-volume");
	//lmplayer->statusbar = GTK_WIDGET(gtk_builder_get_object(lmplayer->xml, "player-statusbar"));
	lmplayer->view = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-view");
	lmplayer->playlist_view_button = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, 
			"player-playlist-view-button");
	lmplayer->order_switch_button = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, 
			"player-order-switch-button");
	lmplayer->about_button = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, 
			"player-about-button");
	lmplayer->plugin_button = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, 
			"player-plugin-button");
	lmplayer->prefs_button = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, 
			"player-preference-button");
	lmplayer->plugins_box = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, 
			"player-plugins-box");
	lmplayer->extra_widget_box = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, 
			"player-extra-widget-box");
	lmplayer->current_time_label = (GtkWidget *)gtk_builder_get_object(lmplayer->builder,
			"player-current-time-label");
	lmplayer->total_time_label = (GtkWidget *)gtk_builder_get_object(lmplayer->builder,
			"player-total-time-label");
	
	lmplayer_search_view_setup(lmplayer);

	lmplayer->repeat = FALSE;
	lmplayer->repeat_one = FALSE;

	lmplayer_callback_connect(lmplayer);

	lmplayer_ui_manager_setup(lmplayer);
	video_widget_create(lmplayer); 

	//lmplayer_action_load_playlists(lmplayer);
	
	lmplayer->state = STATE_STOPPED;
	lmplayer->seek_lock = FALSE;

	lmplayer_setup_file_monitoring(lmplayer);
	lmplayer_setup_file_filters();
	lmplayer_setup_toolbar(lmplayer);
	lmplayer_setup_statusbar(lmplayer);


	lmplayer_options_process_late(lmplayer, &optionstate);

	lmplayer_object_plugins_init(lmplayer);

#if 0
	if(optionstate.filenames != NULL && lmplayer_action_open_files(lmplayer, optionstate.filenames))
	{
		hasfiles = TRUE;
		lmplayer_action_play_pause(lmplayer);
	}
	else
	{
		lmplayer_action_set_mrl(lmplayer, NULL, NULL);
	}
#endif

	gtk_widget_show_all(GTK_WIDGET(lmplayer->win));

	lmplayer_search_init(lmplayer);

	lmplayer_action_load_playlists(lmplayer);
	lmplayer_action_stop(lmplayer);

	if(lmplayer->uapp != NULL) 
	{
		g_signal_connect(lmplayer->uapp, "message-received",
				  G_CALLBACK(lmplayer_message_received_cb), lmplayer);
	}

	if(optionstate.filenames != NULL && lmplayer_action_open_files(lmplayer, optionstate.filenames))
	{
		hasfiles = TRUE;
		lmplayer_action_play_pause(lmplayer);
	}
	else
	{
		lmplayer_action_set_mrl(lmplayer, NULL, NULL);
	}

	gtk_main();
	return 0;
}


