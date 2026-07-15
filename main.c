#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 16
#define SEARCH_RANGE 16
#define FRAME_WIDTH 1280
#define FRAME_HEIGHT 720

// Idea to represent the video stream we can change if something else makes more sense.
typedef struct {
    int frame_count;
    int height;
    int width;
    unsigned char **frame;   // Double pointer: array of frame row pointers
    unsigned char *raw_data; // Flat memory block of all pixel brightness values
} GrayVideoStream;

int main() {
  printf("Hello World!\n");
  const char* video_path = "jojo-op.mp4";

    // This could be extracted to its own setup function

    int width = FRAME_WIDTH;
    int height = FRAME_HEIGHT;
    int channels = 3; // 3 for RGB24 format
    int total_frames = 100; // Number of frames you want to load

    // 2. Calculate dynamic dimensions
    // Rows = total number of frames in the video sequence
    // Columns = total bytes required for a single decoded frame
    int rows = total_frames;
    int cols = height * width * channels;

    printf("Allocating memory for a 2D array: %d rows x %d columns...\n", rows, cols);

    // 3. Allocate a 2D array using a single contiguous block via malloc
    // This approach prevents cache misses and simplifies freeing memory
    //
    // TODO: Change this line possibly and make it work with a struct
    unsigned char *video_matrix = (unsigned char *)malloc(rows * cols * sizeof(unsigned char));

    // TODO: finish rest of video alloc


}


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


// Front loop unrolling 1 slide
//
// x and y are the position
int oneBlockOfImage(int x, int y){

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
}
