file(GLOB_RECURSE source_files *.cpp *.h *.def *.rgs *.idl *.rc *.manifest)

add_library(scriptservice SHARED ${source_files})

include_directories("${CMAKE_CURRENT_BINARY_DIR}/$(IntDir)")

add_definitions("-D_CRT_SECURE_NO_WARNINGS -D_UNICODE -D_ATL_STATIC_REGISTRY -D_CRT_NON_CONFORMING_SWPRINTFS")

target_link_libraries(scriptservice libcomhelper)
add_dependencies(scriptservice libcomhelper magpie exportversion)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /I\"${outdir_bin_release}\"")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /I\"${outdir_bin_debug}\"")

set_property(TARGET scriptservice PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${outdir_bin_release})
set_property(TARGET scriptservice PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${outdir_bin_debug})
set_property(TARGET scriptservice PROPERTY LIBRARY_OUTPUT_DIRECTORY_RELEASE ${outdir_bin_release})
set_property(TARGET scriptservice PROPERTY LIBRARY_OUTPUT_DIRECTORY_DEBUG ${outdir_bin_debug})
set_property(TARGET scriptservice PROPERTY ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${outdir_lib_release})
set_property(TARGET scriptservice PROPERTY ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${outdir_lib_debug})

file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" src_dir)
file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}" binary_dir)

if(DISABLE_REGFREE_ACTIVATION)
	add_custom_command(
		TARGET scriptservice
		POST_BUILD
		COMMAND if $<CONFIGURATION>==Debug regsvr32.exe /s \"$(TargetPath)\"
		)
else(DISABLE_REGFREE_ACTIVATION)
	add_custom_command(
		TARGET scriptservice
		POST_BUILD
		COMMAND mt.exe -manifest \"${src_dir}/scriptservice.dll.manifest\" \"${binary_dir}/scriptservice.ident.manifest\" -outputresource:\"$(TargetPath)\;\#2\"
		)
endif(DISABLE_REGFREE_ACTIVATION)
