// After editing this file, run "go generate" in this directory.

#ifndef WUFFS_BASE_HEADER_H
#define WUFFS_BASE_HEADER_H

// Copyright 2017 The Wuffs Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Wuffs assumes that:
//  - converting a uint32_t to a size_t will never overflow.
//  - converting a size_t to a uint64_t will never overflow.
#if (__WORDSIZE != 32) && (__WORDSIZE != 64)
#error "Wuffs requires a word size of either 32 or 64 bits"
#endif

// WUFFS_VERSION is the major.minor version number as a uint32_t. The major
// number is the high 16 bits. The minor number is the low 16 bits.
//
// The intention is to bump the version number at least on every API / ABI
// backwards incompatible change.
//
// For now, the API and ABI are simply unstable and can change at any time.
//
// TODO: don't hard code this in base-header.h.
#define WUFFS_VERSION ((uint32_t)0x00001)

// ---------------- Fundamentals

// Flicks are a unit of time. One flick (frame-tick) is 1 / 705_600_000 of a
// second. See https://github.com/OculusVR/Flicks
typedef uint64_t wuffs_base__flicks;

#define WUFFS_BASE__FLICKS_PER_SECOND ((uint64_t)705600000)
#define WUFFS_BASE__FLICKS_PER_MILLISECOND ((uint64_t)705600)

// wuffs_base__empty_struct is used when a Wuffs function returns an empty
// struct. In C, if a function f returns void, you can't say "x = f()", but in
// Wuffs, if a function g returns empty, you can say "y = g()".
typedef struct {
} wuffs_base__empty_struct;

// wuffs_base__slice_u8 is a 1-dimensional buffer.
//
// A value with all fields NULL or zero is a valid, empty slice.
typedef struct {
  uint8_t* ptr;
  size_t len;
} wuffs_base__slice_u8;

// wuffs_base__table_u8 is a 2-dimensional buffer.
//
// A value with all fields NULL or zero is a valid, empty table.
typedef struct {
  uint8_t* ptr;
  size_t width;
  size_t height;
  size_t stride;
} wuffs_base__table_u8;

// --------

static inline uint8_t wuffs_base__u8__min(uint8_t x, uint8_t y) {
  return x < y ? x : y;
}

static inline uint8_t wuffs_base__u8__max(uint8_t x, uint8_t y) {
  return x > y ? x : y;
}

static inline uint16_t wuffs_base__u16__min(uint16_t x, uint16_t y) {
  return x < y ? x : y;
}

static inline uint16_t wuffs_base__u16__max(uint16_t x, uint16_t y) {
  return x > y ? x : y;
}

static inline uint32_t wuffs_base__u32__min(uint32_t x, uint32_t y) {
  return x < y ? x : y;
}

static inline uint32_t wuffs_base__u32__max(uint32_t x, uint32_t y) {
  return x > y ? x : y;
}

static inline uint64_t wuffs_base__u64__min(uint64_t x, uint64_t y) {
  return x < y ? x : y;
}

static inline uint64_t wuffs_base__u64__max(uint64_t x, uint64_t y) {
  return x > y ? x : y;
}

// --------

// Saturating arithmetic (sat_add, sat_sub) branchless bit-twiddling algorithms
// are per https://locklessinc.com/articles/sat_arithmetic/
//
// It is important that the underlying types are unsigned integers, as signed
// integer arithmetic overflow is undefined behavior in C.

static inline uint8_t wuffs_base__u8__sat_add(uint8_t x, uint8_t y) {
  uint8_t res = x + y;
  res |= -(res < x);
  return res;
}

static inline uint8_t wuffs_base__u8__sat_sub(uint8_t x, uint8_t y) {
  uint8_t res = x - y;
  res &= -(res <= x);
  return res;
}

static inline uint16_t wuffs_base__u16__sat_add(uint16_t x, uint16_t y) {
  uint16_t res = x + y;
  res |= -(res < x);
  return res;
}

static inline uint16_t wuffs_base__u16__sat_sub(uint16_t x, uint16_t y) {
  uint16_t res = x - y;
  res &= -(res <= x);
  return res;
}

static inline uint32_t wuffs_base__u32__sat_add(uint32_t x, uint32_t y) {
  uint32_t res = x + y;
  res |= -(res < x);
  return res;
}

static inline uint32_t wuffs_base__u32__sat_sub(uint32_t x, uint32_t y) {
  uint32_t res = x - y;
  res &= -(res <= x);
  return res;
}

static inline uint64_t wuffs_base__u64__sat_add(uint64_t x, uint64_t y) {
  uint64_t res = x + y;
  res |= -(res < x);
  return res;
}

static inline uint64_t wuffs_base__u64__sat_sub(uint64_t x, uint64_t y) {
  uint64_t res = x - y;
  res &= -(res <= x);
  return res;
}

// --------

// Clang also defines "__GNUC__".

static inline uint16_t wuffs_base__u16__byte_swapped(uint16_t x) {
#if defined(__GNUC__)
  return __builtin_bswap16(x);
#else
  return (x >> 8) | (x << 8);
#endif
}

static inline uint32_t wuffs_base__u32__byte_swapped(uint32_t x) {
#if defined(__GNUC__)
  return __builtin_bswap32(x);
#else
  static const uint32_t mask8 = 0x00FF00FF;
  x = ((x >> 8) & mask8) | ((x & mask8) << 8);
  return (x >> 16) | (x << 16);
#endif
}

static inline uint64_t wuffs_base__u64__byte_swapped(uint64_t x) {
#if defined(__GNUC__)
  return __builtin_bswap64(x);
#else
  static const uint64_t mask8 = 0x00FF00FF00FF00FF;
  static const uint64_t mask16 = 0x0000FFFF0000FFFF;
  x = ((x >> 8) & mask8) | ((x & mask8) << 8);
  x = ((x >> 16) & mask16) | ((x & mask16) << 16);
  return (x >> 32) | (x << 32);
#endif
}

// ---------------- Ranges and Rects

// Ranges are either inclusive ("range_ii") or exclusive ("range_ie") on the
// high end. Both the "ii" and "ie" flavors are useful in practice.
//
// The "ei" and "ee" flavors also exist in theory, but aren't widely used. In
// Wuffs, the low end is always inclusive.
//
// The "ii" (closed interval) flavor is useful when refining e.g. "the set of
// all uint32_t values" to a contiguous subset: "uint32_t values in the closed
// interval [M, N]", for uint32_t values M and N. An unrefined type (in other
// words, the set of all uint32_t values) is not representable in the "ie"
// flavor because if N equals ((1<<32) - 1) then (N + 1) will overflow.
//
// On the other hand, the "ie" (half-open interval) flavor is recommended by
// Dijkstra's "Why numbering should start at zero" at
// http://www.cs.utexas.edu/users/EWD/ewd08xx/EWD831.PDF and a further
// discussion of motivating rationale is at
// https://www.quora.com/Why-are-Python-ranges-half-open-exclusive-instead-of-closed-inclusive
//
// For example, with "ie", the number of elements in "uint32_t values in the
// half-open interval [M, N)" is equal to max(0, N-M). Furthermore, that number
// of elements (in one dimension, a length, in two dimensions, a width or
// height) is itself representable as a uint32_t without overflow, again for
// uint32_t values M and N. In the contrasting "ii" flavor, the length of the
// closed interval [0, (1<<32) - 1] is 1<<32, which cannot be represented as a
// uint32_t. In Wuffs, because of this potential overflow, the "ie" flavor has
// length / width / height methods, but the "ii" flavor does not.
//
// It is valid for min > max (for range_ii) or for min >= max (for range_ie),
// in which case the range is empty. There are multiple representations of an
// empty range.

typedef struct {
  uint32_t min_inclusive;
  uint32_t max_inclusive;
} wuffs_base__range_ii_u32;

static inline bool wuffs_base__range_ii_u32__is_empty(
    wuffs_base__range_ii_u32 r) {
  return r.min_inclusive > r.max_inclusive;
}

static inline bool wuffs_base__range_ii_u32__contains(
    wuffs_base__range_ii_u32 r,
    uint32_t x) {
  return (r.min_inclusive <= x) && (x <= r.max_inclusive);
}

static inline wuffs_base__range_ii_u32 wuffs_base__range_ii_u32__intersection(
    wuffs_base__range_ii_u32 r,
    wuffs_base__range_ii_u32 s) {
  r.min_inclusive = wuffs_base__u32__max(r.min_inclusive, s.min_inclusive);
  r.max_inclusive = wuffs_base__u32__min(r.max_inclusive, s.max_inclusive);
  return r;
}

static inline wuffs_base__range_ii_u32 wuffs_base__range_ii_u32__union(
    wuffs_base__range_ii_u32 r,
    wuffs_base__range_ii_u32 s) {
  if (wuffs_base__range_ii_u32__is_empty(r)) {
    return s;
  }
  if (wuffs_base__range_ii_u32__is_empty(s)) {
    return r;
  }
  r.min_inclusive = wuffs_base__u32__min(r.min_inclusive, s.min_inclusive);
  r.max_inclusive = wuffs_base__u32__max(r.max_inclusive, s.max_inclusive);
  return r;
}

// --------

typedef struct {
  uint32_t min_inclusive;
  uint32_t max_exclusive;
} wuffs_base__range_ie_u32;

static inline bool wuffs_base__range_ie_u32__is_empty(
    wuffs_base__range_ie_u32 r) {
  return r.min_inclusive >= r.max_exclusive;
}

static inline bool wuffs_base__range_ie_u32__contains(
    wuffs_base__range_ie_u32 r,
    uint32_t x) {
  return (r.min_inclusive <= x) && (x < r.max_exclusive);
}

static inline wuffs_base__range_ie_u32 wuffs_base__range_ie_u32__intersection(
    wuffs_base__range_ie_u32 r,
    wuffs_base__range_ie_u32 s) {
  r.min_inclusive = wuffs_base__u32__max(r.min_inclusive, s.min_inclusive);
  r.max_exclusive = wuffs_base__u32__min(r.max_exclusive, s.max_exclusive);
  return r;
}

static inline wuffs_base__range_ie_u32 wuffs_base__range_ie_u32__union(
    wuffs_base__range_ie_u32 r,
    wuffs_base__range_ie_u32 s) {
  if (wuffs_base__range_ie_u32__is_empty(r)) {
    return s;
  }
  if (wuffs_base__range_ie_u32__is_empty(s)) {
    return r;
  }
  r.min_inclusive = wuffs_base__u32__min(r.min_inclusive, s.min_inclusive);
  r.max_exclusive = wuffs_base__u32__max(r.max_exclusive, s.max_exclusive);
  return r;
}

static inline uint32_t wuffs_base__range_ie_u32__length(
    wuffs_base__range_ie_u32 r) {
  return wuffs_base__u32__sat_sub(r.max_exclusive, r.min_inclusive);
}

// --------

typedef struct {
  uint64_t min_inclusive;
  uint64_t max_inclusive;
} wuffs_base__range_ii_u64;

static inline bool wuffs_base__range_ii_u64__is_empty(
    wuffs_base__range_ii_u64 r) {
  return r.min_inclusive > r.max_inclusive;
}

static inline bool wuffs_base__range_ii_u64__contains(
    wuffs_base__range_ii_u64 r,
    uint64_t x) {
  return (r.min_inclusive <= x) && (x <= r.max_inclusive);
}

static inline wuffs_base__range_ii_u64 wuffs_base__range_ii_u64__intersection(
    wuffs_base__range_ii_u64 r,
    wuffs_base__range_ii_u64 s) {
  r.min_inclusive = wuffs_base__u64__max(r.min_inclusive, s.min_inclusive);
  r.max_inclusive = wuffs_base__u64__min(r.max_inclusive, s.max_inclusive);
  return r;
}

static inline wuffs_base__range_ii_u64 wuffs_base__range_ii_u64__union(
    wuffs_base__range_ii_u64 r,
    wuffs_base__range_ii_u64 s) {
  if (wuffs_base__range_ii_u64__is_empty(r)) {
    return s;
  }
  if (wuffs_base__range_ii_u64__is_empty(s)) {
    return r;
  }
  r.min_inclusive = wuffs_base__u64__min(r.min_inclusive, s.min_inclusive);
  r.max_inclusive = wuffs_base__u64__max(r.max_inclusive, s.max_inclusive);
  return r;
}

// --------

typedef struct {
  uint64_t min_inclusive;
  uint64_t max_exclusive;
} wuffs_base__range_ie_u64;

static inline bool wuffs_base__range_ie_u64__is_empty(
    wuffs_base__range_ie_u64 r) {
  return r.min_inclusive >= r.max_exclusive;
}

static inline bool wuffs_base__range_ie_u64__contains(
    wuffs_base__range_ie_u64 r,
    uint64_t x) {
  return (r.min_inclusive <= x) && (x < r.max_exclusive);
}

static inline wuffs_base__range_ie_u64 wuffs_base__range_ie_u64__intersection(
    wuffs_base__range_ie_u64 r,
    wuffs_base__range_ie_u64 s) {
  r.min_inclusive = wuffs_base__u64__max(r.min_inclusive, s.min_inclusive);
  r.max_exclusive = wuffs_base__u64__min(r.max_exclusive, s.max_exclusive);
  return r;
}

static inline wuffs_base__range_ie_u64 wuffs_base__range_ie_u64__union(
    wuffs_base__range_ie_u64 r,
    wuffs_base__range_ie_u64 s) {
  if (wuffs_base__range_ie_u64__is_empty(r)) {
    return s;
  }
  if (wuffs_base__range_ie_u64__is_empty(s)) {
    return r;
  }
  r.min_inclusive = wuffs_base__u64__min(r.min_inclusive, s.min_inclusive);
  r.max_exclusive = wuffs_base__u64__max(r.max_exclusive, s.max_exclusive);
  return r;
}

static inline uint64_t wuffs_base__range_ie_u64__length(
    wuffs_base__range_ie_u64 r) {
  return wuffs_base__u64__sat_sub(r.max_exclusive, r.min_inclusive);
}

// --------

// wuffs_base__rect_ii_u32 is a rectangle (a 2-dimensional range) on the
// integer grid. The "ii" means that the bounds are inclusive on the low end
// and inclusive on the high end. It contains all points (x, y) such that
// ((min_inclusive_x <= x) && (x <= max_inclusive_x)) and likewise for y.
//
// It is valid for min > max, in which case the rectangle is empty. There are
// multiple representations of an empty rectangle.
//
// The X and Y axes increase right and down.
typedef struct {
  uint32_t min_inclusive_x;
  uint32_t min_inclusive_y;
  uint32_t max_inclusive_x;
  uint32_t max_inclusive_y;
} wuffs_base__rect_ii_u32;

static inline bool wuffs_base__rect_ii_u32__is_empty(
    wuffs_base__rect_ii_u32 r) {
  return (r.min_inclusive_x > r.max_inclusive_x) ||
         (r.min_inclusive_y > r.max_inclusive_y);
}

static inline bool wuffs_base__rect_ii_u32__contains(wuffs_base__rect_ii_u32 r,
                                                     uint32_t x,
                                                     uint32_t y) {
  return (r.min_inclusive_x <= x) && (x <= r.max_inclusive_x) &&
         (r.min_inclusive_y <= y) && (y <= r.max_inclusive_y);
}

static inline wuffs_base__rect_ii_u32 wuffs_base__rect_ii_u32__intersection(
    wuffs_base__rect_ii_u32 r,
    wuffs_base__rect_ii_u32 s) {
  r.min_inclusive_x =
      wuffs_base__u32__max(r.min_inclusive_x, s.min_inclusive_x);
  r.min_inclusive_y =
      wuffs_base__u32__max(r.min_inclusive_y, s.min_inclusive_y);
  r.max_inclusive_x =
      wuffs_base__u32__min(r.max_inclusive_x, s.max_inclusive_x);
  r.max_inclusive_y =
      wuffs_base__u32__min(r.max_inclusive_y, s.max_inclusive_y);
  return r;
}

static inline wuffs_base__rect_ii_u32 wuffs_base__rect_ii_u32__union(
    wuffs_base__rect_ii_u32 r,
    wuffs_base__rect_ii_u32 s) {
  if (wuffs_base__rect_ii_u32__is_empty(r)) {
    return s;
  }
  if (wuffs_base__rect_ii_u32__is_empty(s)) {
    return r;
  }
  r.min_inclusive_x =
      wuffs_base__u32__min(r.min_inclusive_x, s.min_inclusive_x);
  r.min_inclusive_y =
      wuffs_base__u32__min(r.min_inclusive_y, s.min_inclusive_y);
  r.max_inclusive_x =
      wuffs_base__u32__max(r.max_inclusive_x, s.max_inclusive_x);
  r.max_inclusive_y =
      wuffs_base__u32__max(r.max_inclusive_y, s.max_inclusive_y);
  return r;
}

// --------

// wuffs_base__rect_ie_u32 is a rectangle (a 2-dimensional range) on the
// integer grid. The "ie" means that the bounds are inclusive on the low end
// and exclusive on the high end. It contains all points (x, y) such that
// ((min_inclusive_x <= x) && (x < max_exclusive_x)) and likewise for y.
//
// It is valid for min >= max, in which case the rectangle is empty. There are
// multiple representations of an empty rectangle, including a value with all
// fields zero.
//
// The X and Y axes increase right and down.
typedef struct {
  uint32_t min_inclusive_x;
  uint32_t min_inclusive_y;
  uint32_t max_exclusive_x;
  uint32_t max_exclusive_y;
} wuffs_base__rect_ie_u32;

static inline bool wuffs_base__rect_ie_u32__is_empty(
    wuffs_base__rect_ie_u32 r) {
  return (r.min_inclusive_x >= r.max_exclusive_x) ||
         (r.min_inclusive_y >= r.max_exclusive_y);
}

static inline bool wuffs_base__rect_ie_u32__contains(wuffs_base__rect_ie_u32 r,
                                                     uint32_t x,
                                                     uint32_t y) {
  return (r.min_inclusive_x <= x) && (x < r.max_exclusive_x) &&
         (r.min_inclusive_y <= y) && (y < r.max_exclusive_y);
}

static inline wuffs_base__rect_ie_u32 wuffs_base__rect_ie_u32__intersection(
    wuffs_base__rect_ie_u32 r,
    wuffs_base__rect_ie_u32 s) {
  r.min_inclusive_x =
      wuffs_base__u32__max(r.min_inclusive_x, s.min_inclusive_x);
  r.min_inclusive_y =
      wuffs_base__u32__max(r.min_inclusive_y, s.min_inclusive_y);
  r.max_exclusive_x =
      wuffs_base__u32__min(r.max_exclusive_x, s.max_exclusive_x);
  r.max_exclusive_y =
      wuffs_base__u32__min(r.max_exclusive_y, s.max_exclusive_y);
  return r;
}

static inline wuffs_base__rect_ie_u32 wuffs_base__rect_ie_u32__union(
    wuffs_base__rect_ie_u32 r,
    wuffs_base__rect_ie_u32 s) {
  if (wuffs_base__rect_ie_u32__is_empty(r)) {
    return s;
  }
  if (wuffs_base__rect_ie_u32__is_empty(s)) {
    return r;
  }
  r.min_inclusive_x =
      wuffs_base__u32__min(r.min_inclusive_x, s.min_inclusive_x);
  r.min_inclusive_y =
      wuffs_base__u32__min(r.min_inclusive_y, s.min_inclusive_y);
  r.max_exclusive_x =
      wuffs_base__u32__max(r.max_exclusive_x, s.max_exclusive_x);
  r.max_exclusive_y =
      wuffs_base__u32__max(r.max_exclusive_y, s.max_exclusive_y);
  return r;
}

static inline uint32_t wuffs_base__rect_ie_u32__width(
    wuffs_base__rect_ie_u32 r) {
  return wuffs_base__u32__sat_sub(r.max_exclusive_x, r.min_inclusive_x);
}

static inline uint32_t wuffs_base__rect_ie_u32__height(
    wuffs_base__rect_ie_u32 r) {
  return wuffs_base__u32__sat_sub(r.max_exclusive_y, r.min_inclusive_y);
}

// ---------------- I/O

// wuffs_base__io_buffer is a 1-dimensional buffer (a pointer and length), plus
// additional indexes into that buffer, plus an opened / closed flag.
//
// A value with all fields NULL or zero is a valid, empty buffer.
typedef struct {
  uint8_t* ptr;  // Pointer.
  size_t len;    // Length.
  size_t wi;     // Write index. Invariant: wi <= len.
  size_t ri;     // Read  index. Invariant: ri <= wi.
  bool closed;   // No further writes are expected.
} wuffs_base__io_buffer;

// wuffs_base__io_limit provides a limited view of a 1-dimensional byte stream:
// its first N bytes. That N can be greater than a buffer's current read or
// write capacity. N decreases naturally over time as bytes are read from or
// written to the stream.
//
// A value with all fields NULL or zero is a valid, unlimited view.
typedef struct wuffs_base__io_limit {
  uint64_t* ptr_to_len;               // Pointer to N.
  struct wuffs_base__io_limit* next;  // Linked list of limits.
} wuffs_base__io_limit;

typedef struct {
  // Do not access the private_impl's fields directly. There is no API/ABI
  // compatibility or safety guarantee if you do so.
  struct {
    wuffs_base__io_buffer* buf;
    uint8_t* bounds[2];
    wuffs_base__io_limit limit;
  } private_impl;
} wuffs_base__io_reader;

typedef struct {
  // Do not access the private_impl's fields directly. There is no API/ABI
  // compatibility or safety guarantee if you do so.
  struct {
    wuffs_base__io_buffer* buf;
    uint8_t* bounds[2];
    wuffs_base__io_limit limit;
  } private_impl;
} wuffs_base__io_writer;

static inline wuffs_base__io_reader wuffs_base__io_buffer__reader(
    wuffs_base__io_buffer* buf) {
  wuffs_base__io_reader ret = ((wuffs_base__io_reader){});
  if (buf) {
    ret.private_impl.buf = buf;
    ret.private_impl.bounds[0] = buf->ptr + buf->ri;
    ret.private_impl.bounds[1] = buf->ptr + buf->wi;
  }
  return ret;
}

static inline wuffs_base__io_writer wuffs_base__io_buffer__writer(
    wuffs_base__io_buffer* buf) {
  wuffs_base__io_writer ret = ((wuffs_base__io_writer){});
  if (buf) {
    ret.private_impl.buf = buf;
    ret.private_impl.bounds[0] = buf->ptr + buf->wi;
    ret.private_impl.bounds[1] = buf->ptr + buf->len;
  }
  return ret;
}

// ---------------- Images

// wuffs_base__pixel_format encodes the format of the bytes that constitute an
// image frame's pixel data. Its bits:
//  - bit        31  is reserved.
//  - bits 30 .. 28 encodes color (and channel order, in terms of memory).
//  - bits 27 .. 26 are reserved.
//  - bits 25 .. 24 encodes transparency.
//  - bit        23 indicates big-endian/MSB-first (as opposed to little/LSB).
//  - bit        22 indicates floating point (as opposed to integer).
//  - bits 21 .. 20 are the number of planes, minus 1. Zero means packed.
//  - bits 19 .. 16 encodes the number of bits (depth) in an index value.
//                  Zero means direct, not palette-indexed.
//  - bits 15 .. 12 encodes the number of bits (depth) in the 3rd channel.
//  - bits 11 ..  8 encodes the number of bits (depth) in the 2nd channel.
//  - bits  7 ..  4 encodes the number of bits (depth) in the 1st channel.
//  - bits  3 ..  0 encodes the number of bits (depth) in the 0th channel.
//
// The bit fields of a wuffs_base__pixel_format are not independent. For
// example, the number of planes should not be greater than the number of
// channels. Similarly, bits 15..4 are unused (and should be zero) if bits
// 31..24 (color and transparency) together imply only 1 channel (gray, no
// alpha) and floating point samples should mean a bit depth of 16, 32 or 64.
//
// Formats hold between 1 and 4 channels. For example: Y (1 channel: gray), YA
// (2 channels: gray and alpha), BGR (3 channels: blue, green, red) or CMYK (4
// channels: cyan, magenta, yellow, black).
//
// For direct formats with N > 1 channels, those channels can be laid out in
// either 1 (packed) or N (planar) planes. For example, RGBA data is usually
// packed, but YUV data is usually planar, due to chroma subsampling (for
// details, see the wuffs_base__pixel_subsampling type). For indexed formats,
// the palette (always 256 × 4 bytes) holds up to 4 packed bytes of color data
// per index value, and there is only 1 plane (for the index). The distance
// between successive palette elements is always 4 bytes.
//
// The color field is encoded in 3 bits:
//  - 0 means                 A (Alpha).
//  - 1 means   Y       or   YA (Gray, Alpha).
//  - 2 means BGR, BGRX or BGRA (Blue, Green, Red, X-padding or Alpha).
//  - 3 means RGB, RGBX or RGBA (Red, Green, Blue, X-padding or Alpha).
//  - 4 means YUV       or YUVA (Luma, Chroma-blue, Chroma-red, Alpha).
//  - 5 means CMY       or CMYK (Cyan, Magenta, Yellow, Black).
//  - all other values are reserved.
//
// In Wuffs, channels are given in memory order, regardless of endianness,
// since the C type for the pixel data is an array of bytes, not an array of
// uint32_t. For example, packed BGRA with 8 bits per channel means that the
// bytes in memory are always Blue, Green, Red then Alpha. On big-endian
// systems, that is the uint32_t 0xBBGGRRAA. On little-endian, 0xAARRGGBB.
//
// When the color field (3 bits) encodes multiple options, the transparency
// field (2 bits) distinguishes them:
//  - 0 means fully opaque, no extra channels
//  - 1 means fully opaque, one extra channel (X or K, padding or black).
//  - 2 means one extra alpha channel, other channels are non-premultiplied.
//  - 3 means one extra alpha channel, other channels are     premultiplied.
//
// The zero wuffs_base__pixel_format value is an invalid pixel format, as it is
// invalid to combine the zero color (alpha only) with the zero transparency.
//
// Bit depth is encoded in 4 bits:
//  -  0 means the channel or index is unused.
//  -  x means a bit depth of  x, for x in the range 1..8.
//  -  9 means a bit depth of 10.
//  - 10 means a bit depth of 12.
//  - 11 means a bit depth of 16.
//  - 12 means a bit depth of 24.
//  - 13 means a bit depth of 32.
//  - 14 means a bit depth of 48.
//  - 15 means a bit depth of 64.
//
// For example, wuffs_base__pixel_format 0x3280BBBB is a natural format for
// decoding a PNG image - network byte order (also known as big-endian),
// packed, non-premultiplied alpha - that happens to be 16-bit-depth truecolor
// with alpha (RGBA). In memory order:
//
//  ptr+0  ptr+1  ptr+2  ptr+3  ptr+4  ptr+5  ptr+6  ptr+7
//  Rhi    Rlo    Ghi    Glo    Bhi    Blo    Ahi    Alo
//
// For example, the value wuffs_base__pixel_format 0x20000565 means BGR with no
// alpha or padding, 5/6/5 bits for blue/green/red, packed 2 bytes per pixel,
// laid out LSB-first in memory order:
//
//  ptr+0...........  ptr+1...........
//  MSB          LSB  MSB          LSB
//  G₂G₁G₀B₄B₃B₂B₁B₀  R₄R₃R₂R₁R₀G₅G₄G₃
//
// On little-endian systems (but not big-endian), this Wuffs pixel format value
// (0x20000565) corresponds to the Cairo library's CAIRO_FORMAT_RGB16_565, the
// SDL2 (Simple DirectMedia Layer 2) library's SDL_PIXELFORMAT_RGB565 and the
// Skia library's kRGB_565_SkColorType. Note BGR in Wuffs versus RGB in the
// other libraries.
//
// Regardless of endianness, this Wuffs pixel format value (0x20000565)
// corresponds to the V4L2 (Video For Linux 2) library's V4L2_PIX_FMT_RGB565
// and the Wayland-DRM library's WL_DRM_FORMAT_RGB565.
//
// Different software libraries name their pixel formats (and especially their
// channel order) either according to memory layout or as bits of a native
// integer type like uint32_t. The two conventions differ because of a system's
// endianness. As mentioned earlier, Wuffs pixel formats are always in memory
// order. More detail of other software libraries' naming conventions is in the
// Pixel Format Guide at https://afrantzis.github.io/pixel-format-guide/
//
// Do not manipulate these bits directly; they are private implementation
// details. Use methods such as wuffs_base__pixel_format__num_planes instead.
typedef uint32_t wuffs_base__pixel_format;

// Common 8-bit-depth pixel formats. This list is not exhaustive; not all valid
// wuffs_base__pixel_format values are present.

#define WUFFS_BASE__PIXEL_FORMAT__INVALID ((wuffs_base__pixel_format)0x00000000)

#define WUFFS_BASE__PIXEL_FORMAT__A ((wuffs_base__pixel_format)0x02000008)

#define WUFFS_BASE__PIXEL_FORMAT__Y ((wuffs_base__pixel_format)0x10000008)
#define WUFFS_BASE__PIXEL_FORMAT__YA_NONPREMUL \
  ((wuffs_base__pixel_format)0x12000008)
#define WUFFS_BASE__PIXEL_FORMAT__YA_PREMUL \
  ((wuffs_base__pixel_format)0x13000008)

#define WUFFS_BASE__PIXEL_FORMAT__BGR ((wuffs_base__pixel_format)0x20000888)
#define WUFFS_BASE__PIXEL_FORMAT__BGRX ((wuffs_base__pixel_format)0x21008888)
#define WUFFS_BASE__PIXEL_FORMAT__BGRX_INDEXED \
  ((wuffs_base__pixel_format)0x21088888)
#define WUFFS_BASE__PIXEL_FORMAT__BGRA_NONPREMUL \
  ((wuffs_base__pixel_format)0x22008888)
#define WUFFS_BASE__PIXEL_FORMAT__BGRA_NONPREMUL_INDEXED \
  ((wuffs_base__pixel_format)0x22088888)
#define WUFFS_BASE__PIXEL_FORMAT__BGRA_PREMUL \
  ((wuffs_base__pixel_format)0x23008888)

#define WUFFS_BASE__PIXEL_FORMAT__RGB ((wuffs_base__pixel_format)0x30000888)
#define WUFFS_BASE__PIXEL_FORMAT__RGBX ((wuffs_base__pixel_format)0x31008888)
#define WUFFS_BASE__PIXEL_FORMAT__RGBX_INDEXED \
  ((wuffs_base__pixel_format)0x31088888)
#define WUFFS_BASE__PIXEL_FORMAT__RGBA_NONPREMUL \
  ((wuffs_base__pixel_format)0x32008888)
#define WUFFS_BASE__PIXEL_FORMAT__RGBA_NONPREMUL_INDEXED \
  ((wuffs_base__pixel_format)0x32088888)
#define WUFFS_BASE__PIXEL_FORMAT__RGBA_PREMUL \
  ((wuffs_base__pixel_format)0x33008888)

#define WUFFS_BASE__PIXEL_FORMAT__YUV ((wuffs_base__pixel_format)0x40200888)
#define WUFFS_BASE__PIXEL_FORMAT__YUVK ((wuffs_base__pixel_format)0x41308888)
#define WUFFS_BASE__PIXEL_FORMAT__YUVA_NONPREMUL \
  ((wuffs_base__pixel_format)0x42308888)

#define WUFFS_BASE__PIXEL_FORMAT__CMY ((wuffs_base__pixel_format)0x50200888)
#define WUFFS_BASE__PIXEL_FORMAT__CMYK ((wuffs_base__pixel_format)0x51308888)

static inline bool wuffs_base__pixel_format__is_valid(
    wuffs_base__pixel_format f) {
  return f != 0;
}

static inline bool wuffs_base__pixel_format__is_indexed(
    wuffs_base__pixel_format f) {
  return ((f >> 16) & 0x0F) != 0;
}

#define WUFFS_BASE__PIXEL_FORMAT__NUM_PLANES_MAX 4

static inline uint32_t wuffs_base__pixel_format__num_planes(
    wuffs_base__pixel_format f) {
  return f ? (((f >> 20) & 0x03) + 1) : 0;
}

typedef struct {
  wuffs_base__table_u8 planes[WUFFS_BASE__PIXEL_FORMAT__NUM_PLANES_MAX];
} wuffs_base__pixel_buffer;

// --------

// wuffs_base__pixel_subsampling encodes the mapping of pixel space coordinates
// (x, y) to pixel buffer indices (i, j). That mapping can differ for each
// plane p. For a depth of 8 bits (1 byte), the p'th plane's sample starts at
// (planes[p].ptr + (j * planes[p].stride) + i).
//
// For packed pixel formats, the mapping is trivial: i = x and j = y. For
// planar pixel formats, the mapping can differ due to chroma subsampling. For
// example, consider a three plane YUV pixel format with 4:2:2 subsampling. For
// the luma (Y) channel, there is one sample for every pixel, but for the
// chroma (U, V) channels, there is one sample for every two pixels: pairs of
// horizontally adjacent pixels form one macropixel, i = x / 2 and j == y. In
// general, for a given p:
//  - i = (x + bias_x) >> shift_x.
//  - j = (y + bias_y) >> shift_y.
// where biases and shifts are in the range 0..3 and 0..2 respectively.
//
// In general, the biases will be zero after decoding an image. However, making
// a sub-image may change the bias, since the (x, y) coordinates are relative
// to the sub-image's top-left origin, but the backing pixel buffers were
// created relative to the original image's origin.
//
// For each plane p, each of those four numbers (biases and shifts) are encoded
// in two bits, which combine to form an 8 bit unsigned integer:
//
//  e_p = (bias_x << 6) | (shift_x << 4) | (bias_y << 2) | (shift_y << 0)
//
// Those e_p values (e_0 for the first plane, e_1 for the second plane, etc)
// combine to form a wuffs_base__pixel_subsampling value:
//
//  pixsub = (e_3 << 24) | (e_2 << 16) | (e_1 << 8) | (e_0 << 0)
//
// Do not manipulate these bits directly; they are private implementation
// details. Use methods such as wuffs_base__pixel_subsampling__bias_x instead.
typedef uint32_t wuffs_base__pixel_subsampling;

#define WUFFS_BASE__PIXEL_SUBSAMPLING__NONE ((wuffs_base__pixel_subsampling)0);

#define WUFFS_BASE__PIXEL_SUBSAMPLING__444 \
  ((wuffs_base__pixel_subsampling)0x000000);
#define WUFFS_BASE__PIXEL_SUBSAMPLING__440 \
  ((wuffs_base__pixel_subsampling)0x010100);
#define WUFFS_BASE__PIXEL_SUBSAMPLING__422 \
  ((wuffs_base__pixel_subsampling)0x101000);
#define WUFFS_BASE__PIXEL_SUBSAMPLING__420 \
  ((wuffs_base__pixel_subsampling)0x111100);
#define WUFFS_BASE__PIXEL_SUBSAMPLING__411 \
  ((wuffs_base__pixel_subsampling)0x202000);
#define WUFFS_BASE__PIXEL_SUBSAMPLING__410 \
  ((wuffs_base__pixel_subsampling)0x212100);

static inline uint32_t wuffs_base__pixel_subsampling__bias_x(
    wuffs_base__pixel_subsampling s,
    uint32_t plane) {
  uint32_t shift = ((plane & 0x03) * 8) + 6;
  return (s >> shift) & 0x03;
}

static inline uint32_t wuffs_base__pixel_subsampling__shift_x(
    wuffs_base__pixel_subsampling s,
    uint32_t plane) {
  uint32_t shift = ((plane & 0x03) * 8) + 4;
  return (s >> shift) & 0x03;
}

static inline uint32_t wuffs_base__pixel_subsampling__bias_y(
    wuffs_base__pixel_subsampling s,
    uint32_t plane) {
  uint32_t shift = ((plane & 0x03) * 8) + 2;
  return (s >> shift) & 0x03;
}

static inline uint32_t wuffs_base__pixel_subsampling__shift_y(
    wuffs_base__pixel_subsampling s,
    uint32_t plane) {
  uint32_t shift = ((plane & 0x03) * 8) + 0;
  return (s >> shift) & 0x03;
}

// --------

typedef struct {
  // Do not access the private_impl's fields directly. There is no API/ABI
  // compatibility or safety guarantee if you do so.
  struct {
    wuffs_base__pixel_format pixfmt;
    wuffs_base__pixel_subsampling pixsub;
    uint32_t width;
    uint32_t height;
    uint32_t num_loops;
  } private_impl;
} wuffs_base__image_config;

static inline void wuffs_base__image_config__initialize(
    wuffs_base__image_config* c,
    wuffs_base__pixel_format pixfmt,
    wuffs_base__pixel_subsampling pixsub,
    uint32_t width,
    uint32_t height,
    uint32_t num_loops) {
  if (!c) {
    return;
  }
  // TODO: move the check from wuffs_base__image_config__is_valid here. Should
  // this function return bool? An error type?
  c->private_impl.pixfmt = pixfmt;
  c->private_impl.pixsub = pixsub;
  c->private_impl.width = width;
  c->private_impl.height = height;
  c->private_impl.num_loops = num_loops;
}

static inline void wuffs_base__image_config__invalidate(
    wuffs_base__image_config* c) {
  if (c) {
    *c = ((wuffs_base__image_config){});
  }
}

static inline bool wuffs_base__image_config__is_valid(
    wuffs_base__image_config* c) {
  if (!c || !c->private_impl.pixfmt) {
    return false;
  }
  uint64_t wh =
      ((uint64_t)c->private_impl.width) * ((uint64_t)c->private_impl.height);
  // TODO: handle things other than 1 byte per pixel.
  //
  // TODO: move the check to wuffs_base__image_config__initialize.
  return wh <= ((uint64_t)SIZE_MAX);
}

static inline wuffs_base__pixel_format wuffs_base__image_config__pixel_format(
    wuffs_base__image_config* c) {
  return c ? c->private_impl.pixfmt : 0;
}

static inline wuffs_base__pixel_subsampling
wuffs_base__image_config__pixel_subsampling(wuffs_base__image_config* c) {
  return wuffs_base__image_config__is_valid(c) ? c->private_impl.pixsub : 0;
}

static inline uint32_t wuffs_base__image_config__width(
    wuffs_base__image_config* c) {
  return wuffs_base__image_config__is_valid(c) ? c->private_impl.width : 0;
}

static inline uint32_t wuffs_base__image_config__height(
    wuffs_base__image_config* c) {
  return wuffs_base__image_config__is_valid(c) ? c->private_impl.height : 0;
}

static inline uint32_t wuffs_base__image_config__num_loops(
    wuffs_base__image_config* c) {
  return wuffs_base__image_config__is_valid(c) ? c->private_impl.num_loops : 0;
}

// TODO: this is the right API for planar (not packed) pixbufs? Should it allow
// decoding into a color model different from the format's intrinsic one? For
// example, decoding a JPEG image straight to RGBA instead of to YCbCr?
static inline size_t wuffs_base__image_config__pixbuf_size(
    wuffs_base__image_config* c) {
  if (wuffs_base__image_config__is_valid(c)) {
    uint64_t wh =
        ((uint64_t)c->private_impl.width) * ((uint64_t)c->private_impl.height);
    // TODO: handle things other than 1 byte per pixel.
    return (size_t)wh;
  }
  return 0;
}

// --------

typedef struct {
  // Do not access the private_impl's fields directly. There is no API/ABI
  // compatibility or safety guarantee if you do so.
  struct {
    wuffs_base__image_config config;
    uint32_t loop_count;  // 0-based count of the current loop.
    wuffs_base__pixel_buffer pixbuf;
    // TODO: color spaces.
    wuffs_base__rect_ie_u32 dirty_rect;
    wuffs_base__flicks duration;
    uint8_t palette[1024];
  } private_impl;
} wuffs_base__image_buffer;

static inline void wuffs_base__image_buffer__initialize(
    wuffs_base__image_buffer* f,
    wuffs_base__image_config config,
    wuffs_base__pixel_buffer pixbuf) {
  if (!f) {
    return;
  }
  *f = ((wuffs_base__image_buffer){});
  f->private_impl.config = config;
  f->private_impl.pixbuf = pixbuf;
}

static inline void wuffs_base__image_buffer__update(
    wuffs_base__image_buffer* f,
    wuffs_base__rect_ie_u32 dirty_rect,
    wuffs_base__flicks duration,
    uint8_t* palette_ptr,
    size_t palette_len) {
  if (!f) {
    return;
  }
  f->private_impl.dirty_rect = dirty_rect;
  f->private_impl.duration = duration;
  if (palette_ptr) {
    memmove(f->private_impl.palette, palette_ptr,
            palette_len <= 1024 ? palette_len : 1024);
  }
}

// wuffs_base__image_buffer__loop returns whether the image decoder should loop
// back to the beginning of the animation, assuming that we've reached the end
// of the encoded stream. If so, it increments f's count of the animation loops
// played so far.
static inline bool wuffs_base__image_buffer__loop(wuffs_base__image_buffer* f) {
  if (!f) {
    return false;
  }
  uint32_t n = f->private_impl.config.private_impl.num_loops;
  if (n == 0) {
    return true;
  }
  if (f->private_impl.loop_count < n - 1) {
    f->private_impl.loop_count++;
    return true;
  }
  return false;
}

// wuffs_base__image_buffer__dirty_rect returns an upper bound for what part of
// this frame's pixels differs from the previous frame.
static inline wuffs_base__rect_ie_u32 wuffs_base__image_buffer__dirty_rect(
    wuffs_base__image_buffer* f) {
  return f ? f->private_impl.dirty_rect : ((wuffs_base__rect_ie_u32){0});
}

// wuffs_base__image_buffer__duration returns the amount of time to display
// this frame. Zero means to display forever - a still (non-animated) image.
static inline wuffs_base__flicks wuffs_base__image_buffer__duration(
    wuffs_base__image_buffer* f) {
  return f ? f->private_impl.duration : 0;
}

// wuffs_base__image_buffer__palette returns the palette that the pixel data
// can index. The backing array is inside f and has length 1024.
static inline uint8_t* wuffs_base__image_buffer__palette(
    wuffs_base__image_buffer* f) {
  return f ? f->private_impl.palette : NULL;
}

#endif  // WUFFS_BASE_HEADER_H
