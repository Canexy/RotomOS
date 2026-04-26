#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_GC9A01  _panel_instance; // Nota: Para AMOLED real cambiaremos el driver luego, pero mantenemos la estructura
    lgfx::Bus_SPI       _bus_instance; 
    lgfx::Touch_CST816S _touch_instance;

public:
    LGFX(void) {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host = SPI2_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 80000000; 
            cfg.pin_sclk = 6;  
            cfg.pin_mosi = 7;
            cfg.pin_miso = -1;
            cfg.pin_dc   = 2;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs           = 10;
            cfg.pin_rst          = -1;
            cfg.pin_busy         = -1;
            cfg.panel_width      = 466; // <--- Actualizado a 466
            cfg.panel_height     = 466; // <--- Actualizado a 466
            cfg.offset_x         = 0;
            cfg.offset_y         = 0;
            _panel_instance.config(cfg);
        }
        {
            auto cfg = _touch_instance.config();
            cfg.pin_int  = 5;   
            cfg.pin_sda  = 4;   
            cfg.pin_scl  = 1;   // <--- Cambiado de 5 a 1 para evitar conflicto con pin_int
            cfg.i2c_port = 1;
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }
        setPanel(&_panel_instance);
    }
};