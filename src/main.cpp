#include "hal.h"
#include "ui.h"
#include "system_data.h"

#ifndef ESP32
    #define millis SDL_GetTicks
#endif

SystemState myStatus; 
uint32_t last_sim_time = 0;

void setup() {
    hal_setup();
    ui_init();
}

void loop() {
    hal_loop();
    
    uint32_t now = millis();
    if (now - last_sim_time > 1000) {
        // 1. Actualizamos Pasos
        myStatus.steps += 1; 

        // 2. Actualizamos la Hora Real
        int h, m;
        hal_get_time(&h, &m); // Pedimos hora al HAL
        char time_buf[10];
        snprintf(time_buf, sizeof(time_buf), "%02d:%02d", h, m);
        ui_set_time(time_buf); // Se la enviamos a la UI

        last_sim_time = now;
    }

    lv_timer_handler(); 
    SDL_Delay(5);
}

#ifndef ESP32
int main(int argc, char** argv) { setup(); while(1) loop(); return 0; }
#endif