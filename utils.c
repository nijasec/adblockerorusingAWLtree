//
// Created by anijas on 26/12/2019.
//

#include "utils.h"


long get_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000LL + ts.tv_nsec / 1e6;
}

