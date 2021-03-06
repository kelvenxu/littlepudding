
#include <gdk/gdk.h>
#include <gtk/gtk.h>

#define LMPLAYER_OBJECT_HAS_SIGNAL(obj, name) (g_signal_lookup (name, g_type_from_name (G_OBJECT_TYPE_NAME (obj))) != 0)

void lmplayer_gdk_window_set_invisible_cursor (GdkWindow *window);
void lmplayer_gdk_window_set_waiting_cursor (GdkWindow *window);

void yuy2toyv12 (guint8 *y, guint8 *u, guint8 *v, guint8 *input,
		 int width, int height);
guint8 *yv12torgb (guint8 *src_y, guint8 *src_u, guint8 *src_v,
		   int width, int height);

gboolean lmplayer_display_is_local (void);

char *lmplayer_time_to_string (gint64 msecs);
char *lmplayer_time_to_string_text (gint64 msecs);

void lmplayer_widget_set_preferred_size (GtkWidget *widget, gint width,
				      gint height);
gboolean lmplayer_ratio_fits_screen (GdkWindow *window, int video_width,
				  int video_height, gfloat ratio);

