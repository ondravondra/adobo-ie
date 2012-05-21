file(GLOB_RECURSE source_files *.cpp *.h)

add_library(libhtmlpanel STATIC ${source_files})

add_definitions("-D_CRT_SECURE_NO_WARNINGS -D_UNICODE -D_CRT_NON_CONFORMING_SWPRINTFS")

set_target_properties(libhtmlpanel PROPERTIES STATIC_LIBRARY_FLAGS "/NODEFAULTLIB")

include_directories(include "../libcomhelper/include")

add_dependencies(libhtmlpanel libcomhelper)

set_property(TARGET libhtmlpanel PROPERTY ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${outdir_lib_release})
set_property(TARGET libhtmlpanel PROPERTY ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${outdir_lib_debug})
