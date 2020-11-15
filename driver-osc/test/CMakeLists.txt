set(REQUIRED_TEST_TARGETS "")
macro(make_test name file)
add_executable(test_${name}_runner
	${file}
)
target_include_directories(test_${name}_runner
	PUBLIC
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/test
	${PROJECT_BINARY_DIR}
)
target_link_libraries(test_${name}_runner
	nonsession
	GTest::GTest
)
gtest_discover_tests(
	test_${name}_runner
	# PROPERTIES ENVIRONMENT KEY=value
	PROPERTIES ENVIRONMENT GTEST_COLOR=yes
)
set(REQUIRED_TEST_TARGETS ${REQUIRED_TEST_TARGETS} test_${name}_runner)
endmacro()

#make_test(F1InputChange F1InputChange.cpp)
make_test(NonSessionHandler NonSessionHandler.cpp)

set(REQUIRED_TEST_TARGETS ${REQUIRED_TEST_TARGETS} PARENT_SCOPE)

