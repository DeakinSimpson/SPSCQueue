//
// Created by deakin on 10/12/26.
//

#pragma once
#include <cstddef>
#include <memory>

namespace TSC
{
    template <typename T, typename Alloc = std::allocator<T>>
    class Queue : private Alloc
    {
        std::size_t capacity_;
        T* ring_;
        std::size_t pushCursor_ {};
        std::size_t popCursor_ {};

    public:
        /**
         *
         * @param capacity The max size of the queue
         * @param alloc Set the default allocator to Alloc
         */
        explicit Queue(std::size_t capacity, Alloc const& alloc = Alloc {})
            : Alloc {alloc}
            , capacity_ {capacity}
            , ring_ {std::allocator_traits<T>::allocate(*this, capacity) }
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

            std::allocator_traits<T>::deallocate(*this, ring_, capacity_);
        }

        auto capacity() const { return capacity_; }
        auto size() const { return pushCursor_ - popCursor_; }
        auto empty() const { return size() == 0; }
        auto full() const { return size() == capacity_(); }

        auto push(T const& value);
        auto pop(T* value);
    };
}
