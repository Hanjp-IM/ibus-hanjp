#include "hanjp.h"
#include "hanjpchar.h"
#include "hangulinternals.h"
#include <stdlib.h>

static void hic_on_translate(HangulInputContext*, int, ucschar*, void*);
static bool hic_on_transition(HangulInputContext*, ucschar, const ucschar*, void*);
static void hic_on_translate_full(HangulInputContext*, int, ucschar*, void*);
static bool hic_on_transition_full(HangulInputContext*, ucschar, const ucschar*, void*);
static bool hanjp_ic_flush_internal(HanjpInputContext* hjic);
static void hanjp_ic_save_hangul_preedit_string(HanjpInputContext* hjic);
static ucschar hangul_resolve_bangjeom(ucschar prev);

static const ucschar hangul_double_dot_tone_mark = 0x302f;

struct _HanjpInputContext {
  HangulInputContext *hic;
  int output_type;
  ucschar preedit_string[64];
  int kana_idx;
  ucschar commit_string[64];
  bool use_full;
  bool mode_old_hangul_free;
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

  if(hangul_ic_has_choseong(hic) && hangul_ic_has_jungseong(hic)) //'ㅇ'이 아니면 이중 모음을 허락하지 않음
  {
    if(hangul_is_jungseong(ch)){
      if(buf[0] != HANJP_CHOSEONG_IEUNG)
        return false;
    }
  }

  if(buf[0] == HANJP_CHOSEONG_SSANGNIEUN)
    return false;

  if(hangul_is_jongseong(ch)){
    return false;
  }

  return true;
}

static void hic_on_translate_full(HangulInputContext* hic, int ascii, ucschar* ch, void* data)
{
  ucschar c;

  switch(c)
  {
    case HANJP_CHOSEONG_SSANGNIEUN:
    *ch = HANJP_CHOSEONG_NIEUN; break;
    case HANJP_CHOSEONG_SSANGSIOS:
    *ch = HANJP_CHOSEONG_SIOS;
  }
}

static bool hic_on_transition_full(HangulInputContext* hic, ucschar ch, const ucschar* buf, void* data)
{
  if(hangul_is_jongseong(ch))
  {
    if(hangul_ic_has_jongseong(hic))
      return false;

    if(!hangul_is_kana_batchim(hangul_jongseong_to_choseong(ch)))
      return false;
  }

  if(buf[0] == HANJP_CHOSEONG_SSANGNIEUN)
    return false;

  return true;
}

void hanjp_ic_set_use_full(HanjpInputContext *hjic, bool set){
  hjic->use_full = set;

  if(set){
    hangul_ic_connect_callback(hjic->hic, "translaste", hic_on_translate_full, NULL);
    hangul_ic_connect_callback(hjic->hic, "transition", hic_on_transition_full, NULL);
  }
  else{
    hangul_ic_connect_callback(hjic->hic, "translaste", hic_on_translate, NULL);
    hangul_ic_connect_callback(hjic->hic, "transition", hic_on_transition, NULL);
  }
}

void hanjp_ic_set_old_hangul_free(HanjpInputContext *hjic, bool set){
  hjic->mode_old_hangul_free = set;
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

  hjic = (HanjpInputContext*)malloc(sizeof(HanjpInputContext));

  hjic->hic = hangul_ic_new(keyboard);
  hangul_ic_set_output_mode(hjic->hic, HANGUL_OUTPUT_JAMO);
  hjic->output_type = HANJP_OUTPUT_JP_HIRAGANA;
  hjic->preedit_string[0] = 0;
  hjic->kana_idx = 0;
  hanjp_ic_set_use_full(hjic, false);
  hjic->mode_old_hangul_free = false;


  return hjic;
}

void hanjp_ic_delete(HanjpInputContext *hjic)
{
  hangul_ic_delete(hjic->hic);
  free(hjic);
}

bool hanjp_ic_process(HanjpInputContext* hjic, int ascii)
{
  int ret;
  int i, j;
  const ucschar* hic_commit = NULL;
  const ucschar* hic_preedit = NULL;
  ucschar hangul[12] = {0};
  ucschar non_hangul[12] = {0};
  ucschar converted_string[12] = {0};
  ucschar prev = 0;

  if(!hjic){
    return false;
  }

  if(!hangul_ic_process(hjic->hic, ascii)){ //자소 푸시
    hangul_ic_process(hjic->hic, ascii); //처리가 안됐으면 다시 넣음
  }

  if(hjic->kana_idx > 0){
    prev = hjic->preedit_string[hjic->kana_idx - 1];
  }
  else{
    prev = 0;
  }

  hic_commit = hangul_ic_get_commit_string(hjic->hic);
  hic_preedit = hangul_ic_get_preedit_string(hjic->hic);

  hjic->commit_string[0] = 0;

  if(hic_commit[0] != 0){ //hic 커밋이 일어나면
    for(i=0; hangul_is_jamo(hic_commit[i]); i++){ //commit string에서 한글 부분 복사
      hangul[i] = hic_commit[i];
    }
    hangul[i] = 0;
    for(j=0; hic_commit[i+j]; j++){ //commit string에서 한글이 아닌 부분 복사
      non_hangul[j] = hic_commit[i+j];
    }
    non_hangul[j] = 0;

    if(hjic->use_full)
      ret = hangul_to_kana_full(converted_string, hangul, hic_preedit[0], hjic->output_type);
    else
      ret = hangul_to_kana(converted_string, prev, hangul, hic_preedit[0], hjic->output_type);

    if(ret == -1){
      return false;
    }

    for(i=0; converted_string[i]; i++){ //변환된 문자 이어 붙이기
      hjic->preedit_string[hjic->kana_idx++] = converted_string[i];
    }
    if(non_hangul[0] != 0)
    { 
      if(hangul_double_dot_tone_mark == non_hangul[0])
      {
        ucschar ch;
        ch = hangul_resolve_bangjeom(prev);
        if(ch)
        {
          hjic->preedit_string[hjic->kana_idx++] = ch;
          i=1;
        }
        else
          i=0;
      }
      else
        i=0;

      if(non_hangul[i] != 0) //한글 자소가 아닌 문자가 오면 커밋하기
      {
        for(; i<64 && non_hangul[i]; i++){ //나머지 이어 붙이기
          hjic->preedit_string[hjic->kana_idx++] = non_hangul[i];
        }
        hjic->preedit_string[hjic->kana_idx] = 0;
        hanjp_ic_flush_internal(hjic);
      }
    }
  }
  
  hanjp_ic_save_hangul_preedit_string(hjic);
  return true;
}

bool hanjp_ic_backspace(HanjpInputContext *hjic)
{
  int i;
  int hangul_len = 0;

  if(!hjic){
    return false;
  }

  if(hjic->preedit_string[0] == 0){
    return false;
  }

  if(hangul_ic_is_empty(hjic->hic)){ //가나 문자만 있으면 kana_idx 줄이기
    hjic->kana_idx--;
  }
  else{
    if(!hangul_ic_backspace(hjic->hic)){
      return false;
    }
  }
  hanjp_ic_save_hangul_preedit_string(hjic);

  return true;
}

const ucschar* hanjp_ic_get_preedit_string(HanjpInputContext* hjic)
{
  if(!hjic){
    return NULL;
  }

  return hjic->preedit_string;
}

const ucschar* hanjp_ic_get_commit_string(HanjpInputContext* hjic)
{
  if(!hjic){
    return NULL;
  }

  return hjic->commit_string;
}

bool hanjp_ic_flush(HanjpInputContext *hjic)
{
  if(!hjic){
    return false;
  }

  hjic->preedit_string[0] = 0;
  hjic->kana_idx = 0;
  hjic->commit_string[0] = 0;

  hangul_ic_flush(hjic->hic);

  return true;
}

static bool hanjp_ic_flush_internal(HanjpInputContext* hjic)
{
  int i;

  if(!hjic){
    return false;
  }

  for(i=0; hjic->preedit_string[i]; i++){
    hjic->commit_string[i] = hjic->preedit_string[i];
  }
  hjic->commit_string[i] = 0;

  hjic->preedit_string[0] = 0;
  hjic->kana_idx = 0;

  hangul_ic_flush(hjic->hic);

  return false;
}

static void hanjp_ic_save_hangul_preedit_string(HanjpInputContext* hjic)
{
  int i;
  const ucschar *hic_preedit;

  hic_preedit = hangul_ic_get_preedit_string(hjic->hic);

  for(i=0; hic_preedit[i]; i++)
  {
    hjic->preedit_string[hjic->kana_idx + i] = hic_preedit[i]; 
  }
  hjic->preedit_string[hjic->kana_idx + i] = 0;
}

static ucschar hangul_resolve_bangjeom(ucschar prev)
{
  return 0;
}