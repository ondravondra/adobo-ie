
# this script is used to include the library in a project

include("${CMAKE_CURRENT_LIST_DIR}/staticlibs.cmake")

set(adobo_DLL_DIR_DEBUG "${CMAKE_CURRENT_LIST_DIR}/_bin/${outdir_cfg_debug}")
set(adobo_DLL_DIR_RELEASE "${CMAKE_CURRENT_LIST_DIR}/_bin/${outdir_cfg_release}")

set(magpie_DLL "magpie.dll")
set(dependecy_list_debug ${dependecy_list_debug} "${adobo_DLL_DIR_DEBUG}/${magpie_DLL}")
set(dependecy_list_release ${dependecy_list_release} "${adobo_DLL_DIR_RELEASE}/${magpie_DLL}")

set(scriptservice_DLL "scriptservice.dll")
set(scriptservice_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/scriptservice/include)
set(dependecy_list_debug ${dependecy_list_debug} "${adobo_DLL_DIR_DEBUG}/${scriptservice_DLL}")
set(dependecy_list_release ${dependecy_list_release} "${adobo_DLL_DIR_RELEASE}/${scriptservice_DLL}")
