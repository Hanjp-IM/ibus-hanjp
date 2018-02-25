#include <hangul.h>

typedef enum {
  HANJP_INPUT_JP_HIRAGANA,
  HANJP_INPUT_JP_KATAKANA,
  HANJP_INPUT_JP_HALF_KATAKANA,
  HANJP_INPUT_EN_FULL
} HanjpInputType;

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
  ucschar preedit_string[64];
  ucschar commit_string[64];
  HanjpState state;
  HanjpInputType Type;
} HanjpInputContext;

/*Input Context*/
HanjpInputContext* hanjp_ic_new(const char* keyboard);
void hanjp_ic_delete(HanjpInputContext *hic);
bool hanjp_ic_process(HanjpInputContext* hic, int ascii);
void hanjp_ic_reset(HanjpInputContext *hic);
bool hanjp_ic_backspace(HanjpInputContext *hic);
bool hanjp_ic_no_change_key(HanjpInputContext* hic);
bool hanjp_ic_change_key(HanjpInputContext* hic);
bool hanjp_ic_hiragana_katakana_toggle_key(HanjpInputContext *hic);
void hanjp_ic_set_input_type(HanjpInputType type);
HanjpInputType hanjp_ic_get_input_type(void);

/*start and quit*/
void hanjp_init();
void hanjp_fini();

/*convert functions*/
bool hanjp_syllable_to_kana(ucschar *const dest, ucschar syllable, ucschar next_c, HanjpInputType type);
bool hanjp_jamo_to_kana(ucschar *const dest, ucschar cho, ucschar jung, ucschar jong, ucschar next_c, HanjpInputType type);
//need to add kana to kanji convert functions

