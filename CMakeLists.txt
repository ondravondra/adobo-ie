
cmake_minimum_required(VERSION 2.8.7)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

project("adobo-ie")

if(CMAKE_CL_64)
	set(outdir_cfg_release Release64)
	set(outdir_cfg_debug Debug64)
else(CMAKE_CL_64)
	set(outdir_cfg_release Release32)
	set(outdir_cfg_debug Debug32)
endif(CMAKE_CL_64)

set(outdir_bin_release ${CMAKE_CURRENT_SOURCE_DIR}/_bin/${outdir_cfg_release}/)
set(outdir_bin_debug ${CMAKE_CURRENT_SOURCE_DIR}/_bin/${outdir_cfg_debug}/)
set(outdir_lib_release ${CMAKE_CURRENT_SOURCE_DIR}/_lib/${outdir_cfg_release}/)
set(outdir_lib_debug ${CMAKE_CURRENT_SOURCE_DIR}/_lib/${outdir_cfg_debug}/)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

if(version_build)
	add_definitions("-DVER_NUMBER_BUILD=${version_build}")
endif(version_build)

if(NOT omit_activation_manifests)
	# USE_ACTIVATION_MANIFESTS enables pushing manifests into the activation context using CreateActCtx calls
	# this is used to be able to load adobo shared com libraries without the need for their global registration
	# this way each IE extension can have its own copy of adobo and be independent on others installed on end user's OS
	add_definitions("-DUSE_ACTIVATION_MANIFESTS")
endif(NOT omit_activation_manifests)

add_subdirectory(libcomhelper)
add_subdirectory(libsalsitaextension)
add_subdirectory(libhtmlpanel)
add_subdirectory(magpie)
add_subdirectory(scriptservice)
add_subdirectory(exportversion)
