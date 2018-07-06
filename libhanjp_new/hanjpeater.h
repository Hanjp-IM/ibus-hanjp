#include <hangul.h>

#define EATFLG_ATDT 04
#define EATFLG_Q    02
#define EATFLG_P    01

typedef struct _HanjpEater HanjpEater;

HanjpEater* eater_new(const char* keyboard);
void eater_delete(HanjpEater* eater);
int eater_push(HanjpEater* eater, ucschar ch, ucschar* outer, int outer_length);
const ucschar* eater_get_preedit(HanjpEater* eater);
bool eater_backspace(HanjpEater* eater);
void eater_flush(HanjpEater* eater);

typedef enum {
    HANJP_CHOSEONG_VOID = -1,
    HANJP_CHOSEONG_IEUNG = 0x1110,
    HANJP_CHOSEONG_K = 0x110F,
    HANJP_CHOSEONG_G = 0x1100,
    HANJP_CHOSEONG_S = 0x1109,
    HANJP_CHOSEONG_Z = 0x110C,
    HANJP_CHOSEONG_T = 0x1110,
    HANJP_CHOSEONG_CH = 0x110E,
    HANJP_CHOSEONG_D = 0x1103,
    HANJP_CHOSEONG_J = 0x1140, //PANSIOS
    HANJP_CHOSEONG_N = 0x1102,
    HANJP_CHOSEONG_H = 0x1112,
    HANJP_CHOSEONG_B = 0x1107,
    HANJP_CHOSEONG_P = 0x1110,
    HANJP_CHOSEONG_M = 0x1106,
    HANJP_CHOSEONG_R = 0x1105,
    HANJP_CHOSEONG_OLD_IEUNG = 0x140C
}HanjpChoseongId;

typedef enum {
    HANJP_JUNGSEONG_A = 0x1161,
    HANJP_JUNGSEONG_I = 0x1175,
    HANJP_JUNGSEONG_U = 0x116E,
    HANJP_JUNGSEONG_E = 0x1166,
    HANJP_JUNGSEONG_O = 0x1169,
    HANJP_JUNGSEONG_YA = 0x1163,
    HANJP_JUNGSEONG_YU = 0x1172,
    HANJP_JUNGSEONG_YO = 0x116D,
    HANJP_JUNGSEONG_YE = 0x1168,
    HANJP_JUNGSEONG_OA = 0x116A, // ㅘ
    HANJP_JUNGSEONG_OE = 0x116C, // ㅚ
    HANJP_JUNGSEONG_UA = 0x, 
    HANJP_JUNGSEONG_UI = 0x1174, // ㅢ
    HANJP_JUNGSEONG_UE = 0x1170, // ㅞ
    HANJP_JUNGSEONG_UO = 0x116F, // ㅝ
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
