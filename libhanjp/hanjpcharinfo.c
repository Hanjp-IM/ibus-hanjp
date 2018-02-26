#include "hanjp.h"

const ucschar hanjp_id_to_kana(int id, HanjpInputJpType type)
{
    switch(type)
    {
        case HANJP_INPUT_JP_HALF_KATAKANA:
            return kana_ordinate_table[id][HANJP_INPUT_JP_HALF_KATAKANA];
        case HANJP_INPUT_JP_KATAKANA:
            return kana_ordinate_table[id][HANJP_INPUT_JP_KATAKANA];
        case HANJP_INPUT_JP_HIRAGANA:
        default:
            return kana_ordinate_table[id][HANJP_INPUT_JP_HIRAGANA];
    }
}

int hanjp_kana_to_id(const ucschar c);
{
    int id;
    int num;
    enum {
        FLAG_NORMAL,
        FLAG_HALF
    } flag;

    if(c >= hiragana_base && c <= hiragana_end) //hiragana
    {
        num = c - hiragana_base;
        flag = FLAG_NORMAL;
    }
        
    else if(c >= katakana_base && c <= katakana_end) //katakana
    {
        num = c - katakana_base;
        flag = FLAG_NOMAL;
    }
    else if(c >= half_full_base && half_full_end) //half katakana
    {
        num = c - half_full_base;
        flag = FLAG_HALF;
    }
    else
        return -1;

    switch(flag)
    {
        case FLAG_NORMAL:
        switch(num)
        {
            case 2: //a, i, u, e, o
            case 4:
            case 6:
            case 8:
            case 10:
            id = HANJP_KANA_A + (num - 2) / 2;
            break;
            case 1: //small a, i, u, e, o
            case 3:
            case 5:
            case 7:
            case 9:
            id = HANJP_KANA_SMALL_A + (num - 1) / 2;
            break;
            case 11: //ka, ki, ku, ke, ko
            case 13:
            case 15:
            case 17:
            case 19:
            id = HANJP_KANA_KA + (num - 11) / 2;
            break;
            case 12: //ga, gi, gu, ge, go
            case 14:
            case 16:
            case 18:
            case 20:
            id = HANJP_KANA_GA + (num - 12) / 2:
            break;
            case 21: //sa, si, su, se, so
            case 23:
            case 25:
            case 27:
            case 29:
            id = HANJP_KANA_SA + (num - 21) / 2:
            break;
            case 22: //za, zi, zu, ze, zo
            case 24:
            case 26:
            case 28:
            case 30:
            id = HANJP_KANA_ZA + (num - 22) / 2:
            break;
            case 31: //ta, chi, tsu, te, to
            case 33:
            id = HANJP_KANA_TA + (num - 31) / 2;
            break;
            case 36:
            case 38:
            case 40:
            id = HANJP_KANA_TSU + (num - 36) / 2;
            break;
            case 35: //small tsu
            id = HANJP_KANA_SMALL_TSU;
            break;
            case 32: //da, ji, ju, de, do
            case 34:
            id = HANJP_KANA_DA + (num - 32) / 2;
            break;
            case 37:
            case 39:
            case 41;
            id = HANJP_KANA_JU + (num - 37) / 2;
            break;
            case 42: //na, ni, nu, ne, no
            case 43:
            case 44:
            case 45:
            case 46:
            id = HANJP_KANA_NA + (num - 42):
            break;
            case 47: //ha, hi, hu, he, ho
            case 50:
            case 53:
            case 56:
            case 59:
            id = HANJP_KANA_HA + (num - 47) / 3;
            break;
            case 48: //ba, bi, bu, be, bo
            case 51: 
            case 54:
            case 57:
            case 60:
            id = HANJP_KANA_BA + (num - 48) / 3;
            break;
            case 49: //pa, pi, pu, pe, po
            case 52: 
            case 55:
            case 58:
            case 61:
            id = HANJP_KANA_PA + (num - 49) / 3;
            break;
            case 62: //ma, mi, mu, me, mo
            case 63:
            case 64:
            case 65:
            case 66:
            id = HANJP_KANA_MA + (num - 62);
            break;
            case 68: //ya, yu, yo
            case 70:
            case 72:
            id = HANJP_KANA_YA + (num - 68) / 2;
            break;
            case 67: //small ya, yu, yo
            case 69:
            case 71:
            id = HANJP_KANA_SMALL_YA + (num - 67) / 2;
            break;
            case 73: //ra, ri, ru, re, ro
            case 74:
            case 75:
            case 76:
            case 77:
            id = HANJP_KANA_RI + (num - 73);            
            break;
            case 79: //wa, wo
            case 82:
            id = HANJP_KANA_WA + (num - 79) / 3;
            break;
            case 83: //nn
            id = HANJP_KANA_NN;            
            break;
        }
        break;
        case FLAG_HALF:
        switch(num)
        {
            case 103: //small a, i, u, e, o
            case 104:
            case 105:
            case 106:
            case 107:
            id = HANJP_KANA_SMALL_A + (num - 103);
            break;
            case 108: //small ya, yu, yo
            case 109:
            case 110:
            id = HANJP_KANA_SMALL_YA + (num - 108);
            break;
            case 111: //small tsu
            id = HANJP_KANA_SMALL_TSU;
            break;
            case 113: //a, i, u, e, o
            case 114:
            case 115:
            case 116:
            case 117:
            case 118: //ka, ki, ku, ke, ko
            case 119:
            case 120:
            case 121:
            case 122:
            id = HANJP_KANA_A + (num - 103);
            break;
            case 123: //sa, si, su, se, so
            case 124:
            case 125:
            case 126:
            case 127:
            id = HANJP_KANA_SA + (num - 123);
            break;
            case 128: //ta, chi, tsu, te, to
            case 129:
            case 130:
            case 131:
            case 132:
            id = HANJP_KANA_TA + (num - 128);
            break;
            case 133: //na, ni, nu, ne, no
            case 134:
            case 135:
            case 136:
            case 137:
            id = HANJP_KANA_NA + (num - 133);
            break;
            case 138: //ha, hi, hu, he, ho
            case 139:
            case 140:
            case 141:
            case 142:
            id = HANJP_KANA_HA + (num - 138);
            break;
            case 143: //ma, mi, mu, me, mo
            case 144:
            case 145:
            case 146:
            case 147:
            id = HANJP_KANA_MA + (num - 143);
            break;
            case 148: //ya, yu, yo
            case 149:
            case 150:
            id = HANJP_KANA_YA + (num - 148);
            break;
            case 151: //ra, ri, ru, re, ro
            case 152:
            case 153:
            case 154:
            case 155:
            id = HANJP_KANA_RA + (num - 151);
            break;
            case 156: //wa, wo
            id = HANJP_KANA_WA;
            break;
            case 102:
            id = HANJP_KANA_WO;
            break;
            case 157: //nn
            id = HANJP_KANA_NN;
            break;
            default:
            return -1;
        }
        break;
    }

    return id;
}

int hanjp_choseong_to_id(const ucschar c)
{
    int id, num;

    num = c - choseong_kiyeok;

    switch(num)
    {
        case 0: //ㄱ
        id = HANJP_CHOSEONG_G;
        break;
        case 1: //ㄲ
        case 15: //ㅋ
        id = HANJP_CHOSEONG_K;
        break;
        case 2: //ㄴ
        id = HANJP_CHOSEONG_N;
        break;
        case 3: //ㄷ
        id = HANJP_CHOSEONG_D;
        break;
        case 4: //ㄸ
        case 16: //ㅌ
        id = HANJP_CHOSEONG_T;
        break;
        case 5: //ㄹ
        id = HANJP_CHOSEONG_R;
        break;
        case 6: //ㅁ
        id = HANJP_CHOSEONG_M;
        break;
        case 7: //ㅂ
        id = HANJP_CHOSEONG_B;
        case 8: //ㅃ
        case 17: //ㅍ
        id = HANJP_CHOSEONG_P;
        break;
        case 9: //ㅅ
        case 10: //ㅆ
        id = HANJP_CHOSEONG_S;
        break;
        case 11: //ㅇ
        id = HANJP_CHOSEONG_VOID;
        break;
        case 12: //ㅈ
        id = HANJP_CHOSEONG_Z;
        break;
        case 13: //ㅉ
        case 14: //ㅊ
        id = HANJP_CHOSEONG_CH;
        break;
        case 18: //ㅎ
        id = HANJP_CHOSEONG_H;
        break;
        case 64: //반시옷
        id = HANJP_CHOSEONG_J;
        break;
        case 76: //옛이응
        id = HANJP_CHOSEONG_OLD_IEUNG;
        break;
        default:
        return -1;
    }

    return id;
}
//maybe need to fixed
int hanjp_jungseong_to_id(const ucschar c)
{
    int id, num;

    num = c - jungseong_a;

    switch(num)
    {
        case 0: //ㅏ
        id = HANJP_JUNGSEONG_A;
        break;
        case 1: //ㅐ
        case 5: //ㅔ
        id = HANJP_JUNGSEONG_E;
        break;
        case 2: //ㅑ
        id = HANJP_JUNGSEONG_YA;
        break;
        case 3: //ㅒ
        case 7: //ㅖ
        id = HANJP_JUNGSEONG_YE;
        break;
        case 8: //ㅗ
        id = HANJP_JUNGSEONG_O;
        break;
        case 9: //ㅘ
        id = HANJP_JUNGSEONG_OA;
        break;
        case 10: //ㅙ
        case 11: //ㅚ
        id = HANJP_JUNGSEONG_OE;
        break;
        case 12: //ㅛ
        id = HANJP_JUNGSEONG_YU;
        break;
        case 13: //ㅜ
        case 18: //ㅡ
        id = HANJP_JUNGSEONG_U;
        break;
        case 14: //ㅝ
        id = HANJP_JUNGSEONG_UO;
        break;
        case 15: //ㅞ
        id = HANJP_JUNGSEONG_UE;
        break;
        case 16: //ㅟ
        case 19: //ㅢ
        id = HANJP_JUNGSEONG_UI;
        break;
        case 17: //ㅠ
        id = HANJP_JUNGSEONG_YU;
        break;
        case 20: //ㅣ
        id = HANJP_JUNGSEONG_I;
        break;
        case 56:
        id = HANJP_JUNGSEONG_UA;
        break;
        default:
        return -1;
    }

    return id;
}

int hanjp_jongseong_to_id(const ucschar c)
{
    int id, num;

    num = c - jongseong_kiyeok;

    switch(num)
    {
        case 0: //ㄱ
        id = HANJP_JONGSEONG_KIYEOK;
        break;
        case 3: //ㄴ
        id = HANJP_JONGSEONG_NIEUN;
        break;
        case 15: //ㅁ
        id = HANJP_JONGSEONG_MIEUM;
        break;
        case 16: //ㅂ
        id = HANJP_JONGSEONG_PIEUP;
        break;
        case 18: //ㅅ
        id = HANJP_JONGSOENG_SIOS;
        break;
        case 19: //ㅆ
        id = HANJP_JONGSEONG_SSANGSIOS;
        break;
        case 20: //ㅇ
        id = HANJP_JONGSEONG_IEUNG;
        break;
        case 103: //ㄴㄴㄴ
        id = HANJP_JONGSEONG_SSANGNIEUN;
        default:
        return -1;
    }

    return id;
}

bool hanjp_final_conso_conjoinable(ucschar c, ucschar next_c)
{
    
}