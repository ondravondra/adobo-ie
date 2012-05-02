
# this script is used to include the library in a project
# use this to only include definitions for useful static libraries
# it doesn't add any copy dependencies

set(libcomhelper_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/libcomhelper)
set(libcomhelper_LIB "libcomhelper.lib")
set(libcomhelper_LIB_DIR_DEBUG "${CMAKE_CURRENT_LIST_DIR}/_lib/${outdir_cfg_debug}")
set(libcomhelper_LIB_DIR_RELEASE "${CMAKE_CURRENT_LIST_DIR}/_lib/${outdir_cfg_release}")

add_library(libcomhelper STATIC IMPORTED)

set_property(TARGET libcomhelper PROPERTY IMPORTED_LOCATION_DEBUG "${libcomhelper_LIB_DIR_DEBUG}/${libcomhelper_LIB}")
set_property(TARGET libcomhelper PROPERTY IMPORTED_LOCATION_RELEASE "${libcomhelper_LIB_DIR_RELEASE}/${libcomhelper_LIB}")

set(libsalsitaextension_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/libsalsitaextension)
set(libsalsitaextension_LIB "libsalsitaextension.lib")
set(libsalsitaextension_LIB_DIR_DEBUG "${CMAKE_CURRENT_LIST_DIR}/_lib/${outdir_cfg_debug}")
set(libsalsitaextension_LIB_DIR_RELEASE "${CMAKE_CURRENT_LIST_DIR}/_lib/${outdir_cfg_release}")

add_library(libsalsitaextension STATIC IMPORTED)

set_property(TARGET libsalsitaextension PROPERTY IMPORTED_LOCATION_DEBUG "${libsalsitaextension_LIB_DIR_DEBUG}/${libsalsitaextension_LIB}")
set_property(TARGET libsalsitaextension PROPERTY IMPORTED_LOCATION_RELEASE "${libsalsitaextension_LIB_DIR_RELEASE}/${libsalsitaextension_LIB}")

