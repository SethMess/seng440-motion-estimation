#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdint.h>

#include "main.h" // BLOCK_SIZE and find_all_motion_vectors(), the core-algorithm
                  // entry point run_selftest drives

int write_pgm(const char *path, int w, int h, const uint8_t *buf);

/**
 * draw_motion_vectors — writes "vectors.pgm": current frame with each
 * block's motion vector drawn from block center toward its source
 * position in the reference frame. Dot = block center; line points
 * to (center + r, center + s).
 */
void draw_motion_vectors(int w, int h, const uint8_t *cur_flat,
                         int bw, int bh,          /* blocks across, down */
                         int best_rs[bh][bw], int best_ss[bh][bw],
                         const char *out_path);

// gen_shifted: copy src into dst shifted right by dx and down by dy,
// duplicating edge pixels into the vacated band.
void gen_shifted(int w, int h, const uint8_t *src, uint8_t *dst, int dx, int dy);

/**
 * run_selftest — end-to-end check against a known ground truth.
 * Shifts ref by (dx, dy), runs full motion estimation, and verifies
 * every interior block reports exactly the negated shift.
 * Returns 0 on pass, nonzero on fail (usable as exit code).
 */
int run_selftest(int w, int h, const uint8_t *ref_flat, int dx, int dy);

#endif // TEST_UTILS_H
