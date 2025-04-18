#ifndef _USER_CUBE_DETECTION_H_
#define _USER_CUBE_DETECTION_H_

#include "zf_common_typedef.h"

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

#endif