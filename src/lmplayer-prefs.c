
#include "lmplayer-prefs.h"
#include "lmplayer-interface.h"
#include "search-library.h"
#include "lmplayer-plugin-manager.h"
#include <stdlib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

static void
library_path_set_cb(GtkFileChooserButton *file_chooser, LmplayerObject *lmplayer)
{
	gchar *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
	gconf_client_set_string(lmplayer->gc, GCONF_PREFIX"/library_path", path, NULL);
	g_free(path);
}

static void
monitor_library_toggled_cb(GtkToggleButton *button, LmplayerObject *lmplayer)
{
	gconf_client_set_bool(lmplayer->gc, GCONF_PREFIX"/monitor_library", 
			gtk_toggle_button_get_active(button),
			NULL);
}

static void
index_library_clicked_cb(GtkButton *button, LmplayerObject *lmplayer)
{
	gchar *path = gconf_client_get_string(lmplayer->gc, GCONF_PREFIX"/library_path", NULL);
	lmplayer_search_index(path);
	//g_free(path); //FIXME:
}

static void 
setup_media_library_page(LmplayerObject *lmplayer, GtkWidget *notebook)
{
	g_return_if_fail(lmplayer && notebook);

  GtkBuilder *builder = gtk_builder_new();
  g_return_if_fail(GTK_IS_BUILDER(builder));

	builder = lmplayer_interface_load("lmplayer-library-prefs.ui", TRUE, NULL, NULL);

	GtkWidget *vbox = (GtkWidget*)gtk_builder_get_object(builder, "library-prefs-vbox");

	GtkWidget *label = gtk_label_new(_("Music"));

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);

	GtkWidget *file_chooser = (GtkWidget*)gtk_builder_get_object(builder, "library-path-chooser");
	gchar *path = gconf_client_get_string(lmplayer->gc, GCONF_PREFIX"/library_path", NULL);
	if(path && g_file_test(path, G_FILE_TEST_IS_DIR))
	{
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(file_chooser), path);
	}
	else
	{
		path = g_build_path(getenv("HOME"), _("Music"), NULL);
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(file_chooser), path);
		gconf_client_set_string(lmplayer->gc, GCONF_PREFIX"/library_path", path, NULL);
	}

	g_signal_connect(file_chooser, "file-set", G_CALLBACK(library_path_set_cb), lmplayer);

	GtkWidget *monitor_library = (GtkWidget*)gtk_builder_get_object(builder, "monitor-library-check");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(monitor_library), 
			gconf_client_get_bool(lmplayer->gc, GCONF_PREFIX"/monitor_library", NULL));

	g_signal_connect(monitor_library, "toggled", G_CALLBACK(monitor_library_toggled_cb), lmplayer);

	GtkWidget *index_button = (GtkWidget*)gtk_builder_get_object(builder, "library-index-button");
	g_signal_connect(index_button, "clicked", G_CALLBACK(index_library_clicked_cb), lmplayer);

	g_free(path);
}

static void
setup_plugin_manager_page(LmplayerObject *lmplayer, GtkWidget *notebook)
{
	GtkWidget *manager = lmplayer_plugin_manager_new();

	//gtk_container_add(GTK_CONTAINER(GTK_DIALOG(lmplayer->plugins_manager_dialog)->vbox), manager);
	GtkWidget *label = gtk_label_new(_("Plugin"));

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), manager, label);

	//gtk_widget_show_all(GTK_WIDGET(manager));
}

static GtkWidget *
create_notebook(LmplayerObject *lmplayer)
{
	if(!lmplayer) return NULL;

	GtkWidget *notebook = gtk_notebook_new();

	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), TRUE);

	setup_media_library_page(lmplayer, notebook);
	setup_plugin_manager_page(lmplayer, notebook);

	return notebook;
}

void
lmplayer_setup_prefs(LmplayerObject *lmplayer)
{
	g_return_if_fail(lmplayer != NULL);

	lmplayer->prefs = gtk_dialog_new();
	gtk_window_set_transient_for(GTK_WINDOW(lmplayer->prefs), GTK_WINDOW(lmplayer->win));

	gtk_dialog_add_button(GTK_DIALOG(lmplayer->prefs), GTK_STOCK_CLOSE, GTK_RESPONSE_OK);
	gtk_dialog_set_default_response(GTK_DIALOG(lmplayer->prefs), GTK_RESPONSE_OK);

	GtkWidget *l = create_notebook(lmplayer);

	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(lmplayer->prefs)->vbox), l, TRUE, TRUE, 0);

	gtk_widget_show_all(GTK_DIALOG(lmplayer->prefs)->vbox);
	gtk_window_set_title(GTK_WINDOW(lmplayer->prefs), _("Preferences"));

	g_signal_connect(G_OBJECT(lmplayer->prefs), "response", G_CALLBACK(gtk_widget_hide), NULL);
	g_signal_connect(G_OBJECT(lmplayer->prefs), "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), NULL);
	g_signal_connect(G_OBJECT(lmplayer->prefs), "destroy", G_CALLBACK(gtk_widget_destroyed), &lmplayer->prefs);
}

void
lmplayer_prefs_show(LmplayerObject *lmplayer)
{
	static gboolean has_setup = FALSE;
	if(!has_setup)
		lmplayer_setup_prefs(lmplayer);

	gtk_widget_show(lmplayer->prefs);
}

