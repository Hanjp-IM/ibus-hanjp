#include "hanjp.h"

static bool HanjpOnTransition(HangulInputContext* hic, 
        ucschar c, const ucschar* buf, void* data)
{
    
}

static void HanjpOnTranslate(HangulInputContext* hic, 
        int ascii, ucschar* pc, void* data)
{
    
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
    new_hic->mode = HANJP_INPUT_MODE_EN;
    new_hic->state = HANJP_STATE_COMMIT;
    new_hic->Type = HANJP_INPUT_EN;
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
    //need to be written
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
    hic->EnType = HANJP_INPUT_EN;
    hic->JpType = HANJP_INPUT_JP_HIRAGANA;
}

bool hanjp_ic_backspace(HanjpInputContext *hic)
{
    //need to be written
}