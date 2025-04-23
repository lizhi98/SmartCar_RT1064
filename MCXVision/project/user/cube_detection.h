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

typedef struct _CubeDebugInfo {
    bool  exist;
    int32 x_center;
    int32 y_center;
    int32 x_min;
    int32 x_max;
    int32 y_min;
    int32 y_max;
} CubeDebugInfo;

extern CubeInfo cube_info;
extern CubeDebugInfo cube_debug_info;

void find_red_cube_center(uint16 *scc8660_image);

#endif