
#include <gtest/gtest.h>

#include "SPSC_Queue.hpp"

TEST(empty, returns_true_on_empty_list)
{
    TSC::Queue<int> q(1000);

    EXPECT_EQ(q.empty(), true);
}

TEST(empty, returns_false_on_list_with_elements)
{
    TSC::Queue<int> q(1000);

    q.push(1);

    EXPECT_EQ(q.empty(), false);
}

TEST(capacity, return_correct_capacity)
{
    TSC::Queue<int> q(1000);

    EXPECT_EQ(q.capacity(), 1000);
}

TEST(size, return_zero_when_empty)
{
    TSC::Queue<int> q(1000);

    EXPECT_EQ(q.size(), 0);
}

TEST(size, return_size_when_containing_elements)
{
    TSC::Queue<int> q(1000);

    q.push(1);
    q.push(1);

    EXPECT_EQ(q.size(), 2);
}

TEST(full, return_false_when_not_full)
{
    TSC::Queue<int> q(1000);

    q.push(1);

    EXPECT_EQ(q.full(), false);
}

TEST(full, return_false_when_close_to_full)
{
    TSC::Queue<int> q(2);

    q.push(1);

    EXPECT_EQ(q.full(), false);
}

TEST(full, return_true_when_full)
{
    TSC::Queue<int> q(1);

    q.push(1);

    EXPECT_EQ(q.full(), true);
}