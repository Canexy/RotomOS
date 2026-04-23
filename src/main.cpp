#include "hal.h"
#include "ui.h"
#include "system_data.h"

// --- TRADUCTOR DE TIEMPO PARA PC ---
#ifndef ESP32
    #define millis SDL_GetTicks
#endif

// AQUÍ vive la variable de verdad (la embajada)
SystemState myStatus; 

uint32_t last_sim_time = 0;

void setup() {
    hal_setup();
    ui_init();
}

void loop() {
    hal_loop();
    
    // Solo actualizamos la lógica si ha pasado tiempo
    if (millis() - last_sim_time > 1000) {
        myStatus.steps += 1; // Un paso por segundo para no volvernos locos
        last_sim_time = millis();
    }

    lv_timer_handler(); // <--- Esta función es la que "pinta"
    SDL_Delay(5);       // <--- Pausa necesaria para que el procesador respire
}

#ifndef ESP32
int main(int argc, char** argv) { setup(); while(1) loop(); return 0; }
#endif