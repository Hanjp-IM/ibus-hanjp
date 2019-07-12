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
    
}

static bool hic_on_transition(HangulInputContext* hic, ucschar ch, const ucschar* buf, void* data)
{
  if(hangul_is_jungseong(ch)) //'ㅇ'이 아니면 이중 모음을 허락하지 않음
  {
    switch(ch)
    {
      case HANJP_JUNGSEONG_WE:
      case HANJP_JUNGSEONG_WI:
      case HANJP_JUNGSEONG_YI:
      case HANJP_JUNGSEONG_WEO:
      return false; 
      case HANJP_JUNGSEONG_WA:
      return true;
    }
  }

  if(buf[0] == HANJP_CHOSEONG_SSANGNIEUN)
    return false;

  if(hangul_is_jongseong(ch))
    return false;

  return true;
}

static void hic_on_translate_full(HangulInputContext* hic, int ascii, ucschar* ch, void* data)
{
  if(hangul_ic_has_jungseong(hic))
  {
    switch(*ch)
    {
      case HANJP_CHOSEONG_SSANGNIEUN:
      *ch = HANJP_CHOSEONG_NIEUN; break;
      case HANJP_CHOSEONG_SSANGSIOS:
      *ch = HANJP_CHOSEONG_SIOS; break;
      default:
      break;
    } 
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

void hanjp_ic_set_old_hangul_free(HanjpInputContext *hjic, bool set)
{
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
  hangul_ic_set_option(hjic->hic, HANGUL_IC_OPTION_COMBI_ON_DOUBLE_STROKE, false);
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
  bool hangul_ret;
  int i, j;
  const ucschar* hic_commit = NULL;
  const ucschar* hic_preedit = NULL;
  ucschar hangul[12] = {0};
  ucschar non_hangul[12] = {0};
  ucschar converted_string[12] = {0};
  static ucschar prev = 0;

  if(!hjic){
    return false;
  }

  hic_commit = hangul_ic_get_commit_string(hjic->hic);
  hic_preedit = hangul_ic_get_preedit_string(hjic->hic);

  hjic->commit_string[0] = 0;


  hangul_ret = hangul_ic_process(hjic->hic, ascii);

  if(hic_commit[0] != 0){ //hic 커밋이 일어나면
    for(i=0; hangul_is_jamo(hic_commit[i]); i++){ //commit string에서 한글 부분 복사
      hangul[i] = hic_commit[i];
    }
    hangul[i] = 0;
    for(j=0; hic_commit[i+j]; j++){ //commit string에서 한글이 아닌 부분 복사
      non_hangul[j] = hic_commit[i+j];
    }
    non_hangul[j] = 0;

    ret = hangul_to_kana(converted_string, prev, hangul, hic_preedit[0], hjic->output_type, hjic->use_full);

    if(ret == -1){
      hanjp_ic_flush_internal(hjic);
      return false;
    }

    for(i=0; converted_string[i]; i++){ //변환된 문자 이어 붙이기
      hjic->preedit_string[hjic->kana_idx++] = converted_string[i];
    }
    hjic->preedit_string[hjic->kana_idx] = 0;

    if(hangul[0]) //prev 저장
      prev = (hangul[1] == HANGUL_JUNGSEONG_FILLER) ? hangul[0] : hangul[1];
    else
      prev = 0;

    if(non_hangul[0] != 0)
    {
      ucschar ch;
      ch = kana_resolve_bangjeom((hjic->kana_idx > 0) ? hjic->preedit_string[hjic->kana_idx-1] : 0);
      prev = 0;
      if((non_hangul[0] == hangul_double_dot_tone_mark) && ch)
      {
        hjic->preedit_string[hjic->kana_idx++] = ch;
        hjic->preedit_string[hjic->kana_idx] = 0;
        i=1;
      }
      else{
        i=0;
      }

      if(non_hangul[i])
      {
        for(; i<64 && non_hangul[i]; i++){ //나머지 이어 붙이기
          hjic->preedit_string[hjic->kana_idx++] = non_hangul[i];
        }
        hjic->preedit_string[hjic->kana_idx] = 0;
        if(!hanjp_ic_flush_internal(hjic))
          return false;
      }
    }
  }
  
  if(!hangul_ret){ //자소 푸시
    hangul_ic_process(hjic->hic, ascii); //처리가 안됐으면 다시 넣음
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

  hjic->commit_string[0] = 0;

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

  return true;
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

int hanjp_ic_get_output_type(HanjpInputContext* hjic)
{
  return hjic->output_type;
}

void hanjp_ic_set_output_type(HanjpInputContext* hjic, int type)
{
  if((type >= HANJP_OUTPUT_JP_HIRAGANA) && (type < HANJP_OUTPUT_EN_FULL)) //아직 EN_FULL을 지원하지 않음
    hjic->output_type = type;
  else
    hjic->output_type = HANJP_OUTPUT_JP_HIRAGANA;
}

void hanjp_ic_select_keyboard(HanjpInputContext *hjic, const char* id)
{
  hangul_ic_select_keyboard(hjic->hic, id);
}

bool hanjp_ic_is_transliteration(HanjpInputContext *hjic)
{
  return hangul_ic_is_transliteration(hjic->hic);
}