#ifndef _USER_CUBE_DETECTION_H_
#define _USER_CUBE_DETECTION_H_

#include "zf_common_typedef.h"

#include <stdint.h>

#define SCC8660_W               (320)
#define SCC8660_H               (240)
#define R_BASE_THRESHOLD        (18)    // 红色基础阈值（5位值）
#define MIN_RED_PIXELS          (80)    // 最小有效红色像素数

typedef enum _CubeDetectionState{
    CUBE_INSIDE_VIEW,
    CUBE_OUTSIDE_VIEW,
}CubeDetectionState;

typedef struct _CubeInfo{
    CubeDetectionState state; // 立方体在不在视野内
    int32 x_offset;
    int32 y_offset;
}CubeInfo;

extern CubeInfo cube_info;

void find_red_cube_center(uint16_t *scc8660_image, int *x, int *y);

#endif