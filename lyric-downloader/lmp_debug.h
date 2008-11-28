
#ifndef __LMP_DEBUG_H
#define __LMP_DEBUG_H

#include <stdarg.h>
#include <glib.h>

G_BEGIN_DECLS

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define lmp_debug(...) lmp_debug_real (__func__, __FILE__, __LINE__, TRUE, __VA_ARGS__)
#elif defined(__GNUC__) && __GNUC__ >= 3
#define lmp_debug(...) lmp_debug_real (__FUNCTION__, __FILE__, __LINE__, TRUE, __VA_ARGS__)
#else
#define lmp_debug
#endif

void lmp_debug_set_flag(gboolean debug_flag);

gboolean lmp_debug_matches      (const char *func,
				const char *file);
void lmp_debug_real             (const char *func,
				const char *file,
				int line,
				gboolean newline,
				const char *format, ...) G_GNUC_PRINTF (5, 6);

G_END_DECLS

#endif /* __LMP_DEBUG_H */
