#include "hanjpengine.h"
#include "hanjp.h"

struct _IBusHanjpEngine {
    IBusEngine parent_instance;
    gint input_mode;
    HanjpInputContext *context;
};

G_DEFINE_TYPE(IBusHanjpEngine, ibus_hanjp_engine, IBUS_TYPE_ENGINE)

static void
hanjp_engine_dispose(GObject *gobject)
{
    g_clear_object(&IBUS_HANJPENGINE(gobject)->context);
    G_OBJECT_CLASS(ibus_hanjp_engine_parent_class)->dispose(gobject);
}

static void
hanjp_engine_finalize(GObject *gobject)
{
    G_OBJECT_CLASS(ibus_hanjp_engine_parent_class)->finalize(gobject);
}

static void
ibus_hanjp_engine_class_init(IBusHanjpEngineClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->dispose = hanjp_engine_dispose;
    object_class->finalize = hanjp_engine_finalize;
}

static void
ibus_hanjp_engine_init(IBusHanjpEngine *self)
{

}