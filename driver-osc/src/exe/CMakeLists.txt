add_executable(debug
	debug.cpp
)
target_link_libraries(debug
	f1hid
	Threads::Threads
)
