#pragma once
#include <cstdint>
#include <cstring>
#include <common/fxtbits.h>

template<std::uint64_t max_size>
class FxtBitsSet{
    static constexpr std::uint64_t hardware_array_size = (max_size / 64) + (max_size % 64 ? 1 : 0);
    FxtBits _data[hardware_array_size];
public:
    FxtBitsSet(){ memset(_data, 0, hardware_array_size << 3); }

    inline void set(const std::uint64_t index) noexcept{
        if (__glibc_unlikely(index >= max_size)) exit(1);
        const std::uint64_t array_index = index / 64, shift_index = index % 64;
        _data[array_index].set(shift_index);
    }

    [[nodiscard]] inline std::uint8_t get(const std::uint64_t index) const noexcept{
        if (__glibc_unlikely(index >= max_size)) exit(1);
        const std::uint64_t array_index = index / 64, shift_index = index % 64;
        return _data[array_index].get(shift_index);
    }
};
