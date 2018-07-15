#include <hangul.h>

typedef struct _HanjpEater HanjpEater;

HanjpEater* eater_new(const char* keyboard);
void eater_delete(HanjpEater* eater);
int eater_push(HanjpEater* eater, int ascii, ucschar* outer, int outer_length, HanjpOutputType type);
const ucschar* eater_get_preedit(HanjpEater* eater);
bool eater_backspace(HanjpEater* eater);
void eater_flush(HanjpEater* eater);
bool eater_is_empty(HanjpEater* eater);