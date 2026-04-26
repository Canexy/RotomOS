#ifndef SYSTEM_DATA_H
#define SYSTEM_DATA_H

#include <stdint.h>

struct SystemState {
    int hour = 12;
    int minute = 30;
    int battery = 100;

    char pkm_name[12] = "Cosmog"; // Pokémon actualizado
    int level = 1; 
    uint32_t exp = 0;
    
    int hunger = 100;
    int happiness = 100;

    uint32_t steps = 0;
    uint32_t step_goal = 5000;
};

extern SystemState myStatus;

#endif