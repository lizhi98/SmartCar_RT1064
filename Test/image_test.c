#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#ifndef IMAGE_DEBUG
#define IMAGE_DEBUG
#endif

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

#include "../Main/project/user/inc/mt_image.h"

const int COLORS[] = { '0', '7', '2', '3', '6', '4' };
const char *TRACK_DISPLAY[] = { "None", "Left", "Right", "Both" };
const char *ELEMENT_DISPLAY[] = {
    "Zebra", "CurveLeft", "CurveRight", "Normal",
    "CrossBefore", "Cross",
    "LoopLeftBefore", "LoopRightBefore",
    "LoopLeftBefore2", "LoopRightBefore2",
    "LoopLeft", "LoopRight",
};

int main(int argc, char *argv[]) {
    char *filename = argv[1];
    char *prev_data = argv[2];
    char *ctrl = argv[3];

    FILE *fp_data = fopen(filename, "r");
    if (! fp_data) return 1;
    uint8 image[HEIGHT][REAL_WIDTH];
    for (uint8 i = 0; i < HEIGHT; i ++)
        for (uint8 j = 0; j < REAL_WIDTH; j ++)
            fscanf(fp_data, "%d", &image[i][j]);
    fclose(fp_data);

    if (strlen(prev_data)) {
        sscanf(prev_data,
            "[prev element = %*d (%*[a-zA-Z0-9])]\n"
            "[offset = %f]\n"
            "[time = %*d ms]\n"
            "[element = %d (%*[a-zA-Z0-9])]\n",
            &image_result.offset,
            &image_result.element_type
        );
    }

    bool y_highlight_map[HEIGHT] = { 0 };
    bool x_highlight_map[WIDTH] = { 0 };
    bool show_original = false;

    if (strlen(ctrl)) {
        char ctrl_type;
        char ctrl_arg[16];
        int len;
        printf("\x1B[2J\x1B[H\n");
        while (sscanf(ctrl, "%c:%[^;];%n", &ctrl_type, ctrl_arg, &len) > 0) {
            ctrl += len;
            switch (ctrl_type) {
                case 'x': {
                    int x = atoi(ctrl_arg);
                    if (x >= 0 && x < WIDTH) x_highlight_map[x] = true;
                    break;
                }
                case 'y': {
                    int y = atoi(ctrl_arg);
                    if (y >= 0 && y < HEIGHT) y_highlight_map[y] = true;
                    break;
                }
                case 'o': {
                    show_original = true;
                    break;
                }
            }
        }
    }

    eprintf("[prev element = %d (%s)]\n", image_result.element_type, ELEMENT_DISPLAY[image_result.element_type]);

    clock_t start_time = clock();
    uint8 threshold = otsu_calc_threshold(image, OTSU_THRESHOLD_MIN, OTSU_THRESHOLD_MAX);
    otsu_binarize_image(image, threshold);
	search(image);
    clock_t end_time = clock();

    printf("\x1B[2J\x1B[H\n");
    for (int i = Y_SEARCH_MIN; i < HEIGHT; i ++) {
        printf("%03d ", i);
        for (int j = 0; j < WIDTH; j ++) {
            char color = (y_highlight_map[i] || x_highlight_map[j])
                ? '5'
                : show_original
                    ? '0'
                    : COLORS[image[i][j]];
            printf("\x1B[4%cm%c\x1B[0m", color, image[i][j] + '0');
        }
        printf("\n");
    }

    eprintf("[offset = %f]\n", image_result.offset);
    eprintf("[time = %d ms]\n", (end_time - start_time) * 1000 / CLOCKS_PER_SEC);
    eprintf("[element = %d (%s)]\n", image_result.element_type, ELEMENT_DISPLAY[image_result.element_type]);

    return 0;
}
