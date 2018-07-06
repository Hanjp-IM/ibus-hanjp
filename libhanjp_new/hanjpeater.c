#include "hanjp.h"
#include "hanjpeater.h"
#include <stdlib.h>

/*오타마타 조작 함수*/
static void hic_on_translate(HangulInputContext*, int, ucschar*, void*);
static bool hic_on_transition(HangulInputContext*, ucschar, const ucschar*, void*);
static const ucschar hangul_to_kana(ucschar* src);

const ucschar kana_table[][] = {
    {0x3042, 0x3044, 0x3046, 0x3048, 0x304A}, // O
    {0x304B, 0x304D, 0x304F, 0x3051, 0x3053}, // K
    {0x3055, 0x3057, 0x3059, 0x305B, 0x305D}, // S
    {0x305F, 0x3061, 0x3063, 0x3066, 0x3068},
    {}
};
struct _HanjpEater{
    ucschar q[2];
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
    //구현할 부분
    //hangul buffer에 뭐가 들어있는지 볼 수 있다.
    //초성이 'ㅇ'이 아닌 경우,
    //받침이 입력된 경우 false
    return true;
}

static const ucschar hangul_to_kana(ucschar* src)
{
    //구현할 부분
    //ucschar key 2개로 kana 문자 맵핑
    // src[0] - 초성, src[1] - 중성

    int i=-1, j=-1;

    switch(src[0]){
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

    switch(src[1]){
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
    hangul_ic_flush(eater->hic);
}

int eater_push(HanjpEater* eater, ucschar ch, ucschar* outer, int outer_length)
{
    int res;
    int flag = 0;

    if(0 <= ch || ch >= 127){ //ascii
        res = hangul_ic_process(eater->hic, ch); //hic에 자소 푸쉬

        //구현할 부분
        //res==false인 경우 처리
        //hic commit이 있는 경우 처리
    }
    else { //non ascii

    }

    return flag;
}

bool eater_backspace(HanjpEater* eater)
{
    int ret;

    //오류시 false
    if(!eater) {
        return false;
    }

    ret = hangul_ic_backspace(eater->hic);

    if(!ret) {
        return false;
    }

    if(hangul_ic_is_empty(eater->hic)) {
       eater_flush(eater); //한글 조합 중이 아니면 eater 전체를 비움 
    }

    return true;
}

const ucschar* eater_get_preedit(HanjpEater* eater){
    return eater->q; //queue 포인터 반환
}