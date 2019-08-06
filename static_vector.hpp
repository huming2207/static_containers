#pragma once

#include <iostream>
#include <type_traits>

template<class T, std::size_t N>
class static_vector
{
    private:
        typename std::aligned_storage<sizeof(T), alignof(T)>::type data[N];
        std::size_t curr_size = 1;

    private:
        int move_right()
        {
            if(curr_size >= N) return -1;       // Array is full now, can't swap
            if(curr_size - 1 == 0) return 0;    // Array is empty, no need to swap anyway

            // Allocate an empty item at the end
            new(&data[curr_size]) T();

            for(std::size_t idx = curr_size; idx > 0; idx--) {
                std::swap(data[idx], data[idx - 1]);
            }

            return 0;
        }

    public:
        const T& front()
        {
            if(curr_size - 1 < 1) {
                new(&data[0]) T();
                curr_size += 1;
            }


#if __cplusplus < 201703L
            return *reinterpret_cast<const T*>(&data[0]);
#else
            return *std::launder(reinterpret_cast<const T*>(&data[0]));
#endif
        }

        const T& back()
        {
            if(curr_size - 1 < 1) {
                new(&data[0]) T();
                curr_size += 1;
            }

#if __cplusplus < 201703L
            return *reinterpret_cast<const T*>(&data[curr_size]);
#else
            return *std::launder(reinterpret_cast<const T*>(&data[curr_size]));
#endif
        }

#if __cplusplus >= 201703L
        [[nodiscard]]
#endif
        /**
         * Get the current size of vector
         * @return Current amount of objects in the vector
         */
        std::size_t size() const
        {
            return curr_size;
        }

#if __cplusplus >= 201703L
        [[nodiscard]]
#endif
        /**
         * Get the maximum size of vector
         * @return Maximum amount of objects in the vector
         */
        std::size_t max_size() const
        {
            return N;
        }

#if __cplusplus >= 201703L
        [[nodiscard]]
#endif
        /**
         * Get empty status
         * @return True if empty
         */
        bool empty() const
        {
            return curr_size == 0;
        }

        /**
         * Clear the vector
         */
        void clear()
        {
            for(std::size_t pos = 0; pos < curr_size; pos++) {
                // note: needs std::launder as of C++17
#if __cplusplus < 201703L
                reinterpret_cast<T*>(&data[pos])->~T();
#else
                std::launder(reinterpret_cast<T*>(&data[pos]))->~T();
#endif
            }

            curr_size = 0;
        }

        /**
         * Constructs an element in-place at the end
         * @tparam Args argument types passed in to constructor
         * @param args arguments passed in to constructor
         * @return 0 if success, -1 if failed
         */
        template<typename ...Args> int emplace_back(Args&&... args)
        {
            if(curr_size >= N) return -1; // Out of range

            // Construct value in memory of aligned storage, using inplace operator new
            new(&data[curr_size - 1]) T(std::forward<Args>(args)...);
            ++curr_size;
        }

        /**
        * Constructs an element in-place at front
        * @tparam Args argument types passed in to constructor
        * @param args arguments passed in to constructor
        * @return 0 if success, -1 if failed
        */
        template<typename ...Args> int emplace(Args&&... args)
        {
            if(move_right() != 0) return -1;

            // Construct value in memory of aligned storage, using inplace operator new
            new(&data[0]) T(std::forward<Args>(args)...);
            ++curr_size;
        }

        /**
        * Push an item at front
        * @param item New item
        * @return 0 if success, -1 if failed
        */
        int push(const T& item)
        {
            if(move_right() != 0) return -1;

            // Construct value in memory of aligned storage, using inplace operator new
            new(&data[0]) T(item);
            ++curr_size;
        }

        /**
         * Insert an item to a specified position
         * @param pos Position index
         * @param item Item to insert
         * @return 0 if success, -1 if out of range
         */
        int insert(std::size_t pos, const T& item)
        {
            if(curr_size < pos) return -1;
            if(data[pos] != nullptr) {
#if __cplusplus < 201703L
                reinterpret_cast<T*>(&data[pos])->~T();
#else
                std::launder(reinterpret_cast<T*>(&data[pos]))->~T();
#endif
            }

            new(&data[curr_size]) T(item);
            return 0;
        }

        const T& operator[](std::size_t pos) const
        {
#if __cplusplus < 201703L
            return *reinterpret_cast<const T*>(&data[pos]);
#else
            return *std::launder(reinterpret_cast<const T*>(&data[pos]));
#endif
        }


        ~static_vector()
        {
            clear();
        }
};

