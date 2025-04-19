#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifndef IMAGE_DEBUG
#define IMAGE_DEBUG
#endif

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

#include "../Main/project/user/inc/mt_image.h"

const int COLORS[] = { '0', '7', '2', '3', '6' };
const char *TRACK_DISPLAY[] = { "None", "Left", "Right", "Both" };
const char *ELEMENT_DISPLAY[] = { "Normal", "CurveLeft", "CurveRight", "Zebra", "LoopLeft", "LoopRight", "Cross" };

int main(int argc, char *argv[]) {
    char *filename = argv[1];
    char *prev_data = argv[2];

    FILE* fp_data = fopen(filename, "r");
    if (! fp_data) return 1;
    uint8 image[HEIGHT][REAL_WIDTH];
    for (uint8 i = 0; i < HEIGHT; i ++)
        for (uint8 j = 0; j < REAL_WIDTH; j ++)
            fscanf(fp_data, "%d", &image[i][j]);
    fclose(fp_data);

    if (strlen(prev_data)) {
        sscanf(prev_data,
            "[prev element = %*d (%*[a-zA-Z])]\n"
            "[offset = %f]\n"
            "[time = %*d ms]\n"
            "[element = %d (%*[a-zA-Z])]\n",
            &image_result.offset,
            &image_result.element_type
        );
    }

    eprintf("[prev element = %d (%s)]\n", image_result.element_type, ELEMENT_DISPLAY[image_result.element_type]);

    clock_t start_time = clock();
    uint8 threshold = otsu_calc_threshold(image, OTSU_THRESHOLD_MIN, OTSU_THRESHOLD_MAX);
    otsu_binarize_image(image, threshold);
	search(image);
    clock_t end_time = clock();

    printf("\x1B[2J\x1B[H\n");
    for (int i = Y_BD_MIN; i < HEIGHT; i ++) {
        printf("%03d ", i);
        for (int j = 0; j < WIDTH; j ++) {
            printf("\x1B[4%cm%c\x1B[0m", COLORS[image[i][j]], image[i][j] + '0');
        }
        printf("\n");
    }

    eprintf("[offset = %f]\n", image_result.offset);
    eprintf("[time = %d ms]\n", (end_time - start_time) * 1000 / CLOCKS_PER_SEC);
    eprintf("[element = %d (%s)]\n", image_result.element_type, ELEMENT_DISPLAY[image_result.element_type]);

    return 0;
}
