#include "hanjp.h"

/*Description:This functions is input filter that works as on_transition function in HangulInputContext.
This function just filters jongseongs that are not listed in HanjpJongseong list.
The key process is need in outside of HangulInputContext*/
static bool HanjpOnTransition(HangulInputContext* hic, 
        ucschar c, const ucschar* buf, void* data){
    ucschar cho = 0;
    ucschar jung = 0;
    ucschar jong = 0;
    int jong_id;

    hangul_syllable_to_jamo(buf, &cho, &jung, &jong);

    if(cho) //jongseong rule
    {
        if(jung)
        {
            if(jong)
            {
                jong_id = hanjp_jongseong_to_id(jong);

                //exit push function when not allowed character is pushed
                if(jong_id == -1)  
                    return false;
            }
        }
    }

    return true;
}

/*Description: This function is recover function that works as on_translate function in HangulInputContext.
This function recovers latter that was filtered by HanjpOnTransition before processing key event.
*/
static void HanjpOnTranslate(HangulInputContext* hic, 
        int ascii, ucschar* pc, void* data){
    
}

HanjpInputContext* hanjp_ic_new(const char* keyboard)
{
    HanjpInputContext *new_hic;

    new_hic = (HanjpInputContext*) malloc(sizeof(HanjpInputContext));

    new_hic->hic = hangul_ic_new(keyboard);
    hangul_ic_connect_callback(hic, "translaste", HanjpTranslate, NULL);
    hangul_ic_connect_callback(hic, "transition", HanjpTransition, NULL);
    new_hic->preedit_string[0] = 0;
    new_hic->commit_string[0] = 0;
    new_hic->state = HANJP_STATE_COMMIT;
    new_hic->Type = HANJP_INPUT_JP_HIRAGANA;
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
    ucschar c;

    if(hic == NULL)
    return false;

    c = hangul_keyboard_get_mapping(hic->keyboard, 0, ascii);

    if(ascii == '\b')
        return hanjp_ic_backspace(hic);

    if(!hangul_ic_process(hic->hic, ascii)) //on not allowed batchim comes
    {
        hangul_ic_save_commit_string(hic);
        hangul_ic_flush(hic->hic);
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
    hic->mode = HANJP_INPUT_MODE_EN;
    hic->state = HANJP_STATE_COMMIT;
    hic->Type = HANJP_INPUT_EN;
}

bool hanjp_ic_backspace(HanjpInputContext *hic)
{
    //need to be written
}

bool hanjp_ic_no_change_key(HanjpInputContext* hic)
{

}

bool hanjp_ic_change_key(HanjpInputContext* hic)
{
    
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
