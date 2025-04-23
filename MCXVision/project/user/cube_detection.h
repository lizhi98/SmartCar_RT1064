#ifndef _USER_CUBE_DETECTION_H_
#define _USER_CUBE_DETECTION_H_

#include "zf_common_typedef.h"
#include "zf_common_headfile.h"

#include <stdint.h>

#define R_BASE_THRESHOLD        (150)
#define MIN_RED_PIXELS          (4200)

typedef struct _CubeInfo {
    bool    exist;
    uint32  pixel_count;
    uint16  x_center;
    uint16  y_center;
    int32   x_min;
    int32   x_max;
    int32   y_min;
    int32   y_max;
} CubeInfo;

typedef struct _RGB {
    uint8 r;
    uint8 g;
    uint8 b;
} RGB;

extern CubeInfo cube_info;

void find_red_cube_center(uint16 *scc8660_image);

#endif