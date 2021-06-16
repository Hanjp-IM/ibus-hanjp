#include "hanjp.h"
#include <ibus.h>
#include "engine.h"
#include <memory>

using namespace std;
using namespace Hanjp;

enum InputMode {
	INPUT_MODE_EN,
	INPUT_MODE_JP,
	INPUT_MODE_KR
};

struct IBusHanjpEngine {
	IBusEngine parent;
    InputMode input_mode;
    unique_ptr<InputContext> context;
};

struct IBusHanjpEngineClass {
	IBusEngineClass parent;
};

void ibus_hanjp_init() {
    init();
}

void ibus_hanjp_exit() {
    fini();
}

static void engine_init(IBusHanjpEngine* hanjp);
static void engine_class_init(IBusHanjpEngineClass *klass);

//event handlers
static gboolean engine_process_key_event(
        IBusEngine *engine,
        guint keyval,
        guint keycode,
        guint state);
static void engine_focus_in(IBusEngine *engine);
static void engine_focus_out(IBusEngine *engine);
static void engine_reset(IBusEngine *engine);
static void engine_enable(IBusEngine *engine);
static void engine_disable(IBusEngine *engine);
static void engine_cursor_up(IBusEngine *engine);
static void engine_cursor_down(IBusEngine* engine);
static void engine_candidate_clicked(IBusEngine *engine,
        guint   index,
        guint   button,
        guint   state);

static void engine_distroy(IBusEngine *engine);

GType ibus_hanjp_engine_type() {
    static GType type = 0;
    if(type == 0) {
        const GTypeInfo info = {
            sizeof(IBusHanjpEngineClass),

            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,

            (GClassInitFunc) engine_init,
            (GClassFinalizeFunc) NULL,
            NULL,

            sizeof(IBusHanjpEngine),
            0,
            (GInstanceInitFunc) engine_init,    
        };
        type = g_type_register_static(
                IBUS_TYPE_ENGINE,
                "IBusHanjpEngine",
                &info, (GTypeFlags) 0);
    }
    return type;
}

static void engine_init(IBusHanjpEngine* hanjp) {
    hanjp->context = make_unique<InputContext>();
    hanjp->input_mode = INPUT_MODE_JP;
}

static void engine_class_init(IBusHanjpEngineClass *klass) {
    IBusEngineClass *engine_class = IBUS_ENGINE_CLASS(klass);

    engine_class->process_key_event = engine_process_key_event;
    engine_class->reset = engine_reset;
    engine_class->enable = engine_enable;
    engine_class->disable = engine_disable;
    engine_class->focus_in = engine_focus_in;
    engine_class->focus_out = engine_focus_out;
    engine_class->cursor_up = engine_cursor_up;
    engine_class->cursor_down = engine_cursor_down;
    engine_class->candidate_clicked = engine_candidate_clicked;
}

static gboolean engine_process_key_event(IBusEngine *engine,
        guint keyval,
        guint keycode,
        guint state)
{
    IBusHanjpEngine *hanjp = (IBusHanjpEngine *)engine;
    IBusText *text;
    const guint mask = IBUS_RELEASE_MASK | IBUS_SHIFT_MASK | IBUS_MOD1_MASK;
    const u32string& preedit = hanjp->context->get_preedit_string();
    const u32string& committed = hanjp->context->get_commit_string();

    if(state & mask) {
        return FALSE;
    }

    if(keyval == IBUS_BackSpace) {
        return hanjp->context->backspace();
    }

    if(state & IBUS_LOCK_MASK) {
        if(keyval >= 'A' && keyval <= 'z') {
            const int gap = 'a' - 'A';
            if(keyval <= 'Z') {
                keyval += gap;
            }
            else {
                keyval -= gap;
            }
        }
    }

    hanjp->context->process(keyval);

    //Commit text
    if(!committed.empty()) {
        text = ibus_text_new_from_ucs4((const gunichar *)preedit.data());
        ibus_engine_commit_text(engine, text);
    }

    //preedit string
    if (!preedit.empty()) {
        text = ibus_text_new_from_ucs4((const gunichar *)preedit.data());
        ibus_text_append_attribute (text, IBUS_ATTR_TYPE_UNDERLINE,
                IBUS_ATTR_UNDERLINE_SINGLE, 0, preedit.size());
        ibus_text_append_attribute (text, IBUS_ATTR_TYPE_FOREGROUND,
                0x00ffffff, preedit.size(), -1);
        ibus_text_append_attribute (text, IBUS_ATTR_TYPE_BACKGROUND,
                0x00000000, preedit.size(), -1);
        ibus_engine_update_preedit_text_with_mode ((IBusEngine *)hanjp,
                text, ibus_text_get_length(text),
                TRUE, IBUS_ENGINE_PREEDIT_COMMIT);
    }
    else {
        text = ibus_text_new_from_static_string ("");
        ibus_engine_update_preedit_text ((IBusEngine *)hanjp, text, 0, FALSE);
    }

    return TRUE;
}
