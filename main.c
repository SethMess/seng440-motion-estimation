#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Size of comparison blocks
#define BLOCK_SIZE 16
// Amount of pixels to search on either side of the search block
#define SEARCH_RANGE 16
#define FRAME_WIDTH 320
#define FRAME_HEIGHT 240

// TODO: maybe have a struct for storing the frames?

int main() {
  printf("Hello World!\n");
  const char* video_path = "jojo-op.mp4";



}

// TODO: Seth: load frame files to two malloc buffers
// Read a PGM frame file into malloc buffer
uint8_t *load_pgm(const char *path, int *w, int *h){

  return 0;
}

// Can remove this probably
// This is starting as the unoptimised version from slide 11 of the pdf
int oneBlockOfImage_unoptimized(){
  int A [16][16] , B [16][16] , diff , sad = 0;
  int i , j ;

  for( i =0; i <16; i ++)
    for( j =0; j <16; j ++) {
      diff = A [ x + i ][ y + j ] - B [( x + r ) + i ][( y + s ) + j ];
      if( diff < 0){
        sad -= diff ;
      }else{
        sad += diff ;
      }
    }
}


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
 * @param cur     Pointer to the current frame's pixel buffer (the newer
 *                frame, the one being "explained"). Flat array, row-major.
 * @param ref     Pointer to the reference frame's pixel buffer (the
 *                previous frame we search inside).
 * @param x, y    Top-left corner of the block in the CURRENT frame.
 *                Always a multiple of 16 in this project, since blocks tile
 *                the frame without overlapping.
 * @param r, s    Candidate motion offset: horizontal (r) and vertical (s)
 *                displacement to apply when reading from the reference
 *                frame. May be negative. The reference block's corner is
 *                (x+r, y+s).
 * @param stride  Number of bytes between the start of one row and the
 *                next in both buffers (= frame width here, 320).
 *                Lets pixel (col, row) be found at buf[row*stride + col].
 *
 *                Note: default for this is probably 1 for our algorithm.
 *
 * @return  Sum over all 256 pixel pairs of |cur_pixel - ref_pixel|.
 *          Range 0..65280 (256 x 255), so it fits in 16 bits, but
 *          returned as uint32_t for convenience. 0 = perfect match.
 *
 * Assumes the caller has already guaranteed (x+r, y+s) keeps the whole
 * 16x16 reference block inside the frame — no bounds checking here,
 * because this function runs ~330k times per frame and must stay lean.
 */
uint32_t sad_baseline(const uint8_t *cur, const uint8_t *ref, int x, int y, int r, int s, int stride){

  int A [16][16] , B [16][16] , diff1 , diff2 , sad = 0;
  int i , j;

  for( i =0; i <16; i ++){
    for( j =0; j <16; j +=2) {
      diff1 = A [ x + i ][ y + j ] - B [( x + r ) + i ][( y + s ) + j ];
      diff2 = A [ x + i ][ y + j +1] - B [( x + r ) + i ][( y + s ) + j +1];

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
  return 0;
}

// Would be similar as the one above but using neo instructions
uint32_t sad_neon();

// This can be our implementation that will use a custom sad instruction
uint32_t sad_custom_asm();


// TODO:
/**
 * find_motion_vector — best-match search for ONE block.
 *
 * Exhaustively evaluates every candidate offset (r, s) in the search
 * window around the block's own position and records the offset with
 * the minimum SAD. This is the "full search" — optimal within the
 * window by construction.
 *
 * @param sad     Function pointer selecting which SAD kernel to use
 *                (baseline / ternary / NEON). This is the only thing
 *                that changes between benchmark runs; the search logic
 *                itself is identical for all variants.
 * @param cur     Current frame buffer.
 * @param ref     Reference frame buffer.
 * @param x, y    Top-left corner of the block being matched.
 * @param w, h    Frame dimensions (320, 240). Needed here — unlike in
 *                the SAD kernel — because THIS function is the one
 *                responsible for clamping the window so that no
 *                candidate reads outside the frame.
 * @param best_r  Out-parameter: receives the winning horizontal offset,
 *                in the range [-16, +16] (narrower near frame edges).
 * @param best_s  Out-parameter: receives the winning vertical offset.
 *
 * The (best_r, best_s) pair IS the motion vector for this block —
 * "this block appears to have come from (x+best_r, y+best_s) in the
 * previous frame."
 *
 * Cost: up to 33 x 33 = 1089 SAD calls per block; fewer at edges.
 */
// Note this could be set up to have the sad_fn be a variable with what sad function to point to but we can do conditional compilation or something else.
void find_motion_vector(/*sad_fn sad,*/ const uint8_t *cur, const uint8_t *ref, int x, int y, int w, int h, int *best_r, int *best_s);
