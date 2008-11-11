#include "audio-spectrum.h"

#include <glib.h>
#include <gst/gst.h>
#include "lmp-debug.h"

void audio_spectrum_get_data(
		const GstStructure* s,
		gint bands, 
		gfloat* spect)
{
	const GValue *list = NULL;
	const GValue *value = NULL;
	guint major, minor, micro, nano;
	guint i;
			
	gst_version(&major, &minor, &micro, &nano);
	if((major == 0) && (minor == 10) && (micro >= 16))
	{
		list = gst_structure_get_value(s, "magnitude");
		for(i = 0; i < bands; ++i)
		{
			value = gst_value_list_get_value(list, i);
			spect[i] = 80.0 + g_value_get_float(value);
		}
	}
	else
	{
		list = gst_structure_get_value(s, "spectrum");
		for(i = 0; i < bands; ++i)
		{
			value = gst_value_list_get_value(list, i);
			spect[i] = (gfloat)g_value_get_uchar(value);
		}
	}
}
