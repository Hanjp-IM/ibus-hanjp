#ifndef HANJP_ENGINE
#define HANJP_ENGINE

#include <ibus.h>
#include <glib.h>
#include "config.h"

GType ibus_hanjp_engine_get_type();
void ibus_hanjp_init(IBusBus *bus);
void ibus_hanjp_exit();

#endif