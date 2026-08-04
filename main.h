#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

// Size of comparison blocks
#define BLOCK_SIZE 16
// Amount of pixels to search on either side of the search block
#define SEARCH_RANGE 16
#define FRAME_WIDTH 320
#define FRAME_HEIGHT 240

// Read a PGM frame file into malloc buffer
uint8_t *load_frame(const char *path, int *w, int *h);

/**
 * Find all motion vectors
 *
 * For each block of pixels (BLOCK_SIZE x BLOCK_SIZE), stores the
 * motion vector for the nearest best matching pixel block.
 *
 * @param w             Frame width
 * @param h             Frame height
 * @param cur_frame     Current frame buffer.
 * @param next_frame    Next frame buffer.
 */
void find_all_motion_vectors(int w, int h, const uint8_t cur_frame[h][w],
                             const uint8_t next_frame[h][w], int best_rs[h/BLOCK_SIZE][w/BLOCK_SIZE],
                             int best_ss[h/BLOCK_SIZE][w/BLOCK_SIZE]);

#endif // MAIN_H
