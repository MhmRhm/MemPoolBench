include_guard(GLOBAL)

add_library(precompiled INTERFACE)

target_precompile_headers(precompiled INTERFACE
	<stdexcept>
	<functional>
	<chrono>

	<iostream>
	<string>
	<string_view>
	<format>
	<source_location>

	<vector>
	<list>
	<queue>
	<map>
	<unordered_map>

	<thread>
	<memory>

	<cstddef>
	<cstdint>
)
