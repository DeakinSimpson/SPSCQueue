
#include <gtest/gtest.h>

#include "SPSC_Queue.hpp"

TEST(empty, returns_empty_on_empty_list)
{
    TSC::Queue<int> q(1000);

    EXPECT_EQ(q.empty(), true);
}