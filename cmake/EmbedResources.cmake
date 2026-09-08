# Builds files into the binary rather than shipping them beside it.
#
# The editor has no install tree and no working directory it can trust - Bron/Util/Paths.h
# spells out why - so the small things its own interface is made of (icons, and a UI font
# if one is ever added) are compiled in, the same way Bron/Graphics/BuiltinShaders.h holds
# the engine's shaders. Project content is the opposite case and stays on disk under the
# asset root, where the user can edit it.

include_guard(GLOBAL)

# bron_embed_resources(<target>
#     NAMESPACE <ns>      C++ namespace for the generated declarations
#     BASENAME  <name>    names the generated <name>.h / <name>.cpp
#     FILES     <file>... the files to embed, relative to the current source directory
# )
#
# Adds the generated source to <target> and puts its directory on the target's include
# path, so the header is reached as #include "<name>.h".
#
# Each call must use its own namespace: the generated header declares the Resource type
# in it, and two headers declaring it in the same namespace would collide.
function(bron_embed_resources target)
	cmake_parse_arguments(PARSE_ARGV 1 ARG "" "NAMESPACE;BASENAME" "FILES")

	if(NOT ARG_NAMESPACE OR NOT ARG_BASENAME OR NOT ARG_FILES)
		message(FATAL_ERROR "bron_embed_resources: NAMESPACE, BASENAME and FILES are all required")
	endif()

	set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
	set(header "${output_dir}/${ARG_BASENAME}.h")
	set(source "${output_dir}/${ARG_BASENAME}.cpp")

	# The script takes absolute paths, so that where it runs from cannot matter, and takes
	# them joined by "|" because a ";" in a -D argument is eaten as a list separator.
	set(inputs "")
	foreach(file ${ARG_FILES})
		get_filename_component(absolute "${file}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
		if(NOT EXISTS "${absolute}")
			message(FATAL_ERROR "bron_embed_resources: ${absolute} does not exist")
		endif()

		list(APPEND absolute_files "${absolute}")
		string(APPEND inputs "${absolute}|")
	endforeach()

	add_custom_command(
		OUTPUT "${header}" "${source}"
		COMMAND ${CMAKE_COMMAND}
			-DINPUTS=${inputs}
			-DHEADER=${header}
			-DSOURCE=${source}
			-DNAMESPACE=${ARG_NAMESPACE}
			-P "${CMAKE_SOURCE_DIR}/cmake/GenerateEmbeddedResources.cmake"
		# The script itself is a dependency too: editing how the bytes are laid out has to
		# regenerate them, or the build keeps whatever the previous version wrote.
		DEPENDS ${absolute_files} "${CMAKE_SOURCE_DIR}/cmake/GenerateEmbeddedResources.cmake"
		COMMENT "Embedding ${ARG_BASENAME}"
		VERBATIM
	)

	target_sources(${target} PRIVATE "${header}" "${source}")
	target_include_directories(${target} PRIVATE "${output_dir}")
endfunction()
