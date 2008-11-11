/*
 * lmplayer-notebook.h
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

/* This file is a modified version of the epiphany file ephy-notebook.h
 * Here the relevant copyright:
 *
 *  Copyright (C) 2002 Christophe Fergeau
 *  Copyright (C) 2003 Marco Pesenti Gritti
 *  Copyright (C) 2003, 2004 Christian Persch
 *
 */
 
#ifndef _LMPLAYER_NOTEBOOK_H_
#define _LMPLAYER_NOTEBOOK_H_

#include <glib.h>
#include <gtk/gtknotebook.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define LMPLAYER_TYPE_NOTEBOOK		(lmplayer_notebook_get_type ())
#define LMPLAYER_NOTEBOOK(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), LMPLAYER_TYPE_NOTEBOOK, LmplayerNotebook))
#define LMPLAYER_NOTEBOOK_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), LMPLAYER_TYPE_NOTEBOOK, LmplayerNotebookClass))
#define LMPLAYER_IS_NOTEBOOK(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), LMPLAYER_TYPE_NOTEBOOK))
#define LMPLAYER_IS_NOTEBOOK_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), LMPLAYER_TYPE_NOTEBOOK))
#define LMPLAYER_NOTEBOOK_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), LMPLAYER_TYPE_NOTEBOOK, LmplayerNotebookClass))

/* Private structure type */
typedef struct _LmplayerNotebookPrivate	LmplayerNotebookPrivate;

/*
 * Main object structure
 */
typedef struct _LmplayerNotebook		LmplayerNotebook;
 
struct _LmplayerNotebook
{
	GtkNotebook notebook;

	/*< private >*/
	LmplayerNotebookPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _LmplayerNotebookClass	LmplayerNotebookClass;

struct _LmplayerNotebookClass
{
	GtkNotebookClass parent_class;

	/* Signals */
	void (* tab_added) (LmplayerNotebook *notebook, GtkWidget *tab);
	void (* tab_removed) (LmplayerNotebook *notebook, GtkWidget *tab);
	void (* tab_detached) (LmplayerNotebook *notebook, GtkWidget *tab);
	void (* tabs_reordered) (LmplayerNotebook *notebook);
	void (* tab_close_request) (LmplayerNotebook *notebook, GtkWidget *tab);
};

/*
 * Public methods
 */
GType lmplayer_notebook_get_type (void) G_GNUC_CONST;

GtkWidget *lmplayer_notebook_new (void);

void lmplayer_notebook_add_tab (LmplayerNotebook *nb, GtkWidget *tab, gint position, gboolean jump_to);

void lmplayer_notebook_remove_tab (LmplayerNotebook *nb, GtkWidget *tab);

void lmplayer_notebook_remove_all_tabs (LmplayerNotebook *nb);

void lmplayer_notebook_reorder_tab (LmplayerNotebook *src, GtkWidget *tab, gint dest_position);
			    			 
void lmplayer_notebook_move_tab	(LmplayerNotebook *src, LmplayerNotebook *dest, GtkWidget *tab, gint dest_position);

/* FIXME: do we really need this function ? */
void lmplayer_notebook_set_always_show_tabs	(LmplayerNotebook *nb, gboolean show_tabs);

void lmplayer_notebook_set_close_buttons_sensitive (LmplayerNotebook *nb, gboolean sensitive);

gboolean lmplayer_notebook_get_close_buttons_sensitive (LmplayerNotebook *nb);

void lmplayer_notebook_set_tab_drag_and_drop_enabled (LmplayerNotebook *nb, gboolean enable);

gboolean lmplayer_notebook_get_tab_drag_and_drop_enabled (LmplayerNotebook *nb);

G_END_DECLS

#endif /* _LMPLAYER_NOTEBOOK_H_ */
