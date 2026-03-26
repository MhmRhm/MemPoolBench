add_executable(google_test_libsee
	google_test_see.cpp
)

target_link_libraries(google_test_libsee
	PRIVATE precompiled
	PRIVATE libsee_obj
)

Format(google_test_libsee .)
AddGTests(google_test_libsee)
EnableCoverage(libsee_obj)
