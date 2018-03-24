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
    GString *preedit;
    gint cursor_pos;

    IBusLookupTable *table;
};

struct _IBusHanjpEngineClass {
	IBusEngineClass parent;
};

/* functions prototype */
static void	ibus_hanjp_engine_class_init	(IBusHanjpEngineClass	*klass);
static void	ibus_hanjp_engine_init		(IBusHanjpEngine		*engine);
static GObject*
                ibus_hangul_engine_constructor
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

// Commits string to client
static void ibus_hanjp_engine_flush        (IBusHanjpEngine       *hanjp);
static void ibus_hanjp_engine_clear_preedit_text
                                            (IBusHanjpEngine       *hanjp);
static void ibus_hanjp_engine_update_preedit_text
                                            (IBusHanjpEngine       *hanjp);

static void ibus_hanjp_engine_update_lookup_table
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
                                            (IBusHanjpEngine       *hangul,
                                             int                     input_mode);

static EnchantBroker *broker = NULL;
static EnchantDict *dict = NULL;

static const GTypeInfo type_info = { // Class Type Info
    sizeof(IBusHangulEngineClass),

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

GType ibus_hanjp_engine_get_type(void)
{
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

	ibus_object_class->destroy = (IBusObjectDestroyFunc) ibus_hanjp_engine_destroy;

        engine_class->process_key_event = ibus_hanjp_engine_process_key_event;

        engine_class->reset = ibus_hanjp_engine_reset;
        engine_class->enable = ibus_hanjp_engine_enable;
        engine_class->disable = ibus_hanjp_engine_disable;

        engine_class->focus_in = ibus_hanjp_engine_focus_in;
        engien_class->focus_out = ibus_hanjp_engine_focus_out;

        engine_class->cursor_up = ibus_hanjp_engine_cursor_up;
        engien_class->cursor_down = ibus_hanjp_engine_cursor_down;

        engien_class->candidate_clicked = ibus_hanjp_engine_candidate_clicked;
}

// Engine init function
static void
ibus_hanjp_engine_init (IBusHanjpEngine *hanjp)
{
        /*
    if (broker == NULL) {
        broker = hanjp_broker_init ();
        dict = hanjp_broker_request_dict (broker, "en");
    }
        */
        // Init Context
        hanjp->context = hanjp_ic_new("2hj");

        hanjp->preedit = g_string_new ("");
        hanjp->cursor_pos = 0;
        hanjp->table = ibus_lookup_table_new (9, 0, TRUE, TRUE);
        g_object_ref_sink (hanjp->table);
}

// Engine object destroy func
static void
ibus_hanjp_engine_destroy (IBusHanjpEngine *hanjp)
{
        if (hanjp->preedit) {
                g_string_free (hanjp->preedit, TRUE);
                hanjp->preedit = NULL;
        }

        if (hanjp->table) {
                g_object_unref (hanjp->table);
                hanjp->table = NULL;
        }

        if(hanjp->context){
                hanjp_ic_delete(hanjp->context);
                hanjp->context = NULL;
        }

	((IBusObjectClass *) ibus_hanjp_engine_parent_class)->destroy ((IBusObject *)hanjp);
}

static void
ibus_hanjp_engine_update_lookup_table (IBusHanjpEngine *hanjp)
{
    gchar ** sugs;
    gint n_sug, i;
    gboolean retval;

    if (hanjp->preedit->len == 0) {
        ibus_engine_hide_lookup_table ((IBusEngine *) hanjp);
        return;
    }

    ibus_lookup_table_clear (hanjp->table);
    
    sugs = hanjp_dict_suggest (dict,
                                 hanjp->preedit->str,
                                 hanjp->preedit->len,
                                 &n_sug);

    if (sugs == NULL || n_sug == 0) {
        ibus_engine_hide_lookup_table ((IBusEngine *) hanjp);
        return;
    }

    for (i = 0; i < n_sug; i++) {
        ibus_lookup_table_append_candidate (hanjp->table, ibus_text_new_from_string (sugs[i]));
    }

    ibus_engine_update_lookup_table ((IBusEngine *) hanjp, hanjp->table, TRUE);

    if (sugs)
        hanjp_dict_free_suggestions (dict, sugs);
}

static void
ibus_hanjp_engine_update_preedit (IBusHanjpEngine *hanjp)
{
    const ucschar *hic_preedit;
    IBusText *text;
    gint retval;

    hic_preedit = hanjp_ic_get_preedit_string (hanjp->context);
    // text->attrs = ibus_attr_list_new ();


    if (hanjp->preedit->len > 0) {
        IBusPreeditFocusMode preedit_option = IBUS_ENGINE_PREEDIT_COMMIT;
        // retval = hanjp_dict_check (dict, hanjp->preedit->str, hanjp->preedit->len);
        
        if (retval != NULL)
            preedit_option = IBUS_ENGINE_PREEDIT_CLEAR;
        /*
        ibus_attr_list_append (text->attrs,
                            ibus_attr_underline_new (IBUS_ATTR_UNDERLINE_SINGLE, 0, hanjp->preedit->len));
        ibus_attr_list_append (text->attrs,
                            ibus_attr_foreground_new (0xff0000, 0, hanjp->preedit->len));
        ibus_attr_list_append (text->attrs,
                            ibus_attr_background_new (0x000000, 0, hanjp->preedit->len));
        */
    } else {
        text = ibus_text_new_from_static_string (hanjp->preedit->str);
        ibus_engine_update_preedit_text ((IBusEngine *)hanjp, text, hanjp->cursor_pos, TRUE);
    }

}

/* commit preedit to client and update preedit */
static gboolean
ibus_hanjp_engine_commit_preedit (IBusHanjpEngine *hanjp)
{
    if (hanjp->preedit->len == 0)
        return FALSE;
    
    ibus_hanjp_engine_commit_string (hanjp, hanjp->preedit->str);
    g_string_assign (hanjp->preedit, "");
    hanjp->cursor_pos = 0;

    ibus_hanjp_engine_update (hanjp);

    return TRUE;
}


static void
ibus_hanjp_engine_commit_string (IBusHanjpEngine *hanjp,
                                   const gchar       *string)
{
    IBusText *text;
    text = ibus_text_new_from_static_string (string);
    ibus_engine_commit_text ((IBusEngine *)hanjp, text);
}

static void
ibus_hanjp_engine_update (IBusHanjpEngine *hanjp)
{
    ibus_hanjp_engine_update_preedit (hanjp);
    ibus_engine_hide_lookup_table ((IBusEngine *)hanjp);
}

#define is_alpha(c) (((c) >= IBUS_a && (c) <= IBUS_z) || ((c) >= IBUS_A && (c) <= IBUS_Z))

// function that processes key press event
static gboolean 
ibus_hanjp_engine_process_key_event (IBusEngine *engine,
                                       guint       keyval,
                                       guint       keycode,
                                       guint       modifiers)
{
    IBusText *text;
    IBusHanjpEngine *hanjp = (IBusHanjpEngine *)engine;

    if (modifiers & IBUS_RELEASE_MASK)
        return FALSE;

    if (modifiers != 0) {
        if (hanjp->preedit->len == 0)
            return FALSE;
    }

    switch (keyval) {
    case IBUS_KEY_space:
            //TODO - Replace with kanji conversion process
        g_string_append (hanjp->preedit, " ");
        return ibus_hanjp_engine_commit_preedit (hanjp);
    case IBUS_KEY_Return:
        return ibus_hanjp_engine_commit_preedit (hanjp);
    case IBUS_KEY_Escape:
        if (hanjp->preedit->len == 0)
            return FALSE;

        g_string_assign (hanjp->preedit, "");
        hanjp->cursor_pos = 0;
        ibus_hanjp_engine_update (hanjp);
        return TRUE;        

    case IBUS_KEY_Left:
        if (hanjp->preedit->len == 0)
            return FALSE;
        if (hanjp->cursor_pos > 0) {
            hanjp->cursor_pos --;
            ibus_hanjp_engine_update (hanjp);
        }
        return TRUE;

    case IBUS_KEY_Right:
        if (hanjp->preedit->len == 0)
            return FALSE;
        if (hanjp->cursor_pos < hanjp->preedit->len) {
            hanjp->cursor_pos ++;
            ibus_hanjp_engine_update (hanjp);
        }
        return TRUE;
    
    case IBUS_KEY_Up:
        if (hanjp->preedit->len == 0)
            return FALSE;
        if (hanjp->cursor_pos != 0) {
            hanjp->cursor_pos = 0;
            ibus_hanjp_engine_update (hanjp);
        }
        return TRUE;

    case IBUS_KEY_Down:
        if (hanjp->preedit->len == 0)
            return FALSE;
        
        if (hanjp->cursor_pos != hanjp->preedit->len) {
            hanjp->cursor_pos = hanjp->preedit->len;
            ibus_hanjp_engine_update (hanjp);
        }
        
        return TRUE;
    
    case IBUS_KEY_BackSpace:
        if (hanjp->preedit->len == 0)
            return FALSE;
        if (hanjp->cursor_pos > 0) {
                hanjp->cursor_pos --;
                g_string_erase (hanjp->preedit, hanjp->cursor_pos, 1);
                hanjp_ic_backspace(hanjp->context);
                ibus_hanjp_engine_update (hanjp);
        }
        return TRUE;

    case IBUS_KEY_Delete:
        if (hanjp->preedit->len == 0)
            return FALSE;
        if (hanjp->cursor_pos < hanjp->preedit->len) {
            g_string_erase (hanjp->preedit, hanjp->cursor_pos, 1);
            ibus_hanjp_engine_update (hanjp);
        }
        return TRUE;
    }

    if (is_alpha (keyval)) {
        g_string_insert_c (hanjp->preedit,
                           hanjp->cursor_pos,
                           keyval);

        hanjp->cursor_pos ++;
        ibus_hanjp_engine_update (hanjp);
        
        return TRUE;
    }

    return FALSE;
}

// Process candidate click event
static void ibus_hanjp_engine_candidate_clicked(IBusEngine *engine,
                                                guint       index,
                                                guint       button,
                                                guint       state)
{

}
