#include "hanjp.h"

static const ucschar hiragana_base = 0x3040;
static const ucschar hiragana_end = 0x309F;
static const ucschar katakana_base = 0x30A0;
static const ucschar katakana_end = 0x30FF;
static const ucschar half_full_base = 0xFF00;
static const ucschar half_full_end = 0xFFEF

/*(hiragana, katakana, half katakana)
Ordinate Description: 
Repeat 'a, i, u, e, o' changing initial consonant.
Initial consonant ordinate:
ㅇ,ㅋ,ㄱ,ㅅ,ㅈ,ㅌ,ㄷ,ㄴ,ㅎ,ㅂ,ㅍ,ㅁ,ㄹ
Then, ya,yu,yo,wa,wo,nn
Then, small, a,i,u,e,o; ya,yu,yo,wa,wo,nn,tsu
이 순서는 목요일에 회의 합시다.
*/
static const ucschar kana_ordinate_table[/*id*/][3] = {
    {0x3042, 0x30A2, 0xFF71},
    //so on
};

typedef enum {
    HANJP_CHOSEONG_VOID,
    HANJP_CHOSEONG_K,
    HANJP_CHOSEONG_I,
    //and so on
} HanjpChoseongId;

static const ucschar hangul_choseong_table[/*id*/] = {
    //hangul jaso(ㅇ,ㅋ,ㄱ,ㅅ) oriented by kana table ordinate
};

static ucschar hanjp_id_to_kana(int id, HanjpInputJpType type)
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

static int hanjp_kana_to_id(ucschar c)
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

ucschar* hanjp_syllable_to_kana(ucschar syllable, ucschar next_c, HanjpInputJpType type)
{
    ucschar conv_char;
    //need to be written
}

ucschar* hanjp_jamo_to_kana(ucschar cho, ucschar jung, ucschar jong, ucschar next_c, HanjpInputJpType type)
{
    int kana_id, cho_id, jung_id, jong_id;

    kana_id
}