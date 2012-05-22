file(GLOB_RECURSE source_files *.cpp *.h *.def *.rgs *.idl *.rc *.manifest *.js)
set(generated_files "${CMAKE_CURRENT_BINARY_DIR}/$(IntDir)/magpie_i.c")

add_library(magpie SHARED ${source_files} ${generated_files})
set_source_files_properties(${generated_files} PROPERTIES GENERATED 1)

include_directories("${CMAKE_CURRENT_BINARY_DIR}/$(IntDir)" "../libcomhelper/include")

add_definitions("-D_CRT_SECURE_NO_WARNINGS -D_UNICODE -D_ATL_STATIC_REGISTRY -D_CRT_NON_CONFORMING_SWPRINTFS")

if(DUMP_MODULE_TIMES)
	add_definitions("-DDUMP_MODULE_TIMES")
endif(DUMP_MODULE_TIMES)

target_link_libraries(magpie libcomhelper libhtmlpanel)
add_dependencies(magpie libcomhelper libhtmlpanel exportversion)

set_property(TARGET magpie PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${outdir_bin_release})
set_property(TARGET magpie PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${outdir_bin_debug})
set_property(TARGET magpie PROPERTY LIBRARY_OUTPUT_DIRECTORY_RELEASE ${outdir_bin_release})
set_property(TARGET magpie PROPERTY LIBRARY_OUTPUT_DIRECTORY_DEBUG ${outdir_bin_debug})
set_property(TARGET magpie PROPERTY ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${outdir_lib_release})
set_property(TARGET magpie PROPERTY ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${outdir_lib_debug})

file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" src_dir)
file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}" binary_dir)

if(DISABLE_REGFREE_ACTIVATION)
	add_custom_command(
		TARGET magpie
		POST_BUILD
		COMMAND if $<CONFIGURATION>==Debug regsvr32.exe /s \"$(TargetPath)\"
		)
else(DISABLE_REGFREE_ACTIVATION)
	add_custom_command(
		TARGET magpie
		POST_BUILD
		COMMAND mt.exe -manifest \"${src_dir}/magpie.dll.manifest\" \"${binary_dir}/magpie.ident.manifest\" -outputresource:\"$(TargetPath)\;\#2\"
		)
endif(DISABLE_REGFREE_ACTIVATION)