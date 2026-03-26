include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(pool::precompiled ALIAS precompiled)
add_library(pool::libpool_obj ALIAS libpool_obj)
add_library(pool::libpool_static ALIAS libpool_static)
add_library(pool::libpool_shared ALIAS libpool_shared)

export(
	TARGETS precompiled libpool_obj libpool_static libpool_shared
	NAMESPACE pool::
	FILE "${PROJECT_BINARY_DIR}/libpool-targets.cmake"
)

install(
	TARGETS precompiled libpool_obj libpool_static libpool_shared
	EXPORT libpool-targets
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libpool
)

if(UNIX)
	install(CODE "execute_process(COMMAND ldconfig)")
endif()

install(
	EXPORT libpool-targets
	NAMESPACE pool::
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/libpool"
)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/libpool-config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/libpool-config.cmake"
	INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/libpool/cmake"
	PATH_VARS CMAKE_INSTALL_INCLUDEDIR
)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/libpool-config-version.cmake"
	VERSION ${PACKAGE_VERSION}
	COMPATIBILITY ExactVersion
)
install(
	FILES
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/libpool-config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/libpool-config-version.cmake"
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/libpool"
)

set(CPACK_PACKAGE_CONTACT "Mohammad Rahimi <https://github.com/MhmRhm>")
set(CPACK_PACKAGE_DESCRIPTION "SeeMake: a CMake project template.")
include(CPack)
