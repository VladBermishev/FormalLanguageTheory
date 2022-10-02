#pragma once
#include <cstdint>
#include <cstring>
#include <common/fxtbits.h>

template< std::uint64_t max_value >
class FxtBitsSet{
    static constexpr std::uint64_t hardware_array_size = (max_value / 64) + (max_value % 64 ? 1 : 0);
    FxtBits _data[hardware_array_size];
public:
    FxtBitsSet(){ memset(_data, 0, hardware_array_size << 3); }

    inline void insert(const std::uint64_t value) noexcept{
        if (__glibc_unlikely(value >= max_value)) exit(1);
        const std::uint64_t array_index = value / 64, shift_index = value % 64;
        _data[array_index].set(shift_index);
    }

    [[nodiscard]] inline std::uint8_t contains(const std::uint64_t value) const noexcept{
        if (__glibc_unlikely(value >= max_value)) exit(1);
        const std::uint64_t array_index = value / 64, shift_index = value % 64;
        return _data[array_index].get(shift_index);
    }
};
