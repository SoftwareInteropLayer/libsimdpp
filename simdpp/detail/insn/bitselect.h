/*  Copyright (C) 2024  Povilas Kanapickas <povilas@radix.lt>

    Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
            http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LIBSIMDPP_SIMDPP_DETAIL_INSN_BITSELECT_H
#define LIBSIMDPP_SIMDPP_DETAIL_INSN_BITSELECT_H

#ifndef LIBSIMDPP_SIMD_H
#error "This file must be included through simd.h"
#endif

#include <simdpp/types.h>
#include <simdpp/core/bit_and.h>
#include <simdpp/core/bit_andnot.h>
#include <simdpp/core/bit_or.h>
#include <simdpp/core/to_mask.h>
#include <simdpp/detail/null/shuffle.h>

namespace simdpp {
namespace SIMDPP_ARCH_NAMESPACE {
namespace detail {
namespace insn {

// -----------------------------------------------------------------------------
// uint8, uint8, uint8

static SIMDPP_INL
    uint8<16> i_bitselect(const uint8<16>& on, const uint8<16>& off, const uint8<16>& mask)
{
#if SIMDPP_USE_NULL
    return detail::null::bitselect(on, off, mask);
#elif SIMDPP_USE_AVX512VL
    return _mm_ternarylogic_epi32(on.native(), off.native(), mask.native(), 0xe4);
#elif SIMDPP_USE_XOP
    return _mm_cmov_si128(on.native(), off.native(), mask.native());
#elif SIMDPP_USE_SSE2
    uint8<16> mask_on = bit_and(on, mask);
    uint8<16> mask_off = bit_andnot(off, mask);
    return bit_or(mask_on, mask_off);
#elif SIMDPP_USE_NEON
    return vbslq_u8(mask.native(), on.native(), off.native());
#elif SIMDPP_USE_ALTIVEC
    return vec_sel(off.native(), on.native(), mask.native());
#elif SIMDPP_USE_MSA
    return __msa_bsel_v(mask.native(), off.native(), on.native());
#endif
}

#if SIMDPP_USE_AVX2
static SIMDPP_INL
    uint8<32> i_bitselect(const uint8<32>& on, const uint8<32>& off, const uint8<32>& mask)
{
#if SIMDPP_USE_AVX512VL
    return _mm256_ternarylogic_epi32(on.native(), off.native(), mask.native(), 0xe4);
#else
    uint8<32> mask_on = bit_and(on, mask);
    uint8<32> mask_off = bit_andnot(off, mask);
    return bit_or(mask_on, mask_off);
#endif
}
#endif

#if SIMDPP_USE_AVX512BW
SIMDPP_INL uint8<64> i_bitselect(const uint8<64>& on, const uint8<64>& off, const uint8<64>& mask)
{
    return _mm512_ternarylogic_epi32(on.native(), off.native(), mask.native(), 0xe4);
}
#endif

// -----------------------------------------------------------------------------
// uint8, uint8, mask_int8

static SIMDPP_INL
    uint8<16> i_bitselect(const uint8<16>& on, const uint8<16>& off, const mask_int8<16>& mask)
{
#if SIMDPP_USE_NULL
    return detail::null::blend_mask(on, off, mask);
#elif SIMDPP_USE_AVX512VL
    return _mm_mask_blend_epi8(mask.native(), off.native(), on.native());
#else
    return i_bitselect(on, off, uint8<16>(mask));
#endif
}

#if SIMDPP_USE_AVX2
static SIMDPP_INL
    uint8<32> i_bitselect(const uint8<32>& on, const uint8<32>& off, const mask_int8<32>& mask)
{
#if SIMDPP_USE_AVX512VL
    return _mm256_mask_blend_epi8(mask.native(), off.native(), on.native());
#else
    return i_bitselect(on, off, uint8<32>(mask));
#endif
}
#endif

#if SIMDPP_USE_AVX512BW
SIMDPP_INL uint8<64> i_bitselect(const uint8<64>& on, const uint8<64>& off, const mask_int8<64>& mask)
{
    return _mm512_mask_blend_epi8(mask.native(), off.native(), on.native());
}
#endif

// -----------------------------------------------------------------------------
// mask_int8, mask_int8, mask_int8

static SIMDPP_INL
    mask_int8<16> i_bitselect(const mask_int8<16>& on, const mask_int8<16>& off, const mask_int8<16>& mask)
{
#if SIMDPP_USE_NULL
    return detail::null::blend_mask(on, off, mask);
#elif SIMDPP_USE_AVX512VL
    return (on.native() & mask.native()) | (off.native() & ~(mask.native()));
#else
    return mask_int8<16>(i_bitselect(uint8<16>(on), uint8<16>(off), uint8<16>(mask)));
#endif
}

#if SIMDPP_USE_AVX2
static SIMDPP_INL
    mask_int8<32> i_bitselect(const mask_int8<32>& on, const mask_int8<32>& off,
                const mask_int8<32>& mask)
{
#if SIMDPP_USE_AVX512VL
    return (on.native() & mask.native()) | (off.native() & ~(mask.native()));
#else
    return mask_int8<32>(i_bitselect(uint8<32>(on), uint8<32>(off), uint8<32>(mask)));
#endif
}
#endif

#if SIMDPP_USE_AVX512BW
SIMDPP_INL mask_int8<64> i_bitselect(const mask_int8<64>& on, const mask_int8<64>& off,
                                     const mask_int8<64>& mask)
{
    return (on.native() & mask.native()) | (off.native() & ~(mask.native()));
}
#endif

// -----------------------------------------------------------------------------
// uint16, uint16, uint16

static SIMDPP_INL
    uint16<8> i_bitselect(const uint16<8>& on, const uint16<8>& off, const uint16<8>& mask)
{
    return uint16<8>(i_bitselect(uint8<16>(on), uint8<16>(off), uint8<16>(mask)));
}

#if SIMDPP_USE_AVX2
static SIMDPP_INL
    uint16<16> i_bitselect(const uint16<16>& on, const uint16<16>& off, const uint16<16>& mask)
{
#if SIMDPP_USE_AVX512VL
    return _mm256_ternarylogic_epi32(on.native(), off.native(), mask.native(), 0xe4);
#else
    return uint16<16>(i_bitselect(uint8<32>(on), uint8<32>(off), uint8<32>(mask)));
#endif
}
#endif

#if SIMDPP_USE_AVX512BW
SIMDPP_INL uint16<32> i_bitselect(const uint16<32>& on, const uint16<32>& off, const uint16<32>& mask)
{
    return _mm512_ternarylogic_epi32(on.native(), off.native(), mask.native(), 0xe4);
}
#endif

// -----------------------------------------------------------------------------
// uint16, uint16, mask_int16

static SIMDPP_INL
    uint16<8> i_bitselect(const uint16<8>& on, const uint16<8>& off, const mask_int16<8>& mask)
{
#if SIMDPP_USE_NULL
    return detail::null::blend_mask(on, off, mask);
#else
    return uint16<8>(i_bitselect(uint8<16>(on), uint8<16>(off), uint8<16>(mask)));
#endif
}

#if SIMDPP_USE_AVX2
static SIMDPP_INL
    uint16<16> i_bitselect(const uint16<16>& on, const uint16<16>& off, const mask_int16<16>& mask)
{
#if SIMDPP_USE_AVX512VL
    return _mm256_mask_blend_epi16(mask.native(), off.native(), on.native());
#else
    return i_bitselect(on, off, uint16<16>(mask));
#endif
}
#endif

#if SIMDPP_USE_AVX512BW
SIMDPP_INL uint16<32> i_bitselect(const uint16<32>& on, const uint16<32>& off, const mask_int16<32>& mask)
{
    return _mm512_mask_blend_epi16(mask.native(), off.native(), on.native());
}
#endif

// -----------------------------------------------------------------------------
// mask_int16, mask_int16, mask_int16

static SIMDPP_INL
    mask_int16<8> i_bitselect(const mask_int16<8>& on, const mask_int16<8>& off, const mask_int16<8>& mask)
{
#if SIMDPP_USE_NULL
    return detail::null::blend_mask(on, off, mask);
#elif SIMDPP_USE_AVX512VL
    return (on.native() & mask.native()) | (off.native() & ~(mask.native()));
#else
    return to_mask(i_bitselect(uint16<8>(on), uint16<8>(off), uint16<8>(mask)));
#endif
}

#if SIMDPP_USE_AVX2
static SIMDPP_INL
    mask_int16<16> i_bitselect(const mask_int16<16>& on, const mask_int16<16>& off, const mask_int16<16>& mask)
{
#if SIMDPP_USE_AVX512VL
    return (on.native() & mask.native()) | (off.native() & ~(mask.native()));
#else
    return to_mask(i_bitselect(uint16<16>(on), uint16<16>(off), uint16<16>(mask)));
#endif
}
#endif

#if SIMDPP_USE_AVX512BW
SIMDPP_INL mask_int16<32> i_bitselect(const mask_int16<32>& on, const mask_int16<32>& off, const mask_int16<32>& mask)
{
    return (on.native() & mask.native()) | (off.native() & ~mask.native());
}
#endif

// -----------------------------------------------------------------------------
// uint32, uint32, uint32

static SIMDPP_INL
    uint32<4> i_bitselect(const uint32<4>& on, const uint32<4>& off, const uint32<4>& mask)
{
    return uint32<4>(i_bitselect(uint8<16>(on), uint8<16>(off), uint8<16>(mask)));
}

#if SIMDPP_USE_AVX2
static SIMDPP_INL
    uint32<8> i_bitselect(const uint32<8>& on, const uint32<8>& off, const uint32<8>& mask)
{
#if SIMDPP_USE_AVX512VL
    return _mm256_ternarylogic_epi32(on.native(), off.native(), mask.native(), 0xe4);
#else
    uint32<8> mask_on = bit_and(on, mask);
    uint32<8> mask_off = bit_andnot(off, mask);
    return bit_or(mask_on, mask_off);
#endif
}
#endif

#if SIMDPP_USE_AVX512F
static SIMDPP_INL
    uint32<16> i_bitselect(const uint32<16>& on, const uint32<16>& off, const uint32<16>& mask)
{
    return _mm512_ternarylogic_epi32(on.native(), off.native(), mask.native(), 0xe4);
}
#endif

// -----------------------------------------------------------------------------
// uint32, uint32, mask_int32

static SIMDPP_INL
    uint32<4> i_bitselect(const uint32<4>& on, const uint32<4>& off, const mask_int32<4>& mask)
{
#if SIMDPP_USE_NULL
    return detail::null::blend_mask(on, off, mask);
#else
    return uint32<4>(i_bitselect(uint8<16>(on), uint8<16>(off), uint8<16>(mask)));
#endif
}

#if SIMDPP_USE_AVX2
static SIMDPP_INL
    uint32<8> i_bitselect(const uint32<8>& on, const uint32<8>& off, const mask_int32<8>& mask)
{
#if SIMDPP_USE_AVX512VL
    return _mm256_mask_blend_epi32(mask.native(), off.native(), on.native());
#else
    return i_bitselect(on, off, uint32<8>(mask));
#endif
}
#endif

#if SIMDPP_USE_AVX512F
static SIMDPP_INL
    uint32<16> i_bitselect(const uint32<16>& on, const uint32<16>& off, const mask_int32<16>& mask)
{
    return _mm512_mask_blend_epi32(mask.native(), off.native(), on.native());
}
#endif

// -----------------------------------------------------------------------------
// mask_int32, mask_int32, mask_int32

static SIMDPP_INL
    mask_int32<4> i_bitselect(const mask_int32<4>& on, const mask_int32<4>& off, const mask_int32<4>& mask)
{
#if SIMDPP_USE_NULL
    return detail::null::blend_mask(on, off, mask);
#elif SIMDPP_USE_AVX512VL
    return (on.native() & mask.native()) | (off.native() & ~(mask.native()));
#else
    return to_mask(i_bitselect(uint32<4>(on), uint32<4>(off), uint32<4>(mask)));
#endif
}

#if SIMDPP_USE_AVX2
static SIMDPP_INL
    mask_int32<8> i_bitselect(const mask_int32<8>& on, const mask_int32<8>& off, const mask_int32<8>& mask)
{
#if SIMDPP_USE_AVX512VL
    return (on.native() & mask.native()) | (off.native() & ~(mask.native()));
#else
    return to_mask(i_bitselect(uint32<8>(on), uint32<8>(off), uint32<8>(mask)));
#endif
}
#endif

#if SIMDPP_USE_AVX512F
static SIMDPP_INL
    mask_int32<16> i_bitselect(const mask_int32<16>& on, const mask_int32<16>& off, const mask_int32<16>& mask)
{
    return _mm512_kor(_mm512_kand(on.native(), mask.native()),
                      _mm512_kandn(mask.native(), off.native()));
}
#endif

// -----------------------------------------------------------------------------
// float32, float32, float32

static SIMDPP_INL
    float32<4> i_bitselect(const float32<4>& on, const float32<4>& off, const float32<4>& mask)
{
#if SIMDPP_USE_NULL || SIMDPP_USE_NEON_NO_FLT_SP
    return detail::null::bitselect(on, off, mask);
#elif SIMDPP_USE_AVX512VL
    return _mm_castsi128_ps(_mm_ternarylogic_epi32(_mm_castps_si128(on.native()),
                                                   _mm_castps_si128(off.native()),
                                                   _mm_castps_si128(mask.native()),
                                                   0xe4));
#elif SIMDPP_USE_SSE2
    float32<4> mask_on = bit_and(on, mask);
    float32<4> mask_off = bit_andnot(off, mask);
    return bit_or(mask_on, mask_off);
#elif SIMDPP_USE_NEON
    return vbslq_f32(uint32x4(mask).native(), on.native(), off.native());
#elif SIMDPP_USE_ALTIVEC
    return vec_sel(off.native(), on.native(), mask.native());
#elif SIMDPP_USE_MSA
    return (v4f32) __msa_bsel_v((v16u8) mask.native(),
                                (v16u8) off.native(), (v16u8) on.native());
#endif
}

#if SIMDPP_USE_AVX
static SIMDPP_INL
    float32<8> i_bitselect(const float32<8>& on, const float32<8>& off, const float32<8>& mask)
{
#if SIMDPP_USE_AVX512VL
    return _mm256_castsi256_ps(_mm256_ternarylogic_epi32(_mm256_castps_si256(on.native()),
                                                         _mm256_castps_si256(off.native()),
                                                         _mm256_castps_si256(mask.native()),
                                                         0xe4));
#else
    float32<8> mask_on = bit_and(on, mask);
    float32<8> mask_off = bit_andnot(off, mask);
    return bit_or(mask_on, mask_off);
#endif
}
#endif

#if SIMDPP_USE_AVX512F
static SIMDPP_INL
    float32<16> i_bitselect(const float32<16>& on, const float32<16>& off, const float32<16>& mask)
{
    return _mm512_castsi512_ps(_mm512_ternarylogic_epi32(_mm512_castps_si512(on.native()),
                                                         _mm512_castps_si512(off.native()),
                                                         _mm512_castps_si512(mask.native()),
                                                         0xe4));
}
#endif

// -----------------------------------------------------------------------------
// float32, float32, mask_float32

static SIMDPP_INL
    float32<4> i_bitselect(const float32<4>& on, const float32<4>& off, const mask_float32<4>& mask)
{
#if SIMDPP_USE_NULL
    return detail::null::blend_mask(on, off, mask);
#elif SIMDPP_USE_AVX512VL
    return _mm_mask_blend_ps(mask.native(), off.native(), on.native());
#else
    return i_bitselect(on, off, float32<4>(mask));
#endif
}

#if SIMDPP_USE_AVX
static SIMDPP_INL
    float32<8> i_bitselect(const float32<8>& on, const float32<8>& off, const mask_float32<8>& mask)
{
#if SIMDPP_USE_AVX512VL
    return _mm256_mask_blend_ps(mask.native(), off.native(), on.native());
#else
    return i_bitselect(on, off, float32<8>(mask));
#endif
}
#endif

#if SIMDPP_USE_AVX512F
static SIMDPP_INL
    float32<16> i_bitselect(const float32<16>& on, const float32<16>& off, const mask_float32<16>& mask)
{
    return _mm512_mask_blend_ps(mask.native(), off.native(), on.native());
}
#endif

// -----------------------------------------------------------------------------
// mask_float32, mask_float32, mask_float32

static SIMDPP_INL
    mask_float32<4> i_bitselect(const mask_float32<4>& on, const mask_float32<4>& off, const mask_float32<4>& mask)
{
#if SIMDPP_USE_NULL || SIMDPP_USE_NEON_NO_FLT_SP
    return detail::null::blend_mask(on, off, mask);
#elif SIMDPP_USE_AVX512VL
    return (on.native() & mask.native()) | (off.native() & ~(mask.native()));
#else
    return to_mask(i_bitselect(float32<4>(on), float32<4>(off), float32<4>(mask)));
#endif
}

#if SIMDPP_USE_AVX
static SIMDPP_INL
    mask_float32<8> i_bitselect(const mask_float32<8>& on, const mask_float32<8>& off,const mask_float32<8>& mask)
{
#if SIMDPP_USE_AVX512VL
    return (on.native() & mask.native()) | (off.native() & ~(mask.native()));
#else
    return to_mask(i_bitselect(float32<8>(on), float32<8>(off), float32<8>(mask)));
#endif
}
#endif

#if SIMDPP_USE_AVX512F
static SIMDPP_INL
    mask_float32<16> i_bitselect(const mask_float32<16>& on, const mask_float32<16>& off, const mask_float32<16>& mask)
{
    return _mm512_kor(_mm512_kand(on.native(), mask.native()),
                      _mm512_kandn(mask.native(), off.native()));
}
#endif

// -----------------------------------------------------------------------------
// uint64, uint64, uint64

static SIMDPP_INL
    uint64<2> i_bitselect(const uint64<2>& on, const uint64<2>& off, const uint64<2>& mask)
{
#if SIMDPP_USE_NULL || (SIMDPP_USE_ALTIVEC && !SIMDPP_USE_VSX_207)
    return detail::null::bitselect(on, off, mask);
#elif SIMDPP_USE_AVX512VL
    return _mm_ternarylogic_epi64(on.native(), off.native(), mask.native(), 0xe4);
#else
    return uint64<2>(i_bitselect(uint8<16>(on), uint8<16>(off), uint8<16>(mask)));
#endif
}

#if SIMDPP_USE_AVX2
static SIMDPP_INL
    uint64<4> i_bitselect(const uint64<4>& on, const uint64<4>& off, const uint64<4>& mask)
{
#if SIMDPP_USE_AVX512VL
    return _mm256_ternarylogic_epi64(on.native(), off.native(), mask.native(), 0xe4);
#else
    uint64<4> mask_on = bit_and(on, mask);
    uint64<4> mask_off = bit_andnot(off, mask);
    return bit_or(mask_on, mask_off);
#endif
}
#endif

#if SIMDPP_USE_AVX512F
static SIMDPP_INL
    uint64<8> i_bitselect(const uint64<8>& on, const uint64<8>& off, const uint64<8>& mask)
{
    return _mm512_ternarylogic_epi64(on.native(), off.native(), mask.native(), 0xe4);
}
#endif

// -----------------------------------------------------------------------------
// uint64, uint64, mask_int64

static SIMDPP_INL
    uint64<2> i_bitselect(const uint64<2>& on, const uint64<2>& off, const mask_int64<2>& mask)
{
#if SIMDPP_USE_NULL || (SIMDPP_USE_ALTIVEC && !SIMDPP_USE_VSX_207)
    return detail::null::blend_mask(on, off, mask);
#elif SIMDPP_USE_AVX512VL
    return _mm_mask_blend_epi64(mask.native(), off.native(), on.native());
#else
    return uint64<2>(i_bitselect(uint8<16>(on), uint8<16>(off), uint8<16>(mask)));
#endif
}

#if SIMDPP_USE_AVX2
static SIMDPP_INL
    uint64<4> i_bitselect(const uint64<4>& on, const uint64<4>& off, const mask_int64<4>& mask)
{
#if SIMDPP_USE_AVX512VL
    return _mm256_mask_blend_epi64(mask.native(), off.native(), on.native());
#else
    return i_bitselect(on, off, uint64<4>(mask));
#endif
}
#endif

#if SIMDPP_USE_AVX512F
static SIMDPP_INL
    uint64<8> i_bitselect(const uint64<8>& on, const uint64<8>& off, const mask_int64<8>& mask)
{
    return _mm512_mask_blend_epi64(mask.native(), off.native(), on.native());
}
#endif

// -----------------------------------------------------------------------------
// mask_int64, mask_int64, mask_int64

static SIMDPP_INL
    mask_int64<2> i_bitselect(const mask_int64<2>& on, const mask_int64<2>& off, const mask_int64<2>& mask)
{
#if SIMDPP_USE_NULL || (SIMDPP_USE_ALTIVEC && !SIMDPP_USE_VSX_207)
    return detail::null::blend_mask(on, off, mask);
#elif SIMDPP_USE_AVX512VL
    return (on.native() & mask.native()) | (off.native() & ~(mask.native()));
#else
    return to_mask(i_bitselect(uint64<2>(on), uint64<2>(off), uint64<2>(mask)));
#endif
}

#if SIMDPP_USE_AVX2
static SIMDPP_INL
    mask_int64<4> i_bitselect(const mask_int64<4>& on, const mask_int64<4>& off, const mask_int64<4>& mask)
{
#if SIMDPP_USE_AVX512VL
    return (on.native() & mask.native()) | (off.native() & ~(mask.native()));
#else
    return to_mask(i_bitselect(uint64<4>(on), uint64<4>(off), uint64<4>(mask)));
#endif
}
#endif

#if SIMDPP_USE_AVX512F
static SIMDPP_INL
    mask_int64<8> i_bitselect(const mask_int64<8>& on, const mask_int64<8>& off, const mask_int64<8>& mask)
{
    return _mm512_kor(_mm512_kand(on.native(), mask.native()),
                      _mm512_kandn(mask.native(), off.native()));
}
#endif

// -----------------------------------------------------------------------------
// float64, float64, float64

static SIMDPP_INL
    float64<2> i_bitselect(const float64<2>& on, const float64<2>& off, const float64<2>& mask)
{
#if SIMDPP_USE_AVX512VL
    return _mm_castsi128_pd(_mm_ternarylogic_epi64(_mm_castpd_si128(on.native()),
                                                   _mm_castpd_si128(off.native()),
                                                   _mm_castpd_si128(mask.native()),
                                                   0xe4));
#elif SIMDPP_USE_SSE2
    float64x2 mask_on = bit_and(on, mask);
    float64x2 mask_off = bit_andnot(off, mask);
    return bit_or(mask_on, mask_off);
#elif SIMDPP_USE_NEON64
    return vbslq_f64(vreinterpretq_u64_f64(mask.native()), on.native(), off.native());
#elif SIMDPP_USE_VSX_206
    return vec_sel(off.native(), on.native(), mask.native());
#elif SIMDPP_USE_MSA
    return (v2f64) __msa_bsel_v((v16u8) mask.native(),
                                (v16u8) off.native(), (v16u8) on.native());
#elif SIMDPP_USE_NULL || SIMDPP_USE_NEON32 || SIMDPP_USE_ALTIVEC
    return detail::null::bitselect(on, off, mask);
#endif
}

#if SIMDPP_USE_AVX
static SIMDPP_INL
    float64<4> i_bitselect(const float64<4>& on, const float64<4>& off, const float64<4>& mask)
{
#if SIMDPP_USE_AVX512VL
    return _mm256_castsi256_pd(_mm256_ternarylogic_epi32(_mm256_castpd_si256(on.native()),
                                                         _mm256_castpd_si256(off.native()),
                                                         _mm256_castpd_si256(mask.native()),
                                                         0xe4));
#else
    float64<4> mask_on = bit_and(on, mask);
    float64<4> mask_off = bit_andnot(off, mask);
    return bit_or(mask_on, mask_off);
#endif
}
#endif

#if SIMDPP_USE_AVX512F
static SIMDPP_INL
    float64<8> i_bitselect(const float64<8>& on, const float64<8>& off, const float64<8>& mask)
{
    return _mm512_castsi512_pd(_mm512_ternarylogic_epi32(_mm512_castpd_si512(on.native()),
                                                         _mm512_castpd_si512(off.native()),
                                                         _mm512_castpd_si512(mask.native()),
                                                         0xe4));
}
#endif

// -----------------------------------------------------------------------------
// float64, float64, mask_float64

static SIMDPP_INL
    float64<2> i_bitselect(const float64<2>& on, const float64<2>& off, const mask_float64<2>& mask)
{
#if SIMDPP_USE_NULL || SIMDPP_USE_NEON32 || (SIMDPP_USE_ALTIVEC && !SIMDPP_USE_VSX_206)
    return detail::null::blend_mask(on, off, mask);
#else
    return i_bitselect(on, off, float64<2>(mask));
#endif
}

#if SIMDPP_USE_AVX
static SIMDPP_INL
    float64<4> i_bitselect(const float64<4>& on, const float64<4>& off, const mask_float64<4>& mask)
{
#if SIMDPP_USE_AVX512VL
    return _mm256_mask_blend_pd(mask.native(), off.native(), on.native());
#else
    return i_bitselect(on, off, float64<4>(mask));
#endif
}
#endif

#if SIMDPP_USE_AVX512F
static SIMDPP_INL
    float64<8> i_bitselect(const float64<8>& on, const float64<8>& off, const mask_float64<8>& mask)
{
    return _mm512_mask_blend_pd(mask.native(), off.native(), on.native());
}
#endif

// -----------------------------------------------------------------------------
// mask_float64, mask_float64, mask_float64

static SIMDPP_INL
    mask_float64<2> i_bitselect(const mask_float64<2>& on, const mask_float64<2>& off, const mask_float64<2>& mask)
{
#if SIMDPP_USE_NULL || SIMDPP_USE_NEON32 || (SIMDPP_USE_ALTIVEC && !SIMDPP_USE_VSX_206)
    return detail::null::bitselect(on, off, mask);
#elif SIMDPP_USE_AVX512VL
    return (on.native() & mask.native()) | (off.native() & ~(mask.native()));
#else
    return to_mask(i_bitselect(float64<2>(on), float64<2>(off), float64<2>(mask)));
#endif
}

#if SIMDPP_USE_AVX
static SIMDPP_INL
    mask_float64<4> i_bitselect(const mask_float64<4>& on, const mask_float64<4>& off, const mask_float64<4>& mask)
{
#if SIMDPP_USE_AVX512VL
    return (on.native() & mask.native()) | (off.native() & ~(mask.native()));
#else
    return to_mask(i_bitselect(float64<4>(on), float64<4>(off), float64<4>(mask)));
#endif
}
#endif

#if SIMDPP_USE_AVX512F
static SIMDPP_INL
    mask_float64<8> i_bitselect(const mask_float64<8>& on, const mask_float64<8>& off, const mask_float64<8>& mask)
{
    return _mm512_kor(_mm512_kand(on.native(), mask.native()),
                      _mm512_kandn(mask.native(), off.native()));
}
#endif

// -----------------------------------------------------------------------------

template<class V1, class V2, class V3> SIMDPP_INL
    V1 i_bitselect(const V1& on, const V2& off, const V3& mask)
{
    SIMDPP_VEC_ARRAY_IMPL3(V1, i_bitselect, on, off, mask)
}


} // namespace insn
} // namespace detail
} // namespace SIMDPP_ARCH_NAMESPACE
} // namespace simdpp

#endif

