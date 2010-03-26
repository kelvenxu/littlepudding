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
#include "lmplayer-debug.h"
#include "lmplayer-skin.h"
#include "lmplayer-magnetic.h"
#include "search-library.h"
#include "lmplayer-search.h"
#include "lmplayer-config.h"
#include "lmplayer-prefs.h"
#include "lmplayer-plugin-manager.h"


static void lmplayer_message_connection_receive_cb (const char *msg, LmplayerObject *lmp);

gboolean seek_slider_pressed_cb (GtkWidget *widget, GdkEventButton *event, LmplayerObject *lmplayer);
void seek_slider_changed_cb (GtkAdjustment *adj, LmplayerObject *lmplayer);
gboolean seek_slider_released_cb(GtkWidget *widget, GdkEventButton *event, LmplayerObject *lmplayer);
static void lmplayer_volume_value_changed_cb(GtkScaleButton *hscale, gdouble value, LmplayerObject *lmplayer);
static gchar * lmplayer_build_default_playlist_filename();

//static gboolean lmplayer_build_lyric_name(LmplayerObject *lmplayer);
//static gboolean lmplayer_load_local_lyric(LmplayerObject *lmplayer);

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


/*
static void
lmplayer_dvb_setup_result (int result, const char *device, gpointer user_data)
{
	LmplayerObject *lmplayer = (LmplayerObject*) user_data;

	lmplayer_action_play_media (lmplayer, MEDIA_TYPE_DVB, device);
}
*/


static void 
lmplayer_statusbar_set_text(LmplayerObject *lmplayer, gchar *text)
{
#if 0
	SkinStatusBar *sb;
	g_return_if_fail(LMPLAYER_IS_OBJECT(lmplayer));

	sb = (SkinStatusBar*)skin_builder_get_object(lmplayer->builder, "player-statusbar");

	skin_status_bar_set_text(sb, text);
#endif
}

// 取得string类型的metadata, 并将其转换成utf8编码
static gchar *
lmplayer_metadata_get_artist(LmplayerObject *lmplayer)
{
	gchar *str = NULL;
	gchar *str_utf8 = NULL;
	GValue value = { 0, };

	bacon_video_widget_get_metadata(lmplayer->bvw, BVW_INFO_ARTIST, &value);
	str = g_value_dup_string (&value);
	g_value_unset (&value);

	if(str)
	{
		str_utf8 = lmplayer_encode_convert_to_utf8(str);
		g_free (str);
	}

	return str_utf8;
}

static gchar *
lmplayer_metadata_get_title(LmplayerObject *lmplayer)
{
	gchar *str = NULL;
	gchar *str_utf8 = NULL;
	GValue value = { 0, };

	bacon_video_widget_get_metadata(lmplayer->bvw, BVW_INFO_TITLE, &value);
	str = g_value_dup_string (&value);
	g_value_unset (&value);

	if(str)
	{
		str_utf8 = lmplayer_encode_convert_to_utf8(str);
		g_free (str);
	}

	return str_utf8;
}

#if 0
static gboolean
lmplayer_build_lyric_name(LmplayerObject *lmplayer)
{
	gchar *fn;
	GError *error = NULL;
	gboolean flag;
	gint i;

	if(lmplayer->lyric_filename)
	{
		g_free(lmplayer->lyric_filename);
		lmplayer->lyric_filename = NULL;
	}

	if(lmplayer->mrl == NULL)
		return FALSE;

	fn = g_filename_from_uri(lmplayer->mrl, NULL, &error);
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
		lmplayer->has_lyric = FALSE;
		return FALSE;
	}
	
	lmplayer->lyric_filename = g_strdup_printf("%s.lrc", fn);
	
	return TRUE;
}
#endif

#if 0
static gboolean
lmplayer_load_local_lyric(LmplayerObject *lmplayer)
{
	static gint count = 0;

	if(g_file_test(lmplayer->lyric_filename, G_FILE_TEST_EXISTS))
	{
		lmplayer_debug("load lyric: %s successfully\n", lmplayer->lyric_filename);
		skin_lyric_add_file(lmplayer->lyricview, lmplayer->lyric_filename);
		lmplayer->has_lyric = TRUE;
	}

	++count;
	
	if(lmplayer->has_lyric || count == 5)
		return FALSE;

	return TRUE;
}
#endif

void
lmplayer_load_net_lyric(LmplayerObject *lmplayer)
{
	gchar *title;
	gchar *artist;
	//gchar *cmd;

	printf("download lyric from net\n");
	if(g_file_test(lmplayer->lyric_filename, G_FILE_TEST_EXISTS))
		return;

    title = lmplayer_metadata_get_title(lmplayer);
	if(title == NULL)
		return;

   	artist = lmplayer_metadata_get_artist(lmplayer);
	if(artist == NULL)
	{
		g_free(title);
		return;
	}
	//cmd = g_strdup_printf("lyric-downloader --title %s --artist %s --output %s", 
	//		title, artist, lmplayer->lyric_filename);
	//printf("%s\n", cmd);
	//int re = system(cmd);
	
	if(fork() == 0)
	{
		execlp("lyric-downloader", 
				"lyric-downloader",
				"-a", artist,
				"-t", title,
				"-o", lmplayer->lyric_filename,
				NULL);
	}
	//g_free(cmd);
	g_free(title);
	g_free(artist);
	lmplayer->lyric_downloaded = TRUE;
	//g_timeout_add(1000, (GSourceFunc)lmplayer_load_local_lyric, lmplayer);
}

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
		lmplayer_playlist_add_mrl(lmplayer->playlist, uri, NULL);
		g_free(uri);
		uri = NULL;
	}
}

void
lmplayer_action_change_skin(LmplayerObject *lmplayer)
{
}

gboolean
seek_slider_pressed_cb (GtkWidget *widget, GdkEventButton *event, LmplayerObject *lmplayer)
{
	lmplayer_debug(" ");
	lmplayer->seek_lock = TRUE;
	//lmplayer_statusbar_set_seeking (LMPLAYER_STATUSBAR(lmplayer->statusbar), TRUE);

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

	//lmplayer_statusbar_set_seeking(LMPLAYER_STATUSBAR(lmplayer->statusbar), FALSE);
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
			lmplayer_playlist_set_repeat(lmplayer->playlist, FALSE);
			lmplayer->repeat = FALSE;
			lmplayer->repeat_one = FALSE;
			printf("normal order\n");
			gtk_image_set_from_file(GTK_IMAGE(image), DATADIR"/lmplayer/order.png");
			break;
		case LMPLAYER_ORDER_REPEAT:
			lmplayer_playlist_set_repeat(lmplayer->playlist, TRUE);
			lmplayer->repeat = TRUE;
			lmplayer->repeat_one = FALSE;
			gtk_image_set_from_file(GTK_IMAGE(image), DATADIR"/lmplayer/repeat.png");
			printf("repeat order\n");
			break;
		case LMPLAYER_ORDER_REPEAT_ONE:
			lmplayer_playlist_set_repeat(lmplayer->playlist, FALSE);
			lmplayer->repeat_one = TRUE;
			lmplayer->repeat = FALSE;
			gtk_image_set_from_file(GTK_IMAGE(image), DATADIR"/lmplayer/repeat-one.png");
			printf("repeat_one\n");
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
plugin_button_clicked_cb(GtkButton *button, LmplayerObject *lmplayer)
{
	if(!lmplayer->plugins_manager_dialog)
	{
		GtkWidget *manager;

		lmplayer->plugins_manager_dialog = gtk_dialog_new_with_buttons (_("Configure Plugins"),
							      GTK_WINDOW(lmplayer->win),
							      GTK_DIALOG_DESTROY_WITH_PARENT,
							      GTK_STOCK_CLOSE,
							      GTK_RESPONSE_CLOSE,
							      NULL);

		gtk_container_set_border_width(GTK_CONTAINER(lmplayer->plugins_manager_dialog), 5);
		gtk_box_set_spacing (GTK_BOX(GTK_DIALOG(lmplayer->plugins_manager_dialog)->vbox), 2);
		gtk_dialog_set_has_separator(GTK_DIALOG(lmplayer->plugins_manager_dialog), FALSE);

		g_signal_connect_object(G_OBJECT(lmplayer->plugins_manager_dialog),
					 "delete_event",
					 G_CALLBACK (lmplayer_plugins_window_delete_cb),
					 NULL, 0);

		g_signal_connect_object(G_OBJECT(lmplayer->plugins_manager_dialog),
					 "response",
					 G_CALLBACK (lmplayer_plugins_response_cb),
					 NULL, 0);

		manager = lmplayer_plugin_manager_new();

		gtk_widget_show_all(GTK_WIDGET(manager));
		gtk_container_add(GTK_CONTAINER(GTK_DIALOG(lmplayer->plugins_manager_dialog)->vbox), manager);
	}

	gtk_window_present(GTK_WINDOW(lmplayer->plugins_manager_dialog));

	//if(lmplayer->plugins_manager_dialog)
	//	gtk_widget_show_all(lmplayer->plugins_manager_dialog);
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
	g_signal_connect(G_OBJECT(lmplayer->plugin_button), "clicked", G_CALLBACK(plugin_button_clicked_cb), lmplayer);
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

	lmplayer_debug("url: %s", url);
	lmplayer_action_remote (lmp, command, url);

	g_free (url);
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
		lmplayer->conn = bacon_message_connection_new(GETTEXT_PACKAGE);
		if(bacon_message_connection_get_is_server(lmplayer->conn) == FALSE)
		{
			lmplayer_options_process_for_server(lmplayer->conn, &optionstate);
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

	gchar *ui = gconf_client_get_string(gc, GCONF_PREFIX"/ui", NULL);
	if(!ui)
	{
		ui = g_strdup("lmplayer-default.ui");
		gconf_client_set_string(gc, GCONF_PREFIX"/ui", ui, NULL);
	}

	//lmplayer->builder = lmplayer_interface_load("lmplayer-default.ui", TRUE, NULL, NULL);
	//lmplayer->builder = lmplayer_interface_load("lmplayer-classic.ui", TRUE, NULL, NULL);
	lmplayer->builder = lmplayer_interface_load(ui, TRUE, NULL, NULL);
	if(lmplayer->builder == NULL)
	{
		g_free(ui);
		lmplayer_action_exit(NULL);
	}

	g_free(ui);

	
	lmplayer->win = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-window");
	if(lmplayer->win == NULL)
	{
		lmplayer_action_exit(NULL);
	}

	lmplayer->seek = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-progressbar");
	lmplayer->seekadj = gtk_range_get_adjustment(GTK_RANGE(lmplayer->seek));

	lmplayer->volume = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-volume");
	//lmplayer->statusbar = GTK_WIDGET(gtk_builder_get_object(lmplayer->xml, "tmw_statusbar"));
	lmplayer->view = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-view");
	lmplayer->playlist_view_button = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-playlist-view-button");
	lmplayer->order_switch_button = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-order-switch-button");
	lmplayer->about_button = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-about-button");
	lmplayer->plugin_button = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-plugin-button");
	lmplayer->prefs_button = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-preference-button");
	lmplayer->plugins_box = (GtkWidget *)gtk_builder_get_object(lmplayer->builder, "player-plugins-box");
	
	lmplayer_search_view_setup(lmplayer);

	lmplayer->repeat = FALSE;
	lmplayer->repeat_one = FALSE;

	lmplayer_callback_connect(lmplayer);

	lmplayer_ui_manager_setup(lmplayer);
	playlist_widget_setup(lmplayer);
	video_widget_create(lmplayer);
	
	//TODO: 安装其它如会话管理、cd播放、文件监视等功能
	lmplayer->state = STATE_STOPPED;
	lmplayer->seek_lock = FALSE;

	lmplayer_setup_file_monitoring(lmplayer);
	lmplayer_setup_file_filters();
	lmplayer_setup_toolbar(lmplayer);

	lmplayer_options_process_late(lmplayer, &optionstate);

	lmplayer_object_plugins_init(lmplayer);

	if(optionstate.filenames != NULL && lmplayer_action_open_files(lmplayer, optionstate.filenames))
	{
		hasfiles = TRUE;
		lmplayer_action_play_pause(lmplayer);
	}
	else
	{
		lmplayer_action_set_mrl(lmplayer, NULL, NULL);
	}

	if(lmplayer->conn != NULL && bacon_message_connection_get_is_server (lmplayer->conn) != FALSE)
	{
		bacon_message_connection_set_callback (lmplayer->conn,
				(BaconMessageReceivedFunc)
				lmplayer_message_connection_receive_cb, lmplayer);
	}
	
	gtk_widget_show_all(GTK_WIDGET(lmplayer->win));

	if(hasfiles)
	{
		if(lmplayer->pls == NULL)
			lmplayer->pls = lmplayer_build_default_playlist_filename();
	}
	else
	{
		lmplayer_debug("load default playlist");
		lmplayer_action_load_default_playlist(lmplayer);
	}

	gtk_notebook_set_current_page(GTK_NOTEBOOK(lmplayer->view), LMPLAYER_VIEW_TYPE_PLAYLIST);

	lmplayer_search_init(lmplayer);
	gtk_main();
	return 0;
}


