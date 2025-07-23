#include "mt_image.h"
#include "zf_device_mt9v03x.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

ImageResult image_result;

uint8 otsu_counter = 0;
uint8 otsu_threshold = 0;

uint8 image_buffer[HEIGHT][REAL_WIDTH] = { 0 };

#ifdef IMAGE_DEBUG
    #define SET_IMG(x, y, T) do { \
        image[y][x] = T; \
    } while (0)
    #define debug(...) printf(__VA_ARGS__)
#else
    #include <zf_common_font.h>
    void SET_IMG(uint8 x, uint8 y, uint8 T) {
        uint16 color;
        if (T == MID_LINE) color = RGB565_YELLOW;
        else if (T == BOUND) color = RGB565_GREEN;
        else if (T == BOUND_APP) color = RGB565_CYAN;
        else if (T == SPECIAL) color = RGB565_MAGENTA;
        else color = RGB565_RED;
        // ips200_draw_point(x % 188, y % 120, color);
    }
    #define debug(...)
#endif

void process_image(Image image) {
    memcpy(image_buffer, image, sizeof (uint8) * REAL_WIDTH * HEIGHT);
    mt9v03x_finish_flag = 0;
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
            image[i][j] = image[i][j] > threshould ? 255 : 0;
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

int8 limit(int8 x, uint8 a) {
    if (x < - a) return - a;
    if (x > a) return a;
    return x;
}

void search(Image image) {
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

    uint8 xl, xr, y;
    int16 dx, xm;
    uint8 xls[HEIGHT] = { 0 }, xrs[HEIGHT] = { 0 };
    double ml, mr;
    bool ml_set = false, mr_set = false;
    bool l_pad = false, r_pad = false;

    uint8 y0l, y0r;
    uint8 y_start, y_end;
    bool l_unset = true, r_unset = true;
    bool lost = false;
    int8 l_convex, r_convex;
    uint8 l_l, l_r, r_l, r_r;
    int8 l_dx = 0, r_dx = 0;
    bool le_failed;

    uint8 l_ng_count = 0, r_ng_count = 0;
    uint8 l_g_count = BD_G_COUNT_MAX, r_g_count = BD_G_COUNT_MAX;
    bool l_ng = true, r_ng = true;
    bool l_stop0 = false, r_stop0 = false, l_stop = false, r_stop = false;
    uint8 l_segs, r_segs;
    uint16 sw = 0;
    double so = 0;
    double kw = 1;

    // Find the bottom
    bottom:
    for (y = Y_MAX; y > Y_BOTTOM_BOTH_LOST_MIN; y --) {
        xl = xr = X_MID;
        if (image[y][X_MID] != EMPTY) {
            while (image[y][xl - 1] != EMPTY)
                if (-- xl == X_MIN) break;
            while (image[y][xr + 1] != EMPTY)
                if (++ xr == X_MAX) break;
        }
        else {
            while (true) {
                if (-- xl == X_MIN) break;
                if (image[y][xl] != EMPTY) {
                    xr = xl;
                    while (image[y][xl - 1] != EMPTY)
                        if (-- xl == X_MIN) break;
                    break;
                }
                xr ++;
                if (image[y][xr + 1] != EMPTY) {
                    xl = xr;
                    while (image[y][xr + 1] != EMPTY)
                        if (++ xr == X_MAX) break;
                    break;
                }
            }
        }
        if (xl != X_MIN || xr != X_MAX) {
            if (xl != X_MIN) xls[y] = xl;
            if (xr != X_MAX) xrs[y] = xr;
            break;
        }
    }
    l_pad = xls[Y_MAX] == X_MIN;
    r_pad = xrs[Y_MAX] == X_MAX;

    y_end = y;
    SET_IMG(xl, y, BOUND);
    SET_IMG(xr, y, BOUND);

    if (xl == X_MIN)
        for (y = y_end; image[y --][X_MIN] != EMPTY; )
            if (y == Y_BOTTOM_MIN) {
                l_stop0 = l_stop = l_ng = true;
                break;
            }

    if (xr == X_MAX)
        for (y = y_end; image[y --][X_MAX] != EMPTY;)
            if (y == Y_BOTTOM_MIN) {
                r_stop0 = r_stop0 = r_ng = true;
                break;
            }

    if (l_stop0 && r_stop0) {
        return;
    }
    
    if (el == LoopRight) goto loop;
    else if (el == LoopLeft) goto loop;

    // Find the bounds
    if (el == Cross) goto cross_bound;

    normal_bound:
    y0l = y0r = 0;
    l_unset = r_unset = true;
    ml_set = mr_set = false;
    l_convex = r_convex = 0;
    l_l = l_r = r_l = r_r = 0;
    l_segs = r_segs = 0;
    y_end = Y_MAX;
    le_failed = false;

    for (y = Y_MAX - 1, dx = 0; y >= Y_BD_MIN; y --) {
        if (y == Y_NORMAL_MIN && prev_el_normal && ! l_ng_count && ! r_ng_count && l_segs == 1 && r_segs == 1) {
            break;
        }

        if (l_stop) goto left$;

        lost = false;
        for (dx = 0; image[y][xl] == EMPTY; dx ++, xl ++)
            if (dx == DX_BD_MAX) { xl -= DX_BD_MAX; lost = true; break; }
        if (! dx && xl != X_MIN)
            for (; image[y][xl - 1] != EMPTY && xl > X_MIN; dx --, xl --)
                if (dx == - DX_BD_INV_MAX) {
                    xl += DX_BD_INV_MAX;
                    if (! le_failed && image[y - 1][xl] == EMPTY) {
                        uint8 xc = xl, yc = y;
                        uint8 dy = 0;
                        uint8 up_count = 0;
                        for (; xc <= X_MAX; xc ++) {
                            for (dy = 0; image[yc][xc] == EMPTY; yc ++)
                                if (++ dy == LE_DN_MAX) goto l_loop_exit_lost;
                            if (image[yc - 1][xc] != EMPTY) {
                                if (++ up_count == LE_UP_COUNT_MAX) goto l_loop_exit_lost;
                                for (dy = 1, yc --; image[yc - 1][xc] != EMPTY; yc --)
                                    if (++ dy == LE_UP_MAX) goto l_loop_exit_lost;
                            }
                            SET_IMG(xc, yc, BOUND);
                        }
                        if (xc > X_MAX) {
                            y_start = yc;
                            double xlf = xls[Y_MAX];
                            SET_IMG(xls[Y_MAX], Y_MAX, SPECIAL);
                            double m = (double) (xc - xlf) / (Y_MAX - yc);
                            for (y = Y_MAX - 1; y > yc; y --) {
                                xlf += m;
                                xls[y] = (uint8) xlf;
                                SET_IMG((uint8) xlf, y, BOUND_APP);
                            }
                            goto mid;
                        }
                    }
                    goto l_inv_lost;
                    l_loop_exit_lost:
                    le_failed = true;
                    l_inv_lost:
                    lost = true;
                    break;
                }

        if (xl == X_MIN) {
            l_unset = true;
            goto left_ng;
        }
        if (l_unset) {
            y0l = y;
            l_unset = false;
        }

        if (lost) {
            if (el > Cross) l_stop = true;
            else if (ml_set) {
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
            if (dx > 0) l_r ++;
            else l_l ++;
            l_convex += limit(dx - l_dx, CONVEX_LIMIT);
            if (l_convex >= CV_CONVEX && prev_el_normal) {
                el = image_result.element_type = CurveRight;
                break;
            }
            else if (l_convex <= LP_CONVEX && el == LoopLeftBefore) {
                el = image_result.element_type = LoopLeftBefore2;
                prev_el_normal = false;
            }
            l_dx = dx;

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
        if (! dx && xr != X_MAX)
            for (; image[y][xr + 1] != EMPTY && xr < X_MAX; dx ++, xr ++)
                if (dx == DX_BD_INV_MAX) {
                    xr -= DX_BD_INV_MAX;
                    if (! le_failed && l_stop0 && image[y - 1][xr] == EMPTY) {
                        uint8 xc = xr, yc = y;
                        uint8 dy = 0;
                        uint8 up_count = 0;
                        for (; xc >= X_MIN; xc --) {
                            for (dy = 0; image[yc][xc] == EMPTY; yc ++)
                                if (++ dy == LE_DN_MAX) goto r_loop_exit_lost;
                            if (image[yc - 1][xc] != EMPTY) {
                                if (++ up_count == LE_UP_COUNT_MAX) goto r_loop_exit_lost;
                                for (dy = 1, yc --; image[yc - 1][xc] != EMPTY; yc --)
                                    if (++ dy == LE_UP_MAX) goto r_loop_exit_lost;
                            }
                            SET_IMG(xc, yc, BOUND);
                        }
                        if (xc < X_MIN) {
                            y_start = yc;
                            double xrf = xrs[Y_MAX];
                            SET_IMG(xrs[Y_MAX], Y_MAX, SPECIAL);
                            double m = (double) (xc - xrf) / (Y_MAX - yc);
                            for (y = Y_MAX - 1; y > yc; y --) {
                                xrf += m;
                                xrs[y] = (uint8) xrf;
                                SET_IMG((uint8) xrf, y, BOUND_APP);
                            }
                            goto mid;
                        }
                    }
                    goto r_inv_lost;
                    r_loop_exit_lost:
                    le_failed = true;
                    r_inv_lost:
                    lost = true;
                    break;
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
            if (el > Cross) r_stop = true;
            else if (mr_set) {
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
            if (dx > 0) r_l ++;
            else r_r ++;
            r_convex += limit(dx - r_dx, CONVEX_LIMIT);
            if (r_convex >= CV_CONVEX && prev_el_normal) {
                el = image_result.element_type = CurveLeft;
                break;
            }
            else if (r_convex <= LP_CONVEX && el == LoopRightBefore) {
                el = image_result.element_type = LoopRightBefore2;
                prev_el_normal = false;
            }
            r_dx = dx;

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
    }
    y_start = y;

    goto cross_bound$;

    loop:
    y_end = Y_MAX;
    if (el == LoopLeft) {
        for (y = Y_MAX; image[y][X_MIN] == EMPTY && y > Y_LP_MIN; y --);
        for (; image[y - 1][X_MIN] != EMPTY; y --)
            if (y == Y_LP_MIN) goto loop_cancel;
        SET_IMG(X_MIN, y, SPECIAL);

        uint8 y0, y1, xc, yc;
        uint8 up_count = 0, up_failed_count = 0;

        for (xl = X_MIN + 1; ; xl ++) {
            if (xl == X_LP_CORNER_L_MAX) goto loop_cancel;
            if (image[y - 1][xl] != EMPTY) {
                if (++ up_count == LP_UP_COUNT_MAX) {
                    if (xl < X_LP_CORNER_L_MIN) goto loop_cancel;
                    xc = max((int16) xl - LP_UP_COUNT_MAX - X_LP_CORNER_L_OFFSET, X_MIN);
                    y_start = yc = y = y0;
                    SET_IMG(xc, yc, SPECIAL);
                    break;
                }
                for (y1 = y, y --; image[y - 1][xl] != EMPTY; y --)
                    if (y == Y_LP_MIN) {
                        if (++ up_failed_count == LP_UP_FAILED_MAX) goto loop_cancel;
                        y = y1;
                        break;
                    }
            }
            else {
                up_count = 0;
                y0 = y;
            }
            for (y1 = y; image[y][xl] == EMPTY; y ++)
                if (y == y1 - LP_UP_COUNT_MAX) {
                    y = y1;
                    break;
                }
            SET_IMG(xl, y, BOUND);
        }
        
        double xrf = xrs[Y_MAX];
        SET_IMG(xrs[Y_MAX], Y_MAX, SPECIAL);
        double m = (double) (xc - xrf) / (Y_MAX - yc);
        // kw *= calc_kw_by_m(m);
        for (y = Y_MAX - 1; y > yc; y --) {
            xrf += m;
            xrs[y] = (uint8) xrf;
            SET_IMG((uint8) xrf, y, BOUND_APP);
        }
    }
    else {
        for (y = Y_MAX; image[y][X_MAX] == EMPTY && y > Y_LP_MIN; y --);
        for (; image[y - 1][X_MAX] != EMPTY; y --)
            if (y == Y_LP_MIN) goto loop_cancel;
        SET_IMG(X_MAX, y, BOUND);

        uint8 y0, xc, yc;
        uint8 up_count = 0, up_failed_count = 0;

        for (xr = X_MAX - 1; ; xr --) {
            if (xr == X_LP_CORNER_R_MIN) goto loop_cancel;
            if (image[y - 1][xr] != EMPTY) {
                if (++ up_count == LP_UP_COUNT_MAX) {
                    if (xr > X_LP_CORNER_R_MAX) goto loop_cancel;
                    xc = min(xr + X_LP_CORNER_R_OFFSET, X_MAX);
                    y_start = yc = y;
                    SET_IMG(xc, yc, SPECIAL);
                    break;
                }
                for (y0 = y, y --; image[y - 1][xr] != EMPTY; y --)
                    if (y == Y_LP_MIN) {
                        if (++ up_failed_count == LP_UP_FAILED_MAX) goto loop_cancel;
                        y = y0;
                        break;
                    }
            }
            else {
                up_count = 0;
            }
            while (image[y][xr] == EMPTY && y < Y_MAX) y ++;
            SET_IMG(xr, y, BOUND);
        }
        
        double xlf = xls[Y_MAX];
        SET_IMG(xls[Y_MAX], Y_MAX, SPECIAL);
        double m = (double) (xc - xlf) / (Y_MAX - yc);
        // kw *= calc_kw_by_m(m);
        for (y = Y_MAX - 1; y > yc; y --) {
            xlf += m;
            xls[y] = (uint8) xlf;
            SET_IMG((uint8) xlf, y, BOUND_APP);
        }
    }
    goto mid;

    loop_cancel:
    el = image_result.element_type = Normal;
    goto normal_bound;

    cross_bound:
    // Find the cross top
    y = Y_XR_TOP_MIN;
    while (true) {
        xl = xr = X_MID;
        if (image[y][X_MID] != EMPTY) {
            while (image[y][xl - 1] != EMPTY)
                if (-- xl < X_XR_TOP_MIN) goto cross_next_y;
            while (image[y][xr + 1] != EMPTY)
                if (++ xr > X_XR_TOP_MAX) goto cross_next_y;
        }
        else {
            while (true) {
                if (-- xl < X_XR_TOP_MIN) goto cross_next_y;
                if (image[y][xl] != EMPTY) {
                    xr = xl;
                    while (image[y][xl - 1] != EMPTY)
                        if (-- xl < X_XR_TOP_MIN) goto cross_next_y;
                    break;
                }
                xr ++;
                if (image[y][xr + 1] != EMPTY) {
                    xl = xr;
                    while (image[y][xr + 1] != EMPTY)
                        if (++ xr > X_XR_TOP_MAX) goto cross_next_y;
                    break;
                }
            }
        }
        dx = xr - xl;
        if (dx > 5 && dx <= STD_WIDTH[y] * 3) break;
        cross_next_y:
        y += 3;
        if (y > Y_XR_TOP_MAX) {
            goto cross_exit;
        }
    }
    y_start = y;
    SET_IMG(xl, y, MID_LINE);
    SET_IMG(xr, y, MID_LINE);
    bool cross_top = true;
    // Find the cross bound
    for (y ++; y < Y_MAX; y ++) {
        for (dx = 0; image[y][xl - 1] != EMPTY; dx ++, xl --)
            if (dx == DX_BD_MAX) { cross_top = false; break; }
        if (! dx) for (; image[y][xl] == EMPTY; xl ++);
        xls[y] = xl;
        SET_IMG(xl, y, BOUND);
        for (dx = 0; image[y][xr + 1] != EMPTY; dx ++, xr ++)
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
    uint8 y0 = max(y_start, y - Y_XR_M_HEIGHT - Y_XR_M_OFFSET);
    uint8 y1 = y - Y_XR_M_OFFSET;
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
    if (el == LoopLeftBefore2 && (! l_pad && l_r < 2 || l_stop0)) {
        el = image_result.element_type = LoopLeft;
        goto loop;
    }
    else if (el == LoopRightBefore2 && (! r_pad && r_l < 2 || r_stop0)) {
        el = image_result.element_type = LoopRight;
        goto loop;
    }

    if (el <= Normal) {
        if (l_segs >= 3 && r_segs == 1) {
            el = image_result.element_type = LoopLeftBefore;
        }
        else if (l_segs == 1 && r_segs >= 3) {
            el = image_result.element_type = LoopRightBefore;
        }
        else if (l_segs >= 2 && r_segs >= 2) {
            el = image_result.element_type = CrossBefore;
        }
        else if (abs(l_convex) < CV_CONVEX && abs(r_convex) < CV_CONVEX) {
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

    // Calculate midline
    mid:
    so = 0;
    sw = 0;
    int8 dxm = 0;
    uint8 both_count = 0;
    for (y = y_end; y >= y_start; y --) {
        xl = xls[y];
        xr = xrs[y];
        if (xl && xr) {
            dx = xr - xl;
            xm = (xl + xr) >> 1;
            dxm = 0;
            both_count ++;
        }
        else {
            if (! xl && ! xr);
            else if (! xl || el == LoopLeft) {
                dx = kw * STD_WIDTH[y];
                if (both_count > BOTH_COUNT_MIN) dxm = xm - (xr - dx);
                else xm = xr - dx + dxm;
            }
            else if (! xr || el == LoopRight) {
                dx = kw * STD_WIDTH[y];
                if (both_count > BOTH_COUNT_MIN) dxm = xm - (xl + dx);
                else xm = xl + dx + dxm;
            }
            both_count = 0;
        }
        so += (xm - X_MID) * dx;
        sw += dx;
        SET_IMG(xm, y, MID_LINE);
    }
    image_result.offset = so / sw;
    mid$:

    SET_IMG(X_MID + (int8) (image_result.offset), Y_MAX, SPECIAL);
}
