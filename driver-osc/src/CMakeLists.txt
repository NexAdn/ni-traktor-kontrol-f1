add_library(f1hid STATIC
	F1HidDev.cpp
	F1InputState.cpp
)
target_include_directories(f1hid
	PUBLIC
		.
)
add_library(nonsession STATIC
	NonPeer.cpp
	NonSessionHandler.cpp
)
target_include_directories(nonsession
	PUBLIC
		.
		${PROJECT_BINARY_DIR}
)
target_compile_features(nonsession
	PUBLIC
		cxx_std_17
)
target_link_libraries(nonsession
	PUBLIC
		PkgConfig::liblo
		f1hid
)

add_subdirectory(exe)
