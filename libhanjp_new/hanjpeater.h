#include <hangul.h>

typedef struct _HanjpEater HanjpEater;

HanjpEater* eater_new(const char* keyboard);
void eater_delete(HanjpEater* eater);
int eater_push(HanjpEater* eater, int ascii, ucschar* outer, int outer_length, HanjpOutputType type);
const ucschar* eater_get_preedit(HanjpEater* eater);
bool eater_backspace(HanjpEater* eater);
void eater_flush(HanjpEater* eater);
bool eater_is_empty(HanjpEater* eater);

typedef enum {
    HANJP_CHOSEONG_IEUNG = 0x110B,
    HANJP_CHOSEONG_KHIEUKH = 0x110F,
    HANJP_CHOSEONG_KIEYEOK = 0x1100,
    HANJP_CHOSEONG_SIOS = 0x1109,
    HANJP_CHOSEONG_CIEUC = 0x110C,
    HANJP_CHOSEONG_THIEUTH = 0x1110,
    HANJP_CHOSEONG_CHIEUCH = 0x110E,
    HANJP_CHOSEONG_TIKEUT = 0x1103,
    HANJP_CHOSEONG_PANSIOS = 0x1140,
    HANJP_CHOSEONG_NIEUN = 0x1102,
    HANJP_CHOSEONG_HIEUH = 0x1112,
    HANJP_CHOSEONG_PIEUP = 0x1107,
    HANJP_CHOSEONG_PHIEUPH = 0x1111,
    HANJP_CHOSEONG_MIEUM = 0x1106,
    HANJP_CHOSEONG_RIEUL = 0x1105,
    HANJP_CHOSEONG_OLD_IEUNG = 0x140C,
    HANJP_JONGSEONG_SSANGSIOS = 0x110A,
    HANJP_JONGSEONG_SSANGNIEUN = 0x1114
}HanjpChoseongId;

typedef enum {
    HANJP_JUNGSEONG_A = 0x1161,
    HANJP_JUNGSEONG_I = 0x1175,
    HANJP_JUNGSEONG_U = 0x116E,
    HANJP_JUNGSEONG_EU = 0x1173,
    HANJP_JUNGSEONG_E = 0x1166,
    HANJP_JUNGSEONG_AE = 0x1162,
    HANJP_JUNGSEONG_O = 0x1169,
    HANJP_JUNGSEONG_YA = 0x1163,
    HANJP_JUNGSEONG_YU = 0x1172,
    HANJP_JUNGSEONG_YO = 0x116D,
    HANJP_JUNGSEONG_YE = 0x1168,
    HANJP_JUNGSEONG_WA = 0x116A, // ㅘ
} HanjpJungseongId;
