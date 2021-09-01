#include "hanjpengine.h"
#include "hanjp.h"

typedef struct {
    IBusEngine parent_instance;
    gint input_mode;
    HanjpInputContext *context;
} IBusHanjpEngine;

typedef struct {
    IBusEngineClass parent_class;
} IBusHanjpEngineClass;

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
            (GClassFinalizeFunc) engine_class_fini,
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
engine_class_init(IBusHanjpEngineClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
}

static void 
engine_class_fini(IBusHanjpEngineClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
}

static void
engine_init(IBusHanjpEngine *self)
{

}