#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif

#include "ui.h"
#include "vendor/lvgl/lvgl.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("Starting LVGL watchface application...\n");
    lv_init();
    lv_display_t *disp = hal_init(466, 466);

    printf("LVGL initialized, display created\n");
    ui_init();

    while (1) {
        lv_timer_handler();
    }
    return 0;
}
