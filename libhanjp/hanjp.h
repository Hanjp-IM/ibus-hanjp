#include <hangul.h>

typedef enum {
  HANJP_INPUT_MODE_EN,
  HANJP_INPUT_MODE_JP
} HanjpInputMode;

typedef enum {
  HANJP_INPUT_JP_HIRAGANA,
  HANJP_INPUT_JP_KATAKANA,
  HANJP_INPUT_JP_HALF_KATAKANA,
} HanjpInputJpType;

typedef enum {
  HANJP_INPUT_EN,
  HANJP_INPUT_EN_HALF,
  HANJP_INPUT_EN_FULL
} HanjpInputEnType;

/*Description: Hanjp automaton states; it is checked whenever IFM calls key process functions.
 The state HANJP_STATE_START is non editting state when HangulInputContext is not being used.
 The state HANJP_STATE_EDITTING is editting state when HangulInputContext is being used.
 The state HANJP_STATE_COMMIT is state that committing whole string of Japanese that user written; it's the
 state that processing 'no conversion', 'conversion' key. 
*/
typedef enum {
  HANJP_STATE_START,
  HANJP_STATE_EDITTING,
  HANJP_STATE_COMMIT
} HanjpState;

/*Description: Hanjp input context: it is the object that containing whole hanjp functional data.
 preedit_string: it is more like preedit for hangul than Japanese.
 commit_string: commiting string
 mode: input mode
 state: current state
*/
typedef struct {
  HangulInputContext* hic;
  hanjp_automaton_state state;
  ucschar preedit_string[64];
  ucschar commit_string[64];
  HanjpInputMode mode;
  HanjpState state;
  HanjpInputEnType EnType;
  HanjpInputJpType JpType;
} HanjpInputContext;

/*special key event handler*/
void hanjp_no_change_key_handler(HanjpInputContext* hic);
void hanjp_change_key_handler(HanjpInputContext* hic);
void hanjp_hiragana_katakana_toggle_key_handler(HanjpInputContext *hic);
void hanjp_jp_en_toggle_key_handler(HanjpInputContext* hic);

/*Input Context*/
HanjpInputContext* hanjp_ic_new(const char* keyboard);
void hanjp_ic_delete(HanjpInputContext *hic);
bool hanjp_ic_process(HanjpInputContext* hic, int ascii);
void hanjp_ic_reset(HanjpInputContext *hic);
bool hanjp_ic_backspace(HanjpInputContext *hic);

/*start and quit*/
void hanjp_init();
void hanjp_fini();

/*convert functions*/
ucschar hanjp_syllable_to_kana(ucschar syllable, ucschar next_c, HanjpInputJpType type);
ucschar hanjp_jamo_to_kana(ucschar cho, ucschar jung, ucschar jong, ucschar next_c, HanjpInputJpType type);
//need to add kana to kanji convert functions