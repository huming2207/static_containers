/**
 * Originally from: https://codereview.stackexchange.com/questions/211362/implementation-of-static-vector-using-an-array-of-stdaligned-storage-with-std
 */

#pragma once

#include <array>
#include <stdexcept>

#include "aligned_array.hpp"

template<class T, std::size_t N>
class static_vector {
    public:
        using value_type      = T;
        using pointer         = T *;
        using const_pointer   = const T *;
        using reference       = value_type &;
        using const_reference = const value_type &;
        using iterator        = value_type *;
        using const_iterator  = const value_type *;
        using size_type       = std::size_t;

        static_vector() = default;

        ~static_vector() { clear(); }

        static_vector(const static_vector &rhs)
        {
            for (std::size_t pos = 0; pos < rhs.m_size; ++pos)
                m_data[pos] = rhs.m_data[pos];
            m_size = rhs.m_size;
        }

        static_vector &operator=(const static_vector &rhs)
        {
            if (this != std::addressof(rhs)) {
                clear(); // Sets m_size to zero for safety
                for (std::size_t pos = 0; pos < rhs.m_size; ++pos)
                    m_data[pos] = rhs.m_data[pos];
                m_size = rhs.m_size;
            }
            return *this;
        }

        static_vector(static_vector &&rhs)
        {
            // Start by clearing sizes to avoid bad data
            // access in the case of an exception
            std::size_t count_self = m_size;
            std::size_t count_rhs = rhs.m_size;
            m_size = 0;
            rhs.m_size = 0;

            // Can't swap because the destination may be uninitialized
            destroy_n(count_self);
            for (std::size_t pos = 0; pos < count_rhs; ++pos)
                m_data[pos] = std::move(rhs.m_data[pos]);
            m_size = count_rhs;
        }

        static_vector &operator=(static_vector &&rhs)
        {
            // Start by clearing sizes to avoid bad data
            // access in the case of an exception
            std::size_t count_self = m_size;
            std::size_t count_rhs = rhs.m_size;
            m_size = 0;
            rhs.m_size = 0;

            // Can't swap because the destination may be uninitialized
            destroy_n(count_self);
            for (std::size_t pos = 0; pos < count_rhs; ++pos)
                m_data[pos] = std::move(rhs.m_data[pos]);
            m_size = count_rhs;
            return *this;
        }

        // Size and capacity
        constexpr std::size_t size() const { return m_size; }

        constexpr std::size_t max_size() const { return N; }

        constexpr bool empty() const { return m_size == 0; }

        // Iterators
        inline iterator begin() { return &m_data[0]; }

        inline const_iterator begin() const { return &m_data[0]; }

        inline iterator end() { return &m_data[m_size]; }

        inline const_iterator end() const { return &m_data[m_size]; }

        // Access
        inline T &operator[](std::size_t pos)
        {
            return m_data[pos];
        }

        inline const T &operator[](std::size_t pos) const
        {
            return m_data[pos];
        }

        inline T &at(std::size_t pos)
        {
            if ((pos < 0) || (pos >= m_size)) return m_data[0];
            return m_data[pos];
        }

        inline const T &at(std::size_t pos) const
        {
            if ((pos < 0) || (pos >= m_size)) return m_data[0];
            return m_data[pos];
        }

        // Operations
        template<typename ...Args>
        inline T &emplace_back(Args &&... args)
        {
            T &result = m_data.bounded_emplace(m_size, args...);
            ++m_size;
            return result;
        }

        inline void clear()
        {
            std::size_t count = m_size;
            m_size = 0; // In case of exception
            destroy_n(count);
        }

    private:
        void destroy_n(std::size_t count)
        {
            for (std::size_t pos = 0; pos < count; ++pos)
                m_data.destroy(pos);
        }

        aligned_array <T, N> m_data;
        std::size_t m_size = 0;
};
