#include <stdio.h>
#include <math.h>
#include <time.h>

#ifndef IMAGE_DEBUG
#define IMAGE_DEBUG
#endif

#include "../Main/project/user/inc/mt_image.h"

const int COLORS[] = { '0', '7', '2', '3', '6' };
const char *TRACK_DISPLAY[] = { "None", "Left", "Right", "Both" };
const char *ELEMENT_DISPLAY[] = { "Normal", "CurveLeft", "CurveRight", "Zebra", "LoopLeft", "LoopRight", "Cross" };

int main(int argc, char *argv[]) {
    FILE* fp_data = fopen(argv[1], "r");
    if (! fp_data) return 1;
    uint8 image[HEIGHT][REAL_WIDTH];
    for (uint8 i = 0; i < HEIGHT; i ++)
        for (uint8 j = 0; j < REAL_WIDTH; j ++)
            fscanf(fp_data, "%d", &image[i][j]);
    fclose(fp_data);

    clock_t start_time = clock();
    uint8 threshold = otsu_calc_threshold(image, OTSU_THRESHOLD_MIN, OTSU_THRESHOLD_MAX);
    otsu_binarize_image(image, threshold);
	SearchResult search_result = search(image);
    clock_t end_time = clock();

    printf("\x1B[2J\x1B[H\n");
    for (int i = Y_BD_MIN; i < HEIGHT; i ++) {
        printf("%03d ", i);
        for (int j = 0; j < WIDTH; j ++) {
            printf("\x1B[4%cm%c\x1B[0m", COLORS[image[i][j]], image[i][j] + '0');
        }
        printf("\n");
    }
    printf("[offset = %f]\n", search_result.offset);
    printf("[time = %dms]\n", (end_time - start_time) * 1000 / CLOCKS_PER_SEC);
    printf("[element = %s]\n", ELEMENT_DISPLAY[search_result.element_type]);

    return 0;
}
