/* vim:set et sts=4: */

#include <hanjp.h>
#include <ibus.h>

#include "engine.h"

typedef struct _IBusHanjpEngine IBusHanjpEngine;
typedef struct _IBusHanjpEngineClass IBusHanjpEngineClass;

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
static void	ibus_hanjp_engine_destroy		(IBusHanjpEngine		*engine);
static gboolean 
			ibus_hanjp_engine_process_key_event
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
static void ibus_hanjp_property_activate  (IBusEngine             *engine,
                                             const gchar            *prop_name,
                                             gint                    prop_state);
static void ibus_hanjp_engine_property_show
											(IBusEngine             *engine,
                                             const gchar            *prop_name);
static void ibus_hanjp_engine_property_hide
											(IBusEngine             *engine,
                                             const gchar            *prop_name);

static void ibus_hanjp_engine_commit_string
                                            (IBusHanjpEngine      *hanjp,
                                             const gchar            *string);
static void ibus_hanjp_engine_update      (IBusHanjpEngine      *hanjp);

static EnchantBroker *broker = NULL;
static EnchantDict *dict = NULL;

static const GTypeInfo type_info = {
    sizeof(IBusHangulEngineClass),

    (GBaseInitFunc) NULL,
    (GBaseFinalizeFunc) NULL,

    (GClassInitFunc) ibus_hanjp_engine_class_init,
    (GClassFinalizeFunc) NULL,
    NULL,

    sizeof(IBusHanjpEngine),
    0,
    (GInstanceInitFunc) ibus_hangul_engine_init,

    NULL
};

GType ibus_hanjp_engine_get_type(void)
{
    GType type;

    type = g_type_register_static (IBUS_TYPE_ENGINE,
                                "IBusHanjpEngine",
                                &type_info,
                                (GTypeFlags) 0);

    return type;
}

static void
ibus_hanjp_engine_class_init (IBusHanjpEngineClass *klass)
{
	IBusObjectClass *ibus_object_class = IBUS_OBJECT_CLASS (klass);
	IBusEngineClass *engine_class = IBUS_ENGINE_CLASS (klass);
	
	ibus_object_class->destroy = (IBusObjectDestroyFunc) ibus_hanjp_engine_destroy;

    engine_class->process_key_event = ibus_hanjp_engine_process_key_event;
}

static void
ibus_hanjp_engine_init (IBusHanjpEngine *hanjp)
{
    if (broker == NULL) {
        broker = hanjp_broker_init ();
        dict = hanjp_broker_request_dict (broker, "en");
    }

    hanjp->preedit = g_string_new ("");
    hanjp->cursor_pos = 0;

    hanjp->table = ibus_lookup_table_new (9, 0, TRUE, TRUE);
    g_object_ref_sink (hanjp->table);
}

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
    IBusText *text;
    gint retval;

    text = ibus_text_new_from_static_string (hanjp->preedit->str);
    text->attrs = ibus_attr_list_new ();
    
    ibus_attr_list_append (text->attrs,
                           ibus_attr_underline_new (IBUS_ATTR_UNDERLINE_SINGLE, 0, hanjp->preedit->len));

    if (hanjp->preedit->len > 0) {
        retval = hanjp_dict_check (dict, hanjp->preedit->str, hanjp->preedit->len);
        if (retval != 0) {
            ibus_attr_list_append (text->attrs,
                               ibus_attr_foreground_new (0xff0000, 0, hanjp->preedit->len));
        }
    }
    
    ibus_engine_update_preedit_text ((IBusEngine *)hanjp,
                                     text,
                                     hanjp->cursor_pos,
                                     TRUE);

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
