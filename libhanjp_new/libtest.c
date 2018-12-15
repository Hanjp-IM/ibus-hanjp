#include "hanjp.h"
#include <stdio.h>
#include <glib.h>
#include <stdlib.h>

#define ENABLE_EXTERNAL_KEYBOARDS

void TestHangul();
void TestInputContext();

int main()
{
    TestHangul();
    TestInputContext();

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

void TestInputContext(){
    int i;
    HanjpInputContext* hjic;
    char *utf8;
    char* ascii = "dkssudgktpdy.";
    const ucschar *commit_string;

    hangul_init();

    hjic = hanjp_ic_new("2hj");
    
    for(i=0; ascii[i]; i++){
        if(!hanjp_ic_process(hjic, ascii[i]))
            break;
    }
    commit_string = hanjp_ic_get_commit_string(hjic);
    utf8 = g_ucs4_to_utf8(commit_string, 64, NULL, NULL, NULL);
    printf("converted ucs: %s\n", utf8);

    hanjp_ic_delete(hjic);
    hangul_fini();
}