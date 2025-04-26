#include "mt_image.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

ImageResult image_result;

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
	search(image_buffer);
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

const uint8 STD_WIDTH[] = {
    // Y_SEARCH_MIN ~ Y_MAX
    71, 70, 69, 68, 68, 67, 66, 65,
    64, 64, 63, 62, 61, 61, 60, 59,
    58, 58, 57, 56, 55, 55, 54, 53,
    52, 51, 51, 50, 49, 48, 47, 46,
    46, 45, 44, 43, 42, 42, 41, 40,
    39, 38, 38, 37, 36, 35, 34, 34,
    33, 32, 31, 30, 30, 29, 28, 27,
    26, 26, 25, 24, 23, 23, 22, 21,
    20, 19, 19, 18, 17, 16, 15, 15,
    14, 13, 12, 11, 10,  9,  8,  7,
};

double calc_kw_by_m(double m) {
    return sqrt(m * m + 1);
}

void search(Image image) {
#ifdef IMAGE_DEBUG
#define SET_IMG(x, y, T) do { \
    image[y][x] = T; \
} while (0)
#else
#define SET_IMG(x, y, T)
#endif
    ElementType el = image_result.element_type;
    bool el_normal = el <= Normal;

    // Zebra detection
    if (image[Y_MAX][0] != ROAD) {
        uint8 CURR = ROAD;
        uint8 change_count = 0;
        for (uint8 x = X_MIN; x <= X_MAX; x ++) {
            if (image[Y_MAX][x] != CURR) {
                CURR = image[Y_MAX][x];
                change_count ++;
            }
            if (change_count > 9) {
                image_result.element_type = Zebra;
                return;
            }
        }
        if (el == Zebra) image_result.element_type = Normal;
    }

    uint8 xl, xr, xm, y, y_bd_min;
    int8 dx, dy;
    uint8 xls[BD_LENGTH] = { 0 }, xrs[BD_LENGTH] = { 0 };
    double ml, mr;
    bool ml_set = false, mr_set = false;

    uint8 dy0l, dy0r;
    bool l_unset = true, r_unset = true;

    uint8 l_ng_count = 0, r_ng_count = 0;
    uint8 l_g_count = BD_G_COUNT_MAX, r_g_count = BD_G_COUNT_MAX;
    bool l_ng = true, r_ng = true, l_lost = false, r_lost = false;
    uint8 l_segs = 0, r_segs = 0;
    uint16 sw = 0;
    uint8 w = 0;
    double so = 0;

    // Find the bottom
    bottom:
    dy0l = dy0r = 0;
    ml_set = mr_set = false;
    if (el == LoopLeft || el == LoopLeftAfter || el == RampLeft) l_lost = true;
    else {
        for (y = Y_MAX; image[y][X_MIN] == ROAD && y > Y_BOTTOM_MIN; y --);
        if (y == Y_BOTTOM_MIN) l_lost = l_ng = true;
        else {
            for (xl = X_MIN; xl < X_MID && image[Y_MAX][xl] != ROAD; xl ++);
            xls[0] = xl;
            SET_IMG(xl, y, BOUND);
        }
    }

    if (el == LoopRight) r_lost = true;
    else {
        for (y = Y_MAX; image[y][X_MAX] == ROAD && y > Y_BOTTOM_MIN; y --);
        if (y == Y_BOTTOM_MIN) r_lost = r_ng = true;
        else {
            if (el == LoopLeftAfter || el == RampLeft)
                for (xr = X_MID; image[y][xr + 1] != EMPTY; xr ++);
            else
                for (xr = X_MAX; xr > X_MID && image[Y_MAX][xr] != ROAD; xr --);
            xrs[0] = xr;
            SET_IMG(xr, Y_MAX, BOUND);
        }
    }

    // Find the bounds
    for (y = Y_MAX - 1, dx = 0, dy = 1; y >= Y_BD_MIN; y --, dy ++) {
        if (y == Y_NORMAL_MIN && el_normal && ! l_ng_count && ! r_ng_count && l_segs == 1 && r_segs == 1) {
            break;
        }

        if (l_lost) goto left$;
        for (dx = 0; image[y][xl] == EMPTY; dx ++, xl ++)
            if (dx == DX_BD_MAX) { xl -= DX_BD_MAX; break; }
        if (! dx && xl != X_MIN) for (; image[y][xl - 1] == ROAD && xl > X_MIN; dx --, xl --)
            if (dx == - DX_BD_MAX) { xl += DX_BD_MAX; break; }

        if (xl == X_MIN) {
            l_unset = true;
            goto left_ng;
        }
        if (l_unset) {
            dy0l = dy;
            l_unset = false;
        }

        if (abs(dx) == DX_BD_MAX) {
            if (ml_set) {
                xl = ml * (dy - dy0l) + xls[dy0l];
                xls[dy] = xl;
                SET_IMG(xl, y, BOUND_APP);
                goto left_ng;
            }
        }
        else if (xl != X_MIN) {
            if (! l_unset && dy - dy0l && dx > 0 && dx <= DX_M_MAX && xl - xrs[dy0l]) {
                ml = (double) (xl - xls[dy0l]) / (dy - dy0l);
                ml_set = true;
            }
            xls[dy] = xl;
            SET_IMG(xl, y, BOUND);
            if (l_ng_count) {
                l_ng_count = 0;
                if (y > Y_STRICT_G_MAX) l_g_count = BD_G_COUNT_MAX;
                else goto left_g;
            }
            else if (l_g_count) {
                if (-- l_g_count == 0) goto left_g;
            }
        }
        goto left$;

        left_g:
        if (l_ng) {
            l_ng = false;
            l_segs ++;
        }
        goto left$;

        left_ng:
        l_g_count = 0;
        if (++ l_ng_count == BD_NG_COUNT_MAX) l_ng = true;
        left$:

        if (r_lost) goto right$;
        for (dx = 0; image[y][xr] == EMPTY; dx --, xr --)
            if (dx == - DX_BD_MAX) { xr += DX_BD_MAX; break; }
        if (! dx && xr != X_MAX) {
            if (image[y][xr + 1] == ROAD) {
                if (el == LoopLeftAfter && y >= Y_RAMP_CHECKPOINT_MIN) {
                    el = image_result.element_type = RampLeft;
                    break;
                }
                while (true) {
                    dx ++, xr ++;
                    if (image[y][xr + 1] != ROAD) break;
                    if (dx == DX_BD_MAX) { xr -= DX_BD_MAX; break; }
                }
            }
        }

        if (xr == X_MAX) {
            r_unset = true;
            goto right_ng;
        }
        else if (r_unset) {
            dy0r = dy;
            r_unset = false;
        }

        if (abs(dx) == DX_BD_MAX) {
            if (el == LoopLeftAfter || el == RampLeft) break;
            if (mr_set) {
                xr = mr * (dy - dy0r) + xrs[dy0r];
                xrs[dy] = xr;
                SET_IMG(xr, y, BOUND_APP);
                goto right_ng;
            }
        }
        else if (xr != X_MAX) {
            if (! r_unset && dy - dy0r && dx < 0 && dx >= - DX_M_MAX && xr != xrs[dy0r]) {
                mr = (double) (xr - xrs[dy0r]) / (dy - dy0r);
                mr_set = true;
            }
            xrs[dy] = xr;
            SET_IMG(xr, y, BOUND);
            if (r_ng_count) {
                r_ng_count = 0;
                if (y > Y_STRICT_G_MAX) r_g_count = BD_G_COUNT_MAX;
                else goto right_g;
            }
            else if (r_g_count) {
                if (-- r_g_count == 0) goto right_g;
            }
        }
        goto right$;

        right_g:
        if (r_ng) {
            r_ng = false;
            r_segs ++;
        }
        goto right$;

        right_ng:
        r_g_count = 0;
        if (++ r_ng_count == BD_NG_COUNT_MAX) r_ng = true;
        right$:
        ;
    }

    if (el != Cross) goto cross$;
    cross:
    for (y = Y_CROSS_TOP_MIN; ; y ++) {
        break;
    }
    cross$:

    // Analyze element type
    if (el_normal) {
        if (l_segs >= 3) {
            image_result.element_type = LoopLeftBefore;
        }
        else if (l_segs >= 2 && r_segs >= 2) {
            image_result.element_type = CrossBefore;
        }
        goto mid;
    }

    if (el == CrossBefore) {
        if (l_segs < 2 && r_segs < 2) {
            el = image_result.element_type = Cross;
            goto cross;
        }
        goto mid;
    }

    if (el == LoopLeftBefore) {
        if (l_segs < 3) image_result.element_type = LoopLeft;
        goto mid;
    }

    if (el == LoopLeft) {
        y ++;
        for (xl = xr ? xr : X_MAX; image[y][xl] != ROAD && xl >= X_MIN; xl --);
        for (; image[y][xl - 1] == ROAD && xl >= X_MIN; xl --);
        SET_IMG(xl, y, SPECIAL);
        for (dx = 0; y <= Y_MAX; y ++) {
            for (dx = 0; image[y][xl] == EMPTY; dx ++, xl ++);
            if (! dx) for (; image[y][xl - 1] != EMPTY; dx --, xl --)
                if (dx == - DX_BD_MAX) {
                    xl += DX_BD_MAX;
                    SET_IMG(xl, y, SPECIAL);
                    goto loop_left_corner$;
                }
        }
        loop_left_corner$:

        if (y > Y_MAX) {
            el = image_result.element_type = LoopLeftAfter;
            goto bottom;
        }
        else {
            uint8 yc = y;
            double m = (double) ((xrs[0] ? xrs[0] : X_MAX) - xl) / (Y_MAX - yc);
            so = 0;
            sw = 0;
            for (dy = 1, y = yc + 1; y <= Y_MAX; y ++, dy ++) {
                if (y <= Y_MID_LINE_MIN) continue;
                xr = xl + m * dy;
                SET_IMG(xr, y, BOUND_APP);
                w = STD_WIDTH[Y_MAX - y];
                xm = xr - w;
                so += (xm - X_MID) * w;
                sw += w;
                SET_IMG(xm, y, MID_LINE);
            }
            image_result.offset = so / sw - xm + X_MID;
            goto mid$;
        }
    }

    if (el == RampLeft) {
        if (! r_ng && y < Y_BD_MIN) {
            image_result.element_type = Normal;
            goto mid;
        }
        for (y = Y_RAMP_TOP_MIN; image[y][X_MIN] == EMPTY; y ++);
        SET_IMG(X_MIN, y, SPECIAL);
        uint8 xc = xrs[0] ? (xrs[0] + X_MIN) >> 1 : X_MID;
        uint8 yc = (y + Y_MAX) >> 1;
        double m = (double) xc / (Y_MAX - yc);
        so = 0;
        for (dy = 0, y = Y_MAX; y >= yc && y >= Y_MID_LINE_MIN; y --, dy ++) {
            xm = xc - m * dy;
            SET_IMG(xm, y, MID_LINE);
            so += xm - X_MID;
        }
        image_result.offset = so / (Y_MAX - y);
        goto mid$;
    }

    // Calculate midline
    mid:
    uint8 dy_max = Y_MAX - max(y_bd_min, Y_MID_LINE_MIN);
    so = 0;
    sw = 0;
    for (uint8 dy = 0; dy <= dy_max; dy ++) {
        xl = xls[dy];
        xr = xrs[dy];
        if (! xl && ! xr) continue;
        if (! xl) {
            w = STD_WIDTH[dy];
            xm = xr - w;
        }
        else if (! xr) {
            w = STD_WIDTH[dy];
            xm = xl + w;
        }
        else {
            w = xr - xl;
            xm = (xl + xr) >> 1;
        }
        so += (xm - X_MID) * w;
        sw += w;
        SET_IMG(xm, Y_MAX - dy, MID_LINE);
    }
    image_result.offset = so / sw;

    mid$:

    SET_IMG(X_MID + (int8) (image_result.offset), Y_MAX, SPECIAL);
}
