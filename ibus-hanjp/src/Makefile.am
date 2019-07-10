AM_CFLAGS = \
	@IBUS_CFLAGS@ \
	@HANJP_CFLAGS@ \
	-DPKGDATADIR=\"$(pkgdatadir)\" \
	$(NULL)
AM_LDFLAGS = \
	@IBUS_LIBS@ \
	@HANJP_CFLAGS@ \
	$(NULL)

libexec_PROGRAMS = ibus-engine-hanjp

ibus_engine_hanjp_SOURCES = \
	main.c \
	engine.c \
	engine.h \
	i18n.h \
	$(NULL)

ibus_engine_hanjp_CFLAGS = \
	@IBUS_CFLAGS@ \
	@HANJP_CFLAGS@ \
	-DPKGDATADIR=\"$(pkgdatadir)\" \
	-DLOCALEDIR=\"$(localedir)\" \
	-DLIBEXECDIR=\"$(libexecdir)\" \
	-DIBUSHANGUL_DATADIR=\"$(datadir)/ibus-hangul\" \
	$(NULL)

ibus_engine_hanjp_LDADD = \
	@IBUS_LIBS@ \
	@HANJP_LIBS@
	$(NULL)

component_DATA = \
	hanjp.xml \
	$(NULL)
componentdir = @datadir@/ibus/component

EXTRA_DIST = \
	hanjp.xml.in \
	$(NULL)

CLEANFILES = \
	hanjp.xml \
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
