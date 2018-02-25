#include "hanjp.h"

/*-1 represents invalid
The hangul syllable need to treat as string.
Kana characters is single ucs.
*/

static const ucschar hiragana_base = 0x3040;
static const ucschar hiragana_end = 0x309F;
static const ucschar katakana_base = 0x30A0;
static const ucschar katakana_end = 0x30FF;
static const ucschar half_full_base = 0xFF00;
static const ucschar half_full_end = 0xFFEF;
static const ucschar half_katakana_voiced_simbol = 0xFF9E;
static const ucschar half_katakana_semi_voiced_simbol = 0xFF9F;

static const ucschar hanjp_id_to_kana(int id, HanjpInputType type);
static int hanjp_kana_to_id(const ucschar c);
static const ucschar hanjp_id_to_choseong(int id);
static int hanjp_choseong_to_id(const ucschar c);
static const ucschar hanjp_id_to_jungseong(int id);
static int hanjp_jungseong_to_id(const ucschar c);
static const ucschar hanjp_id_to_jongseong(int id);
static int hanjp_jongseong_to_id(const ucschar c);
static bool hanjp_ic_internal_flush(HanjpInputContext* hic);

/*(hiragana, katakana, half katakana)
Ordinate Description: 
Repeat 'a, i, u, e, o' changing initial consonant.
Initial consonant ordinate:
ㅇ,ㅋ,ㄱ,ㅅ,ㅈ,(ㅌ, ㅊ),(ㄷ, z),ㄴ,ㅎ,ㅂ,ㅍ,ㅁ,ㄹ
Then, special characters: ya,yu,yo,wa,wo,nn.
Then, small laters: a,i,u,e,o; ya,yu,yo,wa,wo,nn,tsu.
이 순서는 목요일에 회의 합시다.

Unicode Basic Rules:
Voiced sound: base-sound + 1
Small latter: base - 1
p-sound: base-sound + 2
*/
static const ucschar kana_ordinate_table[/*id*/][3] = {
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

typedef enum {
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
    HANJP_KANA_WA, //WO
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
} HanjpKanaId;

typedef enum {
    HANJP_CHOSEONG_VOID,
    HANJP_CHOSEONG_K,
    HANJP_CHOSEONG_G,
    HANJP_CHOSEONG_S,
    HANJP_CHOSEONG_J,
    HANJP_CHOSEONG_T,
    HANJP_CHOSEONG_CH = 105,
    HANJP_CHOSEONG_D = 6,
    HANJP_CHOSEONG_Z = 106,
    HANJP_CHOSEONG_N = 7,
    HANJP_CHOSEONG_H,
    HANJP_CHOSEONG_B,
    HANJP_CHOSEONG_P,
    HANJP_CHOSEONG_M,
    HANJP_CHOSEONG_R,
    HANJP_CHOSEONG_OLD_IEUNG
} HanjpChoseongId;

typedef enum {
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
    HANJP_JUNGSEONG_UE,
    HANJP_JUNGSEONG_UI
} HanjpJungseongId;

typedef enum {
    HANJP_JONGSEONG_KIYEOK,
    HANJP_JONGSEONG_NIEUN,
    HANJP_JONGSEONG_MIEUM,
    HANJP_JONGSEONG_PIEUP,
    HANJP_JONGSEONG_SIOS,
    HANJP_JONGSEONG_IEUNG,
    HANJP_JONGSEONG_SSANGSIOS,
    HANJP_JONGSEONG_SSANGNIEUN
} HanjpJongseongId;

/*Each table has the defualt phoneme: the phoneme of each id can be not the only one.
ex) HANJP_CHOSEONG_K: ㄱ, ㄲ
Only the jongseong(final consonanat) id is match 1 to 1 to hangul character.
*/
static const ucschar hanjp_choseong_table[/*id*/] = {
    //hangul jaso(ㅇ,ㅋ,ㄱ,ㅅ) oriented by kana table ordinate
};

static const ucschar hanjp_jungseong_table[/*id*/] ={
    //hangul jaso oriented by kana table ordinate
};

static const ucschar hanjp_jongseong_table[/*id*/] = {
    //hangul jaso that allowed to be batchim
};

static const ucschar hanjp_id_to_kana(int id, HanjpInputJpType type)
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

static int hanjp_kana_to_id(const ucschar c)
{
    int id;

    if(c >= hiragana_base && c <= hiragana_end) //hiragana
        id = c - kana_ordinate_table[0][HANJP_INPUT_JP_HIRAGANA];
    else if(c >= katakana_base && c <= katakana_end) //katakana
        id = c - kana_ordinate_table[0][HANJP_INPUT_JP_KATAKANA];
    else if(c >= half_full_base && half_full_end) //half katakana
        id = c - kana_ordinate_table[0][HANJP_INPUT_JP_HALF_KATAKANA];
    else
        id = 0;

    return id;
}

static const ucschar hanjp_id_to_choseong(int id)
{
    return hanjp_choseong_table[id];
}

static int hanjp_choseong_to_id(const ucschar c)
{

}

static const ucschar hanjp_id_to_jungseong(int id)
{
    return hanjp_jungseong_table[id];
}

static int hanjp_jungseong_to_id(const ucschar c)
{

}

static const ucschar hanjp_id_to_jongseong(int id)
{
    return hanjp_jongseong_table[id];
}

static int hanjp_jongseong_to_id(const ucschar c)
{

}

static bool hanjp_final_conso_conjoinable(ucschar c, ucschar next_c)
{
    //will be written
}

bool hanjp_syllable_to_kana(ucschar *const dest, ucschar syllable, ucschar next_c, HanjpInputType type)
{
    ucschar conv_char;
    //need to be written
}

//id to char 함수 바꿔야 함.
bool hanjp_jamo_to_kana(ucschar *const dest, ucschar cho, ucschar jung, ucschar jong, ucschar next_c, HanjpInputType type)
{
    int kana_id, kana_add_id, kana_add_id1, kana_sup_id, cho_id, jung_id, jong_id;
    int dest_index = 0;
    int i;

    ucschar init_char;
    ucschar add_string[3];
    ucschar sup_char;

    if(!dest)
        return false;

    dest[0] = 0;
    kana_id = 0;
    kana_add_id = -1;
    kana_add_id1 = -1;
    kana_sup_id = -1;
    init_char = 0;
    add_string[0] = 0;
    sup_char = 0;
    cho_id = hanjp_choseong_to_id(cho);
    jung_id = hanjp_jungseong_to_id(jung);
    jong_id = hanjp_jongseong_to_id(jong);

    switch(cho_id) //process choseong
    {
        case -1:
        return false;
        case HANJP_CHOSEONG_CH:
        case HANJP_CHOSEONG_Z:
        kana_id += (cho_id - 100) * 5;
        break;
        case HANJP_CHOSEONG_OLD_IEUNG:
        break;
        default:
        break;
        kana_id += cho_id * 5;
    }

    //process jungseong
    if(jung_id >= HANJP_JUNGSEONG_A && jung_id <= HANJP_JUNGSEONG_O)
    {
        switch(cho_id)
        {
            case HANJP_CHOSEONG_OLD_IEUNG:
            return false;
            case HANJP_CHOSEONG_T:
            case HANJP_CHOSEONG_D:
            switch(jung_id)
            {
                case HANJP_JUNGSEONG_I:
                case HANJP_JUNGSEONG_U:
                kana_id += HANJP_JUNGSEONG_E;
                kana_add_id = HANJP_KANA_SMALL_A + (jung_id - HANJP_JUNGSEONG_A);
                break;
                default:
                kana_id += jung_id;
                kana_add_id = -1;
            }
            break;
            case HANJP_CHOSEONG_CH:
            case HANJP_CHOSEONG_Z:
            switch(jung_id)
            {
                case HANJP_JUNGSEONG_A:
                case HANJP_JUNGSEONG_E:
                case HANJP_JUNGSEONG_O:
                kana_id += HANJP_JUNGSEONG_I;
                kana_add_id = HANJP_KANA_SMALL_A + (jung_id - HANJP_JUNGSEONG_A);
                break;
                default:
                kana_id += jung_id;
                kana_add_id = -1;
            }
            break;
            default:
            kana_id += jung_id;
            kana_add_id = -1;
        }
    }
    else if(jung_id <= HANJP_JUGSEONG_YO) //ya, yu, yo
    {
        switch(cho_id)
        {
            case HANJP_CHOSEONG_OLD_IEUNG:
            return false;
            case HANJP_CHOSEONG_VOID:
            kana_id = HANJP_KANA_YA + (jung_id - HANJP_JUNGSEONG_YA);
            kana_add_id = -1;
            case HANJP_CHOSEONG_T:
            case HANJP_CHOSEONG_D:
            kana_id += HANJP_KANA_E;
            kana_add_id = HANJP_KANA_SMALL_I;
            kana_add_id1 = HANJP_KANA_SMALL_YA + (jung_id - HANJP_JUNGSEONG_YA);
            break;
            default:
            kana_id += HANJP_KANA_I;
            kana_add_id = HANJP_KANA_SMALL_YA + (jung_id - HANJP_JUNGSEONG_YA);
        }
    }
    else if(jung_id == HANJP_JUNGSEONG_YE)
    {
        switch(cho_id)
        {
            case HANJP_CHOSEONG_OLD_IEUNG:
            return false;
            default:
            kana_id += HANJP_JUNGSEONG_I;
            kana_add_id = HANJP_KANA_SMALL_E;
        }
    }
    else if(jung_id <= HANJP_JUNGSEONG_OE)
    {
        switch(cho_id)
        {
            case HANJP_CHOSOENG_OLD_IEUNG:
            switch(jung_id)
            {
                case HANJP_JUNGSEONG_OA:
                kana_id = HANJP_KANA_WA;
                break;
                default:
                return false;
            }
            break;
            case HANJP_CHOSEONG_CH:
            case HANJP_CHOSEONG_Z:
            kana_id += HANJP_CHOSEONG_I;
            kana_add_id = HANJP_KANA_SMALL_O;
            switch(jung_id)
            {
                case HANJP_JUNGSEONG_OA:
                kana_add_id1 = HANJP_KANA_SMALL_A;
                break;
                case HANJP_JUNGSEONG_OE:
                kana_add_id1 = HANJP_KANA_SMALL_E;
                break;
            }
            break;
            default:
            kana_id += HANJP_JUNGSEONG_O;
            switch(jung_id)
            {
                case HANJP_JUNGSEONG_OA:
                kana_add_id = HANJP_KANA_SMALL_A;
                break;
                case HANJP_JUNGSEONG_OE:
                kana_add_id = HANJP_KANA_SMALL_E;
                break;
            }
        }
    }
    else if(jung_id <= HANJP_JUNGSEONG_UI)
    {
        switch(cho_id)
        {
            case HANJP_CHOSEONG_OLD_IEUNG:
            return false;
            case HANJP_CHOSEONG_T:
            case HANJP_CHOSEONG_D:
            kana_id += HANJP_JUNGSEONG_I;
            kana_add_id = HANJP_KANA_SMALL_U;
            switch(jung_id)
            {
                case HANJP_JUNGSEONG_UA:
                kana_add_id1 = HANJP_KANA_SMALL_A;
                break;
                case HANJP_JUNGSEONG_UE:
                kana_add_id1 = HANJP_KANA_SMALL_E;
                break;
                case HANJP_JUNGSEONG_UI:
                kana_add_id1 = HANJP_KANA_I;
                break;
            }
            break;
            default:
            kana_id += HANJP_JUNGSEONG_U;
            switch(jung_id)
            {
                case HANJP_JUNGSEONG_UA:
                kana_add_id = HANJP_KANA_SMALL_A;
                break;
                case HANJP_JUNGSEONG_UE:
                kana_add_id = HANJP_KANA_SMALL_E;
                break;
                case HANJP_JUNGSEONG_UI:
                kana_add_id = HANJP_KANA_I;
                break;
            }
        }
    }
    else
        return false;
    
    //process jongseong
    switch(jong_id)
    {
        case HANJP_JONGSEONG_KIYEOK:
        if(hanjp_final_conso_conjoinable(jong, next_c))
            kana_sup_id = HANJP_KANA_SMALL_TSU;
        else
            kana_sup_id = HANJP_KANA_KU;
        break;
        case HANJP_JONGSEONG_MIEUM:
        if(hanjp_final_conso_conjoinable(jong, next_c))
            kana_sup_id = HANJP_KANA_SMALL_TSU;
        else
            kana_sup_id = HANJP_KANA_MU;
        break;
        case HANJP_JONGSEONG_SIOS:
        if(hanjp_final_conso_conjoinable(jong, next_c))
            kana_sup_id = HANJP_KANA_SMALL_TSU;
        else
            kana_sup_id = HANJP_KANA_TO;
        break;
        case HANJP_JONGSEONG_PIEUP:
        if(hanjp_final_conso_conjoinable(jong, next_c))
            kana_sup_id = HANJP_KANA_SMALL_TSU;
        else
            kana_sup_id = HANJP_KANA_BU;
        break;  
        case HANJP_JONGSEONG_NIEUN:
        case HANJP_JONGSEONG_IEUNG:
        kana_sup_id = HANJP_KANA_NN;
        break;
    }

    init_char = hanjp_id_to_kana(kana_id, type);
    add_string[0] = hanjp_id_to_kana(kana_add_id, type)
    if(add_string[1] = hanjp_id_to_kana(kana_add_id1, type))
        add_string[2] = 0;
    else
        add_string[1] = 0;
    sup_char = hanjp_id_to_kana(kana_sup_id, type);

    if(init_char) //push into dest
    {
        dest[dest_index++] = init_char;

        for(i=0; i<3 && add_string[i]; i++)
            dest[dest_index++] = add_string[i];
        
        if(sup_char)
            dest[dest_index++] = sup_char;
    }
    dest[dest_index] = 0;

    return true;
}