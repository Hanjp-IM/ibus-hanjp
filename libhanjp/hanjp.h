#include <hangul.h>

#define ENABLE_EXTERNAL_KEYBOARDS 1

typedef enum {
  HANJP_INPUT_JP_HIRAGANA,
  HANJP_INPUT_JP_KATAKANA,
  HANJP_INPUT_JP_HALF_KATAKANA,
  HANJP_INPUT_EN_FULL
} HanjpInputType;

/*Description: Hanjp automaton states; it indicates current working state of the HanjpInputContext, and
 it will be used in key handlers and the ibus.
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
 preedit_string: it is more like preedit for Japanese than hangul.
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

typedef enum {
    HANJP_KANA_A, //A
    HANJP_KANA_I, //I
    HANJP_KANA_U, //U
    HANJP_KANA_E, //E
    HANJP_KANA_O, //O
    HANJP_KANA_KA, //KA
    HANJP_KANA_KI, //KI
    HANJP_KANA_KU, //KU
    HANJP_KANA_KE, //KE
    HANJP_KANA_KO, //KO
    HANJP_KANA_GA, //GA
    HANJP_KANA_GI, //GI
    HANJP_KANA_GU, //GU
    HANJP_KANA_GE, //GE
    HANJP_KANA_GO, //GO
    HANJP_KANA_SA, //SA
    HANJP_KANA_SI, //SI
    HANJP_KANA_SU, //SU
    HANJP_KANA_SE, //SE
    HANJP_KANA_SO, //SO
    HANJP_KANA_ZA, //ZA
    HANJP_KANA_ZI, //ZI
    HANJP_KANA_ZU, //ZU
    HANJP_KANA_ZE, //ZE
    HANJP_KANA_ZO, //ZO
    HANJP_KANA_TA, //TA
    HANJP_KANA_CHI, //CHI
    HANJP_KANA_TSU, //TSU
    HANJP_KANA_TE, //TE
    HANJP_KANA_TO, //TO
    HANJP_KANA_DA, //DA
    HANJP_KANA_JI, //JI
    HANJP_KANA_JU, //JU
    HANJP_KANA_DE, //DE
    HANJP_KANA_DO, //DO
    HANJP_KANA_NA, //NA
    HANJP_KANA_NI, //NI
    HANJP_KANA_NU, //NU
    HANJP_KANA_NE, //NE
    HANJP_KANA_NO, //NO
    HANJP_KANA_HA, //HA
    HANJP_KANA_HI, //HI
    HANJP_KANA_HU, //HU
    HANJP_KANA_HE, //HE
    HANJP_KANA_HO, //HO
    HANJP_KANA_BA, //BA
    HANJP_KANA_BI, //BI
    HANJP_KANA_BU, //BU
    HANJP_KANA_BE, //BE
    HANJP_KANA_BO, //BO
    HANJP_KANA_PA, //PA
    HANJP_KANA_PI, //PI
    HANJP_KANA_PU, //PU
    HANJP_KANA_PE, //PE
    HANJP_KANA_PO, //PO
    HANJP_KANA_MA, //MA
    HANJP_KANA_MI, //MI
    HANJP_KANA_MU, //MU
    HANJP_KANA_ME, //ME
    HANJP_KANA_MO, //MO
    HANJP_KANA_RA, //RA
    HANJP_KANA_RI, //RI
    HANJP_KANA_RU, //RU
    HANJP_KANA_RE, //RE
    HANJP_KANA_RO, //RO
    HANJP_KANA_YA, //YA
    HANJP_KANA_YU, //YU
    HANJP_KANA_YO, //YO
    HANJP_KANA_WA, //WA
    HANJP_KANA_WO, //WO
    HANJP_KANA_NN, //NN
    HANJP_KANA_SMALL_A, //s A
    HANJP_KANA_SMALL_I, //s I
    HANJP_KANA_SMALL_U, //s U
    HANJP_KANA_SMALL_E, //s E
    HANJP_KANA_SMALL_O, //s O
    HANJP_KANA_SMALL_YA, //s YA
    HANJP_KANA_SMALL_YU, //s YU
    HANJP_KANA_SMALL_YO, //s YO
    HANJP_KANA_SMALL_TSU, //s TSU
} HanjpKanaId;

typedef enum {
    HANJP_CHOSEONG_VOID = -1,
    HANJP_CHOSEONG_IEUNG,
    HANJP_CHOSEONG_K,
    HANJP_CHOSEONG_G,
    HANJP_CHOSEONG_S,
    HANJP_CHOSEONG_Z,
    HANJP_CHOSEONG_T,
    HANJP_CHOSEONG_CH = 105,
    HANJP_CHOSEONG_D = 6,
    HANJP_CHOSEONG_J = 106,
    HANJP_CHOSEONG_N = 7,
    HANJP_CHOSEONG_H,
    HANJP_CHOSEONG_B,
    HANJP_CHOSEONG_P,
    HANJP_CHOSEONG_M,
    HANJP_CHOSEONG_R,
    HANJP_CHOSEONG_OLD_IEUNG
}HanjpChoseongId;

typedef enum {
    HANJP_JUNGSEONG_A,
    HANJP_JUNGSEONG_I,
    HANJP_JUNGSEONG_U,
    HANJP_JUNGSEONG_E,
    HANJP_JUNGSEONG_O,
    HANJP_JUNGSEONG_YA,
    HANJP_JUNGSEONG_YU,
    HANJP_JUNGSEONG_YO,
    HANJP_JUNGSEONG_YE,
    HANJP_JUNGSEONG_OA,
    HANJP_JUNGSEONG_OE,
    HANJP_JUNGSEONG_UA,
    HANJP_JUNGSEONG_UI,
    HANJP_JUNGSEONG_UE,
    HANJP_JUNGSEONG_UO,
} HanjpJungseongId;

typedef enum {
    HANJP_JONGSEONG_KIYEOK,
    HANJP_JONGSEONG_NIEUN,
    HANJP_JONGSEONG_MIEUM,
    HANJP_JONGSEONG_PIEUP,
    HANJP_JONGSEONG_SIOS,
    HANJP_JONGSEONG_IEUNG,
    HANJP_JONGSEONG_SSANGSIOS,
    HANJP_JONGSEONG_SSANGNIEUN
}HanjpJongseongId;

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
const ucschar hanjp_kana_to_small(ucschar c);
const ucschar hanjp_conv_to_full(ucschar c);

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

/*Input Context*/
HanjpInputContext* hanjp_ic_new(const char* keyboard);
void hanjp_ic_delete(HanjpInputContext *hic);
bool hanjp_ic_process(HanjpInputContext* hic, int ascii);
void hanjp_ic_reset(HanjpInputContext *hic);
bool hanjp_ic_backspace(HanjpInputContext *hic);
bool hanjp_ic_no_change_key(HanjpInputContext* hic);
bool hanjp_ic_change_key(HanjpInputContext* hic);
bool hanjp_ic_hiragana_katakana_toggle_key(HanjpInputContext *hic);

void hanjp_ic_set_input_type(HanjpInputContext* hic, HanjpInputType type);
HanjpInputType hanjp_ic_get_input_type(HanjpInputContext* hic);

const ucschar* hanjp_ic_get_preedit_string(HanjpInputContext* hic);
const ucschar* hanjp_ic_get_commit_string(HanjpInputContext* hic);
void hanjp_ic_flush(HanjpInputContext *hic);

/*start and quit*/
int hanjp_init();
int hanjp_fini();

/*convert functions*/
bool hanjp_jamo_to_kana(ucschar *const dest, ucschar cho, ucschar jung, ucschar jong, ucschar next_c, HanjpInputType type);
bool hanjp_syllable_to_kana(ucschar *const dest, ucschar syllable, ucschar next_c, HanjpInputType type);
//need to add kana to kanji convert functions

/*utils*/
void ucs_append(ucschar *dest, ucschar * add);
void ucs_copy(ucschar *dest, ucschar * src);
void ucs_append_char(ucschar *dest, ucschar add);
