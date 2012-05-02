file(GLOB_RECURSE source_files *.cpp *.h)

add_library(libsalsitaextension STATIC ${source_files})

add_definitions("-D_CRT_SECURE_NO_WARNINGS -D_UNICODE -D_CRT_NON_CONFORMING_SWPRINTFS")

set_target_properties(libsalsitaextension PROPERTIES STATIC_LIBRARY_FLAGS "/NODEFAULTLIB")

set_property(TARGET libsalsitaextension PROPERTY ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${outdir_lib_release})
set_property(TARGET libsalsitaextension PROPERTY ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${outdir_lib_debug})
