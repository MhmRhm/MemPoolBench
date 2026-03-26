add_library(libpool_obj OBJECT
	pool.cpp
)

target_compile_options(libpool_obj
	PRIVATE ${DEFAULT_CXX_COMPILE_FLAGS}
	PRIVATE ${DEFAULT_CXX_OPTIMIZE_FLAG}
)

target_include_directories(libpool_obj
	PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

set_target_properties(libpool_obj PROPERTIES
	PUBLIC_HEADER src/libpool/include/libpool/pool.h
	POSITION_INDEPENDENT_CODE 1
)

target_link_libraries(libpool_obj
	PRIVATE precompiled
	PRIVATE Boost::lockfree
)

add_library(libpool_shared SHARED)
target_link_libraries(libpool_shared libpool_obj)
set_target_properties(libpool_shared PROPERTIES
	PREFIX "lib"
	OUTPUT_NAME "pool$<$<CONFIG:Debug>:d>"
)

add_library(libpool_static STATIC)
target_link_libraries(libpool_static libpool_obj)
set_target_properties(libpool_static PROPERTIES
	PREFIX "lib"
	OUTPUT_NAME "pool$<$<CONFIG:Debug>:d>"
)

AddBuildInfo(libpool_obj)
CleanCoverage(libpool_static)
Format(libpool_static .)
AddCppcheck(libpool_static)
Doxygen(libpool_static src/libpool)
