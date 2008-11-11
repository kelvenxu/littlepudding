#ifndef _AUDIO_SPECTRUM_H_
#define _AUDIO_SPECTRUM_H_

#include <glib.h>
#include <gst/gst.h>

void audio_spectrum_get_data(
		const GstStructure* s,
		gint bands, 
		gfloat* spect);

#endif // _AUDIO_SPECTRUM_H_
