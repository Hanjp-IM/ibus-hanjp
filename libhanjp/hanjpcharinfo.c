#include "hanjp.h"

static const ucschar hiragana_base = 0x3040;
static const ucschar hiragana_end = 0x309F;
static const ucschar katakana_base = 0x30A0;
static const ucschar katakana_end = 0x30FF;
static const ucschar half_full_base = 0xFF00;
static const ucschar half_full_end = 0xFFEF;
static const ucschar choseong_kiyeok = 0x1100;
static const ucschar jungseong_a = 0x1161;
static const ucschar jongseong_kiyeok = 0x11a8;
static const ucschar hanjp_tone_mark = 0x302F;

bool hanjp_is_voiced_by_id(int id)
{
    switch(id)
    {
        case HANJP_KANA_GA:
        case HANJP_KANA_GI:
        case HANJP_KANA_GU:
        case HANJP_KANA_GE:
        case HANJP_KANA_GO:
        case HANJP_KANA_ZA:
        case HANJP_KANA_ZI:
        case HANJP_KANA_ZU:
        case HANJP_KANA_ZE:
        case HANJP_KANA_ZO:
        case HANJP_KANA_DA:
        case HANJP_KANA_JI:
        case HANJP_KANA_JU:
        case HANJP_KANA_DE:
        case HANJP_KANA_DO:
        case HANJP_KANA_BA:
        case HANJP_KANA_BI:
        case HANJP_KANA_BU:
        case HANJP_KANA_BE:
        case HANJP_KANA_BO:
        return true;
        default:
        return false;
    }
}

bool hanjp_is_semi_voiced_by_id(int id)
{
    switch(id)
    {
        case HANJP_KANA_PA:
        case HANJP_KANA_PI:
        case HANJP_KANA_PU:
        case HANJP_KANA_PE:
        case HANJP_KANA_PO:
        return true;
        default:
        return false;
    }
}

const ucschar kana_ordinate_table[80][3] = {
    {0x3042, 0x30A2, 0xFF71}, //A
    {0x3044, 0x30A4, 0xFF72}, //I
    {0x3046, 0x30A6, 0xFF73}, //U
    {0x3048, 0x30A8, 0xFF74}, //E
    {0x304A, 0x30AA, 0xFF75}, //O
    {0x304B, 0x30AB, 0xFF76}, //KA
    {0x304D, 0x30AD, 0xFF77}, //KI
    {0x304F, 0x30AF, 0xFF78}, //KU
    {0x3051, 0x30B1, 0xFF79}, //KE
    {0x3053, 0x30B3, 0xFF7A}, //KO
    {0x304C, 0x30AC, 0xFF76}, //GA
    {0x304E, 0x30AE, 0xFF77}, //GI
    {0x3050, 0x30B0, 0xFF78}, //GU
    {0x3052, 0x30B2, 0xFF79}, //GE
    {0x3054, 0x30B4, 0xFF7A}, //GO
    {0x3055, 0x30B5, 0xFF7B}, //SA
    {0x3057, 0x30B7, 0xFF7C}, //SI
    {0x3059, 0x30B9, 0xFF7D}, //SU
    {0x305B, 0x30BB, 0xFF7E}, //SE
    {0x305D, 0x30BD, 0xFF7F}, //SO
    {0x3056, 0x30B6, 0xFF7B}, //ZA
    {0x3058, 0x30B8, 0xFF7C}, //ZI
    {0x305A, 0x30BA, 0xFF7D}, //ZU
    {0x305C, 0x30BC, 0xFF7E}, //ZE
    {0x305E, 0x30BE, 0xFF7F}, //ZO
    {0x305F, 0x30BF, 0xFF80}, //TA
    {0x3061, 0x30C1, 0xFF81}, //CHI
    {0x3064, 0x30C4, 0xFF82}, //TSU
    {0x3066, 0x30C6, 0xFF83}, //TE
    {0x3068, 0x30C8, 0xFF84}, //TO
    {0x305F, 0x30BF, 0xFF80}, //DA
    {0x3062, 0x30C2, 0xFF81}, //JI
    {0x3065, 0x30C5, 0xFF82}, //JU
    {0x3067, 0x30C7, 0xFF83}, //DE
    {0x3069, 0x30C9, 0xFF84}, //DO
    {0x306A, 0x30CA, 0xFF85}, //NA
    {0x306B, 0x30CB, 0xFF86}, //NI
    {0x306C, 0x30CC, 0xFF87}, //NU
    {0x306D, 0x30CD, 0xFF88}, //NE
    {0x306E, 0x30CE, 0xFF89}, //NO
    {0x306F, 0x30CF, 0xFF8A}, //HA
    {0x3072, 0x30D2, 0xFF8B}, //HI
    {0x3075, 0x30D5, 0xFF8C}, //HU
    {0x3078, 0x30D8, 0xFF8D}, //HE
    {0x307B, 0x30DB, 0xFF8E}, //HO
    {0x3070, 0x30D0, 0xFF8A}, //BA
    {0x3073, 0x30D3, 0xFF8B}, //BI
    {0x3076, 0x30D6, 0xFF8C}, //BU
    {0x3079, 0x30D9, 0xFF8D}, //BE
    {0x307C, 0x30DC, 0xFF8E}, //BO
    {0x3071, 0x30D1, 0xFF8A}, //PA
    {0x3074, 0x30D4, 0xFF8B}, //PI
    {0x3077, 0x30D7, 0xFF8C}, //PU
    {0x307A, 0x30DA, 0xFF8D}, //PE
    {0x307D, 0x30DD, 0xFF8E}, //PO
    {0x307E, 0x30DE, 0xFF8F}, //MA
    {0x307F, 0x30DF, 0xFF90}, //MI
    {0x3080, 0x30E0, 0xFF91}, //MU
    {0x3081, 0x30E1, 0xFF92}, //ME
    {0x3082, 0x30E2, 0xFF93}, //MO
    {0x3089, 0x30E9, 0xFF97}, //RA
    {0x308A, 0x30EA, 0xFF98}, //RI
    {0x308B, 0x30EB, 0xFF99}, //RU
    {0x308C, 0x30EC, 0xFF9F}, //RE
    {0x308D, 0x30ED, 0xFFA0}, //RO
    {0x3084, 0x30E4, 0xFF94}, //YA
    {0x3086, 0x30E6, 0xFF95}, //YU
    {0x3088, 0x30E8, 0xFF96}, //YO
    {0x308F, 0x30EF, 0xFF9C}, //WA
    {0x3092, 0x30F2, 0xFF66}, //WO
    {0x3093, 0x30F3, 0xFF9D}, //NN
    {0x3041, 0x30A1, 0xFF67}, //s A
    {0x3043, 0x30A3, 0xFF68}, //s I
    {0x3045, 0x30A5, 0xFF69}, //s U
    {0x3047, 0x30A7, 0xFF6A}, //s E
    {0x3049, 0x30A9, 0xFF6B}, //s O
    {0x3083, 0x30E3, 0xFF6C}, //s YA
    {0x3085, 0x30E5, 0xFF6D}, //s YU
    {0x3087, 0x30E7, 0xFF6E}, //s YO
    {0x3063, 0x30C3, 0xFF6F}, //s TSU
};

enum _HanjpKanaId{
    HANJP_KANA_A, //A
    HANJP_KANA_I, //I
    HANJP_KANA_U, //U
    HANJP_KANA_E, //E
    HANJP_KANA_O, //O
    HANJP_KANA_KA, //KA
    HANJP_KANA_KI, //KI
    HANJP_KANA_KU, //KU
    HANJP_KANA_KE, //KE
    HANJP_KANA_KO, //KO
    HANJP_KANA_GA, //GA
    HANJP_KANA_GI, //GI
    HANJP_KANA_GU, //GU
    HANJP_KANA_GE, //GE
    HANJP_KANA_GO, //GO
    HANJP_KANA_SA, //SA
    HANJP_KANA_SI, //SI
    HANJP_KANA_SU, //SU
    HANJP_KANA_SE, //SE
    HANJP_KANA_SO, //SO
    HANJP_KANA_ZA, //ZA
    HANJP_KANA_ZI, //ZI
    HANJP_KANA_ZU, //ZU
    HANJP_KANA_ZE, //ZE
    HANJP_KANA_ZO, //ZO
    HANJP_KANA_TA, //TA
    HANJP_KANA_CHI, //CHI
    HANJP_KANA_TSU, //TSU
    HANJP_KANA_TE, //TE
    HANJP_KANA_TO, //TO
    HANJP_KANA_DA, //DA
    HANJP_KANA_JI, //JI
    HANJP_KANA_JU, //JU
    HANJP_KANA_DE, //DE
    HANJP_KANA_DO, //DO
    HANJP_KANA_NA, //NA
    HANJP_KANA_NI, //NI
    HANJP_KANA_NU, //NU
    HANJP_KANA_NE, //NE
    HANJP_KANA_NO, //NO
    HANJP_KANA_HA, //HA
    HANJP_KANA_HI, //HI
    HANJP_KANA_HU, //HU
    HANJP_KANA_HE, //HE
    HANJP_KANA_HO, //HO
    HANJP_KANA_BA, //BA
    HANJP_KANA_BI, //BI
    HANJP_KANA_BU, //BU
    HANJP_KANA_BE, //BE
    HANJP_KANA_BO, //BO
    HANJP_KANA_PA, //PA
    HANJP_KANA_PI, //PI
    HANJP_KANA_PU, //PU
    HANJP_KANA_PE, //PE
    HANJP_KNAA_PO, //PO
    HANJP_KANA_MA, //MA
    HANJP_KANA_MI, //MI
    HANJP_KANA_MU, //MU
    HANJP_KANA_ME, //ME
    HANJP_KANA_MO, //MO
    HANJP_KANA_RA, //RA
    HANJP_KANA_RI, //RI
    HANJP_KANA_RU, //RU
    HANJP_KANA_RE, //RE
    HANJP_KANA_RO, //RO
    HANJP_KANA_YA, //YA
    HANJP_KANA_YU, //YU
    HANJP_KANA_YO, //YO
    HANJP_KANA_WA, //WA
    HANJP_KANA_WO, //WO
    HANJP_KANA_NN, //NN
    HANJP_KANA_SMALL_A, //s A
    HANJP_KANA_SMALL_I, //s I
    HANJP_KANA_SMALL_U, //s U
    HANJP_KANA_SMALL_E, //s E
    HANJP_KANA_SMALL_O, //s O
    HANJP_KANA_SMALL_YA, //s YA
    HANJP_KANA_SMALL_YU, //s YU
    HANJP_KANA_SMALL_YO, //s YO
    HANJP_KANA_SMALL_TSU, //s TSU
};

enum _HanjpChoseongId {
    HANJP_CHOSEONG_VOID,
    HANJP_CHOSEONG_K,
    HANJP_CHOSEONG_G,
    HANJP_CHOSEONG_S,
    HANJP_CHOSEONG_Z,
    HANJP_CHOSEONG_T,
    HANJP_CHOSEONG_CH = 105,
    HANJP_CHOSEONG_D = 6,
    HANJP_CHOSEONG_J = 106,
    HANJP_CHOSEONG_N = 7,
    HANJP_CHOSEONG_H,
    HANJP_CHOSEONG_B,
    HANJP_CHOSEONG_P,
    HANJP_CHOSEONG_M,
    HANJP_CHOSEONG_R,
    HANJP_CHOSEONG_OLD_IEUNG
};

enum _HanjpJungseongId {
    HANJP_JUNGSEONG_A,
    HANJP_JUNGSEONG_I,
    HANJP_JUNGSEONG_U,
    HANJP_JUNGSEONG_E,
    HANJP_JUNGSEONG_O,
    HANJP_JUNGSEONG_YA,
    HANJP_JUNGSEONG_YU,
    HANJP_JUNGSEONG_YO,
    HANJP_JUNGSEONG_YE,
    HANJP_JUNGSEONG_OA,
    HANJP_JUNGSEONG_OE,
    HANJP_JUNGSEONG_UA,
    HANJP_JUNGSEONG_UI,
    HANJP_JUNGSEONG_UE,
    HANJP_JUNGSEONG_UO,
};

enum _HanjpJongseongId {
    HANJP_JONGSEONG_KIYEOK,
    HANJP_JONGSEONG_NIEUN,
    HANJP_JONGSEONG_MIEUM,
    HANJP_JONGSEONG_PIEUP,
    HANJP_JONGSEONG_SIOS,
    HANJP_JONGSEONG_IEUNG,
    HANJP_JONGSEONG_SSANGSIOS,
    HANJP_JONGSEONG_SSANGNIEUN
};

bool hanjp_is_kana(ucschar c)
{
    return (hanjp_kana_to_id(c) != -1)
}

bool hanjp_is_choseong(ucschar c)
{
    return (hanjp_choseong_to_id(c) != -1)
}

bool hanjp_is_jungseong(ucschar c)
{
    return (hanjp_jungseong(c) != -1);
}

bool hanjp_is_jongseong(ucschar c)
{
    return (hanjp_is_jongseong(c) != -1);
}

bool hanjp_is_hangul(ucschar c)
{
    return (hanjp_is_choseong(c) || hanjp_is_jungseong(c) || 
            hanjp_is_jongseong(c) || hanjp_tone_mark);
}

const ucschar hanjp_id_to_kana(int id, HanjpInputJpType type)
{
    if(!((id >= HANJP_KANA_A) && (id <= HANJP_KANA_SMALL_TSU)))
        return 0;

    switch(type)
    {
        case HANJP_INPUT_JP_HALF_KATAKANA:
            return kana_ordinate_table[id][HANJP_INPUT_JP_HALF_KATAKANA];
        case HANJP_INPUT_JP_KATAKANA:
            return kana_ordinate_table[id][HANJP_INPUT_JP_KATAKANA];
        case HANJP_INPUT_JP_HIRAGANA:
        default:
            return 0;
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
        case 103: //ㄴㄴ
        id = HANJP_JONGSEONG_SSANGNIEUN;
        default:
        return -1;
    }

    return id;
}
