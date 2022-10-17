#pragma once
#include <immintrin.h>
#include <common/asm.h>

#if (defined(__x86_64__) || defined(_M_X64)) && defined(__GNUC__)
class FxtBits{
    std::uint64_t _value;
public:
    explicit FxtBits(const std::uint64_t value = 0) noexcept: _value(value){}
    FxtBits& operator=(const std::uint64_t value) noexcept{ _value = value;return *this;}
    inline std::uint64_t value() const noexcept{return _value;}
    inline std::uint8_t get(const std::uint64_t index) const noexcept{
        std::uint8_t flag;
        asm("bt %2,%1; setb %0" : "=q" (flag) : "r" (_value), "r" (index));
        return flag;
    }
    inline void set(const std::uint64_t index)noexcept{asm("bts %1,%0" : "+r" (_value) : "r" (index));}
    inline void clear(const std::uint64_t index)noexcept{asm("btr %1,%0" : "+r" (_value) : "r" (index));}
    inline void reverse_bytes() noexcept{asm("bswap %0":"+r"(_value));}
    inline std::uint64_t get_set_bits_amount() const noexcept{return _mm_popcnt_u64(_value);}
    inline std::uint64_t get_index_last_bit() const noexcept{std::uint64_t result = lzcnt_u64(_value);return result == 64?64:63-result;}
    inline std::uint64_t get_index_first_bit() const noexcept{return tzcnt_u64(_value);}
    inline std::uint64_t get_first_bit() const noexcept{return blsi_u64(_value);}
    inline void clear_first_bit() noexcept{asm("blsr %1,%0":"+r"(_value):"r"(_value));}
    inline std::uint64_t get_extracted_value(const std::uint64_t mask)const noexcept{return pext_u64(_value, mask);}
    inline std::uint64_t get_deposited_value(const std::uint64_t mask) const noexcept{return pdep_u64(_value, mask);}
    inline std::uint64_t get_substr_value(const std::uint32_t start_index, const std::uint32_t len) const noexcept{return bextr_u64(_value,start_index,len);}
    template<class F>
    inline void for_each_bit(F proc) const noexcept{
        std::uint64_t idx;
        FxtBits tmp(_value);
        while(tmp._value){
            idx = tmp.get_index_first_bit();
            proc(idx);
            tmp.clear_first_bit();
        }
    }
    inline FxtBits& operator|=(const FxtBits& rhs) noexcept { _value |= rhs._value; return *this;}
    friend inline FxtBits operator|(const FxtBits& lhs, const FxtBits& rhs) noexcept {
        FxtBits res;
        res = lhs._value | rhs._value;
        return res;
    }
    inline FxtBits& operator&=(const FxtBits& rhs) noexcept { _value &= rhs._value; return *this;}
    friend inline FxtBits operator&(const FxtBits& lhs, const FxtBits& rhs) noexcept{
        FxtBits res;
        res = lhs._value & rhs._value;
        return res;
    }
};
#else
class FxtBits{
    std::uint64_t _value;
public:
    explicit FxtBits(const std::uint64_t value = 0) noexcept: _value(value){}
    FxtBits& operator=(const std::uint64_t value) noexcept{ _value = value;return *this;}
    inline std::uint64_t value() const noexcept{return _value;}
    inline std::uint8_t get(const std::uint64_t index) const noexcept{const std::uint64_t res = ( _value >> index) & 1ul ;return res;}
    inline void set(const std::uint64_t index)noexcept{_value |= (1ul << index);}
    inline void clear(const std::uint64_t index)noexcept{_value &= ~(1ul << index);}
    inline void reverse_bytes() noexcept{
        char tmp,*value = (char*)&_value;
        for(std::uint64_t idx = 0; idx < 4; ++idx){tmp = value[idx];value[idx] = value[7-idx];value[7-idx] = tmp;}
    }
    inline void clear_first_bit() noexcept{clear(get_index_first_bit());}
    inline std::uint64_t get_set_bits_amount() const noexcept{std::uint64_t res = 0;for(std::uint64_t tmp = _value; tmp; tmp >>= 1){res += tmp&1;}return res;}
    inline std::uint64_t get_index_last_bit() const noexcept{for(std::int64_t idx = 63; idx >= 0; idx--){if(((_value>>idx)&1) == 1)return idx;}return 64ul;}
    inline std::uint64_t get_index_first_bit() const noexcept{for(std::uint64_t idx = 0,tmp = _value; tmp; idx++, tmp >>=1){if(tmp&1)return idx;}return 64ul;}
    inline std::uint64_t get_first_bit() const noexcept{for(std::uint64_t idx = 0; idx < 64; idx++){if(_value&(1ul<<idx))return (1ul << idx);}return 0ul;}
    inline std::uint64_t get_extracted_value(std::uint64_t mask)const noexcept{
        std::uint64_t result = 0;
        for(std::uint64_t idx = 0, val_idx = 0; idx < 64; ++idx){
            if(((mask>>idx)&1) == 1){
                result = ((_value>>idx)&1) == 1? result | (1ul << val_idx): result & ~(1ul << val_idx);
                val_idx++;
            }
        }
        return result;
    }
    inline std::uint64_t get_deposited_value(std::uint64_t mask)const noexcept{
        std::uint64_t result = 0;
        for(std::uint64_t idx = 0, val_idx = 0; idx < 64; ++idx){
            result = ((mask>>idx)&1) == 1?
                     (((_value>>(val_idx++))&1) == 1?result | (1ul << idx):result & ~(1ul << idx)):
                     result & ~(1ul << idx);
        }
        return result;
    }
    inline std::uint64_t get_substr_value(const std::uint32_t start_index, const std::uint32_t len) const noexcept{
        std::uint64_t result = _value;
        return (result >> start_index) & ((1ul << len) - 1);
    }
    template<class F>
    inline void for_each_bit(F proc) const noexcept{for(std::uint64_t idx = 0, tmp = _value; tmp; tmp >>= 1, idx++){if(tmp&1){proc(idx);}}}

    inline FxtBits& operator|=(const FxtBits& rhs) noexcept { _value |= rhs._value; return *this;}
    friend inline FxtBits operator|(const FxtBits& lhs, const FxtBits& rhs) noexcept {
        FxtBits res;
        res = lhs._value | rhs._value;
        return res;
    }

    inline FxtBits& operator&=(const FxtBits& rhs) noexcept { _value &= rhs._value; return *this;}
    friend inline FxtBits operator&(const FxtBits& lhs, const FxtBits& rhs) noexcept{
        FxtBits res;
        res = lhs._value & rhs._value;
        return res;
    }
};

#endif
