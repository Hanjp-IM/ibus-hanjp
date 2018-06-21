#include <hangul.h>

#define EATFLG_ATDT 04
#define EATFLG_Q    02
#define EATFLG_P    01

typedef struct _HanjpEater HanjpEater;

HanjpEater* eater_new(const char* keyboard);
int eater_push(HanjpEater* eater, ucschar pc, ucschar* outer, int outer_length);
bool eater_backspace(HanjpEater* eater);
