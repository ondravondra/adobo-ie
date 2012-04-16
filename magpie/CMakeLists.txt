file(GLOB_RECURSE source_files *.cpp *.h *.def *.rgs *.idl *.rc)

add_library(magpie SHARED ${source_files})

include_directories("${CMAKE_CURRENT_BINARY_DIR}/$(IntDir)")

add_definitions("-D_CRT_SECURE_NO_WARNINGS -D_UNICODE -D_ATL_STATIC_REGISTRY -D_CRT_NON_CONFORMING_SWPRINTFS")

target_link_libraries(magpie libcomhelper)
add_dependencies(magpie libcomhelper)

set_property(TARGET magpie PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${outdir_bin_release})
set_property(TARGET magpie PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${outdir_bin_debug})
set_property(TARGET magpie PROPERTY LIBRARY_OUTPUT_DIRECTORY_RELEASE ${outdir_bin_release})
set_property(TARGET magpie PROPERTY LIBRARY_OUTPUT_DIRECTORY_DEBUG ${outdir_bin_debug})
set_property(TARGET magpie PROPERTY ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${outdir_lib_release})
set_property(TARGET magpie PROPERTY ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${outdir_lib_debug})

