#include <hanjpchar.h>

static const ucschar kana_table[][5] = {
    // {*A, *I, *U, *E, *O}
    {0x3042, 0x3044, 0x3046, 0x3048, 0x304A}, // O(0)
    {0x304B, 0x304D, 0x304F, 0x3051, 0x3053}, // K(1)
    {0x3055, 0x3057, 0x3059, 0x305B, 0x305D}, // S(2)
    {0x305F, 0x3061, 0x3064, 0x3066, 0x3068}, // T(3)
    {0x306A, 0x306B, 0x306C, 0x306D, 0x306E}, // N(4)
    {0x306F, 0x3072, 0x3075, 0x3078, 0x307B}, // H(5)
    {0x307E, 0x307F, 0x3080, 0x3081, 0x3082}, // M(6)
    {0x3084, 0, 0x3086, 0, 0x3088}, // Y(7)
    {0x3089, 0x308A, 0x308B, 0x308C, 0x308D}, // R(8)
    {0x308F, 0, 0, 0, 0x3092}, // W(9)
    {0x3093, 0, 0, 0, 0} // NN(10)
};

static bool hangul_is_batchim_comport(ucschar ch, ucschar next);
static bool hangul_is_choseong_voiced(ucschar ch);
static bool hangul_is_choseong_p(ucschar ch);
static bool hangul_is_vowel_contracted(ucschar ch);

int hangul_to_kana(ucschar* dest, ucschar prev, ucschar* hangul, ucschar next, HanjpOutputType type)
{
    //구현할 부분
    //ucschar key 2개로 kana 문자 맵핑
    // hangul[0] - 초성, hangul[1] - 중성
    // 기본음 + 보조음

    int i=0, j=0; // 초성, 중성 인덱스
    int is_choseong_void=0, is_jungseong_void=0, adjust=0;
    int has_voiced_sound = 0, has_p_sound = 0, has_contracted_sound=0;
    ucschar support = 0;
    int dest_len = 1;

    has_voiced_sound = hangul_is_choseong_voiced(hangul[0]);
    has_p_sound = hangul_is_choseong_p(hangul[0]);
    has_contracted_sound = hangul_is_vowel_contracted(hangul[1]);

    switch(hangul[0]){
        case HANGUL_CHOSEONG_FILLER: i=0; is_choseong_void=1; break;
        case HANJP_CHOSEONG_IEUNG: // ㅇ
            i=0; break;
        case HANJP_CHOSEONG_KHIEUKH: // ㅋ
        case HANJP_CHOSEONG_SSANGKIYEOK: //ㄲ
        case HANJP_CHOSEONG_KIYEOK: // ㄱ // ㅋ -> ㄱ 탁음
            i=1; break;
        case HANJP_CHOSEONG_SIOS: // ㅅ
        case HANJP_CHOSEONG_SSANGSIOS: //ㅆ
        case HANJP_CHOSEONG_CIEUC: // ㅈ // ㅅ -> ㅈ 탁음
            i=2; break;
        case HANJP_CHOSEONG_THIEUTH: // ㅌ
        case HANJP_CHOSEONG_SSANGTIKEUT: //ㄸ
        case HANJP_CHOSEONG_TIKEUT: // ㄷ // ㅌ -> ㄷ 탁음
            i=3; break;
        case HANJP_CHOSEONG_NIEUN: // ㄴ 
            i=4; break; 
        case HANJP_CHOSEONG_HIEUH: // ㅎ
        case HANJP_CHOSEONG_PIEUP: // ㅂ // ㅎ -> ㅂ 탁음
        case HANJP_CHOSEONG_PHIEUPH: // ㅍ // ㅎ -> ㅍ 반탁음
        case HANJP_CHOSEONG_SSANGPIEUP: // ㅃ // 반탁음
            i=5; break; 
        case HANJP_CHOSEONG_MIEUM: // ㅁ
            i=6; break;
        case HANJP_CHOSEONG_RIEUL: // ㄹ
            i=8; break;
        case HANJP_CHOSEONG_PANSIOS: // ㅿ // ㅊ -> ㅿ 탁음
            switch(hangul[1]){
                case HANJP_JUNGSEONG_I:
                case HANJP_JUNGSEONG_U:
                i=3; break;
                default:
                i=2; break;
            }
        case HANJP_CHOSEONG_OLD_IEUNG: // OLD ㅇ
            i = (hangul[1]==HANJP_JUNGSEONG_O)? 12 : 0;
            break;
        default: return -1;
    }

    switch(hangul[1]){
        case HANGUL_JUNGSEONG_FILLER: j=2; is_jungseong_void=1; break;
        case HANJP_JUNGSEONG_A: //ㅏ
        case HANJP_JUNGSEONG_YA: // 야
            j=0; break; 
        case HANJP_JUNGSEONG_I: // ㅣ
            j=1; break; 
        case HANJP_JUNGSEONG_EU: // ㅡ
        case HANJP_JUNGSEONG_U: // ㅜ
        case HANJP_JUNGSEONG_YU: // 유
            j=2; break; 
        case HANJP_JUNGSEONG_AE: // ㅐ
        case HANJP_JUNGSEONG_E: // ㅔ
            j=3; break; 
        case HANJP_JUNGSEONG_O: // ㅗ
        case HANJP_JUNGSEONG_EO:
        case HANJP_JUNGSEONG_YO: // 요
            j=4; break;
        case HANJP_JUNGSEONG_YE:
        case HANJP_JUNGSEONG_YAE:
            j=1; support=kana_table[0][3]; break;
        case HANJP_JUNGSEONG_WA: // 와
            if(hangul[1] == HANJP_CHOSEONG_IEUNG){
                j=0;
                support = 0;
            }
            else{
                j=4;
                support = kana_table[0][0] - 1;
            }
        default: return -1;
    }

    if(is_choseong_void && is_jungseong_void) {
        dest[0] = 0;
        return 0;
    }

    if(!is_choseong_void && is_jungseong_void && hangul_is_jungseong(prev)){
        if(hangul_is_batchim_comport(hangul[0], next)){
            switch(hangul[0]){
                case HANJP_CHOSEONG_SSANGNIEUN:
                case HANJP_CHOSEONG_NIEUN:
                case HANJP_CHOSEONG_MIEUM:
                dest[0] = kana_table[10][0];
                return 1;
                case HANJP_CHOSEONG_KIYEOK:
                case HANJP_CHOSEONG_SIOS:
                case HANJP_CHOSEONG_SSANGSIOS:
                case HANJP_CHOSEONG_PIEUP:
                dest[0] = kana_table[2][3] - 1;
                return 1;
            }
        }
    }

    if(has_voiced_sound){
        adjust = 1;
    }
    else if(has_p_sound){
        adjust = 2;
    }
    else if(is_choseong_void){
        adjust = -1;
    }
    else{
        adjust = 0;
    }

    if(has_contracted_sound) {
        if(i == 0){
            i = 7;
        }
        else {
            support = kana_table[7][j] - 1;
            j = 1;
        }
    }

    switch(hangul[0]){
        case HANJP_CHOSEONG_CHIEUCH:
        case HANJP_CHOSEONG_PANSIOS:
        switch(hangul[1]){
            case HANJP_JUNGSEONG_A:
            case HANJP_JUNGSEONG_E:
            case HANJP_JUNGSEONG_O:
            j = 1; // ㅣ
            support = kana_table[0][j] - 1;
            break;
        }
        break;
    }

    dest[0] = kana_table[i][j] + adjust;
    
    if(support != 0){
        dest[1] = support;
        dest_len++;
    }
    dest[dest_len] = 0;

    return dest_len;
}

static bool hangul_is_batchim_comport(ucschar ch, ucschar next)
{
    bool res;

    switch(ch){
        case HANJP_CHOSEONG_IEUNG:
        switch(next){
            case HANJP_CHOSEONG_KHIEUKH:
            case HANJP_CHOSEONG_SSANGKIYEOK:
            case HANJP_CHOSEONG_KIYEOK:
            res = true;
            default:
            res = false;
        } break;
        case HANJP_CHOSEONG_KIYEOK:
        switch(next){
            case HANJP_CHOSEONG_KHIEUKH:
            case HANJP_CHOSEONG_SSANGKIYEOK:
            res = true; break;
            default:
            res = false;
        } break;
        case HANJP_CHOSEONG_SIOS:
        switch(next){
            case HANJP_CHOSEONG_SIOS:
            case HANJP_CHOSEONG_SSANGSIOS:
            res = true; break;
            default:
            res = false;
        } break;
        case HANJP_CHOSEONG_NIEUN:
        switch(next){
            case HANJP_CHOSEONG_SIOS:
            case HANJP_CHOSEONG_SSANGSIOS:
            case HANJP_CHOSEONG_THIEUTH:
            case HANJP_CHOSEONG_SSANGTIKEUT:
            case HANJP_CHOSEONG_TIKEUT:
            case HANJP_CHOSEONG_NIEUN:
            case HANJP_CHOSEONG_RIEUL:
            res = true; break;
            default:
            res = false;
        } break;
        case HANJP_CHOSEONG_PIEUP:
        switch(next){
            case HANJP_CHOSEONG_PHIEUPH:
            case HANJP_CHOSEONG_SSANGPIEUP:
            res = true; break;
            default:
            res = false;
        } break;
        case HANJP_CHOSEONG_MIEUM:
        switch(next){
            case HANJP_CHOSEONG_MIEUM:
            case HANJP_CHOSEONG_PIEUP:
            case HANJP_CHOSEONG_PHIEUPH:
            case HANJP_CHOSEONG_SSANGPIEUP:
            res = true; break;
            default:
            res = false;
        } break;
        case HANJP_CHOSEONG_SSANGSIOS: 
        case HANJP_CHOSEONG_SSANGNIEUN:
        res = true; break;
        default: res = false;
    }

    return res;
}

static bool hangul_is_choseong_voiced(ucschar ch)
{
    switch(ch){
        case HANJP_CHOSEONG_KIYEOK:
        case HANJP_CHOSEONG_CIEUC:
        case HANJP_CHOSEONG_TIKEUT:
        case HANJP_CHOSEONG_PANSIOS:
        case HANJP_CHOSEONG_PIEUP:
        return true;
        default:
        return false;
    }
}

static bool hangul_is_choseong_p(ucschar ch){
    switch(ch){
        case HANJP_CHOSEONG_PHIEUPH:
        case HANJP_CHOSEONG_SSANGPIEUP:
        return true;
        default:
        return false;
    }
}

static bool hangul_is_vowel_contracted(ucschar ch){
    switch(ch){
        case HANJP_JUNGSEONG_YA:
        case HANJP_JUNGSEONG_YU:
        case HANJP_JUNGSEONG_YO:
        return true;
        default:
        return false;
    }
}