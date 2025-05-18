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

const uint8 STD_WIDTH[HEIGHT] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    7,  8,  9,  10, 11, 12, 13, 14,
    15, 15, 16, 17, 18, 19, 19, 20,
    21, 22, 23, 23, 24, 25, 26, 26,
    27, 28, 29, 30, 30, 31, 32, 33,
    34, 34, 35, 36, 37, 38, 38, 39,
    40, 41, 42, 42, 43, 44, 45, 46,
    46, 47, 48, 49, 50, 51, 51, 52,
    53, 54, 55, 55, 56, 57, 58, 58,
    59, 60, 61, 61, 62, 63, 64, 64,
    65, 66, 67, 68, 68, 69, 70, 71,
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
    bool prev_el_normal = el <= Normal;

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
    int8 dx;
    uint8 xls[HEIGHT] = { 0 }, xrs[HEIGHT] = { 0 };
    double ml, mr;
    bool ml_set = false, mr_set = false;

    uint8 y0l, y0r;
    uint8 y_start, y_end;
    bool l_unset = true, r_unset = true;
    bool lost = false;
    uint8 l_curve, r_curve;

    uint8 l_ng_count = 0, r_ng_count = 0;
    uint8 l_g_count = BD_G_COUNT_MAX, r_g_count = BD_G_COUNT_MAX;
    bool l_ng = true, r_ng = true, l_stop = false, r_stop = false;
    uint8 l_segs, r_segs;
    uint16 sw = 0;
    double so = 0;

    // Find the bottom
    bottom:
    y0l = y0r = 0;
    ml_set = mr_set = false;
    l_curve = r_curve = 0;
    l_segs = r_segs = 0;
    if (el == LoopLeft || el == LoopLeftAfter || el == RampLeft) l_stop = true;
    else {
        for (y = Y_MAX; image[y][X_MIN] == ROAD && y > Y_BOTTOM_MIN; y --);
        if (y == Y_BOTTOM_MIN) l_stop = l_ng = true;
        else {
            if (el == LoopRightAfter || el == RampRight)
                for (xr = X_MID; image[y][xr + 1] != EMPTY; xr ++);
            else
                for (xr = X_MAX; xr > X_MID && image[Y_MAX][xr] != ROAD; xr --);

            for (xl = X_MIN; xl < X_MID && image[Y_MAX][xl] != ROAD; xl ++);
            xls[y] = xl;
            SET_IMG(xl, y, BOUND);
        }
    }

    if (el == LoopRight || el == LoopRightAfter || el == RampRight) r_stop = true;
    else {
        for (y = Y_MAX; image[y][X_MAX] == ROAD && y > Y_BOTTOM_MIN; y --);
        if (y == Y_BOTTOM_MIN) r_stop = r_ng = true;
        else {
            if (el == LoopLeftAfter || el == RampLeft)
                for (xr = X_MID; image[y][xr + 1] != EMPTY; xr ++);
            else
                for (xr = X_MAX; xr > X_MID && image[Y_MAX][xr] != ROAD; xr --);
            xrs[y] = xr;
            SET_IMG(xr, Y_MAX, BOUND);
        }
    }

    // Find the bounds
    if (el == Cross) goto cross_bound;
    normal_bound:
    y_end = Y_MAX;
    for (y = Y_MAX - 1, dx = 0; y >= Y_BD_MIN; y --) {
        if (y == Y_NORMAL_MIN && prev_el_normal && ! l_ng_count && ! r_ng_count && l_segs == 1 && r_segs == 1) {
            break;
        }

        if (l_stop) goto left$;

        lost = false;
        for (dx = 0; image[y][xl] == EMPTY; dx ++, xl ++)
            if (dx == DX_BD_MAX) { xl -= DX_BD_MAX; lost = true; break; }
        if (! dx && xl != X_MIN) for (; image[y][xl - 1] == ROAD && xl > X_MIN; dx --, xl --)
            if (dx == - DX_BD_INV_MAX) { xl += DX_BD_INV_MAX; lost = true; break; }

        if (xl == X_MIN) {
            l_unset = true;
            goto left_ng;
        }
        if (l_unset) {
            y0l = y;
            l_unset = false;
        }

        if (lost) {
            if (el == LoopRightAfter || el == RampRight) break;
            if (ml_set) {
                xl = ml * (y - y0l) + xls[y0l];
                xls[y] = xl;
                SET_IMG(xl, y, BOUND_APP);
                goto left_ng;
            }
        }
        else if (xl != X_MIN) {
            if (! l_unset && y - y0l && dx > 0 && dx <= DX_M_MAX && xl - xrs[y0l]) {
                ml = (double) (xl - xls[y0l]) / (y - y0l);
                ml_set = true;
            }
            xls[y] = xl;
            SET_IMG(xl, y, BOUND);
            dx = xl - xls[y + 1];
            if (dx >= DX_CURVE) if (++ r_curve == 3) break;
            if (dx <= - DX_CURVE) if (++ l_curve == 3) break;
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

        if (r_stop) goto right$;

        lost = false;
        for (dx = 0; image[y][xr] == EMPTY; dx --, xr --)
            if (dx == - DX_BD_MAX) { xr += DX_BD_MAX; lost = true; break; }
        if (! dx && xr != X_MAX) {
            if (image[y][xr + 1] == ROAD) {
                if (el == LoopLeftAfter && y >= Y_RAMP_CHECKPOINT_MIN) {
                    el = image_result.element_type = RampLeft;
                    break;
                }
                while (true) {
                    dx ++, xr ++;
                    if (image[y][xr + 1] != ROAD) break;
                    if (dx == DX_BD_MAX) { xr -= DX_BD_MAX; lost = true; break; }
                }
            }
        }

        if (xr == X_MAX) {
            r_unset = true;
            goto right_ng;
        }
        else if (r_unset) {
            y0r = y;
            r_unset = false;
        }

        if (lost) {
            if (el == LoopLeftAfter || el == RampLeft) break;
            if (mr_set) {
                xr = mr * (y - y0r) + xrs[y0r];
                xrs[y] = xr;
                SET_IMG(xr, y, BOUND_APP);
                goto right_ng;
            }
        }
        else if (xr != X_MAX) {
            if (! r_unset && y - y0r && dx < 0 && dx >= - DX_M_MAX && xr != xrs[y0r]) {
                mr = (double) (xr - xrs[y0r]) / (y - y0r);
                mr_set = true;
            }
            xrs[y] = xr;
            SET_IMG(xr, y, BOUND);
            dx = - xr + xrs[y + 1];
            if (dx >= DX_CURVE) if (++ l_curve == 3) break;
            if (dx <= - DX_CURVE) if (++ r_curve == 3) break;

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
    y_start = y;
    goto cross_bound$;

    cross_bound:
    // Find the cross top
    y = Y_CROSS_TOP_MIN;
    while (true) {
        xl = xr = X_MID;
        if (image[y][X_MID] == ROAD) {
            while (image[y][xl - 1] == ROAD)
                if (-- xl < X_CROSS_TOP_MIN) goto cross_next_y;
            while (image[y][xr + 1] == ROAD)
                if (++ xr > X_CROSS_TOP_MAX) goto cross_next_y;
        }
        else {
            while (true) {
                if (-- xl < X_CROSS_TOP_MIN) goto cross_next_y;
                if (image[y][xl] == ROAD) {
                    xr = xl;
                    while (image[y][xl - 1] == ROAD)
                        if (-- xl < X_CROSS_TOP_MIN) goto cross_next_y;
                    break;
                }
                xr ++;
                if (image[y][xr + 1] == ROAD) {
                    xl = xr;
                    while (image[y][xr + 1] == ROAD)
                        if (++ xr > X_CROSS_TOP_MAX) goto cross_next_y;
                    break;
                }
            }
        }
        dx = xr - xl;
        if (dx > 5 && dx <= STD_WIDTH[y] * 3) break;
        cross_next_y:
        printf("%d\n", dx);
        y += 3;
        if (y > Y_CROSS_TOP_MAX) {
            // exit(1);
            goto cross_exit;
        }
    }
    y_start = y;
    SET_IMG(xl, y, MID_LINE);
    SET_IMG(xr, y, MID_LINE);
    bool cross_top = true;
    // Find the cross bound
    for (y ++; y < Y_MAX; y ++) {
        for (dx = 0; image[y][xl - 1] == ROAD; dx ++, xl --)
            if (dx == DX_BD_MAX) { cross_top = false; break; }
        if (! dx) for (; image[y][xl] == EMPTY; xl ++);
        xls[y] = xl;
        SET_IMG(xl, y, BOUND);
        for (dx = 0; image[y][xr + 1] == ROAD; dx ++, xr ++)
            if (dx == DX_BD_MAX) { cross_top = false; break; }
        if (! dx) for (; image[y][xr] == EMPTY; xr --);
        xrs[y] = xr;
        if (! cross_top) break;
        SET_IMG(xr, y, BOUND);
    }
    y_end = y;
    if (y == Y_MAX) {
        cross_exit:
        el = image_result.element_type = Normal;
        goto normal_bound;
    }
    cross_app:
    // Append the cross bound
    uint8 y0 = max(y_start, y - Y_CROSS_M_HEIGHT - Y_CROSS_M_OFFSET);
    uint8 y1 = y - Y_CROSS_M_OFFSET;
    ml = (double) (xls[y1] - xls[y0]) / (y1 - y0);
    mr = (double) (xrs[y1] - xrs[y0]) / (y1 - y0);

#ifdef IMAGE_DEBUG
    for (; y < Y_MAX; y ++) {
        SET_IMG(xls[y0] + (int8) (ml * (y - y0)), y, BOUND_APP);
        SET_IMG(xrs[y0] + (int8) (mr * (y - y0)), y, BOUND_APP);
    }
#endif
    cross_bound$:

    // Analyze element type
    if (el <= Normal) {
        if (l_segs >= 3) {
            image_result.element_type = LoopLeftBefore;
        }
        else if (l_segs >= 2 && r_segs >= 2) {
            image_result.element_type = CrossBefore;
        }
        else if (l_curve == 3) {
            image_result.element_type = CurveLeft;
        }
        else if (r_curve == 3) {
            image_result.element_type = CurveRight;
        }
        else {
            image_result.element_type = Normal;
        }
        goto mid;
    }

    if (el == CrossBefore) {
        if (l_segs < 2 && r_segs < 2) {
            el = image_result.element_type = Cross;
            goto cross_bound;
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
            double m = (double) ((xrs[Y_MAX] ? xrs[Y_MAX] : X_MAX) - xl) / (Y_MAX - yc);
            so = 0;
            sw = 0;
            for (y = yc + 1, dx = 0; y <= Y_MAX; y ++) {
                if (y <= Y_MID_LINE_MIN) continue;
                dx += m;
                xr = xl + dx;
                SET_IMG(xr, y, BOUND_APP);
                dx = STD_WIDTH[y];
                xm = xr - dx;
                so += (xm - X_MID) * dx;
                sw += dx;
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
        for (y = Y_MAX, dx = 0; y >= yc && y >= Y_MID_LINE_MIN; y --) {
            xm = xc - dx;
            dx += m;
            SET_IMG(xm, y, MID_LINE);
            so += xm - X_MID;
        }
        image_result.offset = so / (Y_MAX - y);
        goto mid$;
    }

    // Calculate midline
    mid:
    uint8 dy_max = Y_MAX - max(y_bd_min, Y_MID_LINE_MIN);
    uint8 both_count = 0;
    so = 0;
    sw = 0;
    for (y = y_end; y >= y_start; y --) {
        xl = xls[y];
        xr = xrs[y];
        if (! xl && ! xr) continue;
        if (! xl) {
            if (both_count > BOTH_COUNT_MIN) continue;
            dx = STD_WIDTH[y];
            xm = xr - dx;
        }
        else if (! xr) {
            if (both_count > BOTH_COUNT_MIN) continue;
            dx = STD_WIDTH[y];
            xm = xl + dx;
        }
        else {
            both_count ++;
            dx = xr - xl;
            xm = (xl + xr) >> 1;
        }
        so += (xm - X_MID) * dx;
        sw += dx;
        SET_IMG(xm, y, MID_LINE);
    }
    image_result.offset = so / sw;
    mid$:

    SET_IMG(X_MID + (int8) (image_result.offset), Y_MAX, SPECIAL);
}
