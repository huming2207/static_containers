/**
 * Originally from: https://codereview.stackexchange.com/questions/211362/implementation-of-static-vector-using-an-array-of-stdaligned-storage-with-std
 */

#pragma once

#include <array>
#include <memory>
#include <stdexcept>

template<class T, std::size_t N>
class aligned_array
{
    public:
        aligned_array() = default;

        ~aligned_array() = default;

        // Move and copy must be manually implemented per-element by the user
        aligned_array(aligned_array &&rhs) = delete;

        aligned_array &operator=(aligned_array &&rhs) = delete;

        aligned_array(const aligned_array &rhs) = delete;

        aligned_array &operator=(const aligned_array &rhs) = delete;

        T* data()
        {
#if __cplusplus >= 201703L
            return std::launder(reinterpret_cast<T*>(m_data.data()));
#else
            return reinterpret_cast<T*>(m_data.data());
#endif
        }

        T const* data() const
        {
#if __cplusplus >= 201703L
            return std::launder(reinterpret_cast<T const*>(m_data.data()));
#else
            return reinterpret_cast<T const*>(m_data.data());
#endif
        }

        // Size
#if __cplusplus >= 201703L
        [[nodiscard]]
#endif
        constexpr std::size_t size() const noexcept { return N; }

#if __cplusplus >= 201703L
        [[nodiscard]]
#endif
        constexpr std::size_t max_size() const noexcept { return N; }

        // Access
        inline T &operator[](std::size_t pos)
        {
            return *std::launder(
                    reinterpret_cast<T *>(
                            std::addressof(m_data[pos])));
        }

        inline const T &operator[](std::size_t pos) const
        {
            return *std::launder(
                    reinterpret_cast<const T *>(
                            std::addressof(m_data[pos])));
        }

        inline T &at(std::size_t pos)
        {
            return *std::launder(
                    reinterpret_cast<T *>(
                            std::addressof(m_data.at(pos))));
        }

        inline const T &at(std::size_t pos) const
        {
            return *std::launder(
                    reinterpret_cast<const T *>(
                            std::addressof(m_data.at(pos))));
        }

        // Operations
        template<typename ...Args>
        inline T &emplace(size_t pos, Args &&... args)
        {
            return *::new(std::addressof(m_data[pos]))
                            T(std::forward<Args>(args)...);
        }

        template<typename ...Args>
        inline T &bounded_emplace(size_t pos, Args &&... args)
        {
            return *::new(std::addressof(m_data.at(pos)))
                            T(std::forward<Args>(args)...);
        }

        inline void destroy(std::size_t pos)
        {
            std::destroy_at(std::launder(reinterpret_cast<const T *>(std::addressof(m_data[pos]))));
        }

        inline void bounded_destroy(std::size_t pos)
        {
            std::destroy_at(std::launder(reinterpret_cast<const T *>(std::addressof(m_data.at(pos)))));
        }

    private:
        std::array<std::aligned_storage_t<sizeof(T), alignof(T)>, N> m_data;
};

