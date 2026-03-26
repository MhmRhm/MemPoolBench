add_executable(google_bench_libpool
	google_bench_pool.cpp
)

target_link_libraries(google_bench_libpool
	PRIVATE precompiled
	PRIVATE libpool_static
)

Format(google_bench_libpool .)
AddBenchmarks(google_bench_libpool)
Doxygen(google_bench_libpool benchmark/libpool)