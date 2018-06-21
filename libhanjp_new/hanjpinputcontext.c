#include "hanjp.h"
#include "hanjpeater.h"

#define STR_MAX 64

/*HanjpInputContext 기초 규칙
 
*/
struct _HanjpInputContext {
  HanjpEater* eater;
  ucschar preedit_string[STR_MAX];
  int preedit_length; 
  ucschar commit_string[STR_MAX];
};