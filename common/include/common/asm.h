#pragma once
#include "immintrin.h"

/*
 * uint64_t _tzcnt_u64(uint64_t value) [trailing zeroes count]
 *  Description:
 *      Count the number of trailing zero bits in unsigned 64-bit integer value, and return that count.
 *  Example:
 *      _tzcnt_u64(160=10100000bin) == 5
 *      _tzcnt_u64(0=0bin) == 64
 *  Used in:
 *      FxtBits::get_index_first_bit()
 *
 * uint64_t _lzcnt_u64(uint64_t value) [leading zeroes count]
 * Description:
 *      Count the number of leading zero bits in unsigned 64-bit integer a, and return that count in dst.
 *  Example:
 *      _lzcnt_u64(160=10100000bin) == 56
 *      _lzcnt_u64(0=0bin) == 64
 *  Used in:
 *      FxtBits::get_index_last_bit()
 *
 * uint64_t _blsi_u64(uint64_t value)
 * Description:
 *      Extract the lowest set bit from unsigned 64-bit integer value and set the corresponding bit in dst.
 *      All other bits in dst are zeroed, and all bits are zeroed if no bits are set in a. return dst.
 * Example:
 *      _blsi_u64(160=10100000bin) == 32=100000bin
 *      _blsi_u64(0=0bin) == 0=0bin
 * Used in:
 *      FxtBits::get_first_bit()
 *
 * uint64_t _blsr_u64(uint64_t value)
 * Description:
 *      Copy all bits from unsigned 64-bit integer value to dst, and reset (set to 0) the bit in dst that corresponds to the lowest set bit in value.
 * Example:
 *      _blsr_u64(160=10100000bin) == 128=10000000bin
 *      _blsr_u64(0=0bin) == 0=0bin
 * Used in:
 *      FxtBits::clear_first_bit()
 *
 * uint64_t _pext_u64(uint64_t value, uint64_t mask)
 * Description:
 *      Extract bits from unsigned 64-bit integer value at the corresponding bit locations specified by mask to contiguous low bits in dst;
 *      the remaining upper bits in dst are set to zero.
 * Example:
 *      _pext_u64(42=101010bin,62=111101bin) == 20=10100bin
 * Used in:
 *      FxtBits::get_extracted_value(uint64_t mask)
 *
 * uint64_t _pdep_u64(uint64_t value, uint64_t mask)
 * Description:
 *      Deposit contiguous low bits from unsigned 64-bit integer value to dst at the corresponding bit locations specified by mask;
 *      all other bits in dst are set to zero.
 * Example:
 *      _pdep_u64(11=1011bin,43=101011bin) == 100011bin
 * Used in:
 *      FxtBits::get_deposited_value(uint64_t mask)
 *
 * uint64_t _bextr_u64(uint64_t value, uint32_t start_index, uint32_t len)
 * Description:
 *      Extract contiguous bits from unsigned 64-bit integer a, and store the result in dst.
 *      Extract the number of bits specified by len, starting at the bit specified by start.
 * Example:
 *      _bextr_u64(683=1010101011bin,0=0bin,4=100bin)== 8=1011bin
 * Used in:
 *      FxtBits::get_substr_value(uint64_r start_index, uint64_t len)
 *
 * uint8_t _bittest64(int64_t* value, int64_t index)
 * Description:
 *      Return the bit at index index of 64-bit integer value.
 * Example:
 *      _bittest(13=1101bin,1=1bin) == 0
 * Used in:
 *      FxtBits::get(uint64_t index)
 *
 * uint8_t _bittestandreset64(int64_t* value, int64_t index)
 * Description:
 *      Return the bit at index index of 64-bit integer value, and set that bit to zero.
 * Example:
 *      uint64_t value = 15=1111bin;
 *      _bittest(value,1=1bin) == 1
 *      value == 13=1101bin
 * Used in:
 *      FxtBits::clear(uint64_t index)
 *
 * uint8_t _bittestandset64(int64_t* value, int64_t index)
 * Description:
 *      Return the bit at index index of 64-bit integer value, and set that bit to one.
 * Example:
 *      uint64_t value = 13=1101bin;
 *      _bittest(value,1=1bin) == 0
 *      value == 15=1111bin
 * Used in:
 *      FxtBits::set(uint64_t index)
 *
 * int64_t  _bswap64(int64_t value)
 * Description:
 *      Reverse the byte order of 64-bit integer a, and store the result in dst.
 *      This intrinsic is provided for conversion between little and big endian values.
 * Example:
 *      _bswap64(10111101_00..._00..._00..._00..._00..._00..._10000101bin) = 10000101_00..._00..._00..._00..._00..._00..._10111101bin
 * Used in:
 *      FxtBits::reverse_bytes()
 *
 *
 */

#if defined(__BMI__) && defined(__BMI2__)
inline std::uint64_t tzcnt_u64(const std::uint64_t _value) noexcept{std::uint64_t res;asm("tzcnt %1,%0": "=r"(res): "r"(_value));return res;}
inline std::uint64_t lzcnt_u64(const std::uint64_t _value) noexcept{std::uint64_t res;asm("lzcnt %1,%0": "=r"(res): "r"(_value));return res;}
inline std::uint64_t blsi_u64(const std::uint64_t _value) noexcept{std::uint64_t res;asm("blsi %1,%0":"=r"(res):"r"(_value));return res;}
inline std::uint64_t blsr_u64(const std::uint64_t _value) noexcept{std::uint64_t res;asm("blsr %1,%0":"=r"(res):"r"(_value));return res;}
inline std::uint64_t pext_u64(const std::uint64_t _value,const std::uint64_t mask) noexcept{std::uint64_t res;asm("pext %1,%2,%0":"=r"(res):"r"(mask),"r"(_value));return res;}
inline std::uint64_t pdep_u64(const std::uint64_t _value,const std::uint64_t mask) noexcept{std::uint64_t res;asm("pdep %1,%2,%0":"=r"(res):"r"(mask),"r"(_value));return res;}
inline std::uint64_t bextr_u64(const std::uint64_t _value,const std::uint64_t start_index, const std::uint64_t len) noexcept{
    std::uint64_t res;
    asm("bextr %1,%2,%0":"=r"(res):"r"(((start_index & 0xff) | ((len & 0xff) << 8))),"r"(_value));
    return res;
}
#else
inline std::uint64_t tzcnt_u64(const std::uint64_t _value) noexcept{for(std::uint64_t idx = 0,tmp = _value; tmp; idx++, tmp >>=1){if(tmp&1)return idx;}return 64ul;}
inline std::uint64_t lzcnt_u64(const std::uint64_t _value) noexcept{for(std::int64_t idx = 63; idx >= 0; idx--){if(((_value>>idx)&1) == 1)return idx;}return 64ul;}
inline std::uint64_t blsi_u64(const std::uint64_t _value) noexcept{for(std::uint64_t idx = 0; idx < 64; idx++){if(_value&(1ul<<idx))return (1ul << idx);}return 0ul;}
inline std::uint64_t blsr_u64(const std::uint64_t _value) noexcept{std::uint64_t res;asm("blsr %1,%0":"=r"(res):"r"(_value));return res;}
inline std::uint64_t pext_u64(const std::uint64_t _value,const std::uint64_t mask) noexcept{
    std::uint64_t result = 0;
        for(std::uint64_t idx = 0, val_idx = 0; idx < 64; ++idx){
            if(((mask>>idx)&1) == 1){
                result = ((_value>>idx)&1) == 1? result | (1ul << val_idx): result & ~(1ul << val_idx);
                val_idx++;
            }
        }
        return result;
}
inline std::uint64_t pdep_u64(const std::uint64_t _value,const std::uint64_t mask) noexcept{
    std::uint64_t result = 0;
        for(std::uint64_t idx = 0, val_idx = 0; idx < 64; ++idx){
            result = ((mask>>idx)&1) == 1?
                     (((_value>>(val_idx++))&1) == 1?result | (1ul << idx):result & ~(1ul << idx)):
                     result & ~(1ul << idx);
        }
        return result;
}
inline std::uint64_t bextr_u64(const std::uint64_t _value,const std::uint64_t start_index, const std::uint64_t len) noexcept{
    std::uint64_t result = _value;
    return (result >> start_index) & ((1ul << len) - 1);
}
#endif


#ifdef ALLOW_ASM_MARKERS
#define ASM_DO(NAME, CODE) asm volatile(NAME"_start%=:":); CODE; asm volatile(NAME"_end%=:":)
#else
#define ASM_DO(NAME, CODE) CODE;
#endif