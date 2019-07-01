#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ibus.h>
#include <stdlib.h>
#include <locale.h>
#include "i18n.h"
#include "engine.h"

static IBusBus *bus = NULL;
static IBusFactory *factory = NULL;

static void start_component(void);
static void ibus_disconnected_callback(IBusBus  *bus,
                                       gpointer  user_data);

/* options */
static gboolean ibus = FALSE;
static gboolean verbose = FALSE;

int main(gint    argc,
         gchar **argv)
{
	GError *error = NULL;
	GOptionContext *context;

	// Set System Locale
	setlocale(LC_ALL, "");

	bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

	// Init Option contect
	context = g_option_context_new("- ibus hanjp engine component");

	// 옵션 해석 실패시 종료.
	if (!g_option_context_parse (context, &argc, &argv, &error)) {
        	g_print ("Option parsing failed: %s\n", error->message);
        	exit (-1);
    	}

	// 엔진 컴포넌트 시작.
    	start_component ();
   	return 0;
}

// 엔진 컴포넌트 시작 함수
static void start_component(void)
{
	IBusComponent *component;
	IBusConfig *config;
	gboolean result;

	// IBus 초기화
	ibus_init();

	// IBus 버스 초기화
	bus = ibus_bus_new();

	// IBus 데몬과 연결 되었는지 확인
	result = ibus_bus_is_connected(bus);
	if(!result){
		g_warning("Unable to connect to IBus");
		exit(2);
	}

	// IBus config 인스턴스 얻기
	config = ibus_bus_get_config(bus);
	if(config == NULL) {
        	g_warning("Unable to connect to IBus config component");
        	exit(3);
    	}

	// IBus 데몬 연결 끊김 이벤트에 대한 콜백 등록
	g_signal_connect(bus, "disconnected", G_CALLBACK(ibus_disconnected_callback), NULL);

	// IBus Hanjp 엔진 초기화
	ibus_hanjp_init(bus);

	// IBus 컴포넌트 초기화
	component = ibus_component_new("org.ubuntu-kr.IBus.Hanjp",
				       N_("HanJP Input Method"),
				       "0.0.1",
				       "GPL",
				       "Ubuntu Korea Community HanJP-IM Team",
				       "https://github.com/ubuntu-kr/hanjp-im",
				       "",
				       "ibus-hanjp");

	// 초기화한 컴포넌트에 엔진 추가
	ibus_component_add_engine(component, ibus_engine_desc_new("hanjp",
								  N_("HanJP Input Method"),
								  N_("Type Japanese with Korean Hangul"),
								  "jp",
								  "GPL",
								  "Ubuntu Korea Community HanJP-IM Team",
								  "", // 아이콘 파일 경로
								  "us"));

	factory = ibus_factory_new (ibus_bus_get_connection (bus));

    	ibus_factory_add_engine (factory, "hanjp", IBUS_TYPE_HANJP_ENGINE);

    	if (ibus) {
        	ibus_bus_request_name (bus, "org.ubuntu-kr.IBus.Hanjp", 0);
    	}
    	else {
        	ibus_bus_register_component (bus, component);
   	}

    	g_object_unref (component);

	// Start Engine Loop
	ibus_main ();

	// Quit when engine loop end
    	ibus_hanjp_exit ();
}

static void ibus_disconnected_callback(IBusBus  *bus,
                                       gpointer  user_data)
{
	g_debug("bus disconnected");
	ibus_quit();
}
