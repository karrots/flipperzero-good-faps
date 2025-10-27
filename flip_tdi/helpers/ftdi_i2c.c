#include "ftdi_i2c.h"

#include <stddef.h>

#include <furi_hal.h>
#include <furi_hal_resources.h>

#define FTDI_I2C_BASE_CLOCK_HZ 6000000UL
#define FTDI_I2C_MIN_HALF_PERIOD_US 1UL

static void ftdi_i2c_apply_scl(FtdiI2c* i2c, bool enable, bool level) {
    if(enable) {
        furi_hal_gpio_init(i2c->scl_pin, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedVeryHigh);
        furi_hal_gpio_write(i2c->scl_pin, level);
    } else {
        furi_hal_gpio_init(i2c->scl_pin, GpioModeInput, GpioPullNo, GpioSpeedVeryHigh);
    }
    i2c->scl_enabled = enable;
    i2c->scl_state = level;
}

static void ftdi_i2c_apply_sda(FtdiI2c* i2c, bool enable, bool level) {
    if(enable) {
        furi_hal_gpio_init(i2c->sda_out_pin, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedVeryHigh);
        furi_hal_gpio_write(i2c->sda_out_pin, level);
    } else {
        furi_hal_gpio_init(i2c->sda_out_pin, GpioModeInput, GpioPullNo, GpioSpeedVeryHigh);
    }
    i2c->sda_out_enabled = enable;
    i2c->sda_out_state = level;
    i2c->ack_valid = enable;
    i2c->ack_level = level;
}

static inline void ftdi_i2c_release_sda(FtdiI2c* i2c) {
    furi_hal_gpio_init(i2c->sda_out_pin, GpioModeInput, GpioPullNo, GpioSpeedVeryHigh);
}

static inline void ftdi_i2c_restore_sda(FtdiI2c* i2c) {
    ftdi_i2c_apply_sda(i2c, i2c->sda_out_enabled, i2c->sda_out_state);
}

static inline void ftdi_i2c_drive_scl(FtdiI2c* i2c, bool level) {
    if(!i2c->scl_enabled) {
        return;
    }
    furi_hal_gpio_write(i2c->scl_pin, level);
    i2c->scl_state = level;
}

static inline void ftdi_i2c_drive_sda(FtdiI2c* i2c, bool level) {
    if(!i2c->sda_out_enabled) {
        return;
    }
    furi_hal_gpio_write(i2c->sda_out_pin, level);
    i2c->sda_out_state = level;
}

static inline void ftdi_i2c_half_period_delay(const FtdiI2c* i2c) {
    if(i2c->half_period_us < FTDI_I2C_MIN_HALF_PERIOD_US) {
        return;
    }
    furi_delay_us(i2c->half_period_us);
}

void ftdi_i2c_setup(FtdiI2c* i2c) {
    i2c->scl_pin = &gpio_ext_pa7;
    i2c->sda_out_pin = &gpio_ext_pa6;
    i2c->sda_in_pin = &gpio_ext_pa4;

    i2c->half_period_us = 5;
    i2c->value = 0;
    i2c->direction = 0;

    i2c->scl_state = true;
    i2c->scl_enabled = false;
    i2c->sda_out_state = true;
    i2c->sda_out_enabled = false;
    i2c->ack_level = true;
    i2c->ack_valid = false;

    furi_hal_gpio_init(i2c->scl_pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(i2c->sda_out_pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(i2c->sda_in_pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

void ftdi_i2c_enable(FtdiI2c* i2c) {
    ftdi_i2c_apply_scl(i2c, true, true);
    ftdi_i2c_apply_sda(i2c, true, true);
    furi_hal_gpio_init(i2c->sda_in_pin, GpioModeInput, GpioPullNo, GpioSpeedVeryHigh);
}

void ftdi_i2c_disable(FtdiI2c* i2c) {
    furi_hal_gpio_init(i2c->scl_pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(i2c->sda_out_pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(i2c->sda_in_pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    i2c->scl_enabled = false;
    i2c->sda_out_enabled = false;
    i2c->ack_valid = false;
}

void ftdi_i2c_set_lines(FtdiI2c* i2c, uint8_t value, uint8_t direction) {
    i2c->value = value;
    i2c->direction = direction;

    bool scl_enable = direction & 0x01;
    bool sda_enable = direction & 0x02;

    bool scl_level = value & 0x01;
    bool sda_level = value & 0x02;

    ftdi_i2c_apply_scl(i2c, scl_enable, scl_level);
    ftdi_i2c_apply_sda(i2c, sda_enable, sda_level);
}

void ftdi_i2c_set_divisor(FtdiI2c* i2c, uint16_t divisor) {
    uint64_t ticks = (uint64_t)(divisor + 1);
    uint64_t half_period = (ticks * 1000000ULL) / (FTDI_I2C_BASE_CLOCK_HZ * 2ULL);
    if(half_period < FTDI_I2C_MIN_HALF_PERIOD_US) {
        half_period = FTDI_I2C_MIN_HALF_PERIOD_US;
    }
    i2c->half_period_us = (uint32_t)half_period;
}

static bool ftdi_i2c_read_ack(FtdiI2c* i2c) {
    ftdi_i2c_release_sda(i2c);
    ftdi_i2c_half_period_delay(i2c);
    ftdi_i2c_drive_scl(i2c, true);
    ftdi_i2c_half_period_delay(i2c);
    bool ack = !furi_hal_gpio_read(i2c->sda_in_pin);
    ftdi_i2c_drive_scl(i2c, false);
    ftdi_i2c_half_period_delay(i2c);
    ftdi_i2c_restore_sda(i2c);
    return ack;
}

bool ftdi_i2c_write(FtdiI2c* i2c, const uint8_t* data, size_t size) {
    if(!i2c->scl_enabled) {
        return false;
    }

    for(size_t index = 0; index < size; index++) {
        uint8_t value = data[index];
        for(uint8_t mask = 0x80; mask != 0; mask >>= 1) {
            bool bit_level = (value & mask) != 0;
            ftdi_i2c_drive_sda(i2c, bit_level);
            ftdi_i2c_half_period_delay(i2c);
            ftdi_i2c_drive_scl(i2c, true);
            ftdi_i2c_half_period_delay(i2c);
            ftdi_i2c_drive_scl(i2c, false);
        }

        if(!ftdi_i2c_read_ack(i2c)) {
            return false;
        }
    }

    return true;
}

static void ftdi_i2c_send_ack(FtdiI2c* i2c, bool level) {
    bool prev_enabled = i2c->sda_out_enabled;
    bool prev_state = i2c->sda_out_state;
    furi_hal_gpio_init(i2c->sda_out_pin, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_write(i2c->sda_out_pin, level);
    ftdi_i2c_half_period_delay(i2c);
    ftdi_i2c_drive_scl(i2c, true);
    ftdi_i2c_half_period_delay(i2c);
    ftdi_i2c_drive_scl(i2c, false);
    ftdi_i2c_half_period_delay(i2c);
    ftdi_i2c_release_sda(i2c);
    ftdi_i2c_apply_sda(i2c, prev_enabled, prev_state);
}

bool ftdi_i2c_read(FtdiI2c* i2c, uint8_t* data, size_t size) {
    if(!i2c->scl_enabled) {
        return false;
    }

    ftdi_i2c_release_sda(i2c);

    for(size_t index = 0; index < size; index++) {
        uint8_t value = 0;
        for(uint8_t bit = 0; bit < 8; bit++) {
            value <<= 1;
            ftdi_i2c_half_period_delay(i2c);
            ftdi_i2c_drive_scl(i2c, true);
            ftdi_i2c_half_period_delay(i2c);
            if(furi_hal_gpio_read(i2c->sda_in_pin)) {
                value |= 0x01;
            }
            ftdi_i2c_drive_scl(i2c, false);
        }
        data[index] = value;

        bool is_last = (index + 1) == size;
        bool ack_level = is_last ? (i2c->ack_valid ? i2c->ack_level : true) : false;
        ftdi_i2c_send_ack(i2c, ack_level);
    }

    ftdi_i2c_restore_sda(i2c);
    return true;
}
