#include <hangul.h>

#define EATFLG_ATDT 04
#define EATFLG_Q    02
#define EATFLG_P    01

typedef struct _HanjpEater HanjpEater;

HanjpEater* eater_new(const char* keyboard);
void eater_delete(HanjpEater* eater);
int eater_push(HanjpEater* eater, ucschar ch, ucschar* outer, int outer_length);
const ucschar* eater_get_preedit(HanjpEater* eater);
bool eater_backspace(HanjpEater* eater);
void eater_flush(HanjpEater* eater);
bool eater_is_empty(HanjpEater* eater);