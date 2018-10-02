#include "hanjp.h"
#include "hanjpeater.h"
#include <stdio.h>
#include <glib.h>

int TestEater();
int TestInputContext();

int main()
{
    int err_eater;
    int err_ic;
    int init_code, fini_code;

    init_code = hanjp_init();
    printf("init code: %d\n", init_code);

    err_eater = TestEater();
    printf("Eater Test Result: %d\n", err_eater);

    if(err_eater < 0){
        return -1;
    }

    err_ic = TestInputContext();
    printf("Input Context Test Result: %d\n", err_ic);

    if(err_ic < 0){
        return -1;
    }

    fini_code = hanjp_fini();
    printf("fini code: %d\n", fini_code);

    return 0;
}

int TestEater(){
    int i;
    int len;
    int err;
    int pass_case = 0;
    HanjpEater* eater;

    ucschar ucs_string[20];
    ucschar dest[20];
    char* ascii = "zifk";

    eater = eater_new("2hj");

    if(!eater){
        return -1;
    }

    //Step 1
    ucs_string[0] = HANJP_CHOSEONG_PIEUP;
    ucs_string[1] = HANJP_JUNGSEONG_A;
    ucs_string[2] = 0;

    hangul_to_kana(dest, 0, ucs_string, ucs_string[3], HANJP_OUTPUT_JP_HIRAGANA);
    printf("%x %x\n", dest[0], dest[1]);
    pass_case++;

    //Step 2
    len = 0;
    for(i=0; ascii[i]; i++){
        err = eater_push(eater, ascii[i], ucs_string, len, HANJP_OUTPUT_JP_HIRAGANA);
        if(err > 0){
            len += err;
        }
    }

    printf("converted ucs: ");
    for(i=0; i<len; i++){
        printf("%x ", ucs_string[i]);
    }
    printf("\n");

    eater_delete(eater);

    return pass_case;
}

int TestInputContext(){
    return 0;
}