/* vi: set sw=4 ts=4: */
/*
 * lmplayer-tab.c
 *
 * This file is part of lmplayer.
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

#include "lmplayer-tab.h"
#include <glib/gi18n.h>

G_DEFINE_TYPE(LmplayerTab, lmplayer_tab, GTK_TYPE_HBOX);

#define LMPLAYER_TAB_GET_PRIVATE(o)\
	(G_TYPE_INSTANCE_GET_PRIVATE((o), LMPLAYER_TYPE_TAB, LmplayerTabPrivate))

struct _LmplayerTabPrivate 
{
	GtkWidget *page;
	GtkWidget *label;
	GtkWidget *close_button;
	GtkWidget *close_image;
};

/* Signals */
enum
{
	TAB_CLOSE_REQUEST,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };


static void
lmplayer_tab_dispose (LmplayerTab *self)
{
}

static void
lmplayer_tab_finalize (LmplayerTab *self)
{
}

static void
close_button_clicked_cb(GtkWidget *widget, LmplayerTab *tab)
{
	g_signal_emit(tab, signals[TAB_CLOSE_REQUEST], 0);
}

static void
tab_label_style_set_cb(GtkWidget *hbox, GtkStyle *previous_style, LmplayerTab *tab)
{
	gint h, w;

	gtk_icon_size_lookup_for_settings(gtk_widget_get_settings(hbox), GTK_ICON_SIZE_MENU, &w, &h);

	LmplayerTabPrivate *priv = LMPLAYER_TAB_GET_PRIVATE(tab);
	gtk_widget_set_size_request(priv->close_button, w + 2, h + 2);
}

static void
lmplayer_tab_init (LmplayerTab *self)
{
	LmplayerTabPrivate *priv;

	priv = LMPLAYER_TAB_GET_PRIVATE(self);

	priv->label = gtk_label_new("");
	gtk_misc_set_alignment(GTK_MISC(priv->label), 0.0, 0.5);
	gtk_misc_set_padding(GTK_MISC(priv->label), 0, 0);
	gtk_box_pack_start(GTK_BOX(self), priv->label, FALSE, FALSE, 0);

	priv->close_button = gtk_button_new();
	gtk_button_set_relief(GTK_BUTTON(priv->close_button), GTK_RELIEF_NONE);
	gtk_button_set_focus_on_click(GTK_BUTTON(priv->close_button), FALSE);

	/* make it as small as possible */
	GtkRcStyle *rcstyle = gtk_rc_style_new();
	rcstyle->xthickness = rcstyle->ythickness = 0;
	gtk_widget_modify_style(priv->close_button, rcstyle);
	gtk_rc_style_unref(rcstyle),

	priv->close_image = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
	gtk_container_add(GTK_CONTAINER(priv->close_button), priv->close_image);
	gtk_box_pack_start(GTK_BOX(self), priv->close_button, FALSE, FALSE, 0);

	gtk_widget_set_tooltip_text(priv->close_button, _("Close playlist"));
	g_signal_connect(priv->close_button, "clicked", G_CALLBACK(close_button_clicked_cb), self); //FIXME: ?!

	/* Set minimal size */
	g_signal_connect(self, "style-set", G_CALLBACK(tab_label_style_set_cb), self);
	
	gtk_widget_show(priv->label);
	gtk_widget_show(priv->close_image);
	gtk_widget_show(priv->close_button);
	gtk_widget_show(GTK_WIDGET(self));
}

static void
lmplayer_tab_class_init(LmplayerTabClass *self_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (self_class);

	g_type_class_add_private (self_class, sizeof (LmplayerTabPrivate));
	object_class->dispose = (void (*) (GObject *object)) lmplayer_tab_dispose;
	object_class->finalize = (void (*) (GObject *object)) lmplayer_tab_finalize;

	signals[TAB_CLOSE_REQUEST] =
		g_signal_new ("tab-close-request",
			      G_OBJECT_CLASS_TYPE(object_class),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET(LmplayerTabClass, tab_close_request),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE,
			      0);
}

GtkWidget *lmplayer_tab_new()
{
	return GTK_WIDGET(g_object_new(LMPLAYER_TYPE_TAB, 
																"homogeneous", FALSE, 
																"spacing", 4, NULL));
}

void lmplayer_tab_set_page(LmplayerTab *tab, GtkWidget *page)
{
	LmplayerTabPrivate *priv = LMPLAYER_TAB_GET_PRIVATE(tab);

	priv->page = page;
}

GtkWidget *lmplayer_tab_get_page(LmplayerTab *tab)
{
	LmplayerTabPrivate *priv = LMPLAYER_TAB_GET_PRIVATE(tab);
	return priv->page;
}

void lmplayer_tab_set_name(LmplayerTab *tab, const char *new_name)
{
	LmplayerTabPrivate *priv = LMPLAYER_TAB_GET_PRIVATE(tab);
	gtk_label_set_text(GTK_LABEL(priv->label), new_name);
}

const char *lmplayer_tab_get_name(LmplayerTab *tab)
{
	LmplayerTabPrivate *priv = LMPLAYER_TAB_GET_PRIVATE(tab);
	return gtk_label_get_text(GTK_LABEL(priv->label));
}

