//
// Created by deakin on 10/12/26.
//

#pragma once
#include <cstddef>
#include <memory>
#include <atomic>

namespace TSC
{
    template <typename T, typename Alloc = std::allocator<T>>
    class Queue : private Alloc
    {
        std::size_t capacity_;
        T* ring_;

        // checks whether std::size_t is always lock free
        static_assert(std::atomic<std::size_t>::is_always_lock_free);

        /**
         * To prevent the cursors being on the same cache line (which they most
         * likely will be as they are declared right next to eachother) we must
         * specify that we want each cursor to have its own cache line.
         *
         * This is because on modern processors, to increase performance, when
         * an object in memory is retrieved, to save time, instead of just
         * reading that single object to the cpu cache, we instead grab the
         * entire cache line.
         *
         * However because both cursors reside on the cache line, it means that
         * there is a very high likelihood of false sharing, thus, when both
         * cpu cores attempt to place the same cache line in memory, the cache
         * line will be sent back to the cpu to rectify the contention between
         * the cores (greatly reducing performance)
        */
        // used by push thread
        alignas(std::hardware_destructive_interference_size)
            std::atomic<std::size_t> pushCursor_ {};

        /**
         * Adding caching means that the atomic pushCursor that is accessed by
         * each core only needs to be accessed when the current thread believes
         * the queue to be full.
         *
         * This means that instead of both threads trying to share a single
         * cache line, subsequently causing the other thread to wait, the
         * threads are free to push and pop until they believe that the queue
         * is full, then and only then will they get the cache line from memory
         * to confirm.
         */
        alignas(std::hardware_destructive_interference_size)
            std::size_t cachedPushCursor_ {};

        // used by pop thread
        alignas(std::hardware_destructive_interference_size)
            std::atomic<std::size_t> popCursor_ {};

        alignas (std::hardware_destructive_interference_size)
            std::size_t cachedPopCursor_ {};

        /**
         * This padding is added after the popCursor_ so that nothing else
         * appears on its cache line, this is to allow the cursor to be accessed
         * without the chance of yet again causing false sharing
         */
        char padding_
            [std::hardware_destructive_interference_size - sizeof(std::size_t)];

        auto isFull(std::size_t pushCursor, std::size_t popCursor) const
        {
            return (pushCursor - popCursor) == capacity_;
        }

        static auto isEmpty(std::size_t pushCursor, std::size_t popCursor)
        {
            return pushCursor == popCursor;
        }

        auto element(std::size_t cursor)
        {
            return &ring_[cursor % capacity_];
        }

    public:
        /**
         *
         * @param capacity The max size of the queue
         * @param alloc Set the default allocator to Alloc
         */
        explicit Queue(std::size_t capacity, Alloc const& alloc = Alloc {})
            : Alloc {alloc}
            , capacity_ {capacity}
            , ring_ {std::allocator_traits<Alloc>::allocate(*this, capacity) }
        {  }

        /**
         * Loop through all values and deallocate them
         *
         * Then deallocate the container
         */
        ~Queue()
        {
            while (!empty())
            {
                ring_[popCursor_ % capacity_].~T();
                ++popCursor_;
            }

            std::allocator_traits<Alloc>::deallocate(*this, ring_, capacity_);
        }

        auto capacity() const { return capacity_; }
        auto size() const { return pushCursor_ - popCursor_; }
        auto empty() const { return size() == 0; }
        auto full() const { return size() == capacity_; }

        auto push(T const& value)
        {
            const auto pushCursor
                { pushCursor_.load(std::memory_order_relaxed) };

            if (isFull(pushCursor, cachedPopCursor_))
            {
                cachedPopCursor_ = popCursor_.load(std::memory_order_acquire);

                if (isFull(pushCursor, cachedPopCursor_))
                {
                    return false;
                }
            }

            // create new element at pushCursor and store it
            new (element(pushCursor)) T(value);
            pushCursor_.store(pushCursor + 1, std::memory_order_release);

            return true;
        }

        auto pop(T& value)
        {
            const auto popCursor
                { popCursor_.load(std::memory_order_relaxed) };

            if (isEmpty(cachedPushCursor_, popCursor))
            {
                cachedPushCursor_ = pushCursor_.load(std::memory_order_acquire);
                if (isEmpty(cachedPushCursor_, popCursor))
                {
                    return false;
                }
            }


            // copy the value to value found at popCursors location
            value = *element(popCursor);
            element(popCursor)->~T();

            popCursor_.store(popCursor + 1, std::memory_order_release);

            return true;
        }
    };
}
