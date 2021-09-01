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
GType ibus_hanjp_engine_get_type();

G_END_DECLS

#endif // __HANJP_ENGINE_H__