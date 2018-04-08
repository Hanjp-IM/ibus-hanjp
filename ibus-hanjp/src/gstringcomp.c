#include <gstringcomp.h>

GString* g_string_append_ucs4(GString* str, const ucschar* s, gint len)
{
    gchar utf8[100];

    g_ucs4_to_utf8(s, utf8, len);
    return g_string_append(str, utf8);
}

gchar* 
ustring_to_utf8(const ucschar *src, gchar* dest, gint len)
{
    return g_ucs4_to_utf8((const gunichar*)src, len, NULL, NULL, NULL);
}