#ifndef SYSTEM_DATA_H
#define SYSTEM_DATA_H

#include <stdint.h>

struct SystemState {
    int hour = 12;
    int minute = 30;
    uint32_t steps = 0;
    uint32_t step_goal = 5000;
    int pkm_id = 25;
    int happiness = 100;
    int battery = 85;
};

extern SystemState myStatus; // Avisamos que myStatus vive en algún .cpp

#endif