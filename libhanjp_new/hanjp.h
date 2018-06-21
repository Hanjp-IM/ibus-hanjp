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
void hanjp_ic_delete(HanjpInputContext *hic);
bool hanjp_ic_process(HanjpInputContext* hic, int ascii);
void hanjp_ic_reset(HanjpInputContext *hic);
bool hanjp_ic_backspace(HanjpInputContext *hic);
bool hanjp_ic_no_change_key(HanjpInputContext* hic);
bool hanjp_ic_change_key(HanjpInputContext* hic);
bool hanjp_ic_hiragana_katakana_toggle_key(HanjpInputContext *hic);

const ucschar* hanjp_ic_get_preedit_string(HanjpInputContext* hic, HanjpOutputType type);
const ucschar* hanjp_ic_get_commit_string(HanjpInputContext* hic);
void hanjp_ic_flush(HanjpInputContext *hic);

int hanjp_init();
int hanjp_fini();
