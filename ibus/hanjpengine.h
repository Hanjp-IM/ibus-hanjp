#ifndef __HANJP_ENGINE_H__
#define __HANJP_ENGINE_H__

#include <ibus.h>
#include <glib-object.h>
#include "config.h"

G_BEGIN_DECLS

enum {
	HANJP_INPUT_MODE_EN,
	HANJP_INPUT_MODE_JP,
	HANJP_INPUT_MODE_KR
};

#define IBUS_TYPE_HANJP_ENGINE ibus_hanjp_engine_get_type()
G_DECLARE_FINAL_TYPE(IBusHanjpEngine, ibus_hanjp_engine, IBUS, HANJPENGINE, IBusEngine)

IBusHanjpEngine *ibus_hanjp_engine_new();

G_END_DECLS

#endif // __HANJP_ENGINE_H__