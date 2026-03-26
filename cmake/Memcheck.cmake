include(FetchContent)
FetchContent_Declare(
	memcheck-cover
	GIT_REPOSITORY https://github.com/Farigh/memcheck-cover.git
	GIT_TAG release-1.2
)
FetchContent_MakeAvailable(memcheck-cover)

function(AddMemcheck target)
	if(UNIX)
		set(MEMCHECK_PATH ${memcheck-cover_SOURCE_DIR}/bin)
		set(REPORT_PATH "${CMAKE_BINARY_DIR}/valgrind-${target}")
		set(MEMCHECK_SCRIPT [[
			#!/bin/bash
			s=0
			"$1/memcheck_runner.sh" -o "$2/report" -- "$3" || s=$?
			# Run HTML report only if s is 0 or 99
			if [ $s -eq 0 ] || [ $s -eq 99 ]; then
				"$1/generate_html_report.sh" -i "$2" -o "$2"
			fi
			# Report error only if s is 99
			if [ $s -eq 99 ]; then
				echo "❌ Valgrind detected memory errors."
			fi
			# Exit with original status
			exit $s]]
		)
		file(WRITE "${CMAKE_BINARY_DIR}/memcheck_${target}.sh" "${MEMCHECK_SCRIPT}")
		file(CHMOD "${CMAKE_BINARY_DIR}/memcheck_${target}.sh" PERMISSIONS
			OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE
		)
		add_custom_target(memcheck-${target}
			COMMAND ${CMAKE_BINARY_DIR}/memcheck_${target}.sh
				${MEMCHECK_PATH} ${REPORT_PATH} $<TARGET_FILE:${target}>
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	elseif (WIN_MSVC)
		target_compile_definitions(${target}
			PRIVATE _DISABLE_VECTOR_ANNOTATION
			PRIVATE _DISABLE_STRING_ANNOTATION
		)
		target_compile_options(${target}
			PRIVATE /fsanitize=address /Zi
		)
		return()
	endif()
endfunction()
