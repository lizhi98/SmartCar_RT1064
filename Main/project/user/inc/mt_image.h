#ifndef _CODE_MT_IMAGE_H_
#define _CODE_MT_IMAGE_H_

#include "math.h"

#ifndef __linux__
    #include "zf_device_mt9v03x.h"
	#include "zf_common_typedef.h"
    #include "zf_device_ips200.h"
#else
	#define uint8 unsigned char
    #define int8 char
    #define uint16 unsigned short
    #define int16 short
#endif

#ifdef _VSCODE
#define PI 3.14159265
#endif

#ifndef MAX
#define MAX(_a, _b) ((_a) > (_b) ? (_a) : (_b))
#endif
#ifndef MIN
#define MIN(_a, _b) ((_a) < (_b) ? (_a) : (_b))
#endif

#define HEIGHT 120
#define REAL_WIDTH 188
#define WIDTH 187
#define X_MID 93
#define X_MIN 1
#define X_MAX 186
#define Y_MAX 119

typedef uint8 (*Image)[REAL_WIDTH];

extern uint8 image_buffer[HEIGHT][REAL_WIDTH];

typedef enum ElementType_t {
    Zebra,
    CurveLeft,
    CurveRight,
    Normal,
    CrossBefore,
    Cross,
    LoopLeftBefore,
    LoopRightBefore,
    LoopLeftBefore2,
    LoopRightBefore2,
    LoopLeft,
    LoopRight,
    LoopLeftAfter,
    LoopRightAfter,
    RampLeft,
    RampRight,
} ElementType;

typedef struct {
    double offset;
    ElementType element_type;
} ImageResult;

void process_image(Image image);
uint8 otsu_calc_threshold(Image image, uint8 MIN, uint8 MAX);
void otsu_binarize_image(Image image, uint8 threshould);
void search(Image image);

extern ImageResult image_result;
extern uint8 otsu_threshold;

#define OSTU_COUNTER_MAX 10
#define OTSU_THRESHOLD_MIN 20
#define OTSU_THRESHOLD_MAX 200
#define OTSU_COMPRESS_RATIO 1
#define OTSU_COMPRESS_RATIO_SQUARE 1 // OTSU_COMPRESS_RATIO * OTSU_COMPRESS_RATIO
#define OTSU_THRESHOLD_DELTA 4

#define EMPTY 0
#define ROAD 1
#define BOUND 2
#define MID_LINE 3
#define BOUND_APP 4
#define SPECIAL 5

#define Y_SEARCH_MIN 35
#define Y_NORMAL_MIN 60
#define Y_BD_MIN 38
#define Y_BOTTOM_MIN 78
#define Y_BOTTOM_BOTH_LOST_MIN 110
#define Y_LP_END_MIN 35
#define Y_XR_TOP_MIN 50
#define Y_XR_TOP_MAX 80
#define X_XR_TOP_MIN 45
#define X_XR_TOP_MAX 143
#define Y_XR_M_HEIGHT 5
#define Y_XR_M_OFFSET 2
#define Y_STRICT_G_MAX 45
#define Y_MID_LINE_MIN 95
#define X_CURVE_OFFSET 25
#define DX_BD_MAX 5
#define DX_BD_INV_MAX 5
#define DX_M_MAX 2
#define BD_LENGTH 85
#define BD_NG_COUNT_MAX 3
#define BD_G_COUNT_MAX 2
#define CV_CONVEX 4
#define LP_CONVEX (- 4)
#define LP_UP_COUNT_MAX 3
#define LP_UP_FAILED_MAX 3
#define LE_DN_MAX 3
#define LE_UP_MAX 3
#define LE_UP_COUNT_MAX 3
#define Y_LP_MIN 30
#define X_LP_CORNER_R_MIN X_MIN
#define X_LP_CORNER_R_MAX 150
#define X_LP_CORNER_L_MIN 20
#define X_LP_CORNER_L_MAX X_MAX
#define X_LP_CORNER_L_OFFSET 5
#define X_LP_CORNER_R_OFFSET 10
#define CONVEX_LIMIT 2
#define BOTH_COUNT_MIN 5

#endif 
