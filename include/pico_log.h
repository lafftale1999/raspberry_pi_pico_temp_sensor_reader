#ifndef PICO_W_LOGG_H_
#define PICO_W_LOGG_H_

#include "stdio.h"

#define PICO_DEBUG_MODE 1

#if PICO_DEBUG_MODE
#define PICO_LOGI(...) (printf(__VA_ARGS__))
#else
#define PICO_LOGI(...)
#endif

#define PICO_WARNING_MODE 1

#if PICO_WARNING_MODE
#define PICO_LOGW(...) (printf("Warning: %s", __VA_ARGS__))
#else
#define PICO_LOGW(...)
#endif

#define PICO_ERROR_MODE 1

#if PICO_ERROR_MODE
#define PICO_LOGE(...) (printf("ERROR: %s", __VA_ARGS__))
#else
#define PICO_LOGE(...)
#endif

#endif