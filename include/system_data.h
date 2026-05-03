#pragma once
#include <stdint.h>

// Pokémon seleccionable: estructura de datos por especie
struct PkmSpecies {
    uint16_t id;
    char     name[16];
    uint16_t evo_level;      // nivel al que evoluciona (0 = no evoluciona)
    uint16_t evo_target_id;  // ID de la evolución (0 = ninguna)
};

// Estado global del sistema
struct SystemState {
    // Reloj
    int hour   = 12;
    int minute = 0;
    int second = 0;

    // Batería (0-100%)
    int battery = 100;

    // Pokémon compañero
    uint16_t pkm_id      = 789;       // Cosmog por defecto
    char     pkm_name[16]= "Cosmog";
    int      level       = 1;
    uint32_t exp         = 0;

    // Estadísticas vPet
    int hunger    = 100;
    int happiness = 100;

    // Fitness
    uint32_t steps      = 0;
    uint32_t step_goal  = 5000;

    // Tiempo interno (para lógica diaria)
    uint32_t last_step_reset_day = 0; // día del mes del último reset

    // Flags de estado
    bool first_boot = true;
};

extern SystemState myStatus;