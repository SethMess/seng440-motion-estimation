#include <stdio.h>
#include <stdlib.h>

#include "test_utils.h"

// Drawing frames sub section ************************************************

int write_pgm(const char *path, int w, int h, const uint8_t *buf) {
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    fprintf(f, "P5\n%d %d\n255\n", w, h);
    size_t n = fwrite(buf, 1, (size_t)w * h, f);
    fclose(f);
    return n == (size_t)w * h ? 0 : -1;
}

// Plot one pixel, contrast-picked against the background, bounds-checked.
static void plot(uint8_t *img, int w, int h, int px, int py) {
    if (px < 0 || px >= w || py < 0 || py >= h) return;
    img[py * w + px] = img[py * w + px] > 128 ? 0 : 255;
}

void draw_motion_vectors(int w, int h, const uint8_t *cur_flat,
                         int bw, int bh,          /* blocks across, down */
                         int best_rs[bh][bw], int best_ss[bh][bw],
                         const char *out_path) {
    uint8_t *canvas = malloc((size_t)w * h);
    if (!canvas) return;
    for (int i = 0; i < w * h; i++) canvas[i] = cur_flat[i];

    for (int by = 0; by < bh; by++) {
        for (int bx = 0; bx < bw; bx++) {
            int cx = bx * BLOCK_SIZE + BLOCK_SIZE / 2;
            int cy = by * BLOCK_SIZE + BLOCK_SIZE / 2;
            int r = best_rs[by][bx], s = best_ss[by][bx];

            // 2x2 dot marks the block center
            plot(canvas, w, h, cx, cy);     plot(canvas, w, h, cx+1, cy);
            plot(canvas, w, h, cx, cy+1);   plot(canvas, w, h, cx+1, cy+1);

            // parametric line from center to center+(r,s), ~2 steps/pixel
            int steps = 2 * (abs(r) > abs(s) ? abs(r) : abs(s));
            for (int t = 1; t <= steps; t++) {
                int px = cx + (r * t) / steps;
                int py = cy + (s * t) / steps;
                plot(canvas, w, h, px, py);
            }
        }
    }
    write_pgm(out_path, w, h, canvas);
    free(canvas);
}

void gen_shifted(int w, int h, const uint8_t *src, uint8_t *dst, int dx, int dy) {
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            int sx = x - dx; if (sx < 0) sx = 0; if (sx >= w) sx = w - 1;
            int sy = y - dy; if (sy < 0) sy = 0; if (sy >= h) sy = h - 1;
            dst[y * w + x] = src[sy * w + sx];
        }
}

// Drawing Frames sub section end **************************************

// Testing section *******************

int run_selftest(int w, int h, const uint8_t *ref_flat, int dx, int dy) {
    uint8_t *shifted = malloc((size_t)w * h);
    if (!shifted) return -1;
    gen_shifted(w, h, ref_flat, shifted, dx, dy);

    const uint8_t (*cur)[w] = (const uint8_t (*)[w]) shifted;   // shifted = "current"
    const uint8_t (*ref)[w] = (const uint8_t (*)[w]) ref_flat;  // original = "reference"

    int bw = w / BLOCK_SIZE, bh = h / BLOCK_SIZE;
    int best_rs[bh][bw], best_ss[bh][bw];
    find_all_motion_vectors(w, h, cur, ref, best_rs, best_ss);

    // sad_baseline's reference block corner is (x+r, y+s): content shifted
    // right/down by (dx,dy) is *found* by looking back at offset (-dx,-dy),
    // since the vector points from the block's position to where its
    // content came from in the reference frame, not toward the shift.
    // Verified empirically: a (3,1) shift produces uniform (-3,-1) vectors.
    int exp_r = -dx, exp_s = -dy;

    int fails = 0;
    for (int by = 1; by < bh - 1; by++)          // interior only: edge blocks
        for (int bx = 1; bx < bw - 1; bx++)      // see the duplicated band
            if (best_rs[by][bx] != exp_r || best_ss[by][bx] != exp_s) {
                if (fails < 5)                    // print first few, not 250
                    printf("  block (%d,%d): got (%d,%d), expected (%d,%d)\n",
                           bx, by, best_rs[by][bx], best_ss[by][bx], exp_r, exp_s);
                fails++;
            }

    draw_motion_vectors(w, h, shifted, bw, bh, best_rs, best_ss, "selftest.pgm");
    free(shifted);

    printf(fails ? "SELFTEST FAIL: %d/%d interior blocks wrong\n"
                 : "SELFTEST PASS\n", fails, (bw - 2) * (bh - 2));
    return fails;
}
// Testing section *******************
