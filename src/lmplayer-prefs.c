
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
index_library_clicked_cb(GtkButton *button, LmplayerObject *lmplayer)
{
	gchar *path = gconf_client_get_string(lmplayer->gc, GCONF_PREFIX"/library_path", NULL);

	if(!g_thread_supported())
		g_thread_init(NULL);

	lmplayer_search_index(path);

	g_free(path); 
}

static void 
setup_media_library_page(LmplayerObject *lmplayer, GtkWidget *notebook)
{
	g_return_if_fail(lmplayer && notebook);

  GtkBuilder *builder = lmplayer_interface_load("lmplayer-library-prefs.ui", TRUE, NULL, NULL);

  g_return_if_fail(GTK_IS_BUILDER(builder));

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

	GtkWidget *index_button = (GtkWidget*)gtk_builder_get_object(builder, "library-index-button");
	g_signal_connect(index_button, "clicked", G_CALLBACK(index_library_clicked_cb), lmplayer);

	g_free(path);
}

static void
setup_plugin_manager_page(LmplayerObject *lmplayer, GtkWidget *notebook)
{
	GtkWidget *manager = lmplayer_plugin_manager_new();

	GtkWidget *label = gtk_label_new(_("Plugin"));

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), manager, label);
}

enum 
{
	COL_UI_NAME,
	COL_UI_LOCATION,
	NCOLS
};
static GtkWidget *ui_view;

static void
switch_ui_cb(GtkButton *button, LmplayerObject *lmplayer)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *name = NULL;

	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(ui_view));
	gtk_tree_selection_get_selected(selection, &model, &iter);

	gtk_tree_model_get(model, &iter, COL_UI_NAME, &name, -1);

	gconf_client_set_string(lmplayer->gc, GCONF_PREFIX"/ui", name, NULL);

	GtkWidget *dialog = gtk_message_dialog_new(lmplayer_get_main_window(lmplayer),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_INFO,
			GTK_BUTTONS_OK,
			"Lmplayer need restart!");
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

static void
setup_ui_switch_page(LmplayerObject *lmplayer, GtkWidget *notebook)
{
	g_return_if_fail(lmplayer && notebook);

	GtkBuilder *builder = lmplayer_interface_load("lmplayer-ui-switch-prefs.ui", TRUE, NULL, NULL);
  g_return_if_fail(GTK_IS_BUILDER(builder));

	// set store
	GtkListStore *liststore = gtk_list_store_new(NCOLS, G_TYPE_STRING, G_TYPE_STRING);
	
	// travel path
	{
		GError *err = NULL;

		GDir *dir = g_dir_open(DATADIR"/lmplayer/ui", 0, &err);
		if(dir)
		{
			GtkTreeIter iter;
			const char *file = g_dir_read_name(dir);
			while(file)
			{
				gchar *fullpath = g_build_path(G_DIR_SEPARATOR_S, DATADIR"/ui", file, NULL);

				if(fullpath && g_str_has_suffix(file, ".ui"))
				{
					gtk_list_store_append(liststore, &iter);
					gtk_list_store_set(liststore, &iter, 
							COL_UI_NAME, file,
							COL_UI_LOCATION, fullpath,
							-1);
				}

				if(fullpath)
					g_free(fullpath);

				file = g_dir_read_name(dir);
			}

			g_dir_close(dir);
		}
	}

	// create tree view
	ui_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(liststore));
	g_object_unref(liststore);

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(ui_view), FALSE);
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(ui_view));
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);

	GtkTreePath *path = gtk_tree_path_new_first();

	if(path)
	{
		gtk_tree_selection_select_path(selection, path);
		gtk_tree_path_free(path);
	}

	GtkTreeViewColumn *col = gtk_tree_view_column_new();
	gtk_tree_view_append_column(GTK_TREE_VIEW(ui_view), col);

	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", COL_UI_NAME);

	GtkWidget *treeview_box = (GtkWidget *)gtk_builder_get_object(builder, "treeview-box");
	gtk_container_add(GTK_CONTAINER(treeview_box), ui_view);

	GtkWidget *okbutton = (GtkWidget *)gtk_builder_get_object(builder, "use-ui-button");
	g_signal_connect(okbutton, "clicked", G_CALLBACK(switch_ui_cb), lmplayer);

	// append to notebook
	GtkWidget *vbox = (GtkWidget *)gtk_builder_get_object(builder, "ui-switch-vbox");
	GtkWidget *label = gtk_label_new(_("UI"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);
	gtk_widget_show_all(vbox);
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
	setup_ui_switch_page(lmplayer, notebook);

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

