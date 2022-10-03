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
    FxtBitsSet(const FxtBitsSet<max_value>& fxtBitsSet){ memcpy(_data,fxtBitsSet._data,hardware_array_size << 3);}
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
    [[nodiscard]] inline bool empty() const noexcept{
        bool res = true;
        for(std::uint64_t idx = 0; idx < hardware_array_size && res; idx++) res &= _data[idx].value() == 0;
        return res;
    }
    template<typename Proc>
    void for_each_bit(Proc proc) const noexcept{
        FxtBitsSet tmp(*this);
        for(std::uint64_t idx = 0; idx < hardware_array_size; idx++){
            std::uint64_t __index_first_bit;
            while(tmp._data[idx].value()){
                __index_first_bit = tmp._data[idx].get_index_first_bit();
                proc((idx << 6) + __index_first_bit );
                tmp._data[idx].clear_first_bit();
            }
        }
    }

    /* Union */
    FxtBitsSet<max_value>& operator|=(const FxtBitsSet<max_value>& rhs) noexcept{
        for(std::uint64_t idx = 0; idx < hardware_array_size; idx++){ _data[idx] |= rhs._data[idx];}
        return *this;
    }
    friend FxtBitsSet<max_value> operator|(const FxtBitsSet<max_value>& lhs, const FxtBitsSet<max_value>& rhs) noexcept{
        FxtBitsSet<max_value> res;
        for(std::uint64_t idx = 0; idx < lhs.hardware_array_size; idx++){ res._data[idx] = lhs._data[idx] | rhs._data[idx]; }
        return res;
    }
    /* Intersection */
    FxtBitsSet<max_value>& operator&=(const FxtBitsSet<max_value>& rhs) noexcept{
        for(std::uint64_t idx = 0; idx < hardware_array_size; idx++){ _data[idx] &= rhs._data[idx];}
        return *this;
    }
    friend FxtBitsSet<max_value> operator&(const FxtBitsSet<max_value>& lhs, const FxtBitsSet<max_value>& rhs) noexcept{
        FxtBitsSet<max_value> res;
        for(std::uint64_t idx = 0; idx < lhs.hardware_array_size; idx++){ res._data[idx] = lhs._data[idx] & rhs._data[idx]; }
        return res;
    }
};
typedef FxtBitsSet<256> char_bitset_type;
