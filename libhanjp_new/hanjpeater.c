#include "hanjp.h"
#include "hanjpeater.h"
#include <stdlib.h>

#define RSRV_MAX 4 //reserved string MAX

/*오타마타 조작 함수*/
static void hic_on_translate(HangulInputContext*, int, ucschar*, void*);
static bool hic_on_transition(HangulInputContext*, ucschar, const ucschar*, void*);
static void hangul_to_kana(ucschar* dest, ucschar* rsrv, ucschar* hangul, ucschar next, HanjpOutputType type);

struct _HanjpEater{
    ucschar rsrv[RSRV_MAX]; //reserved string
    int rsrv_length;
    HangulInputContext* hic;
};

HanjpEater* eater_new(const char* keyboard)
{
    HanjpEater* eater;

    if(!keyboard) //키보드 기본 값을 2hj로 설정
        keyboard = "2hj";

    eater = malloc(sizeof(HanjpEater));
    eater->rsrv[0] = 0;
    eater->rsrv[1] = 0;
    eater->rsrv[2] = 0;
    eater->rsrv[3] = 0;
    eater->rsrv_length = 0;
    eater->hic = hangul_ic_new(keyboard);
    /*오토마타 조작*/
    hangul_ic_connect_callback(eater->hic, "translaste", hic_on_translate, NULL);
    hangul_ic_connect_callback(eater->hic, "transition", hic_on_transition, NULL);
    hangul_ic_set_output_mode(eater->hic, HANGUL_OUTPUT_JAMO);
}

void eater_delete(HanjpEater* eater)
{
    if(!eater){
        return;
    }

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

static bool hangul_to_kana(ucschar* dest, ucschar* rsrv, ucschar* hangul, ucschar next, HanjpOutputType type)
{
    //구현할 부분
    //ucschar key 2개로 kana 문자 맵핑
}

void eater_flush(HanjpEater* eater)
{
    eater->rsrv[0] = 0;
    eater->rsrv[1] = 0;
    eater->rsrv[2] = 0;
    eater->rsrv[3] = 0;
    hangul_ic_flush(eater->hic);
}

int eater_push(HanjpEater* eater, int ascii, ucschar* outer, int outer_length, HanjpOutputType type)
{
    bool res;
    int commit_len = 0;
    const ucschar* hic_commit = NULL;
    const ucschar* hic_preedit = NULL;
    int i;
    int temp;
    ucschar* rsrv;

    if(!eater){
        return -1;
    }

    res = hangul_ic_process(eater->hic, ascii); //hic에 자소 푸쉬

    if(!res){ //처리가 안됐으면 다시 넣음
        hangul_ic_process(eater->hic, ascii);
    }

    hic_commit = hangul_ic_get_commit_string(eater->hic);
    hic_preedit = hangul_ic_get_preedit_string(eater->hic);

    for(i=0; hic_commit[i] != 0; i++){ //count hic commit length
        commit_len++;
    }

    if(/*Kana preedit is done*/){
        return -2;
    }

    if(eater->rsrv_length + commit_len > RSRV_MAX){
        temp = eater->rsrv_length + commit_len - RSRV_MAX;
        for(i=0; i < RSRV_MAX - temp; i++){
            eater->rsrv[i] = eater->rsrv[i+temp];
        }
        eater->rsrv_length -= temp;
    }

    for(i = 0; i < commit_len; i++){
        eater->rsrv[eater->rsrv_length + i] = hic_commit[i];
    }

    eater->rsrv_legnth += commit_len;

    if(commit_len != 0) {
        hangul_to_kana(outer+outer_length, eater->rsrv, hic_commit, hic_preedit[0]);
    }

    return commit_len;
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

    eater->rsrv[--eater->rsrv_length] = 0;

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