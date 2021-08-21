#include "hanjpengine.h"
#include <glib/gi18n.h>
#include <locale.h>

/* options */
static gboolean ibus = FALSE;
static gboolean verbose = FALSE;

static const GOptionEntry entries[] = {
	{ "ibus", 'i', 0, G_OPTION_ARG_NONE, &ibus, "component is executed by ibus", NULL },
	{ "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "verbose", NULL },
	{ NULL },
};

static void ibus_disconnected_callback(IBusBus *bus, gpointer user_data) {
	g_debug("bus disconnected");
	ibus_quit();
}

int main(int argc, char **argv)
{
	GError *error = NULL;
	GOptionContext *context = NULL;
	IBusComponent *component = NULL;
	IBusConfig *config = NULL;
	IBusBus *bus = NULL;
	IBusFactory *factory = NULL;

	// Set System Locale
	setlocale(LC_ALL, "");
	bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

	// Init Option contect
	context = g_option_context_new("- ibus hanjp engine component");
	g_option_context_add_main_entries(context, entries, "ibus-hanjp");
	g_option_context_parse(context, &argc, &argv, &error);
	g_option_context_free(context);
	if(error) {
		g_print("Option Paring Failed: %s\n", error->message);
		g_error_free(error);
		return -1;
	}

	//start engine component
	ibus_init();

	bus = ibus_bus_new();
	if(!ibus_bus_is_connected(bus)) {
		g_warning("Unable to connect to IBus");
		return -1;
	}

	config = ibus_bus_get_config(bus);
	if(!config) {
		g_warning("Unable to connect to IBus config component");
		return -1;
	}

	g_signal_connect(bus, "disconnected", G_CALLBACK(ibus_disconnected_callback), NULL);
	
	component = ibus_component_new("org.ubuntu-kr.IBus.Hanjp",
			N_("HanJP Input Method"), "0.1.0",
			"GPL", "Ubuntu Korea Community",
			"https://github.com/ubuntu-kr/hanjp-im", "",
			"ibus-hanjp");
	ibus_component_add_engine(component, 
			ibus_engine_desc_new("hanjp",
				N_("HanJP Input Method"),
				N_("Type Japanese with Korean Hangul"),
				"ja", "GPL", "Ubuntu Korea Community",
				DATADIR"/icon/ibus-hanjp.svg", "us"));
	factory = ibus_factory_new(ibus_bus_get_connection(bus));
	ibus_factory_add_engine(factory, "hanjp", IBUS_TYPE_HANJP_ENGINE);

	if(ibus) {
		ibus_bus_request_name(bus, "org.ubuntu-kr.IBus.Hanjp", 0);
	}
	else {
		ibus_bus_register_component(bus, component);
	}
	g_object_unref(component);
	ibus_main(); //start engine loop

	//end of component
	
   	return 0;
}
