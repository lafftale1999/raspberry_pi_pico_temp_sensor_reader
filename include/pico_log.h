#ifndef PICO_W_LOGG_H_
#define PICO_W_LOGG_H_

#include "stdio.h"

#define PICO_DEBUG_MODE 1

#if PICO_DEBUG_MODE
#define PICO_LOGI(...) (printf(__VA_ARGS__))
#else
#define PICO_LOGI(...)
#endif

#endif