file(GLOB_RECURSE source_files *.cpp *.h)

add_library(libsalsitaextension STATIC ${source_files})

add_definitions("-D_CRT_SECURE_NO_WARNINGS -D_UNICODE -D_CRT_NON_CONFORMING_SWPRINTFS")

set_target_properties(libsalsitaextension PROPERTIES STATIC_LIBRARY_FLAGS "/NODEFAULTLIB")

include_directories(include "../libcomhelper/include")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /I${outdir_bin_release}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /I${outdir_bin_debug}")

add_dependencies(libsalsitaextension libcomhelper magpie scriptservice)

set_property(TARGET libsalsitaextension PROPERTY ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${outdir_lib_release})
set_property(TARGET libsalsitaextension PROPERTY ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${outdir_lib_debug})
