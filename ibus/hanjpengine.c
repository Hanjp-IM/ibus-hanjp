#include "hanjpengine.h"
#include "hanjp.h"

typedef struct {
    IBusEngine parent_instance;
    gint input_mode;
    HanjpInputContext *context;
    GArray *preedit;
    GArray *committed;
    GArray *hangul;
} IBusHanjpEngine;

typedef struct {
    IBusEngineClass parent_class;
} IBusHanjpEngineClass;

static IBusEngineClass *parent_class = NULL;

static void engine_init(IBusHanjpEngine* hanjp);
static void engine_class_init(IBusHanjpEngineClass *klass);
static void engine_class_fini(IBusHanjpEngineClass *klass);

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

GType ibus_hanjp_engine_get_type()
{
    static GType type = 0;
    if(type == 0) {
        const GTypeInfo info = {
            sizeof(IBusHanjpEngineClass),
            NULL,       // base_init
            NULL,       // base_finalize
            (GClassInitFunc) engine_class_init,
            NULL,
            NULL,       // class_data
            sizeof(IBusHanjpEngine),
            0,          // n_preallocs
            (GInstanceInitFunc) engine_init
        };
        type = g_type_register_static(IBUS_TYPE_ENGINE, "IBusHanjpEngine", &info, 0);
    }
    return type;
}

static void
engine_dispose(GObject *gobject)
{
    IBusHanjpEngine *engine = (IBusHanjpEngine*) gobject;

    g_clear_object(&engine->context);
    g_clear_object(&engine->preedit);
    g_clear_object(&engine->committed);
    g_clear_object(&engine->hangul);

    G_OBJECT_CLASS(parent_class)->dispose(gobject);
}

static void
engine_class_init(IBusHanjpEngineClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    IBusEngineClass *engine_class = IBUS_OBJECT_CLASS(klass);

    parent_class = (IBusEngineClass*) g_type_class_peek_parent(klass);
    object_class->dispose = engine_dispose;

    engine_class->process_key_event = engine_process_key_event;
}

static void
engine_init(IBusHanjpEngine *self)
{
    self->context = hanjp_ic_new();
    self->input_mode = HANJP_INPUT_MODE_JP;
    self->preedit = hanjp_ic_ref_preedit_string(self->context);
    self->committed = hanjp_ic_ref_hangul_string(self->context);
    self->hangul = hanjp_ic_ref_hangul_string(self->context);
}

static gboolean engine_process_key_event(
        IBusEngine *engine,
        guint keyval,
        guint keycode,
        guint state)
{
    IBusHanjpEngine *hanjp = (IBusHanjpEngine*)engine;
    IBusText *text = NULL;
    const guint mask = IBUS_RELEASE_MASK | IBUS_SHIFT_MASK | IBUS_MOD1_MASK;
    gint am_sig;
    
    if(state & mask) {
        return FALSE;
    }

    if(keyval == IBUS_BackSpace) {
        return hanjp_ic_backspace(hanjp->context);
    }

    if(state & IBUS_LOCK_MASK) {
        if(keyval >= 'A' && keyval <= 'z') {
            if(keyval <= 'Z') {
                keyval += 'a' - 'A';
            }
            else {
                keyval -= 'a' - 'A';
            }
        }
    }

    am_sig = hanjp_ic_process(hanjp->context, keyval);

    switch(am_sig) {
    case 0: // EAT
    case 1: // POP
        text = ibus_text_new_from_ucs4((const gunichar*)hanjp->preedit->data);

        if(hanjp->preedit->len != 0) {
            ibus_text_append_attribute (text, IBUS_ATTR_TYPE_UNDERLINE,
                IBUS_ATTR_UNDERLINE_SINGLE, 0, hanjp->preedit->len);
            ibus_text_append_attribute (text, IBUS_ATTR_TYPE_FOREGROUND,
                    0x00ffffff, hanjp->preedit->len, -1);
            ibus_text_append_attribute (text, IBUS_ATTR_TYPE_BACKGROUND,
                    0x00000000, hanjp->preedit->len, -1);
            ibus_engine_update_preedit_text_with_mode (engine,
                    text, ibus_text_get_length(text),
                    TRUE, IBUS_ENGINE_PREEDIT_COMMIT);
        }
        else{
            ibus_engine_update_preedit_text(engine, text, 0, FALSE);
        }
        break;
    case 2: // FAIL
        text = ibus_text_new_from_ucs4((const gunichar*)hanjp->committed->data);
        ibus_engine_commit_text(engine, text);
        break;
    default:
        return FALSE;
    }

    return TRUE;
}