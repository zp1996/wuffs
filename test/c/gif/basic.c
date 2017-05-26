// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

/*
To manually run this test:

for cc in clang gcc; do
  $cc -std=c99 -Wall -Werror basic.c && ./a.out
  rm -f a.out
done

Each edition should print "PASS", amongst other information, and exit(0).
*/

#include "../testlib/testlib.c"

#include "../../../gen/c/gif.c"

const char* test_filename = "gif/basic.c";

void test_default_literal_width() {
  puffs_gif_lzw_decoder dec;
  puffs_gif_lzw_decoder_constructor(&dec, PUFFS_VERSION);
  if (dec.f_literal_width != 0) {
    FAIL("test_default_literal_width: got %d, want %d", dec.f_literal_width, 0);
    goto cleanup0;
  }
cleanup0:
  puffs_gif_lzw_decoder_destructor(&dec);
}

void test_puffs_version_bad() {
  puffs_gif_lzw_decoder dec;
  puffs_gif_lzw_decoder_constructor(&dec, 0);  // 0 is not PUFFS_VERSION.
  if (dec.status != puffs_gif_error_bad_version) {
    FAIL("test_puffs_version_bad: got %d, want %d", dec.status,
         puffs_gif_error_bad_version);
    goto cleanup0;
  }
cleanup0:
  puffs_gif_lzw_decoder_destructor(&dec);
}

// The empty comments forces clang-format to place one element per line.
test tests[] = {
    test_default_literal_width,  //
    test_puffs_version_bad,      //
    NULL,                        //
};
