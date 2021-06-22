
configure_file(config.h.in config.h)
configure_file(hanjp.xml.in hanjp.xml)
add_executable(ibus-engine-hanjp main.cpp engine.cpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${IBUS_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/libhanjp)
target_link_libraries(ibus-engine-hanjp ${IBUS_LIBRARIES} ${PROJECT_BINARY_DIR}/libhanjp/libhanjp.a ${hangul_LIBRARIES})