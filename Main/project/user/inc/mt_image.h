#ifndef _CODE_MT_IMAGE_H_
#define _CODE_MT_IMAGE_H_

#include "math.h"

#ifndef __linux__
    #include "zf_device_mt9v03x.h"
	#include "zf_common_typedef.h"
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
#define PIXEL 22560 // REAL_WIDTH * HEIGHT
#define X_MID 93
#define X_MAX 186
#define Y_MAX 119

typedef uint8 (*Image)[REAL_WIDTH];

typedef enum ElementType_t {
    Normal,
    CurveLeft,
    CurveRight,
    Zebra,
    LoopLeft,
    LoopRight,
    Cross,
} ElementType;

typedef struct {
    float offset;
    ElementType element_type;
} ImageResult;

void process_image(Image image);
uint8 otsu_calc_threshold(Image image, uint8 min, uint8 max);
void otsu_binarize_image(Image image, uint8 threshould);
void search(Image image);

extern ImageResult image_result;
extern uint8 otsu_threshold;

#define OSTU_COUNTER_MAX 10
#define OTSU_THRESHOLD_MIN 50

#define OTSU_THRESHOLD_MAX 200
#define OTSU_COMPRESS_RATIO 2
#define OTSU_COMPRESS_RATIO_SQUARE 4 // OTSU_COMPRESS_RATIO * OTSU_COMPRESS_RATIO
#define OTSU_THRESHOLD_DELTA 4

#define EMPTY 0
#define ROAD 1
#define BOUND 2
#define BOUND_APP 4
#define MID_LINE 3

#define Y_BD_MIN 35
#define Y_BD_NORMAL_MIN 80
#define DX_BD_MAX 5
#define DX_M_MAX 2
#define BD_LENGTH 85
#define LOST_COUNT_MAX 5

#endif 
