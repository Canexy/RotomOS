#pragma once
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// ── PINS VERIFICADOS del esquemático oficial Waveshare 1.75C ─────────────────
// Fuente: https://files.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.75/
//         + ESPHome device registry (gpio confirmados)
#define PIN_QSPI_CLK  38
#define PIN_QSPI_D0    4
#define PIN_QSPI_D1    5
#define PIN_QSPI_D2    6
#define PIN_QSPI_D3    7
#define PIN_LCD_CS    10
// RST gestionado por AXP2101 (PMIC), no por GPIO directo → -1
#define PIN_LCD_RST   -1

#define PIN_I2C_SDA   15
#define PIN_I2C_SCL   14
#define PIN_TP_INT    41   // Touch interrupt

// I2C addresses de los chips del 1.75C
#define ADDR_TOUCH    0x1A  // CST9217
#define ADDR_RTC      0x51  // PCF85063
#define ADDR_IMU      0x6B  // QMI8658
#define ADDR_PMIC     0x34  // AXP2101

class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_CO5300  _panel_instance;
    lgfx::Bus_QSPI      _bus_instance;
    lgfx::Touch_CST816S _touch_instance;  // CST9217 es compatible con CST816S

public:
    LGFX(void) {
        // ── Bus QSPI ───────────────────────────────────────────────────────
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host   = SPI2_HOST;
            cfg.spi_mode   = 0;
            cfg.freq_write = 40000000;   // 40 MHz (máx. estable del CO5300)
            cfg.pin_sclk   = PIN_QSPI_CLK;
            cfg.pin_mosi   = PIN_QSPI_D0;
            cfg.pin_miso   = PIN_QSPI_D1;
            cfg.pin_dc     = PIN_QSPI_D2;
            cfg.pin_d3     = PIN_QSPI_D3;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        // ── Panel CO5300 ───────────────────────────────────────────────────
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs           = PIN_LCD_CS;
            cfg.pin_rst          = PIN_LCD_RST;
            cfg.pin_busy         = -1;
            cfg.panel_width      = 466;
            cfg.panel_height     = 466;
            cfg.offset_x         = 0;
            cfg.offset_y         = 0;
            cfg.offset_rotation  = 0;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits  = 1;
            cfg.readable         = false;
            cfg.invert           = false;
            cfg.rgb_order        = false;
            cfg.dlen_16bit       = false;
            cfg.bus_shared       = false;
            _panel_instance.config(cfg);
        }
        // ── Touch CST9217 (I2C) ────────────────────────────────────────────
        {
            auto cfg = _touch_instance.config();
            cfg.pin_int  = PIN_TP_INT;
            cfg.pin_sda  = PIN_I2C_SDA;
            cfg.pin_scl  = PIN_I2C_SCL;
            cfg.i2c_port = 0;
            cfg.i2c_addr = ADDR_TOUCH;
            cfg.freq     = 400000;
            cfg.x_min    = 0;
            cfg.x_max    = 465;
            cfg.y_min    = 0;
            cfg.y_max    = 465;
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }
        setPanel(&_panel_instance);
    }
};