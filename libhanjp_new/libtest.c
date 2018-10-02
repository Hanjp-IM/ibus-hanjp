#include "hanjp.h"
#include "hanjpeater.h"
#include "hanjpeater.c"
#include <stdio.h>

int TestEater();
int TestInputContext();

int main()
{
    int err_eater;
    int err_ic;

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

    return 0;
}

int TestEater(){
    int pass_case = 0;

    ucschar ucs_string[20];
    ucschar dest[20];

    //Step 1
    ucs_string[0] = HANJP_CHOSEONG_PIEUP;
    ucs_string[1] = HANJP_CHOSEONG_A;
    ucs_string[2] = HANJP_CHOSEONG_PIEUP;
    ucs_string[3] = HANJP_CHOSEONG_O;
    ucs_string[4] = 0;

    hangul_to_kana(dest, 0, ucs_string, ucs_string[3], HANJP_OUTPUT_JP_HIRAGANA);
    printf("%d %d\n", dest[0], dest[1]);
    pass_case++;
    
    return pass_case;
}

int TestInputContext(){
    return 0;
}