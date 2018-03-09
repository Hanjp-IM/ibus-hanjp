#include "hanjp.h"
#include <stdlib.h>

static bool HanjpOnTransition(HangulInputContext* hic,
        ucschar c, const ucschar *buf, void *data);
static void HanjpOnTranslate(HangulInputContext* hic, 
        int ascii, ucschar* pc, void* data);
static bool hanjp_hic_push(HangulInputContext *hic, ucschar c);
static void hanjp_ic_process_on_hangul_commit(HanjpInputContext* hic);
static void hanjp_ic_append_commit_string(HanjpInputContext* hic, ucschar c);
static void hanjp_ic_flush_internal(HanjpInputContext* hic);
static void hanjp_ic_save_preedit_string(HanjpInputContext* hic);
static void hanjp_ic_save_commit_string(HanjpInputContext* hic);
static void hanjp_ic_append_commit_string(HanjpInputContext* hic, ucschar c);

/*Description:This functions is input filter that works as on_transition function in HangulInputContext.
This function just filters jongseongs that are not listed in HanjpJongseong list.
The key process is need in outside of HangulInputContext*/
/*need to add jungseong rules*/
static bool HanjpOnTransition(HangulInputContext* hic, 
        ucschar c, const ucschar* buf, void* data){
    ucschar cho = buf[0];
    ucschar jung = buf[1];
    ucschar jong = buf[2];

    //There is no need to check choseong or jungseong
    if(hangul_is_choseong(cho))
    {
        if(hangul_is_jungseong(jung))
        {
            if(hangul_is_jongseong(jong))
            {
                if(!hanjp_is_jongseong(jong)) //jongseong is on table
                    return false;
            }
        }
    }

    return true;
}


/*Description: hangul on-translate-character function.
it change passing character into full width character when the type is full en.*/
static void HanjpOnTranslate(HangulInputContext *hic, 
        int ascii, ucschar* pc, void* data){
    HanjpInputType type = *(HanjpInputType*)data;

    switch(type)
    {
        case HANJP_INPUT_EN_FULL:
        *pc = hangul_keyboard_get_mapping(hic->keyboard, 1, ascii); //change to full en
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
    hangul_ic_connect_callback(new_hic->hic, "translaste", HanjpOnTranslate, NULL);
    hangul_ic_connect_callback(new_hic->hic, "transition", HanjpOnTransition, (void*) &new_hic->Type); //let on_transition knows type
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


//need to be fixed
bool hanjp_ic_process(HanjpInputContext* hic, int ascii)
{
    ucschar c;
    HanjpInputType type = hic->Type;
    static int len_hangul_commit_prev = 0;
    static int len_hangul_commit = 0;
    int tableid;

    if(!hic)
        return false;

    len_hangul_commit_prev = len_hangul_commit;
    len_hangul_commit = N_ELEMENTS(hangul_commit);

    if(ascii == '\b')
        return hanjp_ic_backspace(hic);

    switch(type)
    {
        case HANJP_INPUT_EN_FULL:
        tableid = 1;
        break;
        default:
        tableid = 0;
    }

    c = hangul_keyboard_get_mapping(hic->hic->keyboard, tableid, ascii);

    if(c <= 0)
        return false;

    //*key process
    hic->state = HANJP_STATE_EDITTING;

    if(!hangul_ic_process(hic->hic, ascii)) //on not allowed jungseong, jongseong comes
    {
        hanjp_hic_push(hic->hic, c);
        hanjp_ic_process_on_hangul_commit(hic);  
    }
    else if(len_hangul_commit > len_hangul_commit_prev)//on hangulinputcontext is commited
    {
        hanjp_ic_process_on_hangul_commit(hic);

        if(!hanjp_is_hangul(c))
        {
            hic->state = HANJP_STATE_START;
            hanjp_ic_save_commit_string(hic);
        }
    }
    //*

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
    int len_preedit, len_commit;
    HanjpState state;

    if(!hic)
        return false;

    if(!hangul_ic_backspace(hic->hic))
        return false;

    len_preedit = N_ELEMENTS(hic->preedit_string);
    len_commit = N_ELEMENTS(hic->commit_string);
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
    ucschar *preedit_string = hic->preedit_string;
    ucschar *commit_string = hic->commit_string;

    if(preedit_string[0] == 0)
        return false;

    ucs_copy(commit_string, preedit_string);
    hanjp_ic_flush_internal(hic);

    return true;
}

bool hanjp_ic_change_key(HanjpInputContext* hic)
{
    ucschar *preedit_string = hic->preedit_string;
    ucschar *commit_string = hic->commit_string;

    if(preedit_string[0] == 0)
        return false;
    
    //i have no idea

    return true;
}

bool hanjp_ic_hiragana_katakana_toggle_key(HanjpInputContext *hic)
{
    HanjpInputType type = hic->Type;

    switch(type)
    {
        case HANJP_INPUT_JP_HIRAGANA:
        
        break;
        case HANJP_INPUT_JP_KATAKANA:
        break;
        default:
        return false;
    }

    return true;
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

static bool hanjp_hic_push(HangulInputContext *hic, ucschar c)
{
    if(!hic)
        return false;

    if(!hic->hic)
        return false;

    if(!hangul_is_jamo(c))
        return false;

    if (hangul_is_choseong(c)) {
	hic->hic->buffer.choseong = c;
    } else if (hangul_is_jungseong(c)) {
	hic->hic->buffer.jungseong = c;
    } else if (hangul_is_jongseong(c)) {
	hic->hic->buffer.jongseong = c;
    } else {
    }

    hic->hic->buffer.stack[++hic->hic->buffer.index] = c;

    return true;
}

static void hanjp_ic_flush_internal(HanjpInputContext* hic)
{
    hic->hic->preedit_string[0] = 0;
    hic->preedit_string[0] = 0;
}

static void hanjp_ic_process_on_hangul_commit(HanjpInputContext* hic)
{
    ucschar *hangul_commit;
    ucschar conv_string[10] = {0, };
    ucschar cho, jung, jong, next_c;
    int idx;

    static int len_commit = 0;
    static int len_commit_prev = 0;

    hangul_commit = hangul_ic_get_commit_string(hic->hic);
    
    len_commit_prev = len_commit;
    len_commit = N_ELEMENTS(hangul_commit);
    
    idx = len_commit_prev;

    if(cho = hangul_commit[idx++])
    {
        if(jung = hangul_commit[idx++])
        {
            jong = hangul_commit[idx];
        }
        else
        {
            jong = 0;
        }
    }
    else
    {
        jung = 0;
        jong = 0;
    }

    next_c = hic->hic->buffer.choseong;

    hanjp_jamo_to_kana(conv_string, cho, jung, jong, next_c, hic->Type);
    ucs_append(hic->preedit_string, conv_string);
}

static void hanjp_ic_save_commit_string(HanjpInputContext* hic)
{
    ucschar *commit_string = hic->commit_string;
    ucschar *preedit_string = hic->preedit_string;

    ucs_append(commit_string, preedit_string);
    hanjp_ic_flush_internal(hic);
}

static void hanjp_ic_append_commit_string(HanjpInputContext* hic, ucschar c)
{
    int idx;
    ucschar *commit_string = hic->commit_string;

    idx = N_ELEMENTS(commit_string);

    commit_string[idx] = c;
    hanjp_ic_flush_internal(hic);
}
