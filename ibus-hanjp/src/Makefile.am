libexec_PROGRAMS = ibus-engine-hanjp

AM_CFLAGS = \
	@IBUS_CFLAGS@ \
	@HANJP_CFLAGS@ \
	-DPKGDATADIR=\"$(pkgdatadir)\" \
	$(NULL)
AM_LDFLAGS = \
	@IBUS_LIBS@ \
	@HANJP_CFLAGS@ \
	$(NULL)

ibus_engine_hanjp_SOURCES = \
	main.c \
	engine.c \
	engine.h \
	$(NULL)
ibus_engine_hanjp_CFLAGS = \
	@IBUS_CFLAGS@ \
	@HANJP_CFLAGS@ \
	-DPKGDATADIR=\"$(pkgdatadir)\"
	$(NULL)
ibus_engine_hanjp_LDFLAGS = \
	@IBUS_LIBS@ \
	@HANJP_LIBS@
	$(NULL)

component_DATA = \
	enchant.xml \
	$(NULL)
componentdir = @datadir@/ibus/component

EXTRA_DIST = \
	enchant.xml.in \
	$(NULL)

CLEANFILES = \
	enchant.xml \
	$(NULL)

hanjp.xml: hanjp.xml.in
	$(AM_V_GEN) \
	( \
		libexecdir=${libexecdir}; \
		pkgdatadir=${pkgdatadir}; \
		s=`cat $<`; \
		eval "echo \"$${s}\""; \
	) > $@

test: ibus-engine-hanjp
	$(builddir)/ibus-engine-hanjp
