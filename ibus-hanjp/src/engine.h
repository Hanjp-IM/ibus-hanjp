/* vim:set et sts=4: */
#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <ibus.h>

#define IBUS_TYPE_HANJP_ENGINE	\
	(ibus_hanjp_engine_get_type ())

GType   ibus_hanjp_engine_get_type    (void);

void	ibus_hanjp_init(IBusBus *bus);
void	ibus_hanjp_exit(void);

#endif
