add_executable(ibus-engine-hanjp main.cpp engine.cpp)
include_directories(${IBUS_INCLUDE_DIRS} ${hangul_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/libhanjp")
target_link_libraries(ibus-engine-hanjp ${IBUS_LIBRARYS} hanjp)