
# this script is used to include the library in a project

include("${CMAKE_CURRENT_LIST_DIR}/staticlibs.cmake")

set(magpie_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/magpie)
set(magpie_DLL "magpie.dll")
set(magpie_DLL_DIR_DEBUG "${CMAKE_CURRENT_LIST_DIR}/_bin/${outdir_cfg_debug}")
set(magpie_DLL_DIR_RELEASE "${CMAKE_CURRENT_LIST_DIR}/_bin/${outdir_cfg_release}")

add_library(magpie SHARED IMPORTED)

set_property(TARGET magpie PROPERTY IMPORTED_LOCATION_DEBUG "${magpie_DLL_DIR_DEBUG}/${magpie_DLL}")
set_property(TARGET magpie PROPERTY IMPORTED_LOCATION_RELEASE "${magpie_DLL_DIR_RELEASE}/${magpie_DLL}")

set(dependecy_list_debug ${dependecy_list_debug} "${magpie_DLL_DIR_DEBUG}/${magpie_DLL}")
set(dependecy_list_release ${dependecy_list_release} "${magpie_DLL_DIR_RELEASE}/${magpie_DLL}")

