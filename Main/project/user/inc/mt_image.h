#ifndef _CODE_MT_IMAGE_H_
#define _CODE_MT_IMAGE_H_

#include "zf_device_mt9v03x.h"
#include "math.h"


#ifndef __linux__
	#include<zf_common_typedef.h>
#else
	#define uint8 unsigned char
    #define int8 char
#endif

#ifndef PI
#define PI 3.14159265
#endif
#define HEIGHT 120
#define REAL_WIDTH 188
#define WIDTH 187
#define X_MID 93
#define X_MAX 186
#define Y_MAX 119


// TARGET MOTION
// 目标角度
extern double      target_angle;

typedef uint8 (*Image)[REAL_WIDTH];

typedef enum ElementType_t {
    Normal,
    CurveLeft,
    CurveRight,
    Zebra,
} ElementType;

typedef enum Track_t {
    None,
    Left,
    Right,
    Both
} Track;

typedef struct MidlineResult_t {
    float offset;
    ElementType element_type;
#ifdef IMAGE_DEBUG
    Track track;
#endif
} SearchResult;

void process_image(Image image);
uint8 otsu_calc_threshold(Image image, uint8 min, uint8 max);
void otsu_binarize_image(Image image, uint8 threshould);
SearchResult search(Image image);

extern SearchResult search_result;
extern uint8 otsu_threshold;

extern uint8 OTSU_THRESHOLD_MIN;
extern uint8 OTSU_THRESHOLD_MAX;
extern int OSTU_COUNTER_MAX;
extern int OTSU_COMPRESS_RATIO;

#endif 
