#pragma once

#include <iostream>
#include <type_traits>

template<class T, std::size_t N>
class static_vector
{
    private:
        typename std::aligned_storage<sizeof(T), alignof(T)>::type data[N];
        std::size_t curr_size = 0;

    public:
        const T& front() const
        {
            if(curr_size < 1) {
                new(&data[0]) T();
                curr_size += 1;
            }


#if __cplusplus < 201703L
            return *reinterpret_cast<const T*>(&data[0]);
#else
            return *std::launder(reinterpret_cast<const T*>(&data[0]));
#endif
        }

        const T& back() const
        {
            if(curr_size < 1) {
                new(&data[0]) T();
                curr_size += 1;
            }

#if __cplusplus < 201703L
            return *reinterpret_cast<const T*>(&data[curr_size]);
#else
            return *std::launder(reinterpret_cast<const T*>(&data[curr_size]));
#endif
        }

        /**
         * Create an object and leave it at the last position
         * @tparam Args argument types passed in to constructor
         * @param args arguments passed in to constructor
         * @return 0 if success, -1 if failed
         */
        template<typename ...Args> int create_back(Args&&... args)
        {
            if(curr_size >= N) return -1; // Out of range

            // Construct value in memory of aligned storage, using inplace operator new
            new(&data[curr_size]) T(std::forward<Args>(args)...);
            ++curr_size;
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
            for(std::size_t pos = 0; pos < curr_size; ++pos) {
                // note: needs std::launder as of C++17
#if __cplusplus < 201703L
                reinterpret_cast<T*>(&data[pos])->~T();
#else
                std::launder(reinterpret_cast<T*>(&data[pos]))->~T();
#endif
            }
        }
};

