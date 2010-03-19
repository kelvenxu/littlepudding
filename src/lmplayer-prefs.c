
#include "lmplayer-prefs.h"
#include "lmplayer-interface.h"
#include "search-library.h"
#include <stdlib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

static GtkTreeModel *categories_tree_model;
static GtkWidget *categories_tree;
static GtkWidget *notebook;
static GtkWidget *categories_window;

enum
{
	CATEGORY_COLUMN = 0, 
	PAGE_NUM_COLUMN, 
	NUM_COLUMNS
};

enum
{
	LOGO = 0,
	MUSIC_SETTING,
	MUSIC_LIBRARY_SETTING
};

typedef struct _CategoriesTreeItem CategoriesTreeItem;

struct _CategoriesTreeItem 
{
	gchar *category;
	CategoriesTreeItem *children;
	gint notebook_page;
};

static CategoriesTreeItem music_behavior[] = 
{
	{N_("Media library"), NULL, MUSIC_LIBRARY_SETTING},
	{ NULL }
};

static CategoriesTreeItem toplevel[] =
{
	{N_("Music"), music_behavior, LOGO},
	{ NULL }
};

static gint last_selected_page_num = MUSIC_LIBRARY_SETTING;

static GtkTreeModel *
create_categories_tree_model(LmplayerObject *lmplayer)
{
	GtkTreeStore *model;
	GtkTreeIter iter;
	CategoriesTreeItem *category = toplevel;

	model = gtk_tree_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_INT);

	while(category->category)
	{
		CategoriesTreeItem *sub_category = category->children;
		gtk_tree_store_append(model, &iter, NULL);
		gtk_tree_store_set(model, &iter, 
				CATEGORY_COLUMN, gettext(category->category), 
				PAGE_NUM_COLUMN, category->notebook_page, 
				-1);

		while(sub_category->category)
		{
			GtkTreeIter child_iter;
			gtk_tree_store_append(model, &child_iter, &iter);
			gtk_tree_store_set(model, &child_iter, 
					CATEGORY_COLUMN, gettext(sub_category->category),
					PAGE_NUM_COLUMN, sub_category->notebook_page,
					-1);

			sub_category++;
		}

		category++;
	}

	return GTK_TREE_MODEL(model);
}

static void
categories_tree_selection_cb(GtkTreeSelection *selection, LmplayerObject *lmplayer)
{
	GtkTreeIter iter;
	GValue value = {0, };

	if(!gtk_tree_selection_get_selected(selection, NULL, &iter))
		return;

	gtk_tree_model_get_value(categories_tree_model, &iter, PAGE_NUM_COLUMN, &value);

	last_selected_page_num = g_value_get_int(&value);

	if(notebook != NULL)
	{
		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), last_selected_page_num);
	}

	g_value_unset(&value);
}

static gboolean
selection_init(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, LmplayerObject *lmplayer)
{
	GValue value = {0, };
	gint page_num;

	gtk_tree_model_get_value(categories_tree_model, iter, PAGE_NUM_COLUMN, &value);

	page_num = g_value_get_int(&value);

	g_value_unset(&value);

	if(page_num == last_selected_page_num)
	{
		GtkTreeSelection *selection;

		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(categories_tree));
		gtk_tree_selection_select_iter(selection, iter);

		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_num);

		return TRUE;
	}

	return FALSE;
}

static void
categories_tree_realize(GtkWidget *widget, LmplayerObject *lmplayer)
{
	gtk_tree_view_expand_all(GTK_TREE_VIEW(widget));
	gtk_tree_model_foreach(categories_tree_model, (GtkTreeModelForeachFunc)selection_init, lmplayer);
}

static GtkWidget *
create_categories_tree(LmplayerObject *lmplayer)
{
	GtkWidget *sw;
	GtkCellRenderer *renderer;
	GtkTreeSelection *selection;
	GtkTreeViewColumn *column;
	gint col_offset;

	sw = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sw), 
			GTK_SHADOW_ETCHED_IN);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), 
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	gtk_widget_set_size_request(sw, 140, 240);

	categories_tree_model = create_categories_tree_model(lmplayer);

	categories_tree = gtk_tree_view_new_with_model(categories_tree_model);
	g_object_unref(G_OBJECT(categories_tree_model));
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(categories_tree), TRUE);

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(categories_tree));

	gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);

	renderer = gtk_cell_renderer_text_new();
	g_object_set(G_OBJECT(renderer), "xalign", 0.0, NULL);

	col_offset = gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(categories_tree),
			-1, _("Categories"),
			renderer, "text", CATEGORY_COLUMN,
			NULL);

	column = gtk_tree_view_get_column(GTK_TREE_VIEW(categories_tree), col_offset - 1);
	gtk_tree_view_column_set_clickable(GTK_TREE_VIEW_COLUMN(column), FALSE);

	gtk_container_add(GTK_CONTAINER(sw), categories_tree);

	g_signal_connect(selection, "changed", G_CALLBACK(categories_tree_selection_cb), lmplayer);
	g_signal_connect(G_OBJECT(categories_tree), "realize", G_CALLBACK(categories_tree_realize), lmplayer);

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(categories_tree), FALSE);

	return sw;
}

static GtkWidget *
prepare_page(GtkNotebook *notebook, const gchar *caption, const gchar *stock_id)
{
	GtkWidget *vbox = gtk_vbox_new(FALSE, 12);
	gtk_notebook_append_page(notebook, vbox, NULL);

	GtkWidget *vbox1 = gtk_vbox_new(FALSE, 6);
	gtk_box_pack_start(GTK_BOX(vbox), vbox1, FALSE, FALSE, 6);
	GtkWidget *hbox = gtk_hbox_new(FALSE, 6);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox, FALSE, FALSE, 0);

	GtkWidget *image = gtk_image_new_from_stock(stock_id, GTK_ICON_SIZE_LARGE_TOOLBAR);

	gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);
	GtkWidget *label = gtk_label_new(NULL);
	
	gtk_label_set_label(GTK_LABEL(label), caption);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	
	GtkWidget *hseparator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox1), hseparator, FALSE, FALSE, 0);

	return vbox;
}

static void 
setup_logo_page(LmplayerObject *lmplayer)
{
	if(!lmplayer) return;

	GtkWidget *logo = gtk_label_new(_("Please select sub items to set parameters."));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), logo, NULL);
}

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
setup_media_library_page(LmplayerObject *lmplayer)
{
	if(!lmplayer) return;

	GtkWidget *container = prepare_page(GTK_NOTEBOOK(notebook), _("Media library"), GTK_STOCK_CONVERT);

  GtkBuilder *builder = gtk_builder_new();
  g_return_if_fail(GTK_IS_BUILDER(builder));

	builder = lmplayer_interface_load("lmplayer-library-prefs.ui", TRUE, NULL, NULL);

	GtkWidget *vbox = (GtkWidget*)gtk_builder_get_object(builder, "library-prefs-vbox");

	gtk_box_pack_start(GTK_BOX(container), vbox, FALSE, FALSE, 0);

	//FIXME:
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), container, NULL);

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

static GtkWidget *
create_notebook(LmplayerObject *lmplayer)
{
	if(!lmplayer) return NULL;

	notebook = gtk_notebook_new();
	GtkNotebook *nb = GTK_NOTEBOOK(notebook);

	gtk_notebook_set_show_tabs(nb, FALSE);
	gtk_notebook_set_show_border(nb, FALSE);

	setup_logo_page(lmplayer);
	setup_media_library_page(lmplayer);

	gtk_notebook_set_current_page(nb, MUSIC_LIBRARY_SETTING);

	return notebook;
}

static void
resize_categories_tree(LmplayerObject *lmplayer)
{
	GtkRequisition rtv, rsw;
	gtk_widget_size_request(categories_tree, &rtv);
	gtk_widget_size_request(GTK_SCROLLED_WINDOW(categories_window)->vscrollbar, &rsw);
	gtk_widget_set_size_request(categories_window, rtv.width + rsw.width + 25, -1);
}

void
lmplayer_setup_prefs(LmplayerObject *lmplayer)
{
	g_return_if_fail(lmplayer != NULL);

	lmplayer->prefs = gtk_dialog_new();
	gtk_window_set_transient_for(GTK_WINDOW(lmplayer->prefs), GTK_WINDOW(lmplayer->win));

	gtk_dialog_add_button(GTK_DIALOG(lmplayer->prefs), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button(GTK_DIALOG(lmplayer->prefs), GTK_STOCK_OK, GTK_RESPONSE_OK);
	gtk_dialog_set_default_response(GTK_DIALOG(lmplayer->prefs), GTK_RESPONSE_OK);
	
	GtkWidget *hbox;
	hbox = gtk_hbox_new(FALSE, 18);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);

	GtkWidget *r = gtk_vbox_new(FALSE, 6);

	GtkWidget *label = gtk_label_new(_("Options"));
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	g_object_set(G_OBJECT(label), "xalign", 0.0, NULL);

	categories_window = create_categories_tree(lmplayer);

	gtk_box_pack_start(GTK_BOX(r), label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(r), categories_window, TRUE, TRUE, 0);

	GtkWidget *l = create_notebook(lmplayer);

	gtk_box_pack_start(GTK_BOX(hbox), r, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), l, TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(lmplayer->prefs)->vbox), hbox, TRUE, TRUE, 0);

	gtk_widget_show_all(GTK_DIALOG(lmplayer->prefs)->vbox);
	gtk_window_set_title(GTK_WINDOW(lmplayer->prefs), _("Preferences"));

	resize_categories_tree(lmplayer);

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

#if 0
void
lmplayer_prefs_show(LmplayerObject *lmplayer)
{
	static gboolean has_init = FALSE;

	if(has_init)
	{
		gtk_dialog_show();
		return;
	}
	if(!prefs) return;


	gint result;
	while((result = gtk_dialog_run(GTK_DIALOG(lmplayer->prefs))) == GTK_RESPONSE_HELP);

	if(result == GTK_RESPONSE_OK)
	{
		//product_terminal_data_source_widget_updated(prefs->pt);
		//product_terminal_ground_wind_widget_updated(prefs->pt);
		//product_terminal_upload_widget_updated(prefs->pt);
		//product_terminal_control_widget_updated(prefs->pt);
		gtk_widget_destroy(GTK_WIDGET(lmplayer->prefs));
		lmplayer->prefs = NULL;

		return;
	}
	else
	{
		gtk_widget_destroy(GTK_WIDGET(lmplayer->prefs));
		lmplayer->prefs = NULL;
		return;
	}
}


void
prefs_dlg_init(PrefsDlg *prefs, ProductTerminal *pt, GtkWindow *parent_window)
{
	if(!prefs) return;
	prefs->pt = pt;
	prefs->parent_window = parent_window;
}
#endif

