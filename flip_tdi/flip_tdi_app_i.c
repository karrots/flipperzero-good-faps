#include "flip_tdi_app_i.h"

#include <furi.h>

#define TAG "FlipTDI"

void flip_tdi_start(FlipTDIApp* app) {
    furi_assert(app);

    app->ftdi_usb = ftdi_usb_start();
    flip_tdi_set_i2c_mode(app, false);
}

void flip_tdi_stop(FlipTDIApp* app) {
    furi_assert(app);

    if(app->ftdi_usb) {
        ftdi_usb_stop(app->ftdi_usb);
        app->ftdi_usb = NULL;
    }
}

void flip_tdi_set_i2c_mode(FlipTDIApp* app, bool enable) {
    furi_assert(app);
    if(app->ftdi_usb) {
        ftdi_usb_set_i2c_mode(app->ftdi_usb, enable);
    }
}
