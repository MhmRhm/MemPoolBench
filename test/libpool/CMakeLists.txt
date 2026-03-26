add_executable(google_test_libpool
	google_test_pool.cpp
)

target_link_libraries(google_test_libpool
	PRIVATE precompiled
	PRIVATE libpool_obj
)

Format(google_test_libpool .)
AddGTests(google_test_libpool)
EnableCoverage(libpool_obj)
