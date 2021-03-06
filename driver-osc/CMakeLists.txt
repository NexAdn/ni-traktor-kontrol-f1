cmake_minimum_required(VERSION 3.0)
project(tkf1-driver-osc LANGUAGES C CXX VERSION 0.1)

option(WITH_TESTS "Enable unit tests" ON)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
pkg_check_modules(liblo REQUIRED IMPORTED_TARGET liblo)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror -Wno-sign-compare -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

configure_file(
	src/config.h.in
	${PROJECT_BINARY_DIR}/config.h
)

add_subdirectory(src)

if (${WITH_TESTS})
	enable_testing(true)
	find_package(GTest REQUIRED)
	include(GoogleTest)

	set(REQUIRED_TEST_TARGETS "")

	add_subdirectory(test)

	add_custom_target(check
		COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
		DEPENDS ${REQUIRED_TEST_TARGETS}
	)
endif()

add_custom_target(doc
	COMMAND doxygen
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMENT "Generating documentation"
)
