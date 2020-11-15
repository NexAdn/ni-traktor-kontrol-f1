add_executable(debug
	debug.cpp
)
target_link_libraries(debug
	f1hid
	Threads::Threads
)

add_executable(nsm_test
	nsm_test.cpp
)
target_link_libraries(nsm_test
	nonsession
)

add_executable(osc_driver
	osc_driver.cpp
)
target_compile_features(osc_driver
	PUBLIC
		cxx_std_17
)
target_link_libraries(osc_driver
	f1hid
	nonsession
)
