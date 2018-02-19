#include "hanjp.h"

static const ucschar hiragana_base = 0x3040;
static const ucschar katakana_base = 0x30A0;
//static const ucschar half_katakana_base = 0x8ea1;

static ucschar hanjp_code_to_kana(int code, HanjpInputJpType type)
{
    switch(type)
    {
        case HANJP_INPUT_JP_HALF_KATAKANA:
            //currently having no idea
        case HANJP_INPUT_JP_KATAKANA:
            return code + katakana_base;
        case HANJP_INPUT_JP_HIRAGANA:
        default:
            return code + hiragana_base;
    }
}

static int hanjp_kana_to_code(ucschar c)
{
    if(c >= hiragana_base && c < katakana_base)
        return c - hiragana_base;
    else
        return c - katakana_base;
}

ucschar hanjp_syllable_to_kana(ucschar syllable, ucschar next_c, HanjpInputJpType type)
{
    ucschar conv_char;
    //need to be written
}

ucschar hanjp_jamo_to_kana(ucschar cho, ucschar jung, ucschar jong, ucschar next_c, HanjpInputJpType type)
{
    //need to be written
}