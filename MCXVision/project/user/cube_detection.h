#ifndef _USER_CUBE_DETECTION_H_
#define _USER_CUBE_DETECTION_H_

#include "zf_common_typedef.h"
#include "zf_common_headfile.h"

#include <stdint.h>

#define R_BASE_THRESHOLD        (15)
#define MIN_RED_PIXELS          (80)

typedef enum _CubeDetectionState {
    CUBE_INSIDE_VIEW,
    CUBE_OUTSIDE_VIEW,
} CubeDetectionState;

typedef struct _CubeInfo {
    CubeDetectionState state;
    int32 x_offset;
    int32 y_offset;
} CubeInfo;

extern CubeInfo cube_info;

void find_red_cube_center(uint16 *scc8660_image, int *x, int *y);

#endif