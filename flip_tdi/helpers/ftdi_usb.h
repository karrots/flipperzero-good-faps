#pragma once

#include "ftdi.h"
#include <stdbool.h>

typedef struct FtdiUsb FtdiUsb;

FtdiUsb* ftdi_usb_start(void);
void ftdi_usb_stop(FtdiUsb* ftdi);
void ftdi_usb_set_i2c_mode(FtdiUsb* ftdi, bool enable);
