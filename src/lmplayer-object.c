/* vi: set sw=4 ts=4: */
/*
 * lmplayer-object.c
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

#include "lmplayer-object.h"
#include "lmplayerobject-marshal.h"
#include "lmplayer-debug.h"
#include "lmplayer-plugins-engine.h"

enum {
	PROP_0,
	PROP_FULLSCREEN,
	PROP_PLAYING,
	PROP_STREAM_LENGTH,
	PROP_SEEKABLE,
	PROP_CURRENT_TIME,
	PROP_ERROR_SHOWN,
	PROP_CURRENT_MRL
};

enum {
	FILE_OPENED,
	FILE_CLOSED,
	METADATA_UPDATED,
	LAST_SIGNAL
};

static void lmplayer_object_set_property		(GObject *object,
						 guint property_id,
						 const GValue *value,
						 GParamSpec *pspec);
static void lmplayer_object_get_property		(GObject *object,
						 guint property_id,
						 GValue *value,
						 GParamSpec *pspec);
static void lmplayer_object_finalize(GObject *lmplayer);

static int lmplayer_table_signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(LmplayerObject, lmplayer_object, G_TYPE_OBJECT);

/*
#define LMPLAYER_OBJECT_GET_PRIVATE(o)\
	(G_TYPE_INSTANCE_GET_PRIVATE ((o), LMPLAYER_TYPE_OBJECT, LmplayerObjectPrivate))


struct _LmplayerObjectPrivate 
{
};
*/


static void
lmplayer_object_dispose (LmplayerObject *self)
{
}

static void
lmplayer_object_finalize(GObject *object)
{
	G_OBJECT_CLASS(lmplayer_object_parent_class)->finalize(object);
}

static void
lmplayer_object_init (LmplayerObject *self)
{
	//LmplayerObjectPrivate *priv;

	//priv = LMPLAYER_OBJECT_GET_PRIVATE (self);
}

static void
lmplayer_object_set_property (GObject *object,
			   guint property_id,
			   const GValue *value,
			   GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
}

static void
lmplayer_object_get_property (GObject *object,
			   guint property_id,
			   GValue *value,
			   GParamSpec *pspec)
{
	LmplayerObject *lmplayer;

	lmplayer = LMPLAYER_OBJECT(object);

	switch (property_id)
	{
	case PROP_FULLSCREEN:
		g_value_set_boolean(value, lmplayer_is_fullscreen (lmplayer));
		break;
	case PROP_PLAYING:
		g_value_set_boolean(value, lmplayer_is_playing (lmplayer));
		break;
	case PROP_STREAM_LENGTH:
		g_value_set_int64(value, bacon_video_widget_get_stream_length(lmplayer->bvw));
		break;
	case PROP_CURRENT_TIME:
		g_value_set_int64(value, bacon_video_widget_get_current_time(lmplayer->bvw));
		break;
	case PROP_SEEKABLE:
		g_value_set_boolean(value, lmplayer_is_seekable(lmplayer));
		break;
	case PROP_ERROR_SHOWN:
		//g_value_set_boolean (value, XXX);
		break;
	case PROP_CURRENT_MRL:
		g_value_set_string (value, lmplayer->mrl);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
lmplayer_object_class_init (LmplayerObjectClass *self_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (self_class);

	//g_type_class_add_private (self_class, sizeof (LmplayerObjectPrivate));
	object_class->set_property = lmplayer_object_set_property;
	object_class->get_property = lmplayer_object_get_property;

	object_class->dispose = (void (*) (GObject *object)) lmplayer_object_dispose;
	object_class->finalize = (void (*) (GObject *object)) lmplayer_object_finalize;

	g_object_class_install_property (object_class, PROP_FULLSCREEN,
					 g_param_spec_boolean ("fullscreen", NULL, NULL,
							       FALSE, G_PARAM_READABLE));
	g_object_class_install_property (object_class, PROP_PLAYING,
					 g_param_spec_boolean ("playing", NULL, NULL,
							       FALSE, G_PARAM_READABLE));
	g_object_class_install_property (object_class, PROP_STREAM_LENGTH,
					 g_param_spec_int64 ("stream-length", NULL, NULL,
							     G_MININT64, G_MAXINT64, 0,
							     G_PARAM_READABLE));
	g_object_class_install_property (object_class, PROP_CURRENT_TIME,
					 g_param_spec_int64 ("current-time", NULL, NULL,
							     G_MININT64, G_MAXINT64, 0,
							     G_PARAM_READABLE));
	g_object_class_install_property (object_class, PROP_SEEKABLE,
					 g_param_spec_boolean ("seekable", NULL, NULL,
							       FALSE, G_PARAM_READABLE));
	g_object_class_install_property (object_class, PROP_ERROR_SHOWN,
					 g_param_spec_boolean ("error-shown", NULL, NULL,
							       FALSE, G_PARAM_READABLE));
	g_object_class_install_property (object_class, PROP_CURRENT_MRL,
					 g_param_spec_string ("current-mrl", NULL, NULL,
							      NULL, G_PARAM_READABLE));

	lmplayer_table_signals[FILE_OPENED] =
		g_signal_new ("file-opened",
				G_TYPE_FROM_CLASS (object_class),
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET (LmplayerObjectClass, file_opened),
				NULL, NULL,
				g_cclosure_marshal_VOID__STRING,
				G_TYPE_NONE, 1, G_TYPE_STRING);

	lmplayer_table_signals[FILE_CLOSED] =
		g_signal_new ("file-closed",
				G_TYPE_FROM_CLASS (object_class),
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET (LmplayerObjectClass, file_closed),
				NULL, NULL,
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE, 0, G_TYPE_NONE);

	lmplayer_table_signals[METADATA_UPDATED] =
		g_signal_new ("metadata-updated",
				G_TYPE_FROM_CLASS (object_class),
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET (LmplayerObjectClass, metadata_updated),
				NULL, NULL,
				lmplayerobject_marshal_VOID__STRING_STRING_STRING,
				G_TYPE_NONE, 3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
}

/**
 * lmplayer_object_plugins_init:
 * @lmplayer: a #LmplayerObject
 *
 * Initialises the plugin engine and activates all the
 * enabled plugins.
 **/
void
lmplayer_object_plugins_init(LmplayerObject *lmplayer)
{
	lmplayer_plugins_engine_init(lmplayer);
}

/**
 * lmplayer_object_plugins_shutdown:
 *
 * Shuts down the plugin engine and deactivates all the
 * plugins.
 **/
void
lmplayer_object_plugins_shutdown(void)
{
	lmplayer_plugins_engine_shutdown ();
}

void
lmplayer_add_tools_button(LmplayerObject *lmplayer, GtkWidget *button, const char *id)
{
	g_return_if_fail(lmplayer);

	gtk_box_pack_start(GTK_BOX(lmplayer->plugins_box), button, FALSE, FALSE, 0);
	g_object_set(lmplayer->plugins_box, id, button, NULL);
}

void 
lmplayer_remove_tools_button(LmplayerObject *lmplayer, const char *id)
{
	GtkWidget *widget;

	g_return_if_fail(lmplayer);

	g_object_get(lmplayer->plugins_box, id, &widget, NULL);

	if(widget)
		gtk_container_remove(GTK_CONTAINER(lmplayer->plugins_box), widget);
}

LmplayerObject* lmplayer_object_new()
{
	return g_object_new(LMPLAYER_TYPE_OBJECT, NULL);
	//return g_new0(LmplayerObject, 1);
}

void
lmplayer_file_opened (LmplayerObject *lmplayer,
		   const char *mrl)
{
	lmplayer_debug(" ");
	g_signal_emit (G_OBJECT (lmplayer),
		       lmplayer_table_signals[FILE_OPENED],
		       0, mrl);
}

void
lmplayer_file_closed (LmplayerObject *lmplayer)
{
	g_signal_emit (G_OBJECT (lmplayer),
		       lmplayer_table_signals[FILE_CLOSED],
		       0);

}

void
lmplayer_metadata_updated (LmplayerObject *lmplayer,
			const char *artist,
			const char *title,
			const char *album)
{
	g_signal_emit (G_OBJECT (lmplayer),
		       lmplayer_table_signals[METADATA_UPDATED],
		       0,
		       artist,
		       title,
		       album);
}

GtkWindow *
lmplayer_get_main_window(LmplayerObject *lmplayer)
{
	g_return_val_if_fail(LMPLAYER_IS_OBJECT(lmplayer), NULL);

	g_object_ref(G_OBJECT(lmplayer->win));

	return GTK_WINDOW(lmplayer->win);
}

GtkUIManager *
lmplayer_get_ui_manager(LmplayerObject *lmplayer)
{
	g_return_val_if_fail(LMPLAYER_IS_OBJECT(lmplayer), NULL);

	//return lmplayer->ui_manager;
	return NULL;
}

GtkWidget *
lmplayer_get_video_widget(LmplayerObject *lmplayer)
{
	g_return_val_if_fail(LMPLAYER_IS_OBJECT(lmplayer), NULL);

	g_object_ref(G_OBJECT(lmplayer->bvw));

	return GTK_WIDGET(lmplayer->bvw);
}

char *
lmplayer_get_video_widget_backend_name(LmplayerObject *lmplayer)
{
	return bacon_video_widget_get_backend_name(lmplayer->bvw);
}

gint64
lmplayer_get_current_time(LmplayerObject *lmplayer)
{
	g_return_val_if_fail(LMPLAYER_IS_OBJECT(lmplayer), 0);

	return bacon_video_widget_get_current_time(lmplayer->bvw);
}


gboolean
lmplayer_is_fullscreen(LmplayerObject *lmplayer)
{
	//g_return_val_if_fail(LMPLAYER_IS_OBJECT(lmplayer), FALSE);

	//return (lmplayer->controls_visibility == LMPLAYER_CONTROLS_FULLSCREEN);
	return FALSE;
}

/**
 * lmplayer_is_playing:
 * @lmplayer: a #LmplayerObject
 *
 * Returns %TRUE if Lmplayer is playing a stream.
 *
 * Return value: %TRUE if Lmplayer is playing a stream
 **/
gboolean
lmplayer_is_playing (LmplayerObject *lmplayer)
{
	g_return_val_if_fail (LMPLAYER_IS_OBJECT (lmplayer), FALSE);

	if (lmplayer->bvw == NULL)
		return FALSE;

	return bacon_video_widget_is_playing (lmplayer->bvw) != FALSE;
}

gboolean
lmplayer_is_paused (LmplayerObject *lmplayer)
{
	g_return_val_if_fail (LMPLAYER_IS_OBJECT (lmplayer), FALSE);

	return lmplayer->state == STATE_PAUSED;
}

/**
 * lmplayer_is_seekable:
 * @lmplayer: a #LmplayerObject
 *
 * Returns %TRUE if the current stream is seekable.
 *
 * Return value: %TRUE if the current stream is seekable
 **/
gboolean
lmplayer_is_seekable (LmplayerObject *lmplayer)
{
	g_return_val_if_fail (LMPLAYER_IS_OBJECT (lmplayer), FALSE);

	if (lmplayer->bvw == NULL)
		return FALSE;

	return bacon_video_widget_is_seekable (lmplayer->bvw) != FALSE;
}

