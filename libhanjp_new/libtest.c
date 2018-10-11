#include "hanjp.h"
#include "hanjpeater.h"
#include <stdio.h>
#include <glib.h>
#include <stdlib.h>

#define ENABLE_EXTERNAL_KEYBOARDS

void TestHangul();
int TestEater();
int TestInputContext();

int main()
{
    int err_eater;
    int err_ic;

    TestHangul();

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

void TestHangul(){
    HangulInputContext* hic;
	int ascii;
	int i;
	char test[] = "qwertyuiop[]asdfghjkl;'zxcvbnm,./QWERTYUIOP{}ASDFGHJKL:ZXCVBNM<>?";
	char *utf8;
	const ucschar *commit;

	hangul_init();
	printf("Keyboard Count: %d\n", hangul_keyboard_list_get_count());
	
	hic = hangul_ic_new("2hj");
    commit = hangul_ic_get_commit_string(hic);

	for(i=0; test[i]; i++){
		ascii = test[i];
		hangul_ic_process(hic, ascii);
		utf8 = g_ucs4_to_utf8(commit, 64, NULL, NULL, NULL);
		printf("%s\n", utf8);
	}

	hangul_ic_delete(hic);
	hangul_fini();
}

int TestEater(){
    int i;
    int len;
    int err;
    int pass_case = 0;
    HanjpEater* eater;
    char *utf8;
    ucschar ucs_string[20];
    ucschar *dest;
    char* ascii = "dkssudgktpdy.";

    eater = eater_new("2hj");
    dest = malloc(sizeof(ucschar) * 64);

    if(!eater){
        return -1;
    }

    //Step 1
    ucs_string[0] = HANJP_CHOSEONG_PIEUP;
    ucs_string[1] = HANJP_JUNGSEONG_A;
    ucs_string[2] = 0;

    hangul_to_kana(dest, 0, ucs_string, 0, HANJP_OUTPUT_JP_HIRAGANA);
    utf8 = g_ucs4_to_utf8(dest, 64, NULL, NULL, NULL);
    printf("converted ucs: %s\n", utf8);
    pass_case++;

    //Step 2
    len = 0;
    for(i=0; ascii[i]; i++){
        err = eater_push(eater, ascii[i], dest, len, HANJP_OUTPUT_JP_HIRAGANA);
        if(err > 0){
            len += err;
        }
    }

    utf8 = g_ucs4_to_utf8(dest, 64, NULL, NULL, NULL);
    printf("converted ucs: %s\n", utf8);

    eater_delete(eater);

    return pass_case;
}

int TestInputContext(){
    return 0;
}