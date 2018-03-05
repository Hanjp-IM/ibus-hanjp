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
  HANJP_STATE_EDITTING
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

/*character info*/
/*-1 represents invalid
The hangul syllable need to treat as string.
Kana characters is single ucs.
*/
bool hanjp_is_voiced_by_id(int id);
bool hanjp_is_semi_voiced_by_id(int id);
bool hanjp_is_kana(ucschar c);
bool hanjp_is_choseong(ucschar c);
bool hanjp_is_jungseong(ucschar c);
bool hanjp_is_jongseong(ucschar c);
bool hanjp_is_hangul(ucschar c);
const ucschar hanjp_id_to_kana(int id, HanjpInputType type);
int hanjp_kana_to_id(const ucschar c);
int hanjp_choseong_to_id(const ucschar c);
int hanjp_jungseong_to_id(const ucschar c);
int hanjp_jongseong_to_id(const ucschar c);

/*(hiragana, katakana, half katakana)
Ordinate Description: 
Repeat 'a, i, u, e, o' changing initial consonant.
Initial consonant ordinate:
ㅇ,ㅋ,ㄱ,ㅅ,ㅈ,(ㅌ, ㅊ),(ㄷ, z),ㄴ,ㅎ,ㅂ,ㅍ,ㅁ,ㄹ
Then, special characters: ya,yu,yo,wa,wo,nn.
Then, small laters: a,i,u,e,o; ya,yu,yo,wa,wo,nn,tsu.

Unicode Basic Rules:
Voiced sound: base-sound + 1
Small latter: base - 1
p-sound: base-sound + 2
*/
const ucschar kana_ordinate_table[80][3];
typedef enum _HanjpKanaId HanjpKanaId;
typedef enum _HanjpChoseongId HanjpChoseongId;
typedef enum _HanjpJungseongId HanjpJungseongId;
typedef enum _HanjpJongseongId HanjpJongseongId;


/*Input Context*/
HanjpInputContext* hanjp_ic_new(const char* keyboard);
void hanjp_ic_delete(HanjpInputContext *hic);
bool hanjp_ic_process(HanjpInputContext* hic, int ascii);
void hanjp_ic_reset(HanjpInputContext *hic);
bool hanjp_ic_backspace(HanjpInputContext *hic);
bool hanjp_ic_no_change_key(HanjpInputContext* hic);
bool hanjp_ic_change_key(HanjpInputContext* hic);
bool hanjp_ic_hiragana_katakana_toggle_key(HanjpInputContext *hic);

bool hanjp_ic_is_empty(HanjpInputContext *hic);

void hanjp_ic_set_input_type(HanjpInputType type);
HanjpInputType hanjp_ic_get_input_type(void);

const ucschar* hanjp_ic_get_preedit_string(HanjpInputContext* hic);
const ucschar* hanjp_ic_get_commit_string(HanjpInputContext* hic);
const ucschar* hanjp_ic_flush(HanjpInputContext *hic);

/*start and quit*/
void hanjp_init();
void hanjp_fini();

/*convert functions*/
bool hanjp_jamo_to_kana(ucschar *const dest, ucschar cho, ucschar jung, ucschar jong, ucschar next_c, HanjpInputType type);
bool hanjp_syllable_to_kana(ucschar *const dest, ucschar syllable, ucschar next_c, HanjpInputType type);
//need to add kana to kanji convert functions


//utils
void ucs_append(ucschar *dest, ucschar * add);
void ucs_copy(ucschar *dest, ucschar * src);
