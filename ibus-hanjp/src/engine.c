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
                                            (IBusHanjpEngine       *hanjp,
                                             int                     input_mode);
static IBusEngineClass *parent_class = NULL;


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

	((IBusObjectClass *) parent_class)->destroy ((IBusObject *)hanjp);
}

static void
ibus_hanjp_engine_update_preedit (IBusHanjpEngine *hanjp)
{
    const ucschar *hic_preedit;
    IBusText *text;
    // gint retval;

    // hic_preedit = hanjp_ic_get_preedit_string (hanjp->context);
    // text->attrs = ibus_attr_list_new ();

    if (hanjp->preedit->len > 0) {
        // IBusPreeditFocusMode preedit_option = IBUS_ENGINE_PREEDIT_COMMIT;
        // retval = hanjp_dict_check (dict, hanjp->preedit->str, hanjp->preedit->len);
      
        // if (retval != NULL)
        //     preedit_option = IBUS_ENGINE_PREEDIT_CLEAR;
        
        // ibus_attr_list_append (text->attrs,
        //                     ibus_attr_underline_new (IBUS_ATTR_UNDERLINE_SINGLE, 0, hanjp->preedit->len));
        // ibus_attr_list_append (text->attrs,
        //                     ibus_attr_foreground_new (0xff0000, 0, hanjp->preedit->len));
        // ibus_attr_list_append (text->attrs,
        //                     ibus_attr_background_new (0x000000, 0, hanjp->preedit->len));
        text = ibus_text_new_from_static_string (hanjp->preedit->str);
        ibus_engine_update_preedit_text (
            (IBusEngine *)hanjp, 
            text, 
            // hanjp->cursor_pos, 
            ibus_text_get_length (text),
            TRUE);
    } else {
        text = ibus_text_new_from_static_string ("");
        ibus_engine_update_preedit_text (
            (IBusEngine *)hanjp, 
            text, 
            0,
            TRUE);
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

static void ibus_hanjp_engine_update_lookup_table(IBusHanjpEngine *hanjp)
{

}

// is_alphabet macro checks if keyval is alphabet(a~z or A~Z)
#define is_alphabet(c) (((c) >= IBUS_a && (c) <= IBUS_z) || ((c) >= IBUS_A && (c) <= IBUS_Z))

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

    modifiers &= (IBUS_CONTROL_MASK | IBUS_MOD1_MASK);

    if (modifiers == IBUS_CONTROL_MASK && keyval == IBUS_s) {
        ibus_hanjp_engine_update_lookup_table (hanjp);
        return TRUE;
    }

    if (modifiers != 0) {
        if (hanjp->preedit->len == 0)
            return FALSE;
        else
            return TRUE;
    }


    switch (keyval) {
    case IBUS_space:
        g_string_append (hanjp->preedit, " ");
        return ibus_hanjp_engine_commit_preedit (hanjp);
    case IBUS_Return:
        return ibus_hanjp_engine_commit_preedit (hanjp);

    case IBUS_Escape:
        if (hanjp->preedit->len == 0)
            return FALSE;

        g_string_assign (hanjp->preedit, "");
        hanjp->cursor_pos = 0;
        ibus_hanjp_engine_update (hanjp);
        return TRUE;        

    case IBUS_Left:
        if (hanjp->preedit->len == 0)
            return FALSE;
        if (hanjp->cursor_pos > 0) {
            hanjp->cursor_pos --;
            ibus_hanjp_engine_update (hanjp);
        }
        return TRUE;

    case IBUS_Right:
        if (hanjp->preedit->len == 0)
            return FALSE;
        if (hanjp->cursor_pos < hanjp->preedit->len) {
            hanjp->cursor_pos ++;
            ibus_hanjp_engine_update (hanjp);
        }
        return TRUE;
    
    case IBUS_Up:
        if (hanjp->preedit->len == 0)
            return FALSE;
        if (hanjp->cursor_pos != 0) {
            hanjp->cursor_pos = 0;
            ibus_hanjp_engine_update (hanjp);
        }
        return TRUE;

    case IBUS_Down:
        if (hanjp->preedit->len == 0)
            return FALSE;
        
        if (hanjp->cursor_pos != hanjp->preedit->len) {
            hanjp->cursor_pos = hanjp->preedit->len;
            ibus_hanjp_engine_update (hanjp);
        }
        
        return TRUE;
    
    case IBUS_BackSpace:
        if (hanjp->preedit->len == 0)
            return FALSE;
        if (hanjp->cursor_pos > 0) {
            hanjp->cursor_pos --;
            g_string_erase (hanjp->preedit, hanjp->cursor_pos, 1);
            ibus_hanjp_engine_update (hanjp);
        }
        return TRUE;
    
    case IBUS_Delete:
        if (hanjp->preedit->len == 0)
            return FALSE;
        if (hanjp->cursor_pos < hanjp->preedit->len) {
            g_string_erase (hanjp->preedit, hanjp->cursor_pos, 1);
            ibus_hanjp_engine_update (hanjp);
        }
        return TRUE;
    }

    if (is_alphabet (keyval)) {
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
