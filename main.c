#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef SAD_NEON
#include <arm_neon.h>
#endif


// Size of comparison blocks
#define BLOCK_SIZE 16
// Amount of pixels to search on either side of the search block
#define SEARCH_RANGE 16
#define FRAME_WIDTH 320
#define FRAME_HEIGHT 240

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

    // TODO: fix array indexing for cur frame and next frame to be 2d arrays.
    /* int A[16][16], B[16][16], */
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

// TODO: Would be similar as the one above but using neo instructions
uint32_t sad_neon(int stride, const uint8_t cur_frame[][stride], const uint8_t next_frame[][stride], int x, int y, int r, int s){
  return 0;
}

// TODO: This can be our implementation that will use a custom sad instruction
uint32_t sad_custom_asm(int stride, const uint8_t cur_frame[][stride], const uint8_t next_frame[][stride], int x, int y, int r, int s){
  return 0;
}

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
  uint32_t min_sad = 65280; // max SAD return value
  *best_r = 0; // default to 0 if all SAD values are the same
  *best_s = 0; // default to 0 if all SAD values are the same
  uint32_t cur_sad;
  int i,j;
  for(j = -BLOCK_SIZE; j <= BLOCK_SIZE; j++) {
    if((y + j) < 0 || (y + j) > (h-BLOCK_SIZE)) {
      continue;
    }
    for(i = -BLOCK_SIZE; i <= BLOCK_SIZE; i++) {
      if((x + i) < 0 || (x + i) > (w-BLOCK_SIZE)) {
        continue;
      }
#ifdef SAD_BASELINE
      cur_sad = sad_baseline(w, cur_frame, next_frame, x, y, i, j); // note stride is first param now so indexing works nice
#endif
#ifdef SAD_NEON
      cur_sad = sad_neon(w, cur_frame, next_frame, x, y, i, j); // note stride is first param now so indexing works nice
#endif
#ifdef SAD_ASM
      cur_sad = sad_custom_asm(w, cur_frame, next_frame, x, y, i, j); // note stride is first param now so indexing works nice
#endif
      if(cur_sad < min_sad) {
        min_sad = cur_sad;
        *best_r = i;
        *best_s = j;
      }
    }
  }
  printf("sad value: %d at (%d %d)\n", min_sad, x, y);

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
                             const uint8_t next_frame[h][w]  /*, int best_rs[][], int best_ss[][]*/) {

    /* int num_blocks = (w / BLOCK_SIZE) * (h / BLOCK_SIZE); */
    int best_rs[h / BLOCK_SIZE][w / BLOCK_SIZE]; // horizontal motion vector
    int best_ss[h / BLOCK_SIZE][w / BLOCK_SIZE]; // vertical motion vector

    int i, j;

    for (i = 0; i < h; i += BLOCK_SIZE) {
        for (j = 0; j < w; j += BLOCK_SIZE) {
            find_motion_vector(w, h, cur_frame, next_frame, j, i, &best_rs[i / BLOCK_SIZE][j / BLOCK_SIZE], &best_ss[i / BLOCK_SIZE][j / BLOCK_SIZE]);
        }
    }
}


int main() {
    printf("Hello World!\n");

    const char* frame1 = "frames/frame1.pgm";
    const char* frame2 = "frames/frame2.pgm";

    int width;
    int height;

    uint8_t *frame1_buffer = load_frame(frame1, &width, &height); // cur frame
    uint8_t *frame2_buffer = load_frame(frame2, &width, &height); // next frame

    // TODO: make sure this casting is correct
    const uint8_t (*cur_frame)[width] = (const uint8_t (*)[width]) frame1_buffer;  // newer = current
    const uint8_t (*next_frame)[width] = (const uint8_t (*)[width]) frame2_buffer;  // older = reference

    find_all_motion_vectors(width, height, cur_frame, next_frame);

    free(frame1_buffer);
    free(frame2_buffer);
    printf("Freed frame memory\n");
}
