#include <gtest/gtest.h>
#include "test.h"

TEST(FunDefaultTest, Default) { EXPECT_EQ(Default(), true); }

TEST(FunFixedThreadsTest, FixedThreads) { EXPECT_EQ(FixedThreads(), true); }

TEST(FunAutomaticThreadsTest, AutomaticThreads) {
	EXPECT_EQ(AutomaticThreads(), true);
}

TEST(FunAutomaticToFixedTest, AutomaticToFixed) {
	EXPECT_EQ(AutomaticToFixed(), true);
}

TEST(FunFixedToAutomaticTest, FixedToAutomatic) {
	EXPECT_EQ(FixedToAutomatic(), true);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
	// Default();
	// return 0;
}
