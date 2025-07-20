#ifndef _CODE_GRAYSCALE_H
#define _CODE_GRAYSCALE_H

#include "zf_driver_gpio.h"

#define GRAYSCALE_PIN   D4

#define GRAY_IN_TRACK   0
#define GRAY_OUT_TRACK  1

extern uint8 grayscale_data;

void grayscale_init(void);

void grayscale_read(void);

#endif