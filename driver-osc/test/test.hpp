#ifndef MAKE_TEST_HPP__
#define MAKE_TEST_HPP__
#pragma once

#include <gtest/gtest.h>

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

#endif // MAKE_TEST_HPP__
