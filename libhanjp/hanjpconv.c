#include "hanjp.h"

bool hanjp_syllable_to_kana(ucschar *dest, ucschar syllable, ucschar next_c, HanjpInputType type)
{
    ucschar conv_char;
    ucschar cho, jung, jong;

    hangul_syllable_to_jamo(syllable, &cho, &jung, &jong);

    return hanjp_jamo_to_kana(dest, cho, jung, jong, next_c, type);
}

//id to char 함수 바꿔야 함.
bool hanjp_jamo_to_kana(ucschar *dest, ucschar cho, ucschar jung, ucschar jong, ucschar next_c, HanjpInputType type)
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

    if((cho_id == -1) && (jung_id == -1) && (jong_id == -1))
        return false;

    switch(cho_id) //process choseong
    {
        case HANJP_CHOSEONG_CH:
        case HANJP_CHOSEONG_J:
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
            switch(jung_id)
            {
                case HANJP_JUNGSEONG_O:
                kana_id = HANJP_KANA_WO;
                break;
                default:
                return false;
            }
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
            case HANJP_CHOSEONG_J:
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
            return false;
            case HANJP_CHOSEONG_VOID:
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
            case HANJP_CHOSEONG_J:
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
                kana_add_id = HANJP_KANA_SMALL_I;
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