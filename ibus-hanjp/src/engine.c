/* vim:set et sts=4: */

#include <hanjp.h>
#include <ibus.h>
#include "engine.h"

typedef struct _IBusHanjpEngine IBusHanjpEngine;
typedef struct _IBusHanjpEngineClass IBusHanjpEngineClass;

enum {
    INPUT_MODE_EN,
    INPUT_MODE_JP,
    INPUT_MODE_KR
};

struct _IBusHanjpEngine {
	IBusEngine parent;

    /* members */
    HanjpInputContext* context;
};

struct _IBusHanjpEngineClass {
	IBusEngineClass parent;
};

/* functions prototype */
static void	ibus_hanjp_engine_class_init	(IBusHanjpEngineClass	*klass);
static void	ibus_hanjp_engine_init		(IBusHanjpEngine		*engine);
static GObject*
                ibus_hanjp_engine_constructor
                                            (GType                   type,
                                             guint                   n_construct_params,
                                             GObjectConstructParam  *construct_params);
static void	ibus_hanjp_engine_destroy	(IBusHanjpEngine		*engine);
static gboolean ibus_hanjp_engine_process_key_event
                                            (IBusEngine             *engine,
                                             guint               	 keyval,
                                             guint               	 keycode,
                                             guint               	 modifiers);
static void ibus_hanjp_engine_focus_in    (IBusEngine             *engine);
static void ibus_hanjp_engine_focus_out   (IBusEngine             *engine);
static void ibus_hanjp_engine_reset       (IBusEngine             *engine);
static void ibus_hanjp_engine_enable      (IBusEngine             *engine);
static void ibus_hanjp_engine_disable     (IBusEngine             *engine);
static void ibus_hanjp_set_cursor_location (IBusEngine             *engine,
                                             gint                    x,
                                             gint                    y,
                                             gint                    w,
                                             gint                    h);
static void ibus_hanjp_engine_set_capabilities
                                            (IBusEngine             *engine,
                                             guint                   caps);
static void ibus_hanjp_engine_page_up     (IBusEngine             *engine);
static void ibus_hanjp_engine_page_down   (IBusEngine             *engine);
static void ibus_hanjp_engine_cursor_up   (IBusEngine             *engine);
static void ibus_hanjp_engine_cursor_down (IBusEngine             *engine);
static void ibus_hanjp_engine_property_activate  (IBusEngine             *engine,
                                             const gchar            *prop_name,
                                             gint                    prop_state);
static void ibus_hanjp_engine_property_show
					(IBusEngine             *engine,
                                             const gchar            *prop_name);
static void ibus_hanjp_engine_property_hide
					(IBusEngine             *engine,
                                             const gchar            *prop_name);

static void ibus_hanjp_engine_candidate_clicked(IBusEngine *engine,
                                                guint       index,
                                                guint       button,
                                                guint       state);
static void ibus_hanjp_engine_commit_string (IBusHanjpEngine *hanjp,
                                                const gchar       *string);
static void ibus_hanjp_engine_update (IBusHanjpEngine *hanjp);

// Commits string to client
static void ibus_hanjp_engine_flush        (IBusHanjpEngine       *hanjp);
static void ibus_hanjp_engine_clear_preedit_text
                                            (IBusHanjpEngine       *hanjp);
static void ibus_hanjp_engine_update_preedit_text
                                            (IBusHanjpEngine       *hanjp);
static gboolean ibus_hanjp_engine_has_preedit
                                            (IBusHanjpEngine       *hanjp);
static void ibus_hanjp_engine_switch_input_mode
                                            (IBusHanjpEngine       *hanjp);
static void ibus_hanjp_engine_set_input_mode
                                            (IBusHanjpEngine       *hanjp,
                                             int                     input_mode);
static IBusText*
            ibus_hanjp_engine_get_input_mode_symbol
                                            (IBusHanjpEngine       *hanjp,
                                             int                     input_mode);
static IBusEngineClass *parent_class = NULL;

static ucschar_len(ucschar *str)
{
    int i;

    for(i=0; str[i] && i<64; i++);

    return i;
}


void ibus_hanjp_init(IBusBus *bus)
{
    hanjp_init();
}

void ibus_hanjp_exit(void)
{
    hanjp_fini();
}

GType ibus_hanjp_engine_get_type(void)
{
    static GType type;
    
    static const GTypeInfo type_info = { // Class Type Info
        sizeof(IBusHanjpEngineClass),

        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,

        (GClassInitFunc) ibus_hanjp_engine_class_init,
        (GClassFinalizeFunc) NULL,
        NULL,

        sizeof(IBusHanjpEngine),
        0,
        (GInstanceInitFunc) ibus_hanjp_engine_init,

        NULL
    };

    type = g_type_register_static (IBUS_TYPE_ENGINE,
                                "IBusHanjpEngine",
                                &type_info,
                                (GTypeFlags) 0);

    return type;
}

// Class Init function
static void
ibus_hanjp_engine_class_init (IBusHanjpEngineClass *klass)
{
	IBusObjectClass *ibus_object_class = IBUS_OBJECT_CLASS (klass);
	IBusEngineClass *engine_class = IBUS_ENGINE_CLASS (klass);

    parent_class = (IBusEngineClass *) g_type_class_peek_parent (klass);

	ibus_object_class->destroy = (IBusObjectDestroyFunc) ibus_hanjp_engine_destroy;

    engine_class->process_key_event = ibus_hanjp_engine_process_key_event;

    //engine_class->reset = ibus_hanjp_engine_reset;
    //engine_class->enable = ibus_hanjp_engine_enable;
    //engine_class->disable = ibus_hanjp_engine_disable;

    //engine_class->focus_in = ibus_hanjp_engine_focus_in;
    //engien_class->focus_out = ibus_hanjp_engine_focus_out;

    //engine_class->cursor_up = ibus_hanjp_engine_cursor_up;
    //engien_class->cursor_down = ibus_hanjp_engine_cursor_down;

    //engien_class->candidate_clicked = ibus_hanjp_engine_candidate_clicked;
}

// Engine init function
static void
ibus_hanjp_engine_init (IBusHanjpEngine *hanjp)
{
    hanjp->context = hanjp_ic_new("2hj");
}



// Engine object destroy func
static void
ibus_hanjp_engine_destroy (IBusHanjpEngine *hanjp)
{
    if(hanjp->context){
            hanjp_ic_delete(hanjp->context);
            hanjp->context = NULL;
    }

	((IBusObjectClass *) parent_class)->destroy ((IBusObject *)hanjp);
}

static void
ibus_hanjp_engine_update_preedit_text (IBusHanjpEngine *hanjp)
{
    const ucschar *hjic_preedit;
    IBusText *text;
    int preedit_len;

    hjic_preedit = hanjp_ic_get_preedit_string (hanjp->context);
    preedit_len = ucschar_len(hjic_preedit);

    if (hjic_preedit[0]) {
        text = ibus_text_new_from_ucs4 ((gunichar*)hjic_preedit);
        ibus_text_append_attribute (text, IBUS_ATTR_TYPE_UNDERLINE,
                IBUS_ATTR_UNDERLINE_SINGLE, 0, preedit_len);
        ibus_text_append_attribute (text, IBUS_ATTR_TYPE_FOREGROUND,
                0x00ffffff, preedit_len, -1);
        ibus_text_append_attribute (text, IBUS_ATTR_TYPE_BACKGROUND,
                0x00000000, preedit_len, -1);
        ibus_engine_update_preedit_text_with_mode ((IBusEngine *)hanjp,
                                                   text,
                                                   ibus_text_get_length (text),
                                                   TRUE,
                                                   IBUS_ENGINE_PREEDIT_COMMIT);
    } else {
        text = ibus_text_new_from_static_string ("");
        ibus_engine_update_preedit_text ((IBusEngine *)hanjp, text, 0, FALSE);
    }
}

static void
ibus_hanjp_engine_commit_string (IBusHanjpEngine *hanjp,
                                   const gchar       *string)
{
    IBusText *text;
    text = ibus_text_new_from_static_string (string);
    ibus_engine_commit_text ((IBusEngine *)hanjp, text);
}

// is_alphabet macro checks if keyval is alphabet(a~z or A~Z)
#define is_alphabet(c) (((c) >= IBUS_a && (c) <= IBUS_z) || ((c) >= IBUS_A && (c) <= IBUS_Z))

// function that processes key press event
static gboolean
ibus_hanjp_engine_process_key_event (IBusEngine     *engine,
                                      guint           keyval,
                                      guint           keycode,
                                      guint           modifiers)
{
    IBusHanjpEngine *hanjp = (IBusHanjpEngine *) engine;

    gboolean retval;
    const ucschar *str;

    if (modifiers & IBUS_RELEASE_MASK)
        return FALSE;

    if (keyval == IBUS_Shift_L || keyval == IBUS_Shift_R)
        return FALSE;

    if (modifiers & (IBUS_CONTROL_MASK | IBUS_MOD1_MASK))
        return FALSE;

    if (keyval == IBUS_BackSpace) {
        retval = hanjp_ic_backspace (hanjp->context);
    } else {
        // ignore capslock
        if (modifiers & IBUS_LOCK_MASK) {
            if (keyval >= 'A' && keyval <= 'z') {
                if (isupper(keyval))
                    keyval = tolower(keyval);
                else
                    keyval = toupper(keyval);
            }
        }
        retval = hanjp_ic_process (hanjp->context, keyval);
    }

    str = hanjp_ic_get_commit_string (hanjp->context);
    
    if (str && str[0] != 0) {
        IBusText *text = ibus_text_new_from_ucs4 (str);
        ibus_engine_commit_text (engine, text);
    }

    ibus_hanjp_engine_update_preedit_text (hangul);

    if (!retval)
        ibus_hanjp_engine_flush (hangul);

    return retval;
}

static void ibus_hanjp_engine_flush(IBusHanjpEngine *hanjp)
{
    hanjp_ic_flush(hanjp->context);
}
