#ifndef PUFFS_GIF_H
#define PUFFS_GIF_H

// Code generated by puffs-c. DO NOT EDIT.

#ifndef PUFFS_BASE_HEADER_H
#define PUFFS_BASE_HEADER_H

// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Puffs requires a word size of at least 32 bits because it assumes that
// converting a u32 to usize will never overflow. For example, the size of a
// decoded image is often represented, explicitly or implicitly in an image
// file, as a u32, and it is convenient to compare that to a buffer size.
//
// Similarly, the word size is at most 64 bits because it assumes that
// converting a usize to u64 will never overflow.
#if __WORDSIZE < 32
#error "Puffs requires a word size of at least 32 bits"
#elif __WORDSIZE > 64
#error "Puffs requires a word size of at most 64 bits"
#endif

// PUFFS_VERSION is the major.minor version number as a uint32. The major
// number is the high 16 bits. The minor number is the low 16 bits.
//
// The intention is to bump the version number at least on every API / ABI
// backwards incompatible change.
//
// For now, the API and ABI are simply unstable and can change at any time.
//
// TODO: don't hard code this in base-header.h.
#define PUFFS_VERSION (0x00001)

// puffs_base_buf1 is a 1-dimensional buffer (a pointer and length) plus
// additional indexes into that buffer.
//
// A value with all fields NULL or zero is a valid, empty buffer.
typedef struct {
  uint8_t* ptr;  // Pointer.
  size_t len;    // Length.
  size_t wi;     // Write index. Invariant: wi <= len.
  size_t ri;     // Read  index. Invariant: ri <= wi.
  bool closed;   // No further writes are expected.
} puffs_base_buf1;

// puffs_base_limit1 provides a limited view of a 1-dimensional byte stream:
// its first N bytes. That N can be greater than a buffer's current read or
// write capacity. N decreases naturally over time as bytes are read from or
// written to the stream.
//
// A value with all fields NULL or zero is a valid, unlimited view.
typedef struct puffs_base_limit1 {
  uint64_t* ptr_to_len;            // Pointer to N.
  struct puffs_base_limit1* next;  // Linked list of limits.
} puffs_base_limit1;

typedef struct {
  puffs_base_buf1* buf;
  puffs_base_limit1 limit;
} puffs_base_reader1;

typedef struct {
  puffs_base_buf1* buf;
  puffs_base_limit1 limit;
} puffs_base_writer1;

#endif  // PUFFS_BASE_HEADER_H

#ifdef __cplusplus
extern "C" {
#endif

// ---------------- Status Codes

// Status codes are int32_t values:
//  - the sign bit indicates a non-recoverable status code: an error
//  - bits 10-30 hold the packageid: a namespace
//  - bits 8-9 are reserved
//  - bits 0-7 are a package-namespaced numeric code
//
// Do not manipulate these bits directly. Use the API functions such as
// puffs_gif_status_is_error instead.
typedef int32_t puffs_gif_status;

#define puffs_gif_packageid 1017222  // 0x000f8586

#define PUFFS_GIF_STATUS_OK 0                               // 0x00000000
#define PUFFS_GIF_ERROR_BAD_PUFFS_VERSION -2147483647       // 0x80000001
#define PUFFS_GIF_ERROR_BAD_RECEIVER -2147483646            // 0x80000002
#define PUFFS_GIF_ERROR_BAD_ARGUMENT -2147483645            // 0x80000003
#define PUFFS_GIF_ERROR_CONSTRUCTOR_NOT_CALLED -2147483644  // 0x80000004
#define PUFFS_GIF_ERROR_CLOSED_FOR_WRITES -2147483643       // 0x80000005
#define PUFFS_GIF_ERROR_UNEXPECTED_EOF -2147483642          // 0x80000006
#define PUFFS_GIF_SUSPENSION_SHORT_READ 7                   // 0x00000007
#define PUFFS_GIF_SUSPENSION_SHORT_WRITE 8                  // 0x00000008
#define PUFFS_GIF_SUSPENSION_LIMITED_READ 9                 // 0x00000009
#define PUFFS_GIF_SUSPENSION_LIMITED_WRITE 10               // 0x0000000a

#define PUFFS_GIF_ERROR_BAD_GIF_BLOCK -1105848320            // 0xbe161800
#define PUFFS_GIF_ERROR_BAD_GIF_EXTENSION_LABEL -1105848319  // 0xbe161801
#define PUFFS_GIF_ERROR_BAD_GIF_HEADER -1105848318           // 0xbe161802
#define PUFFS_GIF_ERROR_BAD_LZW_LITERAL_WIDTH -1105848317    // 0xbe161803
#define PUFFS_GIF_ERROR_TODO_UNSUPPORTED_LOCAL_COLOR_TABLE \
  -1105848316                                                     // 0xbe161804
#define PUFFS_GIF_ERROR_LZW_CODE_IS_OUT_OF_RANGE -1105848315      // 0xbe161805
#define PUFFS_GIF_ERROR_LZW_PREFIX_CHAIN_IS_CYCLICAL -1105848314  // 0xbe161806

bool puffs_gif_status_is_error(puffs_gif_status s);

const char* puffs_gif_status_string(puffs_gif_status s);

// ---------------- Public Consts

// ---------------- Structs

typedef struct {
  // Do not access the private_impl's fields directly. There is no API/ABI
  // compatibility or safety guarantee if you do so. Instead, use the
  // puffs_gif_lzw_decoder_etc functions.
  //
  // In C++, these fields would be "private", but C does not support that.
  //
  // It is a struct, not a struct*, so that it can be stack allocated.
  struct {
    puffs_gif_status status;
    uint32_t magic;
    uint64_t scratch;

    uint32_t f_literal_width;
    uint8_t f_stack[4096];
    uint8_t f_suffixes[4096];
    uint16_t f_prefixes[4096];

    struct {
      uint32_t coro_susp_point;
      uint32_t v_clear_code;
      uint32_t v_end_code;
      uint32_t v_save_code;
      uint32_t v_prev_code;
      uint32_t v_width;
      uint32_t v_bits;
      uint32_t v_n_bits;
      uint32_t v_code;
      uint32_t v_s;
      uint32_t v_c;
    } c_decode[1];
  } private_impl;
} puffs_gif_lzw_decoder;

typedef struct {
  // Do not access the private_impl's fields directly. There is no API/ABI
  // compatibility or safety guarantee if you do so. Instead, use the
  // puffs_gif_decoder_etc functions.
  //
  // In C++, these fields would be "private", but C does not support that.
  //
  // It is a struct, not a struct*, so that it can be stack allocated.
  struct {
    puffs_gif_status status;
    uint32_t magic;
    uint64_t scratch;

    uint32_t f_width;
    uint32_t f_height;
    uint8_t f_background_color_index;
    uint8_t f_gct[768];
    puffs_gif_lzw_decoder f_lzw;

    struct {
      uint32_t coro_susp_point;
      uint8_t v_c;
    } c_decode[1];
    struct {
      uint32_t coro_susp_point;
      uint8_t v_c[6];
      uint32_t v_i;
    } c_decode_header[1];
    struct {
      uint32_t coro_susp_point;
      uint8_t v_c[7];
      uint32_t v_i;
      uint32_t v_gct_size;
    } c_decode_lsd[1];
    struct {
      uint32_t coro_susp_point;
      uint8_t v_label;
      uint8_t v_block_size;
    } c_decode_extension[1];
    struct {
      uint32_t coro_susp_point;
      uint8_t v_c[9];
      uint32_t v_i;
      bool v_interlace;
      uint8_t v_lw;
      uint8_t v_block_size;
      uint64_t l_lzw_src;
      puffs_base_reader1 v_lzw_src;
    } c_decode_id[1];
  } private_impl;
} puffs_gif_decoder;

// ---------------- Public Constructor and Destructor Prototypes

// puffs_gif_lzw_decoder_constructor is a constructor function.
//
// It should be called before any other puffs_gif_lzw_decoder_* function.
//
// Pass PUFFS_VERSION and 0 for puffs_version and for_internal_use_only.
void puffs_gif_lzw_decoder_constructor(puffs_gif_lzw_decoder* self,
                                       uint32_t puffs_version,
                                       uint32_t for_internal_use_only);

void puffs_gif_lzw_decoder_destructor(puffs_gif_lzw_decoder* self);

// puffs_gif_decoder_constructor is a constructor function.
//
// It should be called before any other puffs_gif_decoder_* function.
//
// Pass PUFFS_VERSION and 0 for puffs_version and for_internal_use_only.
void puffs_gif_decoder_constructor(puffs_gif_decoder* self,
                                   uint32_t puffs_version,
                                   uint32_t for_internal_use_only);

void puffs_gif_decoder_destructor(puffs_gif_decoder* self);

// ---------------- Public Function Prototypes

puffs_gif_status puffs_gif_decoder_decode(puffs_gif_decoder* self,
                                          puffs_base_writer1 a_dst,
                                          puffs_base_reader1 a_src);

void puffs_gif_lzw_decoder_set_literal_width(puffs_gif_lzw_decoder* self,
                                             uint32_t a_lw);

puffs_gif_status puffs_gif_lzw_decoder_decode(puffs_gif_lzw_decoder* self,
                                              puffs_base_writer1 a_dst,
                                              puffs_base_reader1 a_src);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // PUFFS_GIF_H
