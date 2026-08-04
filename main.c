#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __ARM_NEON
    #include <arm_neon.h>
#endif

#include "main.h"
#include "test_utils.h"

// TODO: maybe have a struct for storing the frames?


// TODO: Seth: load frame files to two malloc buffers DONE?
// Read a PGM frame file into malloc buffer
uint8_t *load_frame (const char *path, int *w, int *h){

  FILE *frame = fopen(path, "rb"); // opens in read binary mode, should be portable.
  if (frame == NULL){
    return 0;
  }

  int maxval; // this should get set to 255
  fscanf(frame, "P5 %d %d %d", w, h, &maxval);

  uint8_t *frame_buffer = malloc(sizeof(uint8_t) * ((*w) * (*h)));
  if (frame_buffer == NULL){
    return 0;
  }

  fgetc(frame); // gets line separator and discards it
  fread(frame_buffer, 1, (*w) * (*h), frame); // copy entire frame data into malloced buffer

  printf("Read image of size: %d by %d.\n", (*w), (*h));
  return frame_buffer;
}

// Can remove this probably
// This is starting as the unoptimised version from slide 11 of the pdf
/* int oneBlockOfImage_unoptimized(){ */
  /* int A [16][16] , B [16][16] , diff , sad = 0; */
  /* int i , j ; */

  /* for( i =0; i <16; i ++) */
  /*   for( j =0; j <16; j ++) { */
  /*     diff = A [ x + i ][ y + j ] - B [( x + r ) + i ][( y + s ) + j ]; */
  /*     if( diff < 0){ */
  /*       sad -= diff ; */
  /*     }else{ */
  /*       sad += diff ; */
  /*     } */
  /*   } */
  /* return 0; */
/* } */


// TODO: integrate starter code into an actual function// TODO:  integrate starter code into an actual function
// Sum |cur - ref| over one 16x16 block; (x,y) = block pos, (r,s) = offset.
// One implementation per variant, same signature so they're interchangeable.
/**
 * sad_baseline — distortion metric for one candidate position.
 *
 * Computes the Sum of Absolute Differences between a 16x16 block in
 * the current frame and a 16x16 block in the reference frame shifted
 * by a candidate offset.
 *
 * @param stride        Number of bytes between the start of one row and the
 *                      next in both buffers (= frame width here, 320).
 *                      Lets pixel (col, row) be found at buf[row*stride + col].
 * @param cur_frame     Pointer to the current frame's pixel buffer (the newer
 *                      frame, the one being "explained"). Flat array, row-major. ie frame 1
 * @param next_frame    Pointer to the next frame ie frame 2
 * @param x, y          Top-left corner of the block in the CURRENT frame.
 *                      Always a multiple of 16 in this project, since blocks tile
 *                      the frame without overlapping.
 * @param r, s          Candidate motion offset: horizontal (r) and vertical (s)
 *                      displacement to apply when reading from the reference
 *                      frame. May be negative. The reference block's corner is
 *                      (x+r, y+s).
 *
 *
 * @return  Sum over all 256 pixel pairs of |cur_pixel - ref_pixel|.
 *          Range 0..65280 (256 x 255), so it fits in 16 bits, but
 *          returned as uint32_t for convenience. 0 = perfect match.
 *
 * Assumes the caller has already guaranteed (x+r, y+s) keeps the whole
 * 16x16 reference block inside the frame — no bounds checking here,
 * because this function runs ~330k times per frame and must stay lean.
 */
uint32_t sad_baseline(int stride, const uint8_t cur_frame[][stride], const uint8_t next_frame[][stride], int x, int y, int r, int s){

    int diff , sad = 0;
    int i , j;

    for( i =0; i <16; i ++){
        for( j =0; j <16; j ++) {
            diff = cur_frame [ y + i ][ x + j ] - next_frame [( y + s ) + i ][( x + r ) + j ];

            if( diff < 0){
                sad -= diff ;
            }else{
                sad += diff ;
            }
        }
    }
    return sad;
}

// SAD calculation with loop unrolling
uint32_t sad_unrolling(int stride, const uint8_t cur_frame[][stride], const uint8_t next_frame[][stride], int x, int y, int r, int s){

    int diff1 , diff2 , sad = 0;
    int i , j;

    for( i =0; i <16; i ++){
        for( j =0; j <16; j +=2) {
            diff1 = cur_frame [ y + i ][ x + j ] - next_frame [( y + s ) + i ][( x + r ) + j ];
            diff2 = cur_frame [ y + i ][ x + j +1] - next_frame [( y + s ) + i ][( x + r ) + j +1];

            if( diff1 < 0){
                sad -= diff1 ;
            }else{
                sad += diff1 ;
            }

            if( diff2 < 0){
                sad -= diff2 ;
            }else{
                sad += diff2 ;
            }
        }
    }
    return sad;
}

// SAD calculation with software pipelining
uint32_t sad_pipelining(int stride, const uint8_t cur_frame[][stride], const uint8_t next_frame[][stride], int x, int y, int r, int s){
  int diff , sad = 0;
  int i , j;

  for( i =0; i <16; i ++){
    diff = cur_frame [ y + i ][ x + 0 ] - next_frame [( y + s ) + i ][( x + r ) + 0 ];
    for( j =0; j <15; j ++) {

        if( diff < 0){
            sad -= diff ;
        }else{
            sad += diff ;
        }

        diff = cur_frame [ y + i ][ x + j+1 ] - next_frame [( y + s ) + i ][( x + r ) + j+1 ];
    }
    if( diff < 0){
        sad -= diff ;
    }else{
        sad += diff ;
    }
  }
  return sad;
}


// TODO: Would be similar as the one above but using neo instructions
#ifdef __ARM_NEON
uint32_t sad_neon(int stride, const uint8_t cur_frame[][stride], const uint8_t next_frame[][stride], int x, int y, int r, int s){

    // Re casts these back into big block painters since that works better for NEON
    const uint8_t *cur = &cur_frame[y][x];
    const uint8_t *ref = &next_frame[y + s][x + r];

    uint16x8_t acc_lo = vdupq_n_u16(0);
    uint16x8_t acc_hi = vdupq_n_u16(0);

    int i;
    uint32_t sad;

    for( i =0; i <16; i ++){
        // TODO: add proper neon intrinsics

        // load with vld1q_u8(ptr) cur and next
        uint8x16_t c = vld1q_u8(cur); // 16 pixels of cur row
        uint8x16_t n = vld1q_u8(ref); // 16 pixels of ref row


        // take abs diff on both
        // accumulate
        // add to sad
        // acc += |c - n|, widening 8-bit diffs into 16-bit lanes
        acc_lo = vabal_u8(acc_lo, vget_low_u8(c), vget_low_u8(n));
        acc_hi = vabal_u8(acc_hi, vget_high_u8(c), vget_high_u8(n));

        cur += stride;                  // down one image row
        ref += stride;

    }
    // Fold 16 lanes -> 1 scalar (runs once per block, cost is noise).
    uint16x8_t acc  = vaddq_u16(acc_lo, acc_hi);      // 8 lanes, max 8160
    uint32x4_t s32  = vpaddlq_u16(acc);               // pairwise, 4 x 32-bit
    uint64x2_t s64  = vpaddlq_u32(s32);               // 2 x 64-bit
    sad = (uint32_t)(vgetq_lane_u64(s64, 0) + vgetq_lane_u64(s64, 1));
    return sad;

}
#endif

// TODO: This can be our implementation that will use a custom sad instruction
uint32_t sad_custom_asm(int stride, const uint8_t cur_frame[][stride], const uint8_t next_frame[][stride], int x, int y, int r, int s){
#ifdef __ARM_NEON
    const uint8_t *cur = &cur_frame[y][x];
    const uint8_t *ref = &next_frame[y + s][x + r];
    uint32_t sad = 0;

    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j += 4) {
            uint32_t a, b;
            memcpy(&a, cur + j, 4);        // 4 packed pixels, alignment-safe
            memcpy(&b, ref + j, 4);
            __asm__("usada8 %0, %1, %2, %0"
                    : "+r"(sad)            // Rd and Ra are both 'sad'
                    : "r"(a), "r"(b));
        }
        cur += stride;  ref += stride;
    }
    return sad;
#endif
}

// NOTE: SAD function is defined here
// Picks the SAD implementation this binary was built with (exactly one
// SAD_* macro is defined per Makefile target), so call sites just say
// SAD_FN(...) instead of repeating the #ifdef chain at every call site.
#if defined(SAD_BASELINE)
  #define SAD_FN sad_baseline
#elif defined(SAD_UNROLL)
  #define SAD_FN sad_unrolling
#elif defined(SAD_NEON) && defined(__ARM_NEON)
  #define SAD_FN sad_neon
#elif defined(SAD_ASM)
  #define SAD_FN sad_custom_asm
#elif defined(SAD_PIPELINE)
  #define SAD_FN sad_pipelining
#endif

/**
 * find_motion_vector — best-match search for ONE block.
 *
 * Exhaustively evaluates every candidate offset (r, s) in the search
 * window around the block's own position and records the offset with
 * the minimum SAD.
 *
 * @param w             Frame width
 * @param h             Frame height
 * @param cur_frame     Current frame buffer.
 * @param next_frame    Next frame buffer.
 * @param x             X-value (width) of top-left corner of the block being matched.
 * @param y             Y-value (height) of top-left corner of block being matched.
 * @param best_r        Out-parameter: receives the winning horizontal offset,
 *                      in the range [-16, +16] (narrower near frame edges).
 * @param best_s        Out-parameter: receives the winning vertical offset
 *                      in the range [-16, +16].
 *
 * The (best_r, best_s) pair IS the motion vector for this block —
 * "this block appears to have come from (x+best_r, y+best_s) in the
 * previous frame."
 *
 * Cost: up to 33 x 33 = 1089 SAD calls per block; fewer at edges.
 * Note this could be set up to have the sad_fn be a variable with 
 * what sad function to point to but we can do conditional compilation or something else.
 */
void find_motion_vector(int w, int h, const uint8_t cur_frame[h][w], const uint8_t next_frame[h][w],
                        int x, int y, int *best_r, int *best_s) {
  // Seed with the actual SAD at the zero offset (rather than a magic max
  // constant) so the zero vector wins ties by construction: the comparison
  // below stays strict '<', and a candidate has to beat it, not just match it.
  uint32_t min_sad = SAD_FN(w, cur_frame, next_frame, x, y, 0, 0);
  *best_r = 0; // default to 0 if all SAD values are the same
  *best_s = 0; // default to 0 if all SAD values are the same
  uint32_t cur_sad;
  int i,j;
  for(j = -SEARCH_RANGE; j <= SEARCH_RANGE; j++) {
    if((y + j) < 0 || (y + j) > (h-SEARCH_RANGE)) {
      continue;
    }
    for(i = -SEARCH_RANGE; i <= SEARCH_RANGE; i++) {
      if((x + i) < 0 || (x + i) > (w-SEARCH_RANGE)) {
        continue;
      }
      if (i == 0 && j == 0) continue; // already have this one from the initial calc above
      cur_sad = SAD_FN(w, cur_frame, next_frame, x, y, i, j);
      if(cur_sad < min_sad) {
        min_sad = cur_sad;
        *best_r = i;
        *best_s = j;
      }
    }
  }
#ifdef DEBUG_PRINT
  printf("sad value: %d at (%d %d)\n", min_sad, x, y);
#endif
}

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
                             int best_ss[h/BLOCK_SIZE][w/BLOCK_SIZE]) {

    /* int num_blocks = (w / BLOCK_SIZE) * (h / BLOCK_SIZE); */
    /* int best_rs[h / BLOCK_SIZE][w / BLOCK_SIZE]; // horizontal motion vector */
    /* int best_ss[h / BLOCK_SIZE][w / BLOCK_SIZE]; // vertical motion vector */

    int i, j;

    for (i = 0; i < h; i += BLOCK_SIZE) {
        for (j = 0; j < w; j += BLOCK_SIZE) {
            find_motion_vector(w, h, cur_frame, next_frame, j, i, &best_rs[i / BLOCK_SIZE][j / BLOCK_SIZE], &best_ss[i / BLOCK_SIZE][j / BLOCK_SIZE]);
        }
    }
}



int main(int argc, char *argv[]) {

    const char* frame1 = "frames/frame1.pgm";
    const char* frame2 = "frames/frame2.pgm";

    /* const char* frame1 = "frames/second-frame1.pgm"; */
    /* const char* frame2 = "frames/second-frame2.pgm"; */

    int width;
    int height;

    uint8_t *frame1_buffer = load_frame(frame1, &width, &height); // cur frame
    uint8_t *frame2_buffer = load_frame(frame2, &width, &height); // next frame

    if (argc > 1 && strcmp(argv[1], "--selftest") == 0) {
        // Two shifts to exercise both halves of the search window.
        int rc = run_selftest(width, height, frame1_buffer, 3, 1);
        rc += run_selftest(width, height, frame1_buffer, -3, -2);
        free(frame1_buffer); free(frame2_buffer);
        return rc;                                // exit code = summed test results
    }

    // TODO: make sure this casting is correct
    const uint8_t (*cur_frame)[width] = (const uint8_t (*)[width]) frame1_buffer;  // newer = current
    const uint8_t (*next_frame)[width] = (const uint8_t (*)[width]) frame2_buffer;  // older = reference

    int bw = width / BLOCK_SIZE, bh = height / BLOCK_SIZE;
    int best_rs[bh][bw], best_ss[bh][bw];

    find_all_motion_vectors(width, height, cur_frame, next_frame, best_rs, best_ss);
    draw_motion_vectors(width, height, frame1_buffer, bw, bh,
                        best_rs, best_ss, "vectors.pgm");
    /* find_all_motion_vectors(width, height, cur_frame, next_frame); */

    free(frame1_buffer);
    free(frame2_buffer);
    printf("Freed frame memory\n");
}
