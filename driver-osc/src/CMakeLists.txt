add_library(f1hid STATIC
	F1HidDev.cpp
)
target_include_directories(f1hid
	PUBLIC
		.
)
add_library(nonsession STATIC
	NonSessionHandler.cpp
)
target_include_directories(nonsession
	PUBLIC
		.
)
target_compile_features(nonsession
	PUBLIC
		cxx_std_17
)
target_link_libraries(nonsession
	PUBLIC
		PkgConfig::liblo
)

add_subdirectory(exe)
