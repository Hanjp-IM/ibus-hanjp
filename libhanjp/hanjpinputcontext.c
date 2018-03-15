#include "hanjp.h"
#include <stdlib.h>

static bool HanjpOnTransition(HangulInputContext* hic,
        ucschar c, const ucschar *buf, void *vhjic);
static void HanjpOnTranslate(HangulInputContext* hic, 
        int ascii, ucschar* pc, void* vhjic);
static void hanjp_ic_append_commit_string(HanjpInputContext* hic, ucschar c);
static void hanjp_ic_flush_internal(HanjpInputContext* hic);
static void hanjp_ic_move_preedit_string(HanjpInputContext* hic, ucschar next_c);
static void hanjp_ic_move_commit_string(HanjpInputContext* hic, ucschar next_c);
static void hanjp_ic_save_commit_string(HanjpInputContext* hic);
static void hanjp_ic_append_commit_string(HanjpInputContext* hic, ucschar c);


/*할 일: HanjpOnTransition에서 flag와 함께 필터링(push를 위해), 
변환을 하고 ic_process에서 작업한다.*/
/*푸시를 못할 떄 false*/
static bool HanjpOnTransition(HangulInputContext* hic, 
        ucschar c, const ucschar* buf, void* vhjic){
    ucschar cho = buf[0];
    ucschar jung = buf[1];
    ucschar jong = buf[2];
    HanjpInputContext *hjic = (HanjpInputContext*)vhjic;

    if(!cho && !jung && !jong)
    {
        hjic->state = HANJP_STATE_START;
        return true;
    }

    hjic->state = HANJP_STATE_EDITTING;

    if(hangul_is_choseong(cho)) //choseong, jungseong filter
    {
        if(hangul_is_jungseong(jung))
        {
            if(hanjp_is_jungseong(jung))
            {
                if(hangul_is_jongseong(jong))
                {
                    if(!hanjp_is_jongseong(jong))
                    {
                        hanjp_ic_move_preedit_string(hjic, c);
                        return false; //hangul_ic_process
                    }
                }
            }
            else
            {
                hanjp_ic_move_preedit_string(hjic, c);
                return false; //hangul_ic_process
            }
        }
    }
    else{
        hanjp_ic_move_preedit_string(hjic, c);
    }

    //on commit
    if(hangul_ic_is_empty(hic))
        hanjp_ic_move_preedit_string(hjic, c);

    return true;
}


/*Description: hangul on-translate-character function.
it change passing character into full width character when the type is full en.*/
static void HanjpOnTranslate(HangulInputContext *hic, 
        int ascii, ucschar* pc, void* vhjic){
    HanjpInputContext *hjic = (HanjpInputContext*) vhjic;
    HanjpInputType type = hjic->Type;

    if(!hangul_is_jamo(*pc))
    {
        hjic->state = HANJP_STATE_START;
        return;
    }

    switch(type)
    {
        case HANJP_INPUT_EN_FULL:
        *pc = hanjp_conv_to_full(*pc); //change to full en
        return;
        case HANJP_INPUT_JP_HIRAGANA: //pass jamo
        case HANJP_INPUT_JP_KATAKANA:
        case HANJP_INPUT_JP_HALF_KATAKANA:
        default:
        return;
    }
}

int hanjp_init()
{
    return hangul_init();
}

int hanjp_fini()
{
    return hangul_fini();
}

HanjpInputContext* hanjp_ic_new(const char* keyboard)
{
    HanjpInputContext *new_hic;

    new_hic = (HanjpInputContext*) malloc(sizeof(HanjpInputContext));

    new_hic->hic = hangul_ic_new(keyboard);
    hangul_ic_connect_callback(new_hic->hic, "translaste", HanjpOnTranslate, (void*) new_hic);
    hangul_ic_connect_callback(new_hic->hic, "transition", HanjpOnTransition, (void*) new_hic); //let on_transition knows type
    new_hic->preedit_string[0] = 0;
    new_hic->commit_string[0] = 0;
    new_hic->state = HANJP_STATE_START;
    new_hic->Type = HANJP_INPUT_JP_HIRAGANA;

    return new_hic;
}

void hanjp_ic_delete(HanjpInputContext *hic)
{
    if(!hic)
        return;

    free(hic->hic);
    free(hic);
}

bool hanjp_ic_process(HanjpInputContext* hic, int ascii)
{
    if(!hic)
        return false;

    if(ascii == '\b')
        return hanjp_ic_backspace(hic);

    if(!hangul_ic_process(hic->hic, ascii)) //process filtered key
    {
        if(!hangul_ic_process(hic->hic, ascii))
            return false;
    }

    if(hangul_ic_is_empty(hic->hic)) //on special characters' commit like (. ? # $ %)
    {
        hanjp_ic_move_commit_string(hic, 0);
        hanjp_ic_save_commit_string(hic);
        hic->state = HANJP_STATE_START;
    }

    return true; 
}

void hanjp_ic_reset(HanjpInputContext *hic)
{
    if(!hic)
        return;

    hangul_ic_reset(hic->hic);
    hic->preedit_string[0] = 0;
    hic->commit_string[0] = 0;
    hic->state = HANJP_STATE_START;
    hic->Type = HANJP_INPUT_JP_HIRAGANA;
}

bool hanjp_ic_backspace(HanjpInputContext *hic)
{
    int len_preedit = 0, len_commit = 0;
    HanjpState state;

    if(!hic)
        return false;

    if(!hangul_ic_backspace(hic->hic))
        return false;

    while(hic->preedit_string[len_preedit])
        len_preedit++;

    while(hic->commit_string[len_commit])
        len_commit++;

    state = hic->state;

    switch(state)
    {
        case HANJP_STATE_START:
        if(len_commit == 0)
            return false;
        hic->commit_string[len_commit - 1] = 0;
        break;
        case HANJP_STATE_EDITTING:
        if(len_preedit == 0)
        {
            hic->state = HANJP_STATE_START;
            return false;
        }
        hic->preedit_string[len_preedit - 1] = 0;
        break;
    }

    return true;
}

bool hanjp_ic_no_change_key(HanjpInputContext* hic)
{
    if(!hic)
        return false;

    hic->state = HANJP_STATE_START;
    hanjp_ic_save_commit_string(hic);

    return true;
}

bool hanjp_ic_change_key(HanjpInputContext* hic)
{
    if(!hic)
        return false;

    hic->state = HANJP_STATE_START;
    //Convert function need
    hanjp_ic_save_commit_string(hic);

    return true;
}

bool hanjp_ic_hiragana_katakana_toggle_key(HanjpInputContext *hic)
{

    return hanjp_string_hiragana_katakana_toggle(hic->commit_string, hic->commit_string, hic->Type);
}

void hanjp_ic_set_input_type(HanjpInputContext* hic, HanjpInputType type)
{
    hic->Type = type;
}

HanjpInputType hanjp_ic_get_input_type(HanjpInputContext* hic)
{
    return hic->Type;
}

const ucschar* hanjp_ic_get_preedit_string(HanjpInputContext* hic)
{
    return hic->preedit_string;
}

const ucschar* hanjp_ic_get_commit_string(HanjpInputContext* hic)
{
    return hic->commit_string;
}

void hanjp_ic_flush(HanjpInputContext *hic)
{
    hic->preedit_string[0] = 0;
    hic->commit_string[0] = 0;
    hangul_ic_flush(hic->hic);
}

static void hanjp_ic_flush_internal(HanjpInputContext* hic)
{
    hangul_ic_flush(hic->hic);
    hic->preedit_string[0] = 0;
}

static void hanjp_ic_move_preedit_string(HanjpInputContext* hic, ucschar next_c)
{
    const ucschar *hangul_preedit = hangul_ic_get_preedit_string(hic->hic);;
    ucschar converted_string[10] = {0, };
    ucschar cho, jung, jong;

    if(hangul_is_syllable(hangul_preedit[0]))
    {
        hangul_syllable_to_jamo(hangul_preedit[0], &cho, &jung, &jong);
    }
    else
    {
        cho = hangul_preedit[0];
        if(cho)
        {
            jung = hangul_preedit[1];
            if(jung)
                jong = hangul_preedit[2];
            else
                jong = 0;
        }
        else{
            jung = 0;
            jong = 0;
        }
    }

    hanjp_jamo_to_kana(converted_string, cho, jung, jong, next_c, hic->Type);
    ucs_append(hic->preedit_string, converted_string);
    hangul_ic_flush(hic->hic);
}

static void hanjp_ic_move_commit_string(HanjpInputContext* hic, ucschar next_c)
{
    ucschar *hangul_commit;
    ucschar converted_string[10] = {0, };
    ucschar cho, jung, jong;
    static ucschar* iter = NULL;
    ucschar* end = hangul_ic_get_commit_string(hic->hic);

    //move to next syllable
    if(!iter)
        iter = hangul_ic_get_commit_string(hic->hic);

    end = iter;

    while(*end)
        end++;

    iter = hangul_syllable_iterator_next(iter, end);

    if(*iter)
        hangul_commit = iter;
    else
        hangul_commit = hangul_ic_get_commit_string(hic->hic);

    if(hangul_is_syllable(hangul_commit[0])) //assign cho, jung, jong
    {
        hangul_syllable_to_jamo(hangul_commit[0], &cho, &jung, &jong);
    }
    else
    {
        cho = hangul_commit[0];
        if(cho)
        {
            jung = hangul_commit[1];
            if(jung)
                jong = hangul_commit[2];
            else
                jong = 0;
        }
        else{
            jung = 0;
            jong = 0;
        }
    }

    hanjp_jamo_to_kana(converted_string, cho, jung, jong, next_c, hic->Type);
    ucs_append(hic->preedit_string, converted_string);
    hangul_ic_flush(hic->hic);
}

static void hanjp_ic_append_commit_string(HanjpInputContext* hic, ucschar c)
{
    int idx = 0;
    ucschar *commit_string = hic->commit_string;

    while(commit_string[idx])
        idx++;

    commit_string[idx] = c;
    hanjp_ic_flush_internal(hic);
}

static void hanjp_ic_save_commit_string(HanjpInputContext* hic)
{
    ucs_copy(hic->commit_string, hic->preedit_string);
    hanjp_ic_flush_internal(hic);
}
