#include "hanjp.h"
#include "hanjpchar.h"
#include <stdlib.h>

static void hic_on_translate(HangulInputContext*, int, ucschar*, void*);
static bool hic_on_transition(HangulInputContext*, ucschar, const ucschar*, void*);
static bool hanjp_ic_flush_internal(HanjpInputContext* hjic);

static const ucschar hangul_double_dot_tone_mark = 0x302f;

struct _HanjpInputContext {
  HangulInputContext *hic;
  int output_type;
  ucschar preedit_string[STR_MAX];
  int kana_length; 
  ucschar commit_string[STR_MAX];
};

static void hic_on_translate(HangulInputContext* hic, int ascii, ucschar* ch, void* data)
{
    //구현할 부분
    //전달할 문자를 변환 시킬 수 있다.
}

static bool hic_on_transition(HangulInputContext* hic, ucschar ch, const ucschar* buf, void* data)
{
    //hangul buffer에 뭐가 들어있는지 볼 수 있다.
    //초성이 'ㅇ'이 아닌 경우,
    //받침이 입력된 경우 false

    if(hangul_ic_has_choseong(hic) && hangul_ic_has_jungseong(hic)){
        if(hangul_is_jungseong(ch)){
            if(ch != 0x110B){ //'ㅇ'이 아니면
                return false;
            }
        }
    }

    if(hangul_is_jongseong(ch)){
        return false;
    }

    return true;
}

int hanjp_init()
{
  return hangul_init();
}

int hanjp_fini()
{
  return hangul_fini();
}

HanjpInputContext* hanjp_ic_new(const char* keyboard)
{
  HanjpInputContext* hjic;

  hjic = (hanjpInputContext*)malloc(sizeof(HanjpInputContext));

  hjic->hic = hangul_ic_new(keyboard);
  hangul_ic_connect_callback(eater->hic, "translaste", hic_on_translate, NULL);
  hangul_ic_connect_callback(eater->hic, "transition", hic_on_transition, NULL);
  hangul_ic_set_output_mode(eater->hic, HANGUL_OUTPUT_JAMO);
  hjic->output_type = HANJP_OUTPUT_JP_HIRAGANA;
  hjic->preedit_string[0] = 0;
  hjic->kana_length = 0;
  hjic->commit_string[0] = 0;

  return hjic;
}

void hanjp_ic_delete(HanjpInputContext *hjic)
{
  hangul_ic_delete(hjic->hic);
  free(hjic);
}

bool hanjp_ic_process(HanjpInputContext* hjic, int ascii)
{
  int push_length;
  int i, j;
  ucschar* hic_commit = NULL;
  ucschar* hic_preedit = NULL;
  ucschar hangul[12];
  ucschar non_hangul[12];
  ucschar converted_string[12];
  ucschar prev = 0;

  if(!hjic){
    return false;
  }

  if(!hangul_ic_process(hjic->hic ascii)){ //자소 푸시
    hangul_ic_process(hjic->hic, ascii); //처리가 안됐으면 다시 넣음
  }

  if(hjic->kana_length > 0){
    prev = hjic->preedit_string[kana_length - 1];
  }
  else{
    prev = 0;
  }

  hic_commit = hangul_ic_get_commit_string(hjic->hic);
  hic_preedit = hangul_ic_get_preedit_string(hjic->hic);

  if(hic_commit[0] != 0){
    for(i=0; hangul_is_jamo(hic_commit[i]); i++){ //commit string에서 한글 부분 복사
      hangul[i] = hic_commit[i];
    }
    hangul[i] = 0;
    for(j=0; hic_commit[i+j]; j++){ //commit string에서 한글이 아닌 부분 복사
      non_hangul[j] = hic_commit[i+j];
    }
    non_hangul[j] = 0;

    hangul_to_kana(converted_string, prev, hangul, hic_preedit[0], hjic->output_type);

    for(i=0; converted_string[i]; i++){ //변환된 문자 이어 붙이기
      hjic->preedit_string[hjic->kana_length++] = converted_string[i];
    }
    for(i=0; non_hangul[i]; i++){ //나머지 이어 붙이기
      hjic->preedit_string[hjic->kana_length++] = non_hangul[i];
    }
    hjic->preedit_string[hjic->kana_length] = 0;
  }
  
  for(i=0; hic_preedit[i]; i++){ //한글 preedit 이어 붙이기
      hjic->preedit_string[hjic->kana_length + i] = hic_preedit[i];
    
  }

  return true;
}

bool hanjp_ic_backspace(HanjpInputContext *hjic)
{
  int i;
  int hangul_len = 0;

  if(hjic->preedit_string[0] == 0){
    return false;
  }

  for(i = hjic->preedit_string; hjic->preedit_string[i]; i++){
    hangul_len++;
  }

  hjic->preedit_string[i-1] = 0;

  if(hangul_len == 0){ //가나 문자만 있으면 kana_length 줄이기
    kana_length--;
  }

  return true;
}