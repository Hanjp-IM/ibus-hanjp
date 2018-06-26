#include "hanjp.h"
#include "hanjpeater.h"
#include <stdlib.h>

#define STR_MAX 64

/*HanjpInputContext 기초 규칙
 eater에서는 변환할 수 있는 모든 문자들을 가나문자로 변환해 저장합니다.
 ucs string은 끝에 0이 들어있는 것으로 문자열 끝을 구분합니다. 
*/

enum {
  STATE_AT,
  STATE_DT
};

struct _HanjpInputContext {
  HanjpEater* eater;
  int state;
  ucschar preedit_string[STR_MAX];
  int preedit_length; 
  ucschar commit_string[STR_MAX];
};

static void hanjp_ic_flush_internal(HanjpInputContext* hjic);

HanjpInputContext* hanjp_ic_new(const char* keyboard)
{
  HanjpInputContext* hjic;
  hjic = malloc(sizeof(HanjpInputContext));

  hjic->eater = eater_new(keyboard);
  hjic->state = STATE_DT;
  hjic->preedit_string[0] = 0;
  hjic->preedit_length = 0;
  hjic->commit_string[0] = 0;
}

void hanjp_ic_delete(HanjpInputContext *hjic)
{
  eater_delete(hjic->eater);
  free(hjic);
}

bool hanjp_ic_process(HanjpInputContext* hjic, int ascii)
{
  int eatFlag;

  eatFlag = eater_push(hjic->eater, ascii, hjic->preedit_string, hjic->preedit_length); //eater에 푸시

  if(eatFlag && EATFLG_P){ //푸시 됐을 경우
    hjic->preedit_length++; //preedit string length 1증가
    if(hjic->preedit_length == STR_MAX){
      hanjp_ic_flush_internal(hjic);
    }
    else{
      hjic->preedit_string[hjic->preedit_length] = 0;
    }
  }

  if(eatFlag && EATFLG_ATDT){ //ATDT인 경우
    switch(hjic->state){
      case STATE_AT:
      hjic->state = STATE_DT;
      hanjp_ic_flush_internal(hjic); //commit string으로 옮김
      break;
      case STATE_DT:
      hjic->state = STATE_AT;
      break;
      default:
      return false;
    }
  }

  if(eatFlag && EATFLG_Q){
    //추가 구현
  }

  return true;
}

const ucschar* hanjp_ic_get_commit_string(HanjpInputContext* hjic)
{
  return hjic->commit_string;
}

//commit string으로 옮김
static void hanjp_ic_flush_internal(HanjpInputContext* hjic){
  int i;

  for(i=0; i<hjic->preedit_length; i++){
    hjic->commit_string[i] = hjic->preedit_string[i];
  }
  hjic->commit_string[i] = 0;
  hjic->preedit_string[0] = 0;
  hjic->preedit_length = 0;
}

void hanjp_ic_flush(HanjpInputContext* hjic){
  hjic->preedit_string[0] = 0;
  hjic->commit_string[0] = 0;
  hjic->state = STATE_DT;
  eater_flush(hjic->eater);
}

int hanjp_init()
{
  return hangul_init();
}

int hanjp_fini()
{
  return hangul_fini();
}