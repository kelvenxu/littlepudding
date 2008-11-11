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
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "lmplayer-notebook.h"
//#include "lmplayer-marshal.h"

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

static void move_current_tab_to_another_notebook  (LmplayerNotebook  *src,
						   LmplayerNotebook  *dest,
						   GdkEventMotion *event,
						   gint            dest_position);

/* Local variables */
static GdkCursor *cursor = NULL;

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
			      //LMPLAYER_TYPE_TAB);
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
			      //LMPLAYER_TYPE_TAB);
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
			      //LMPLAYER_TYPE_TAB);
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
			      //LMPLAYER_TYPE_TAB);
				  GTK_TYPE_WIDGET);

	g_type_class_add_private (object_class, sizeof(LmplayerNotebookPrivate));
}

static LmplayerNotebook *
find_notebook_at_pointer (gint abs_x, gint abs_y)
{
	GdkWindow *win_at_pointer;
	GdkWindow *toplevel_win;
	gpointer toplevel = NULL;
	gint x, y;

	/* FIXME multi-head */
	win_at_pointer = gdk_window_at_pointer (&x, &y);
	if (win_at_pointer == NULL)
	{
		/* We are outside all windows of the same application */
		return NULL;
	}

	toplevel_win = gdk_window_get_toplevel (win_at_pointer);

	/* get the GtkWidget which owns the toplevel GdkWindow */
	gdk_window_get_user_data (toplevel_win, &toplevel);

	/* toplevel should be an LmplayerWindow */
	//if ((toplevel != NULL) && 
	 //   LMPLAYER_IS_WINDOW (toplevel))
	//{
		//return LMPLAYER_NOTEBOOK (_lmplayer_window_get_notebook
		//				(LMPLAYER_WINDOW (toplevel)));
		//
	//}

	/* We are outside all windows containing a notebook */
	return NULL;
}

static gboolean
is_in_notebook_window (LmplayerNotebook *notebook,
		       gint           abs_x, 
		       gint           abs_y)
{
	LmplayerNotebook *nb_at_pointer;

	g_return_val_if_fail (notebook != NULL, FALSE);

	nb_at_pointer = find_notebook_at_pointer (abs_x, abs_y);

	return (nb_at_pointer == notebook);
}

static gint
find_tab_num_at_pos (LmplayerNotebook *notebook, 
		     gint           abs_x, 
		     gint           abs_y)
{
	GtkPositionType tab_pos;
	int page_num = 0;
	GtkNotebook *nb = GTK_NOTEBOOK (notebook);
	GtkWidget *page;

	tab_pos = gtk_notebook_get_tab_pos (GTK_NOTEBOOK (notebook));

	if (GTK_NOTEBOOK (notebook)->first_tab == NULL)
	{
		return AFTER_ALL_TABS;
	}

	/* For some reason unfullscreen + quick click can
	   cause a wrong click event to be reported to the tab */
	if (!is_in_notebook_window (notebook, abs_x, abs_y))
	{
		return NOT_IN_APP_WINDOWS;
	}

	while ((page = gtk_notebook_get_nth_page (nb, page_num)) != NULL)
	{
		GtkWidget *tab;
		gint max_x, max_y;
		gint x_root, y_root;

		tab = gtk_notebook_get_tab_label (nb, page);
		g_return_val_if_fail (tab != NULL, AFTER_ALL_TABS);

		if (!GTK_WIDGET_MAPPED (GTK_WIDGET (tab)))
		{
			++page_num;
			continue;
		}

		gdk_window_get_origin (GDK_WINDOW (tab->window),
				       &x_root, &y_root);

		max_x = x_root + tab->allocation.x + tab->allocation.width;
		max_y = y_root + tab->allocation.y + tab->allocation.height;

		if (((tab_pos == GTK_POS_TOP) || 
		     (tab_pos == GTK_POS_BOTTOM)) &&
		    (abs_x <= max_x))
		{
			return page_num;
		}
		else if (((tab_pos == GTK_POS_LEFT) || 
		          (tab_pos == GTK_POS_RIGHT)) && 
		         (abs_y <= max_y))
		{
			return page_num;
		}

		++page_num;
	}
	
	return AFTER_ALL_TABS;
}

static gint 
find_notebook_and_tab_at_pos (gint            abs_x, 
			      gint            abs_y,
			      LmplayerNotebook **notebook,
			      gint           *page_num)
{
	*notebook = find_notebook_at_pointer (abs_x, abs_y);
	if (*notebook == NULL)
	{
		return NOT_IN_APP_WINDOWS;
	}
	
	*page_num = find_tab_num_at_pos (*notebook, abs_x, abs_y);

	if (*page_num < 0)
	{
		return *page_num;
	}
	else
	{
		return 0;
	}
}

/* If dest_position is greater than or equal to the number of tabs 
   of the destination nootebook or negative, tab will be moved to the 
   end of the tabs. */
void
lmplayer_notebook_move_tab (LmplayerNotebook *src,
			 LmplayerNotebook *dest,
			 //LmplayerTab      *tab,
			 GtkWidget *tab,
			 gint           dest_position)
{
	g_return_if_fail (LMPLAYER_IS_NOTEBOOK (src));	
	g_return_if_fail (LMPLAYER_IS_NOTEBOOK (dest));
	g_return_if_fail (src != dest);
	g_return_if_fail (GTK_IS_WIDGET (tab));

	/* make sure the tab isn't destroyed while we move it */
	g_object_ref (tab);
	lmplayer_notebook_remove_tab (src, tab);
	lmplayer_notebook_add_tab (dest, tab, dest_position, TRUE);
	g_object_unref (tab);
}

/* If dest_position is greater than or equal to the number of tabs 
   of the destination nootebook or negative, tab will be moved to the 
   end of the tabs. */
void
lmplayer_notebook_reorder_tab (LmplayerNotebook *src,
			    //LmplayerTab      *tab,
				GtkWidget *tab,
			    gint           dest_position)
{
	gint old_position;
	
	g_return_if_fail (LMPLAYER_IS_NOTEBOOK (src));	
	//g_return_if_fail (LMPLAYER_IS_TAB (tab));
	g_return_if_fail (GTK_IS_WIDGET (tab));

	old_position = gtk_notebook_page_num (GTK_NOTEBOOK (src), 
				    	      GTK_WIDGET (tab));
				    	      
	if (old_position == dest_position)
		return;

	gtk_notebook_reorder_child (GTK_NOTEBOOK (src), 
				    GTK_WIDGET (tab),
				    dest_position);
		
	if (!src->priv->drag_in_progress)
	{
		g_signal_emit (G_OBJECT (src), 
			       signals[TABS_REORDERED], 
			       0);
	}
}

static void
drag_start (LmplayerNotebook *notebook,
	    guint32        time)
{
	notebook->priv->drag_in_progress = TRUE;

	/* get a new cursor, if necessary */
	/* FIXME multi-head */
	if (cursor == NULL)
		cursor = gdk_cursor_new (GDK_FLEUR);

	/* grab the pointer */
	gtk_grab_add (GTK_WIDGET (notebook));

	/* FIXME multi-head */
	if (!gdk_pointer_is_grabbed ())
	{
		gdk_pointer_grab (GTK_WIDGET (notebook)->window,
				  FALSE,
				  GDK_BUTTON1_MOTION_MASK | GDK_BUTTON_RELEASE_MASK,
				  NULL, 
				  cursor, 
				  time);
	}
}

static void
drag_stop (LmplayerNotebook *notebook)
{
	if (notebook->priv->drag_in_progress)
	{
		g_signal_emit (G_OBJECT (notebook), 
			       signals[TABS_REORDERED], 
			       0);
	}

	notebook->priv->drag_in_progress = FALSE;
	if (notebook->priv->motion_notify_handler_id != 0)
	{
		g_signal_handler_disconnect (G_OBJECT (notebook),
					     notebook->priv->motion_notify_handler_id);
		notebook->priv->motion_notify_handler_id = 0;
	}
}

/* This function is only called during dnd, we don't need to emit TABS_REORDERED
 * here, instead we do it on drag_stop
 */
static void
move_current_tab (LmplayerNotebook *notebook,
	          gint           dest_position)
{
	gint cur_page_num;

	cur_page_num = gtk_notebook_get_current_page (GTK_NOTEBOOK (notebook));

	if (dest_position != cur_page_num)
	{
		GtkWidget *cur_tab;
		
		cur_tab = gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook),
						     cur_page_num);
						     
		lmplayer_notebook_reorder_tab (LMPLAYER_NOTEBOOK (notebook),
					    //LMPLAYER_TAB (cur_tab),
					    GTK_WIDGET (cur_tab),
					    dest_position);
	}
}

static gboolean
motion_notify_cb (LmplayerNotebook  *notebook,
		  GdkEventMotion *event,
		  gpointer        data)
{
	LmplayerNotebook *dest;
	gint page_num;
	gint result;

	if (notebook->priv->drag_in_progress == FALSE)
	{
		if (notebook->priv->tab_drag_and_drop_enabled == FALSE)
			return FALSE;
			
		if (gtk_drag_check_threshold (GTK_WIDGET (notebook),
					      notebook->priv->x_start,
					      notebook->priv->y_start,
					      event->x_root, 
					      event->y_root))
		{
			drag_start (notebook, event->time);
			return TRUE;
		}

		return FALSE;
	}

	result = find_notebook_and_tab_at_pos ((gint)event->x_root,
					       (gint)event->y_root,
					       &dest, 
					       &page_num);

	if (result != NOT_IN_APP_WINDOWS)
	{
		if (dest != notebook)
		{
			move_current_tab_to_another_notebook (notebook, 
							      dest,
						      	      event, 
						      	      page_num);
		}
		else
		{
			g_return_val_if_fail (page_num >= -1, FALSE);
			move_current_tab (notebook, page_num);
		}
	}

	return FALSE;
}

static void
move_current_tab_to_another_notebook (LmplayerNotebook  *src,
				      LmplayerNotebook  *dest,
				      GdkEventMotion *event,
				      gint            dest_position)
{
	//LmplayerTab *tab;
	GtkWidget *tab;
	gint cur_page;

	/* This is getting tricky, the tab was dragged in a notebook
	 * in another window of the same app, we move the tab
	 * to that new notebook, and let this notebook handle the
	 * drag
	 */
	g_return_if_fail (LMPLAYER_IS_NOTEBOOK (dest));
	g_return_if_fail (dest != src);

	cur_page = gtk_notebook_get_current_page (GTK_NOTEBOOK (src));
	tab = gtk_notebook_get_nth_page (GTK_NOTEBOOK (src), 
						    cur_page);

	/* stop drag in origin window */
	/* ungrab the pointer if it's grabbed */
	drag_stop (src);
	if (gdk_pointer_is_grabbed ())
	{
		gdk_pointer_ungrab (event->time);
	}
	gtk_grab_remove (GTK_WIDGET (src));

	lmplayer_notebook_move_tab (src, dest, tab, dest_position);

	/* start drag handling in dest notebook */
	dest->priv->motion_notify_handler_id =
		g_signal_connect (G_OBJECT (dest),
				  "motion-notify-event",
				  G_CALLBACK (motion_notify_cb),
				  NULL);

	drag_start (dest, event->time);
}

static gboolean
button_release_cb (LmplayerNotebook  *notebook,
		   GdkEventButton *event,
		   gpointer        data)
{
	if (notebook->priv->drag_in_progress)
	{
		gint cur_page_num;
		GtkWidget *cur_page;

		cur_page_num = gtk_notebook_get_current_page (GTK_NOTEBOOK (notebook));
		cur_page = gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook),
						      cur_page_num);

		/* CHECK: I don't follow the code here -- Paolo  */
		if (!is_in_notebook_window (notebook, event->x_root, event->y_root) &&
		    (gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook)) > 1))
		{
			/* Tab was detached */
			g_signal_emit (G_OBJECT (notebook),
				       signals[TAB_DETACHED], 
				       0, 
				       cur_page);
		}

		/* ungrab the pointer if it's grabbed */
		if (gdk_pointer_is_grabbed ())
		{
			gdk_pointer_ungrab (event->time);
		}
		gtk_grab_remove (GTK_WIDGET (notebook));
	}

	/* This must be called even if a drag isn't happening */
	drag_stop (notebook);

	return FALSE;
}

static gboolean
button_press_cb (LmplayerNotebook  *notebook,
		 GdkEventButton *event,
		 gpointer        data)
{
	gint tab_clicked;

	if (notebook->priv->drag_in_progress)
		return TRUE;

	tab_clicked = find_tab_num_at_pos (notebook,
					   event->x_root,
					   event->y_root);
					   
	if ((event->button == 1) && 
	    (event->type == GDK_BUTTON_PRESS) && 
	    (tab_clicked >= 0))
	{
		notebook->priv->x_start = event->x_root;
		notebook->priv->y_start = event->y_root;
		
		notebook->priv->motion_notify_handler_id =
			g_signal_connect (G_OBJECT (notebook),
					  "motion-notify-event",
					  G_CALLBACK (motion_notify_cb), 
					  NULL);
	}
	else if ((event->type == GDK_BUTTON_PRESS) && 
		 (event->button == 3))
	{
		if (tab_clicked == -1)
		{
			// CHECK: do we really need it?
			
			/* consume event, so that we don't pop up the context menu when
			 * the mouse if not over a tab label
			 */
			return TRUE;
		}
		else
		{
			/* Switch to the page the mouse is over, but don't consume the event */
			gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), 
						       tab_clicked);
		}
	}

	return FALSE;
}

GtkWidget *
lmplayer_notebook_new (void)
{
	return GTK_WIDGET (g_object_new (LMPLAYER_TYPE_NOTEBOOK, NULL));
}

static void
lmplayer_notebook_switch_page_cb (GtkNotebook     *notebook,
                               GtkNotebookPage *page,
                               guint            page_num,
                               gpointer         data)
{
	LmplayerNotebook *nb = LMPLAYER_NOTEBOOK (notebook);
	GtkWidget *child;
	//LmplayerView *view;

	child = gtk_notebook_get_nth_page (notebook, page_num);

	/* Remove the old page, we dont want to grow unnecessarily
	 * the list */
	if (nb->priv->focused_pages)
	{
		nb->priv->focused_pages =
			g_list_remove (nb->priv->focused_pages, child);
	}

	nb->priv->focused_pages = g_list_append (nb->priv->focused_pages,
						 child);

	/* give focus to the view */
	//view = lmplayer_tab_get_view (LMPLAYER_TAB (child));
	//gtk_widget_grab_focus (GTK_WIDGET (view));
}

/*
 * update_tabs_visibility: Hide tabs if there is only one tab
 * and the pref is not set.
 */
static void
update_tabs_visibility (LmplayerNotebook *nb, 
			gboolean       before_inserting)
{
	gboolean show_tabs;
	guint num;

	num = gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb));

	if (before_inserting) num++;

	show_tabs = (nb->priv->always_show_tabs || num > 1);

	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (nb), show_tabs);
}

static void
lmplayer_notebook_init (LmplayerNotebook *notebook)
{
	notebook->priv = LMPLAYER_NOTEBOOK_GET_PRIVATE (notebook);

	notebook->priv->close_buttons_sensitive = TRUE;
	notebook->priv->tab_drag_and_drop_enabled = TRUE;
	
	gtk_notebook_set_scrollable (GTK_NOTEBOOK (notebook), TRUE);
	gtk_notebook_set_show_border (GTK_NOTEBOOK (notebook), FALSE);
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook), FALSE);

	notebook->priv->always_show_tabs = TRUE;

	g_signal_connect (notebook, 
			  "button-press-event",
			  (GCallback)button_press_cb, 
			  NULL);
	g_signal_connect (notebook, 
			  "button-release-event",
			  (GCallback)button_release_cb,
			  NULL);
	gtk_widget_add_events (GTK_WIDGET (notebook), 
			       GDK_BUTTON1_MOTION_MASK);

	g_signal_connect_after (G_OBJECT (notebook), 
				"switch_page",
                                G_CALLBACK (lmplayer_notebook_switch_page_cb),
                                NULL);
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
sync_name (GtkWidget *tab, GParamSpec *pspec, GtkWidget *hbox)
{
	LmplayerNotebook *nb;
	GtkWidget *label;
	GtkWidget *ebox;
	GtkWidget *button;
	//GtkWidget *spinner;
	//gchar *str;
	GtkImage *icon;
	//LmplayerTabState  state;
	
	label = GTK_WIDGET (g_object_get_data (G_OBJECT (hbox), "label"));
	ebox = GTK_WIDGET (g_object_get_data (G_OBJECT (hbox), "label-ebox"));
	icon = GTK_IMAGE (g_object_get_data (G_OBJECT (hbox), "icon"));
	button = GTK_WIDGET (g_object_get_data (G_OBJECT (hbox), "close-button"));
	//spinner = GTK_WIDGET (g_object_get_data (G_OBJECT (hbox), "spinner"));

	nb = LMPLAYER_NOTEBOOK (gtk_widget_get_parent (GTK_WIDGET (tab)));

	g_return_if_fail ((label   != NULL) &&
			  (ebox    != NULL) &&
			  (button  != NULL) &&
			  (icon    != NULL) &&
			  //(spinner != NULL) &&
			  (nb      != NULL));

	//str = _lmplayer_tab_get_name (tab);
	//g_return_if_fail (str != NULL);
	
	//gtk_label_set_text (GTK_LABEL (label), str);
	//g_free (str);
	
	//str = _lmplayer_tab_get_tooltips (tab);
	//g_return_if_fail (str != NULL);
	
	//gtk_widget_set_tooltip_markup (ebox, str);
	//g_free (str);
		
	//state = lmplayer_tab_get_state (tab);
	
	gtk_widget_set_sensitive (button,
				  nb->priv->close_buttons_sensitive //&&  
				  //(state != LMPLAYER_TAB_STATE_CLOSING) &&
				  //(state != LMPLAYER_TAB_STATE_SAVING)  &&
				  //(state != LMPLAYER_TAB_STATE_SHOWING_PRINT_PREVIEW) &&
				  //(state != LMPLAYER_TAB_STATE_SAVING_ERROR));
		);
				  
	//if ((state == LMPLAYER_TAB_STATE_LOADING)   ||
	 //   (state == LMPLAYER_TAB_STATE_SAVING)    ||
	  //  (state == LMPLAYER_TAB_STATE_REVERTING))
	//{
	//	gtk_widget_hide (GTK_WIDGET (icon));
		
	//	gtk_widget_show (spinner);
	//	lmplayer_spinner_start (LMPLAYER_SPINNER (spinner));
	//}
	//else
	{
		//GdkPixbuf *pixbuf;
		
		//pixbuf = _lmplayer_tab_get_icon (tab);
		//gtk_image_set_from_pixbuf (icon, pixbuf);

		//if (pixbuf != NULL)
		//	g_object_unref (pixbuf);

		//gtk_widget_show (GTK_WIDGET (icon));
		
		//gtk_widget_hide (spinner);
		//lmplayer_spinner_stop (LMPLAYER_SPINNER (spinner));
	}
}

static void
close_button_clicked_cb (GtkWidget *widget, 
			 GtkWidget *tab)
{
	LmplayerNotebook *notebook;

	notebook = LMPLAYER_NOTEBOOK (gtk_widget_get_parent (tab));
	g_signal_emit (notebook, signals[TAB_CLOSE_REQUEST], 0, tab);
}

static void
tab_label_style_set_cb (GtkWidget *hbox,
			GtkStyle *previous_style,
			gpointer user_data)
{
	GtkWidget *button;
	gint h, w;

	gtk_icon_size_lookup_for_settings (gtk_widget_get_settings (hbox),
					   GTK_ICON_SIZE_MENU, &w, &h);

	button = g_object_get_data (G_OBJECT (hbox), "close-button");
	gtk_widget_set_size_request (button, w + 2, h + 2);
}

static GtkWidget *
build_tab_label (LmplayerNotebook *nb, GtkWidget *tab)
{
	GtkWidget *hbox, *label_hbox, *label_ebox;
	GtkWidget *label, *dummy_label;
	GtkWidget *close_button;
	GtkRcStyle *rcstyle;
	GtkWidget *image;
	GtkWidget *icon;

	hbox = gtk_hbox_new (FALSE, 4);

	label_ebox = gtk_event_box_new ();
	gtk_event_box_set_visible_window (GTK_EVENT_BOX (label_ebox), FALSE);
	gtk_box_pack_start (GTK_BOX (hbox), label_ebox, TRUE, TRUE, 0);

	label_hbox = gtk_hbox_new (FALSE, 4);
	gtk_container_add (GTK_CONTAINER (label_ebox), label_hbox);

	/* setup close button */
	close_button = gtk_button_new ();
	gtk_button_set_relief (GTK_BUTTON (close_button),
			       GTK_RELIEF_NONE);
	/* don't allow focus on the close button */
	gtk_button_set_focus_on_click (GTK_BUTTON (close_button), FALSE);

	/* make it as small as possible */
	rcstyle = gtk_rc_style_new ();
	rcstyle->xthickness = rcstyle->ythickness = 0;
	gtk_widget_modify_style (close_button, rcstyle);
	gtk_rc_style_unref (rcstyle),

	image = gtk_image_new_from_stock (GTK_STOCK_CLOSE,
					  GTK_ICON_SIZE_MENU);
	gtk_container_add (GTK_CONTAINER (close_button), image);
	gtk_box_pack_start (GTK_BOX (hbox), close_button, FALSE, FALSE, 0);

	gtk_widget_set_tooltip_text (close_button, _("Close playlist"));

	g_signal_connect (close_button,
			  "clicked",
			  G_CALLBACK (close_button_clicked_cb),
			  tab); //FIXME: ?!

	/* setup site icon, empty by default */
	icon = gtk_image_new ();
	gtk_box_pack_start (GTK_BOX (label_hbox), icon, FALSE, FALSE, 0);
	
	/* setup label */
	label = gtk_label_new ("");
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_misc_set_padding (GTK_MISC (label), 0, 0);
	gtk_box_pack_start (GTK_BOX (label_hbox), label, FALSE, FALSE, 0);

	dummy_label = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (label_hbox), dummy_label, TRUE, TRUE, 0);
	
	/* Set minimal size */
	g_signal_connect (hbox, "style-set",
			  G_CALLBACK (tab_label_style_set_cb), NULL);
	
	gtk_widget_show (hbox);
	gtk_widget_show (label_ebox);
	gtk_widget_show (label_hbox);
	gtk_widget_show (label);
	gtk_widget_show (dummy_label);	
	gtk_widget_show (image);
	gtk_widget_show (close_button);
	gtk_widget_show (icon);
	
	g_object_set_data (G_OBJECT (hbox), "label", label);
	g_object_set_data (G_OBJECT (hbox), "label-ebox", label_ebox);
	g_object_set_data (G_OBJECT (hbox), "icon", icon);
	g_object_set_data (G_OBJECT (hbox), "close-button", close_button);
	//g_object_set_data (G_OBJECT (tab), "close-button", close_button); //FIXME: ?!

	return hbox;
}

void
lmplayer_notebook_set_always_show_tabs (LmplayerNotebook *nb, 
				     gboolean       show_tabs)
{
	g_return_if_fail (LMPLAYER_IS_NOTEBOOK (nb));

	nb->priv->always_show_tabs = (show_tabs != FALSE);

	update_tabs_visibility (nb, FALSE);
}

void
lmplayer_notebook_add_tab (LmplayerNotebook *nb,
		        GtkWidget      *tab,
		        gint           position,
		        gboolean       jump_to)
{
	GtkWidget *label;
	GtkLabel *tab_label;
	char *name;

	g_return_if_fail (LMPLAYER_IS_NOTEBOOK (nb));
	g_return_if_fail (GTK_IS_WIDGET (tab));

	label = build_tab_label (nb, tab);

	tab_label = GTK_LABEL(g_object_get_data(G_OBJECT(label), "label"));
	name = (char *)g_object_get_data(G_OBJECT(tab), "playlist-name");

	gtk_label_set_text(tab_label, name);
	
	update_tabs_visibility (nb, TRUE);

	gtk_notebook_insert_page (GTK_NOTEBOOK (nb), 
				  GTK_WIDGET (tab),
				  label, 
				  position);

	sync_name (tab, NULL, label);
		         
	g_signal_connect_object (tab, 
				 "notify::name",
			         G_CALLBACK (sync_name), 
			         label, 
			         0);
	g_signal_connect_object (tab, 
				 "notify::state",
			         G_CALLBACK (sync_name), 
			         label, 
			         0);			         

	g_signal_emit (G_OBJECT (nb), signals[TAB_ADDED], 0, tab);

	/* The signal handler may have reordered the tabs */
	position = gtk_notebook_page_num (GTK_NOTEBOOK (nb), 
					  GTK_WIDGET (tab));

	if (jump_to)
	{
		//LmplayerView *view;
		
		gtk_notebook_set_current_page (GTK_NOTEBOOK (nb), position);
		g_object_set_data (G_OBJECT (tab), 
				   "jump_to",
				   GINT_TO_POINTER (jump_to));
		//view = lmplayer_tab_get_view (tab);
		
		//gtk_widget_grab_focus (GTK_WIDGET (view));
	}
}

static void
smart_tab_switching_on_closure (LmplayerNotebook *nb, GtkWidget *tab)
{
	gboolean jump_to;

	jump_to = GPOINTER_TO_INT (g_object_get_data
				   (G_OBJECT (tab), "jump_to"));

	if (!jump_to || !nb->priv->focused_pages)
	{
		gtk_notebook_next_page (GTK_NOTEBOOK (nb));
	}
	else
	{
		GList *l;
		GtkWidget *child;
		int page_num;

		/* activate the last focused tab */
		l = g_list_last (nb->priv->focused_pages);
		child = GTK_WIDGET (l->data);
		page_num = gtk_notebook_page_num (GTK_NOTEBOOK (nb),
						  child);
		gtk_notebook_set_current_page (GTK_NOTEBOOK (nb), 
					       page_num);
	}
}

static void
remove_tab (GtkWidget *tab, LmplayerNotebook *nb)
{
	GtkWidget *label, *ebox;
	gint position;

	position = gtk_notebook_page_num (GTK_NOTEBOOK (nb), GTK_WIDGET (tab));

	label = gtk_notebook_get_tab_label (GTK_NOTEBOOK (nb), GTK_WIDGET (tab));
	ebox = GTK_WIDGET (g_object_get_data (G_OBJECT (label), "label-ebox"));

	g_signal_handlers_disconnect_by_func (tab,
					      G_CALLBACK (sync_name), 
					      label);

	/* we ref the tab so that it's still alive while the tabs_removed
	 * signal is processed.
	 */
	g_object_ref (tab);

	gtk_notebook_remove_page (GTK_NOTEBOOK (nb), position);

	update_tabs_visibility (nb, FALSE);

	g_signal_emit (G_OBJECT (nb), signals[TAB_REMOVED], 0, tab);

	g_object_unref (tab);	
}

void
lmplayer_notebook_remove_tab (LmplayerNotebook *nb, GtkWidget *tab)
{
	gint position, curr;

	g_return_if_fail (LMPLAYER_IS_NOTEBOOK (nb));
	//g_return_if_fail (LMPLAYER_IS_TAB (tab));
	g_return_if_fail (GTK_IS_WIDGET (tab));

	/* Remove the page from the focused pages list */
	nb->priv->focused_pages =  g_list_remove (nb->priv->focused_pages,
						  tab);

	position = gtk_notebook_page_num (GTK_NOTEBOOK (nb), GTK_WIDGET (tab));
	curr = gtk_notebook_get_current_page (GTK_NOTEBOOK (nb));

	if (position == curr)
	{
		smart_tab_switching_on_closure (nb, tab);
	}

	remove_tab (tab, nb);
}

void
lmplayer_notebook_remove_all_tabs (LmplayerNotebook *nb)
{	
	g_return_if_fail (LMPLAYER_IS_NOTEBOOK (nb));
	
	g_list_free (nb->priv->focused_pages);
	nb->priv->focused_pages = NULL;

	gtk_container_foreach (GTK_CONTAINER (nb),
			       (GtkCallback)remove_tab,
			       nb);
}

static void
set_close_buttons_sensitivity (GtkWidget *tab,
                               LmplayerNotebook *nb)
{
	GtkWidget     *button;
	//LmplayerTabState  state;
	
	button = GTK_WIDGET (g_object_get_data (G_OBJECT (tab), 
						"close-button"));	
	g_return_if_fail (button != NULL);
	
	//state = lmplayer_tab_get_state (tab);
	
	gtk_widget_set_sensitive (button, 
				  nb->priv->close_buttons_sensitive //&&
				  //(state != LMPLAYER_TAB_STATE_CLOSING) &&
				  //(state != LMPLAYER_TAB_STATE_SAVING)  &&
				  //(state != LMPLAYER_TAB_STATE_SHOWING_PRINT_PREVIEW) &&
				  //(state != LMPLAYER_TAB_STATE_PRINTING) &&
				  //(state != LMPLAYER_TAB_STATE_PRINT_PREVIEWING) &&				  
				  //(state != LMPLAYER_TAB_STATE_SAVING_ERROR));
		);
}

void
lmplayer_notebook_set_close_buttons_sensitive (LmplayerNotebook *nb,
					    gboolean       sensitive)
{
	g_return_if_fail (LMPLAYER_IS_NOTEBOOK (nb));
	
	sensitive = (sensitive != FALSE);
	
	if (sensitive == nb->priv->close_buttons_sensitive)
		return;
	
	nb->priv->close_buttons_sensitive = sensitive;
	
	gtk_container_foreach (GTK_CONTAINER (nb),
			       (GtkCallback)set_close_buttons_sensitivity,
			       nb);
}

gboolean
lmplayer_notebook_get_close_buttons_sensitive (LmplayerNotebook *nb)
{
	g_return_val_if_fail (LMPLAYER_IS_NOTEBOOK (nb), TRUE);
	
	return nb->priv->close_buttons_sensitive;
}

void
lmplayer_notebook_set_tab_drag_and_drop_enabled (LmplayerNotebook *nb,
					      gboolean       enable)
{
	g_return_if_fail (LMPLAYER_IS_NOTEBOOK (nb));
	
	enable = (enable != FALSE);
	
	if (enable == nb->priv->tab_drag_and_drop_enabled)
		return;
		
	nb->priv->tab_drag_and_drop_enabled = enable;		
}

gboolean	
lmplayer_notebook_get_tab_drag_and_drop_enabled (LmplayerNotebook *nb)
{
	g_return_val_if_fail (LMPLAYER_IS_NOTEBOOK (nb), TRUE);
	
	return nb->priv->tab_drag_and_drop_enabled;
}

