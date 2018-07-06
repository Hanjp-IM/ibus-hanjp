#include "hanjp.h"
#include "hanjpeater.h"
#include <stdlib.h>

/*오타마타 조작 함수*/
static void hic_on_translate(HangulInputContext*, int, ucschar*, void*);
static bool hic_on_transition(HangulInputContext*, ucschar, const ucschar*, void*);
static const ucschar hangul_to_kana(ucschar pprev, ucschar prev, ucschar* hangul, ucschar next);

const ucschar kana_table[][] = {
    {0x3042, 0x3044, 0x3046, 0x3048, 0x304A}, // O
    {0x304B, 0x304D, 0x304F, 0x3051, 0x3053}, // K
    {0x3055, 0x3057, 0x3059, 0x305B, 0x305D}, // S
    {0x305F, 0x3061, 0x3063, 0x3066, 0x3068}, // T
    {0x306A, 0x306B, 0x306C, 0x306D, 0x306E}, // N
    {0x306F, 0x3072, 0x3075, 0x3078, 0x307B}, // H
    {0x307E, 0x307F, 0x3080, 0x3081, 0x3082}, // M
};
struct _HanjpEater{
    ucschar q[4];
    HangulInputContext* hic;
};

HanjpEater* eater_new(const char* keyboard)
{
    HanjpEater* eater;

    if(!keyboard) //키보드 기본 값을 2hj로 설정
        keyboard = "2hj";

    eater = malloc(sizeof(HanjpEater));
    eater->q[0] = 0;
    eater->q[1] = 0;
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

static const ucschar hangul_to_kana(ucschar pprev, ucschar prev, ucschar* hangul, ucschar next)
{
    //구현할 부분
    //ucschar key 2개로 kana 문자 맵핑
    // src[0] - 초성, src[1] - 중성

    int i=-1, j=-1;

    switch(hangul[0]){
        case HANJP_CHOSEONG_VOID: i=-1; break;
        case HANJP_CHOSEONG_IEUNG: i=0; break;
        case HANJP_CHOSEONG_K: i=1; break;
        case HANJP_CHOSEONG_S: i=2; break;
        case HANJP_CHOSEONG_T: i=3; break;
        case HANJP_CHOSEONG_N: i=4; break;
        case HANJP_CHOSEONG_H: i=5; break;
        case HANJP_CHOSEONG_M: i=6; break;
        case HANJP_CHOSEONG_R: i=8; break;
        case HANJP_CHOSEONG_OLD_IEUNG: i=9; break;
        default: i=-1; break
    }

    switch(hangul[1]){
        case HANJP_JUNGSEONG_A: j=0; break;
        case HANJP_JUNGSEONG_I: j=1; break;
        case HANJP_JUNGSEONG_U: j=2; break;
        case HANJP_JUNGSEONG_E: j=3; break;
        case HANJP_JUNGSEONG_O: j=4; break;
        default: j=-1; break;
    }

}

void eater_flush(HanjpEater* eater)
{
    eater->q[0] = 0;
    eater->q[1] = 0;
    eater->q[2] = 0;
    eater->q[3] = 0;
    hangul_ic_flush(eater->hic);
}

int eater_push(HanjpEater* eater, ucschar ch, ucschar* outer, int outer_length)
{
    int res;
    int flag = 0;
    const ucschar* hic_commit;
    const ucschar* hic_preedit;
    const ucschar* hic_flushed;
    ucschar* q = eater->q;

    if(0 <= ch || ch >= 127){ //ascii
        res = hangul_ic_process(eater->hic, ch); //hic에 자소 푸쉬

        if(!res){ //처리가 안됐으면 다시 넣음
            hangul_ic_process(eater->hic, ch);
        }

        hic_commit = hangul_ic_get_commit_string(eater->hic);
        hic_preedit = hangul_ic_get_preedit_string(eater->hic);
        q[3] = hic_preedit[0];
        
        if(hic_commit[0] != 0){ //hic commit이 일어남
            outer[outer_length] = q[0];
            q[0] = q[1];
            q[1] = q[2];
            //hic commit string은 다음 ic_process까지만 유지됨//
            q[2] = hangul_to_kana(q[0], q[1], hic_commit, q[3]); 
            flag = flag || EATFLG_P;
            if(q[2] == 0x302f) //방점이면
                flag = flag || EATFLG_Q;
        }
    }
    else { //non ascii
        if(q[3] != 0){
            hic_flushed = hangul_ic_flush(eater->hic);

            outer[outer_length] = q[0];
            q[0] = q[1];
            q[1] = q[2];
            q[2] = hangul_to_kana(q[0], q[1], hic_flushed, q[3]); 
            flag = flag || EATFLG_P;
            if(q[2] == 0x302f) //방점이면
                flag = flag || EATFLG_Q;
        }
    }

    return flag;
}

bool eater_backspace(HanjpEater* eater)
{
    int ret;
    const ucschar* preedit;

    //오류시 false
    if(!eater) {
        return false;
    }

    ret = hangul_ic_backspace(eater->hic); //hic bakcspace

    if(!ret) {
        return false;
    }

    preedit = hangul_ic_get_preedit_string(eater->hic);
    eater->q[3] = preedit[0];

    if(hangul_ic_is_empty(eater->hic)) { //한 칸씩 밈
       eater->q[2] = eater->q[1];
       eater->q[1] = eater->q[0];
       eater->q[0] = 0;
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
    if(!eater){
        return NULL;
    }

    return hangul_ic_is_empty(eater->hic);
}