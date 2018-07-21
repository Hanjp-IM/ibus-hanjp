#include "hanjp.h"
#include "hanjpeater.h"
#include <stdlib.h>

/*오타마타 조작 함수*/
static void hic_on_translate(HangulInputContext*, int, ucschar*, void*);
static bool hic_on_transition(HangulInputContext*, ucschar, const ucschar*, void*);
static const ucschar hangul_to_kana(ucschar* dest, int state, ucschar* hangul, ucschar next);

const ucschar kana_table[][] = {
    // {*A, *I, *U, *E, *O}
    {0x3042, 0x3044, 0x3046, 0x3048, 0x304A}, // O
    {0x304B, 0x304D, 0x304F, 0x3051, 0x3053}, // K
    {0x304C, 0x304E, 0x3050, 0x3052, 0x3054}, // G // K -> G
    {0x3055, 0x3057, 0x3059, 0x305B, 0x305D}, // S
    {0x3056, 0x3058, 0x305A, 0x305C, 0x305E}, // Z // S -> Z
    {0x305F, 0x3061, 0x3064, 0x3066, 0x3068}, // T
    {0x3060, 0x3062, 0x3065, 0x3067, 0x3069}, // D // T -> D
    {0x306A, 0x306B, 0x306C, 0x306D, 0x306E}, // N
    {0x306F, 0x3072, 0x3075, 0x3078, 0x307B}, // H
    {0x3070, 0x3073, 0x3076, 0x3079, 0x307C}, // B // H -> B
    {0x3071, 0x3074, 0x3077, 0x307A, 0x307D}, // P // H -> P
    {0x307E, 0x307F, 0x3080, 0x3081, 0x3082}, // M
    {0x3084, 0, 0x3086, 0, 0x3088}, // Y
    {0x3089, 0x308A, 0x308B, 0x308C, 0x308D}, // R
    {0x308F, 0, 0, 0, 0x3092}, // W
    {0x3093} // NN
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

static const void hangul_to_kana(ucschar* dest, int state, ucschar* hangul, ucschar next)
{
    //구현할 부분
    //ucschar key 2개로 kana 문자 맵핑
    // src[0] - 초성, src[1] - 중성

    int i=-1, j=-1, is_choseong_void=0, is_jungseong_void=0, adjust=0;
    int has_contracted_sound=0;

    switch(hangul[0]){
        case HANGUL_CHOSEONG_FILLER: i=0; is_choseong_void=1; break;
        case HANJP_CHOSEONG_IEUNG: i=0; break; // ㅇ
        case HANJP_CHOSEONG_KHIEUKH: i=1; break; // ㅋ
        case HANJP_CHOSEONG_KIEYEOK: i=2; break // ㄱ // ㅋ -> ㄱ 탁음
        case HANJP_CHOSEONG_SIOS: i=3; break; // ㅅ
        case HANJP_CHOSEONG_CIEUC: i=4; break; // ㅈ // ㅅ -> ㅈ 탁음
        case HANJP_CHOSEONG_THIEUTH: i=5; break; // ㅌ
        case HANJP_CHOSEONG_TIKEUT: i=6; break // ㄷ // ㅌ -> ㄷ 탁음
        case HANJP_CHOSEONG_PANSIOS:
            i = (hangul[1]==HANJP_JUNGSEONG_I || 
                hangul[1]==HANJP_JUNGSEONG_EU ||
                hangul[1]==HANJP_JUNGSEONG_U)? 6 : 4;
        case HANJP_CHOSEONG_NIEUN: i=7; break; // ㄴ
        case HANJP_CHOSEONG_HIEUH: i=8; break; // ㅎ
        case HANJP_CHOSEONG_PIEUP: i=9; break; // ㅂ // ㅎ -> ㅂ 탁음
        case HANJP_CHOSEONG_PHIEUPH: i=10; break; // ㅍ // ㅎ -> ㅍ 반탁음
        case HANJP_CHOSEONG_MIEUM: i=11; break; // ㅁ
        case HANJP_CHOSEONG_RIEUL: i=13; break; // ㄹ
        case HANJP_CHOSEONG_OLD_IEUNG: // OLD ㅇ
            i = (hangul[1]==HANJP_JUNGSEONG_O)? 12 : 0;
            break; 
        default: return -1;
    }

    switch(hangul[1]){
        case HANGUL_JUNGSEONG_FILLER: j=2; is_jungseong_void=1; break;
        case HANJP_JUNGSEONG_A: j=0; break; //ㅏ
        case HANJP_JUNGSEONG_I: j=1; break; // ㅣ
        case HANJP_JUNGSEONG_EU: // ㅡ
        case HANJP_JUNGSEONG_U: j=2; break; // ㅜ
        case HANJP_JUNGSEONG_AE: // ㅐ
        case HANJP_JUNGSEONG_E: j=3; break; // ㅔ
        case HANJP_JUNGSEONG_O: j=4; break; // ㅗ
         case HANJP_JUNGSEONG_YA: 
            i=(i==0 || is_choseong_void)?12:i; j=0;  // 야
            has_contracted_sound = i>0? 1 : 0; break;
        case HANJP_JUNGSEONG_YU: 
            i=(i==0 || is_choseong_void)?12:i; j=2;  // 유
            has_contracted_sound = i>0? 1 : 0; break;
        case HANJP_JUNGSEONG_YO: 
            i=(i==0 || is_choseong_void)?12:i; j=4;  // 요
            has_contracted_sound = i>0? 1 : 0; break;
        case HANJP_JUNGSEONG_WA: i=(i==0 || is_choseong_void)?9:i; j=0; break; // 와
        default: return -1;마
    }

    if(is_choseong_void && is_jungseong_void) return -1;

    adjust = is_choseong_void? -1 : 0;
    dest[0] = kana_table[i][j] + adjust;

    if(has_contracted_sound){
        dest[0] = kana_table[i][1];
        dest[1] = kana_table[12][j]-1;
        return 2;
    }else{
        return 1;
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