#include <hangul.h>

#define ENABLE_EXTERNAL_KEYBOARDS 1

typedef enum {
  HANJP_OUTPUT_JP_HIRAGANA,
  HANJP_OUTPUT_JP_KATAKANA,
  HANJP_OUTPUT_JP_HALF_KATAKANA,
  HANJP_OUTPUT_EN_FULL
} HanjpOutputType;

typedef struct _HanjpInputContext HanjpInputContext;

/*Input Context*/
HanjpInputContext* hanjp_ic_new(const char* keyboard);
void hanjp_ic_delete(HanjpInputContext *hjic);
bool hanjp_ic_process(HanjpInputContext* hjic, int ascii);
bool hanjp_ic_backspace(HanjpInputContext *hjic);
//bool hanjp_ic_no_change_key(HanjpInputContext* hjic);
//bool hanjp_ic_change_key(HanjpInputContext* hjic);
//bool hanjp_ic_hiragana_katakana_toggle_key(HanjpInputContext *hjic);

const ucschar* hanjp_ic_get_preedit_string(HanjpInputContext* hjic, HanjpOutputType type);
const ucschar* hanjp_ic_get_commit_string(HanjpInputContext* hjic);
bool hanjp_ic_flush(HanjpInputContext *hjic);

int hanjp_init();
int hanjp_fini();
