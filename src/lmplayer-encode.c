#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include "lmplayer-encode.h"
#include "lmplayer-debug.h"

gchar* lmplayer_encode_convert_to_utf8(const char* str)
{
	GError* error = NULL;
	gchar* pgb18030 = g_convert(str, -1, "iso-8859-1", "utf-8", NULL, NULL, &error);
	if(error)
	{
		//lmplayer_debug("g_convert: %s", error->message);
		return g_strdup(str);
	}

	error = NULL;
	gchar* putf8 = g_convert(pgb18030, -1, "utf-8", "gb18030", NULL, NULL, &error);
	if(error)
	{
		//lmplayer_debug("g_convert to utf-8: %s", error->message);
		return g_strdup(str);
	}

	g_free(pgb18030);
	return putf8;
}

