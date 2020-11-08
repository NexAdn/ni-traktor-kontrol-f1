cmake_minimum_required(VERSION 3.0)
project(tkf1-driver-osc LANGUAGES C CXX VERSION 0.1)

option(WITH_TESTS "Enable unit tests" ON)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
pkg_check_modules(libhid REQUIRED IMPORTED_TARGET libhid)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror -Wno-sign-compare")

add_subdirectory(src)

if (${WITH_TESTS})
	enable_testing(true)
	find_package(GTest REQUIRED)
	include(GoogleTest)

	set(REQUIRED_TEST_TARGETS "")

	add_subdirectory(test)

	add_custom_target(check
		COMMAND ${CMAKE_CTEST_COMMAND}
		DEPENDS ${REQUIRED_TEST_TARGETS}
	)
endif()
