
#include "config.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>
#include <time.h>

#include <glib.h>

#include "lmplayer-debug.h"

gboolean lmplayer_debug_matches (const char *func,
		  const char *file)
{
	/*
	if (debug_match == NULL ||
	   (debug_match != debug_everything &&
	   (strstr (file, debug_match) == NULL) &&
	   (strstr (func, debug_match) == NULL)))
		return FALSE;
		*/

	return TRUE;
}

/* Our own funky debugging function, should only be used when something
 * is not going wrong, if something *is* wrong use g_warning.
 */
void lmplayer_debug_real (const char *func,
	       const char *file,
	       const int line,
	       gboolean newline,
	       const char *format, ...)
{
	va_list args;
	char buffer[1025];
	char str_time[255];
	time_t the_time;

	//if (!lmp_debug_matches (func, file))
	//	return;

	va_start (args, format);

	g_vsnprintf (buffer, 1024, format, args);

	va_end (args);

	time (&the_time);
	strftime (str_time, 254, "%H:%M:%S", localtime (&the_time));

	g_printerr (newline ? "(%s) [%p] [%s] %s:%d: %s\n" : "(%s) [%p] [%s] %s:%d: %s",
		    str_time, g_thread_self (), func, file, line, buffer);
}
