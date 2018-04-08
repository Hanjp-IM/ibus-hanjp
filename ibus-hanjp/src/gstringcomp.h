#include <glib.h>
#include <hangul.h>

GString* g_string_append_ucs4(GString* str, const ucschar* s, gint len);
gchar* ustring_to_utf8(const ucschar *src, gchar* dest, gint len);