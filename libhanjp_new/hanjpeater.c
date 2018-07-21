#include "hanjp.h"
#include "hanjpeater.h"
#include <stdlib.h>

/*오타마타 조작 함수*/
static void hic_on_translate(HangulInputContext*, int, ucschar*, void*);
static bool hic_on_transition(HangulInputContext*, ucschar, const ucschar*, void*);
static int hangul_to_kana(ucschar* dest, int state, ucschar* hangul, ucschar next, HanjpOutputType type);

enum {
    STATE_VOID,
    STATE_CHOSEONG,
    STATE_JUNGSEONG,
    STATE_JONGSEONG
};

struct _HanjpEater{
    HangulInputContext* hic;
    int state;
};

HanjpEater* eater_new(const char* keyboard)
{
    HanjpEater* eater;

    if(!keyboard) //키보드 기본 값을 2hj로 설정
        keyboard = "2hj";

    eater = malloc(sizeof(HanjpEater));
    eater->hic = hangul_ic_new(keyboard);
    /*오토마타 조작*/
    hangul_ic_connect_callback(eater->hic, "translaste", hic_on_translate, NULL);
    hangul_ic_connect_callback(eater->hic, "transition", hic_on_transition, NULL);
    hangul_ic_set_output_mode(eater->hic, HANGUL_OUTPUT_JAMO);
}

void eater_delete(HanjpEater* eater)
{
    hangul_ic_delete(eater->hic);
    free(eater);
}

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

static int hangul_to_kana(ucschar* dest, int state, ucschar* hangul, ucschar next, HanjpOutputType type)
{
    //구현할 부분
    //ucschar key 2개로 kana 문자 맵핑
}

void eater_flush(HanjpEater* eater)
{
    eater->state = STATE_VOID;
    hangul_ic_flush(eater->hic);
}

int eater_push(HanjpEater* eater, int ascii, ucschar* outer, int outer_length, HanjpOutputType type)
{
    bool res;
    const ucschar* hic_commit = NULL;
    const ucschar* hic_preedit = NULL;

    if(!eater || !outer){
        return -1;
    }

    res = hangul_ic_process(eater->hic, ascii); //hic에 자소 푸쉬

    if(!res){ //처리가 안됐으면 다시 넣음
        hangul_ic_process(eater->hic, ascii);
    }

    hic_commit = hangul_ic_get_commit_string(eater->hic);
    hic_preedit = hangul_ic_get_preedit_string(eater->hic);

    return hangul_to_kana(outer + outer_length, eater->state, hic_commit, hic_preedit[0], type);
}

bool eater_backspace(HanjpEater* eater)
{
    int ret;

    if(!eater) {
        return false;
    }

    ret = hangul_ic_backspace(eater->hic); //hic bakcspace

    if(!ret) {
        return false;
    }

    //Switch hangul state
    switch(eater->state){
        case STATE_VOID:
        break;
        case STATE_CHOSEONG:
        eater->state = STATE_VOID;
        break;
        case STATE_JUNGSEONG:
        if(hangul_ic_has_jungseong(eater->hic)){
            eater->state = STATE_JUNGSEONG;
        }
        else{
            eater->state = STATE_CHOSEONG;
        }
        break;
        case STATE_JONGSEONG:
        eater->state = STATE_JUNGSEONG;
        break;
        default:
        return false;
    }

    return true;
}

const ucschar* eater_get_preedit(HanjpEater* eater){
    if(!eater){
        return NULL;
    }

    return hangul_ic_get_preedit_string(eater->hic);
}

bool eater_is_empty(HanjpEater* eater)
{
    return hangul_ic_is_empty(eater->hic);
}