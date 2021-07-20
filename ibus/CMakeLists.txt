find_package(Gettext REQUIRED)
find_package(PkgConfig)
pkg_check_modules(IBUS REQUIRED ibus-1.0)
pkg_check_modules(hangul REQUIRED libhangul)

configure_file(config.h.in config.h)
configure_file(hanjp.xml.in hanjp.xml)
add_executable(ibus-engine-hanjp main.cpp engine.cpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${IBUS_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/libhanjp)
target_link_libraries(ibus-engine-hanjp ${IBUS_LIBRARIES} ${PROJECT_BINARY_DIR}/libhanjp/libhanjp.a ${hangul_LIBRARIES})

install(TARGETS ibus-engine-hanjp DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
install(FILES hanjp.xml ibus-hanjp.svg DESTINATION ${CMAKE_INSTALL_SHAREDSTATEDIR})