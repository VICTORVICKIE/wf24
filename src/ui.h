#ifndef UI_H
#define UI_H

#include "vendor/lvgl/lvgl.h"

// Global variables
extern lv_color_t palette_black;
extern lv_color_t palette_dark_gray;
extern lv_color_t palette_gold;
extern lv_color_t palette_cosmic_latte;

// UI constants
extern const int dimension;
extern const int hours_max;
extern const int clock_arc_width;
extern const int clock_arc_size;
extern const int arc_start;
extern const int arc_end;

// Function declarations
void ui_init(void);
lv_display_t *hal_init(int32_t w, int32_t h);

#endif // UI_H 
