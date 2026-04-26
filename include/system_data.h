#ifndef SYSTEM_DATA_H
#define SYSTEM_DATA_H

#include <stdint.h>
#include <string>

struct SystemState {
    // --- Reloj y Energía ---
    int hour = 12;
    int minute = 30;
    int battery = 85;

    // --- Datos del Entrenador (Para el Social Hub / Streetpass) ---
    char trainer_name[12] = "Red";
    uint16_t trainer_id = 00001;

    // --- VPet: Pokémon Actual ---
    int pkm_id = 25;            // Pikachu por defecto
    char pkm_name[12] = "Rotom";
    int level = 1;
    uint32_t exp = 0;           // Ganada por pasos o pomodoros
    
    // --- VPet: Stats de Vida ---
    int hunger = 100;           // 100 = Lleno, 0 = Hambriento
    int happiness = 100;        // 100 = Muy feliz
    int energy = 100;           // Baja si no duerme
    bool is_sleeping = false;

    // --- Fitness (Pokéwalker) ---
    uint32_t steps = 0;
    uint32_t step_goal = 5000;
    
    // --- Pomodoro ---
    bool pomodoro_active = false;
    int pomodoro_minutes_left = 25;
};

extern SystemState myStatus;

#endif