include(CTest)
enable_testing()

add_subdirectory(src)
add_subdirectory(icons)
add_subdirectory(test)

find_package(Gettext REQUIRED)
find_package(PkgConfig)
pkg_check_modules(IBUS REQUIRED ibus-1.0)
pkg_check_modules(hangul REQUIRED libhangul)