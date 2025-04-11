#include "mt_image.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

SearchResult search_result;

uint8 otsu_counter = 0;
uint8 otsu_threshold = 0;

uint8 image_buffer[HEIGHT][REAL_WIDTH] = { 0 };

void process_image(Image image) {
    memcpy(image_buffer, image, sizeof (uint8) * REAL_WIDTH * HEIGHT);
    if (otsu_counter) {
        otsu_counter --;
    }
    else {
        if (otsu_threshold) otsu_threshold = otsu_calc_threshold(
            image_buffer,
            otsu_threshold - OTSU_THRESHOLD_DELTA,
            otsu_threshold + OTSU_THRESHOLD_DELTA
        );
        else otsu_threshold = otsu_calc_threshold(
            image_buffer, OTSU_THRESHOLD_MIN, OTSU_THRESHOLD_MAX
        );
        otsu_counter = OSTU_COUNTER_MAX;
    }
    otsu_binarize_image(image_buffer, otsu_threshold);
	search_result = search(image_buffer);
}

uint8 otsu_calc_threshold(Image image, uint8 min, uint8 max) {
    int OTSU_PIXEL_COUNT = HEIGHT * WIDTH / OTSU_COMPRESS_RATIO_SQUARE;
    double var_max = 0;
    uint8 k_best;
    for (uint8 k = min; k <= max; k ++) {
        int c = 0, s = 0, sm = 0;
        for (int i = 0; i < HEIGHT; i += OTSU_COMPRESS_RATIO) {
            for (int j = 0; j < WIDTH; j += OTSU_COMPRESS_RATIO) {
                if (image[i][j] <= k) {
                    c ++;
                    sm += image[i][j];
                }
                s += image[i][j];
            }
        }
        double p1 = (double) c / OTSU_PIXEL_COUNT;
        if (p1 == 0) continue;
        if (p1 == 1) break;

        double mg = (double) s / OTSU_PIXEL_COUNT;
        double m = (double) sm / OTSU_PIXEL_COUNT;
        double var = pow(mg * p1 - m, 2) / (p1 * (1 - p1));
        if (var > var_max) {
            var_max = var;
            k_best = k;
        }
    }
    return k_best;
}

void otsu_binarize_image(Image image, uint8 threshould) {
    for (uint8 i = 0; i < HEIGHT; i ++)
        for (uint8 j = 0; j < WIDTH; j ++)
            image[i][j] = image[i][j] > threshould;
}

SearchResult search(Image image) {
#ifdef IMAGE_DEBUG
#define SET_IMG(x, y, T) do { \
    image[y][x] = T; \
} while (0)
#else
#define SET_IMG(x, y, T)
#endif

    // Zebra detection
    if (image[Y_MAX][0] != ROAD) {
        uint8 CURR = ROAD;
        uint8 change_count = 0;
        for (uint8 x = 1; x < X_MAX; x ++) {
            if (image[Y_MAX][x] != CURR) {
                CURR = image[Y_MAX][x];
                change_count ++;
            }
            if (change_count > 9) {
                SearchResult result = { 0 };
                result.element_type = Zebra;
                return result;
            }
        }
    }

    uint8 xl, xr, y;
    int8 dx, dy;
    uint8 xls[BD_LENGTH], xrs[BD_LENGTH];
    float ml, mr;
    bool ml_set = false, mr_set = false;

    uint8 dy0l = 0, dy0r = 0;
    bool dy0l_unset = true, dy0r_unset = true;

    // Find the bottom
    for (xl = 0; xl < X_MID && image[Y_MAX][xl] != ROAD; xl ++);
    xls[0] = xl;
    SET_IMG(xl, Y_MAX, BOUND);
    for (xr = X_MAX; xr > X_MID && image[Y_MAX][xr] != ROAD; xr --);
    xrs[0] = xr;
    SET_IMG(xr, Y_MAX, BOUND);

    // Find the bounds
    for (y = Y_MAX - 1, dx = 0, dy = 1; y >= Y_BD_MIN; y --, dy ++) {
        for (dx = 0; image[y][xl] == EMPTY; dx ++, xl ++)
            if (dx == DX_BD_MAX) { xl -= dx; break; }
        if (! dx) for (; image[y][xl - 1] == ROAD; dx --, xl --)
            if (dx == - DX_BD_MAX) { xl -= dx; break; }

        if (xl == 0) {
            dy0l_unset = true;
            goto left$;
        }
        if (dy0l_unset) {
            dy0l = dy;
            dy0l_unset = false;
        }

        if (abs(dx) == DX_BD_MAX) {
            if (ml_set) {
                xl = ml * (dy - dy0l) + xls[dy0l];
                SET_IMG(xl, y, BOUND_APP);
            }
        }
        else if (xl) {
            if (! dy0l_unset && dy - dy0l && dx >= 0 && dx <= DX_M_MAX) {
                ml = (float) (xl - xls[dy0l]) / (dy - dy0l);
                ml_set = true;
            }
            SET_IMG(xl, y, BOUND);
        }
        xls[dy] = xl;
        left$:

        for (dx = 0; image[y][xr] == EMPTY; dx --, xr --)
            if (dx == - DX_BD_MAX) { xr -= dx; break; }
        if (! dx) for (; image[y][xr + 1] == ROAD; dx ++, xr ++)
            if (dx == DX_BD_MAX) { xr -= dx; break; }

        if (xr == X_MAX) {
            dy0r_unset = true;
            goto right$;
        }
        else if (dy0r_unset) {
            dy0r = dy;
            dy0r_unset = false;
        }

        if (abs(dx == DX_BD_MAX)) {
            if (mr_set) {
                xr = mr * (dy - dy0r) + xrs[dy0r];
                SET_IMG(xr, y, BOUND_APP);
            }
        }
        else if (xr != X_MAX) {
            if (! dy0r_unset && dy - dy0r && dx <= 0 && dx >= - DX_M_MAX) {
                mr = (float) (xr - xrs[dy0r]) / (dy - dy0r);
                mr_set = true;
            }
            SET_IMG(xr, y, BOUND);
        }
        xrs[dy] = xr;
        right$:
    }

    SearchResult result = { 0 };

    return result;
}

