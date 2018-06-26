#include "hanjp.h"
#include "hanjpeater.h"

#define STR_MAX 64

/*HanjpInputContext 기초 규칙
 eater에서는 변환할 수 있는 모든 문자들을 가나문자로 변환해 저장합니다.
*/
struct _HanjpInputContext {
  HanjpEater* eater;
  ucschar preedit_string[STR_MAX];
  int preedit_length; 
  ucschar commit_string[STR_MAX];
};