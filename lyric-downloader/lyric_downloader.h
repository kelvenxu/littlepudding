#ifndef _LMP_LYRIC_DOWNLOAD_H_
#define _LMP_LYRIC_DOWNLOAD_H_

#define MAXPATHLEN 1024

typedef struct _LyricId
{
	int no;
	char* id;
	char* artist;
	char* title;
} LyricId;

gchar* lmp_lyric_download( gchar* filename, gchar* artist, gchar* title); 
void lmp_lyric_download_with_thread( gchar* filename, gchar* artist, gchar* title);

gboolean tt_get_lyrics_content_and_save(unsigned int id, gchar* artist, gchar* title, const gchar* lyric_filename);
gchar* tt_get_lyrics_list(const gchar* artist, const gchar* title);
gchar* tt_get_lyrics_content(gint id, gchar* artist, gchar* title);
GSList* tt_parse_lyricslist(gchar* xml);

void tt_lyrics_list_selection_dialog( GSList* list, const gchar* lyric_filename);
#endif // _LMP_LYRIC_DOWNLOAD_H_

