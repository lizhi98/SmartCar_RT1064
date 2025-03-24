#include <stdio.h>
#include <math.h>
#include <time.h>

#ifndef IMAGE_DEBUG
    #define IMAGE_DEBUG
#endif

#define int unsigned char
	#include "./Data/C/all_334.c"
#undef int

#include "../Main/project/user/inc/mt_image.h"

const int COLORS[] = { '0', '7', '2', '3' };
const char *TRACK_DISPLAY[] = { "None", "Left", "Right", "Both" };
const char *ELEMENT_DISPLAY[] = { "Normal", "CurveLeft", "CurveRight", "Zebra" };

int main() {
    clock_t start_time = clock();
    uint8 threshold = otsu_calc_threshold(image, OTSU_THRESHOLD_MIN, OTSU_THRESHOLD_MAX);
    otsu_binarize_image(image, threshold);
	SearchResult search_result = search(image);
    clock_t end_time = clock();

    for (int i = 0; i < HEIGHT; i ++) {
        for (int j = 0; j < WIDTH; j ++) {
            printf("\x1B[4%cm%c\x1B[0m", COLORS[image[i][j]], image[i][j] + '0');
        }
        printf("\n");
    }
	printf("[threshold = %d]\n", threshold);
    printf("[offset = %f]\n", search_result.offset);
    printf("[time = %dms]\n", (end_time - start_time) * 1000 / CLOCKS_PER_SEC);
    printf("[track = %s]\n", TRACK_DISPLAY[search_result.track]);
    printf("[element = %s]\n", ELEMENT_DISPLAY[search_result.element_type]);

    return 0;
}
