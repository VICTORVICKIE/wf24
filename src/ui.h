#ifndef UI_H
#define UI_H

#include "vendor/lvgl/lvgl.h"

extern const int32_t CLOCK_SIZE;

// Function declarations
void ui_init(void);
lv_display_t *hal_init(int32_t w, int32_t h);

#endif // UI_H
