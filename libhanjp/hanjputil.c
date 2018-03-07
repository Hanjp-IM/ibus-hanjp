#include "hanjp.h"

void ucs_append(ucschar *dest, ucschar * add)
{
    int dest_idx, add_idx;

    if(!dest || !add)
        return;

    dest_idx = 0;
    add_idx = 0;

    while(dest[dest_idx])
        dest_idx++;

    while(add[add_idx])
        dest[dest_idx++] = add[add_idx++];
}

void ucs_copy(ucschar *dest, ucschar * src)
{
    int i;

    if(!dest || !src)
        return;

    for(i=0; src[i]; i++)
        dest[i] = src[i];
}

void ucs_append_char(ucschar *dest, ucschar add)
{
    int dest_idx = 0;

    if(!dest || !add)
        return;

    while(dest[dest_idx])
        dest_idx++;

    dest[dest_idx++] = add;
    dest[dest_idx] = 0;
}