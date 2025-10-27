#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <furi_hal_gpio.h>

typedef struct {
    const GpioPin* scl_pin;
    const GpioPin* sda_out_pin;
    const GpioPin* sda_in_pin;

    uint32_t half_period_us;
    uint8_t value;
    uint8_t direction;

    bool scl_state;
    bool scl_enabled;
    bool sda_out_state;
    bool sda_out_enabled;

    bool ack_level;
    bool ack_valid;
} FtdiI2c;

void ftdi_i2c_setup(FtdiI2c* i2c);
void ftdi_i2c_enable(FtdiI2c* i2c);
void ftdi_i2c_disable(FtdiI2c* i2c);
void ftdi_i2c_set_lines(FtdiI2c* i2c, uint8_t value, uint8_t direction);
void ftdi_i2c_set_divisor(FtdiI2c* i2c, uint16_t divisor);
bool ftdi_i2c_write(FtdiI2c* i2c, const uint8_t* data, size_t size);
bool ftdi_i2c_read(FtdiI2c* i2c, uint8_t* data, size_t size);
