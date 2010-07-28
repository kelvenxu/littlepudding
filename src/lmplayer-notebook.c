/*
 * lmplayer-notebook.c
 * This file is part of lmplayer
 *
 * Copyright (C) 2005 - Paolo Maggi 
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
 */
 
/*
 * Modified by the lmplayer Team, 2005. See the AUTHORS file for a 
 * list of people on the lmplayer Team.  
 * See the ChangeLog files for a list of changes. 
 */

/* This file is a modified version of the epiphany file ephy-notebook.c
 * Here the relevant copyright:
 *
 *  Copyright (C) 2002 Christophe Fergeau
 *  Copyright (C) 2003 Marco Pesenti Gritti
 *  Copyright (C) 2003, 2004 Christian Persch
 *  Copyright (C) 2010 Kelvenxu
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "lmplayer-notebook.h"
#include "lmplayer-tab.h"

#define AFTER_ALL_TABS -1
#define NOT_IN_APP_WINDOWS -2

#define LMPLAYER_NOTEBOOK_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LMPLAYER_TYPE_NOTEBOOK, LmplayerNotebookPrivate))

struct _LmplayerNotebookPrivate
{
	GList         *focused_pages;
	gulong         motion_notify_handler_id;
	gint           x_start;
	gint           y_start;
	gint           drag_in_progress : 1;
	gint	       always_show_tabs : 1;
	gint           close_buttons_sensitive : 1;
	gint           tab_drag_and_drop_enabled : 1;
};

G_DEFINE_TYPE(LmplayerNotebook, lmplayer_notebook, GTK_TYPE_NOTEBOOK)

static void lmplayer_notebook_finalize (GObject *object);

/* Signals */
enum
{
	TAB_ADDED,
	TAB_REMOVED,
	TABS_REORDERED,
	TAB_DETACHED,
	TAB_CLOSE_REQUEST,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void
lmplayer_notebook_class_init (LmplayerNotebookClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = lmplayer_notebook_finalize;

	signals[TAB_ADDED] =
		g_signal_new ("tab_added",
			      G_OBJECT_CLASS_TYPE (object_class),
			      G_SIGNAL_RUN_FIRST,
			      G_STRUCT_OFFSET (LmplayerNotebookClass, tab_added),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__OBJECT,
			      G_TYPE_NONE,
			      1,
						GTK_TYPE_WIDGET);
	signals[TAB_REMOVED] =
		g_signal_new ("tab_removed",
			      G_OBJECT_CLASS_TYPE (object_class),
			      G_SIGNAL_RUN_FIRST,
			      G_STRUCT_OFFSET (LmplayerNotebookClass, tab_removed),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__OBJECT,
			      G_TYPE_NONE,
			      1,
						GTK_TYPE_WIDGET);
	signals[TAB_DETACHED] =
		g_signal_new ("tab_detached",
			      G_OBJECT_CLASS_TYPE (object_class),
			      G_SIGNAL_RUN_FIRST,
			      G_STRUCT_OFFSET (LmplayerNotebookClass, tab_detached),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__OBJECT,
			      G_TYPE_NONE,
			      1,
						GTK_TYPE_WIDGET);
	signals[TABS_REORDERED] =
		g_signal_new ("tabs_reordered",
			      G_OBJECT_CLASS_TYPE (object_class),
			      G_SIGNAL_RUN_FIRST,
			      G_STRUCT_OFFSET (LmplayerNotebookClass, tabs_reordered),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE,
			      0);
	signals[TAB_CLOSE_REQUEST] =
		g_signal_new ("tab-close-request",
			      G_OBJECT_CLASS_TYPE (object_class),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (LmplayerNotebookClass, tab_close_request),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__OBJECT,
			      G_TYPE_NONE,
			      1,
						GTK_TYPE_WIDGET);

	g_type_class_add_private(object_class, sizeof(LmplayerNotebookPrivate));
}

GtkWidget *
lmplayer_notebook_new (void)
{
	return GTK_WIDGET (g_object_new (LMPLAYER_TYPE_NOTEBOOK, NULL));
}

/*
 * update_tabs_visibility: Hide tabs if there is only one tab
 * and the pref is not set.
 */
static void
update_tabs_visibility(LmplayerNotebook *nb, gboolean before_inserting)
{
	gboolean show_tabs;
	guint num;

	num = gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb));

	if (before_inserting) num++;

	show_tabs = (nb->priv->always_show_tabs || num > 1);

	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (nb), show_tabs);
}

static void
lmplayer_notebook_init(LmplayerNotebook *notebook)
{
	notebook->priv = LMPLAYER_NOTEBOOK_GET_PRIVATE (notebook);

	notebook->priv->close_buttons_sensitive = TRUE;
	notebook->priv->tab_drag_and_drop_enabled = TRUE;
	
	gtk_notebook_set_scrollable (GTK_NOTEBOOK (notebook), TRUE);
	gtk_notebook_set_show_border (GTK_NOTEBOOK (notebook), FALSE);
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook), FALSE);

	notebook->priv->always_show_tabs = TRUE;
}

static void
lmplayer_notebook_finalize (GObject *object)
{
	LmplayerNotebook *notebook = LMPLAYER_NOTEBOOK (object);

	if (notebook->priv->focused_pages)
		g_list_free (notebook->priv->focused_pages);

	G_OBJECT_CLASS (lmplayer_notebook_parent_class)->finalize (object);
}

static void
tab_close_request_cb(LmplayerTab *tab, LmplayerNotebook *nb)
{
	GtkWidget *dialog = gtk_message_dialog_new(NULL,
																						GTK_DIALOG_DESTROY_WITH_PARENT,
																						GTK_MESSAGE_INFO,
																						GTK_BUTTONS_YES_NO,
																						_("Close playlist %s ?"),
																						lmplayer_tab_get_name(tab));

	gtk_window_set_title(GTK_WINDOW(dialog), _("Confirm"));
	gint response = gtk_dialog_run(GTK_DIALOG(dialog));

	if(response == GTK_RESPONSE_YES)
	{
		GtkWidget *page = lmplayer_tab_get_page(tab);
		gint page_num = gtk_notebook_page_num(GTK_NOTEBOOK(nb), page);
		gtk_notebook_remove_page(GTK_NOTEBOOK(nb), page_num);
	}

	gtk_widget_destroy(dialog);
}

void
lmplayer_notebook_append_page(LmplayerNotebook *nb, GtkWidget *page, const char *name)
{
	g_return_if_fail(LMPLAYER_IS_NOTEBOOK(nb));
	g_return_if_fail(GTK_IS_WIDGET(page));

	GtkWidget *tab = lmplayer_tab_new();

	lmplayer_tab_set_name(LMPLAYER_TAB(tab), name);
	lmplayer_tab_set_page(LMPLAYER_TAB(tab), page);
	
	update_tabs_visibility(nb, TRUE);

	gtk_notebook_append_page(GTK_NOTEBOOK(nb), GTK_WIDGET(page), tab);

	g_signal_connect(tab, "tab-close-request", G_CALLBACK(tab_close_request_cb), nb);

	g_signal_emit(G_OBJECT(nb), signals[TAB_ADDED], 0, page);
}

void
lmplayer_notebook_remove_page(LmplayerNotebook *nb, GtkWidget *page)
{
}

