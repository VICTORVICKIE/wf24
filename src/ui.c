#include "ui.h"
#include "vendor/lvgl/lvgl.h"
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

LV_IMAGE_DECLARE(saturn_v);
LV_IMAGE_DECLARE(saturn_v_colored);

// Global variables
static lv_obj_t *scr; // Screen background - now static, managed internally

lv_color_t palette_black = LV_COLOR_MAKE(0, 0, 0);
lv_color_t palette_dark_gray = LV_COLOR_MAKE(24, 24, 24);
lv_color_t palette_gold = LV_COLOR_MAKE(255, 215, 0);
lv_color_t palette_cosmic_latte = LV_COLOR_MAKE(255, 248, 231);

#ifdef ENABLE_MANUAL_CONTROLS
// Manual time control variables
static lv_obj_t *time_textarea;
static lv_obj_t *range_textarea;
static lv_obj_t *set_time_btn;
static lv_obj_t *set_range_btn;
static lv_obj_t *manual_time_label;
static lv_obj_t *manual_range_label;
static bool manual_time_mode = false;
static int manual_hour = 0;
static int manual_minute = 0;
static int manual_range = 0;
#endif
static lv_group_t *input_group = NULL;

// Clock variables
static lv_obj_t *scale;
static lv_obj_t *display_scale;  // New display scale with interval 10
static lv_obj_t *actual_scale;   // New actual scale with interval 1
static lv_obj_t *saturn_needle;
static int32_t hour;
static int32_t minute;

// UI constants
const int32_t DIMENSION = 466;
const int32_t HOUR_BEGIN = 0;
const int32_t HOUR_END = 23;
const int32_t MINUTE_BEGIN = 0;
const int32_t MINUTE_END = 59;
const int32_t ANGLE_BEGIN = 0;
const int32_t ANGLE_END = 359;
const int32_t TOTAL_HOURS = HOUR_END - HOUR_BEGIN + 1;
const int32_t TOTAL_ANGLE = ANGLE_END - ANGLE_BEGIN + 1;
const int32_t TOTAL_MINUTES = MINUTE_END - MINUTE_BEGIN + 1;
const int32_t INTERVAL = 1;
const int32_t DISPLAY_INTERVAL = 10;  // New display interval
const int32_t MINUTES_IN_HOUR_PER_INTERVAL = TOTAL_MINUTES / INTERVAL;
const int32_t MINUTES_IN_HOUR_PER_DISPLAY_INTERVAL = TOTAL_MINUTES / DISPLAY_INTERVAL;  // New display interval calculation
const int32_t TOTAL_TICKS = TOTAL_HOURS * MINUTES_IN_HOUR_PER_INTERVAL;
const int32_t TOTAL_DISPLAY_TICKS = TOTAL_HOURS * MINUTES_IN_HOUR_PER_DISPLAY_INTERVAL;  // New display ticks
const float ANGLE_PER_TICK = (float)TOTAL_ANGLE / (float)TOTAL_TICKS;
const float ANGLE_PER_DISPLAY_TICK = (float)TOTAL_ANGLE / (float)TOTAL_DISPLAY_TICKS;  // New display angle per tick
const int32_t TOTAL_TICKS_PLUS_1 = TOTAL_TICKS + 1;
const int32_t TOTAL_DISPLAY_TICKS_PLUS_1 = TOTAL_DISPLAY_TICKS + 1;  // New display ticks plus 1
const int32_t SCALE_ROTATION = -90;
const int32_t TICK_OFFSET = -(int32_t)(SCALE_ROTATION / ANGLE_PER_TICK);
const int32_t DISPLAY_TICK_OFFSET = -(int32_t)(SCALE_ROTATION / ANGLE_PER_DISPLAY_TICK);  // New display tick offset

// Forward declarations for callback functions
#ifdef ENABLE_MANUAL_CONTROLS
static void set_time_btn_cb(lv_event_t *e);
static void set_range_btn_cb(lv_event_t *e);
static void reset_time_btn_cb(lv_event_t *e);
static void textarea_event_cb(lv_event_t *e);
#endif
static void timer_cb(lv_timer_t *timer);

// Forward declarations for UI setup functions
static void setup_gradient_background(void);
static void make_clock_dial(void);
static void make_clock_grooves(void);
#ifdef ENABLE_MANUAL_CONTROLS
static void create_manual_time_controls(void);
#endif
static void print_ui_constants(void);

lv_display_t *hal_init(int32_t w, int32_t h)
{
    lv_display_t *disp = lv_sdl_window_create(w, h);
    lv_indev_t *mouse = lv_sdl_mouse_create();
    lv_indev_t *mousewheel = lv_sdl_mousewheel_create();
    lv_indev_t *kb = lv_sdl_keyboard_create();

    // Enable anti-aliasing for smoother rendering
    lv_display_set_antialiasing(disp, true);

#ifdef ENABLE_MANUAL_CONTROLS
    // Set up keyboard input group
    input_group = lv_group_create();
    lv_indev_set_group(kb, input_group);
#endif

    return disp;
}

void ui_init(void)
{
    // Print UI constants for debugging
    print_ui_constants();

    // Debug: Check if ENABLE_MANUAL_CONTROLS is defined
#ifdef ENABLE_MANUAL_CONTROLS
    printf("DEBUG: ENABLE_MANUAL_CONTROLS is DEFINED\n");
#else
    printf("DEBUG: ENABLE_MANUAL_CONTROLS is NOT DEFINED\n");
#endif

    // Initialize the screen
    scr = lv_scr_act();

    // Set up the UI components
    setup_gradient_background();
    make_clock_dial();
    // make_clock_grooves();
#ifdef ENABLE_MANUAL_CONTROLS
    printf("DEBUG: About to create manual controls\n");
    create_manual_time_controls();
    printf("DEBUG: Manual controls created\n");
#endif
}

#ifdef ENABLE_MANUAL_CONTROLS
static void create_manual_time_controls(void)
{
    // Create a container for the controls
    lv_obj_t *controls_container = lv_obj_create(scr);
    lv_obj_set_size(controls_container, 200, 160);
    lv_obj_align(controls_container, LV_ALIGN_TOP_RIGHT, -10, 10);

    // Style the container
    lv_obj_set_style_bg_color(controls_container, palette_dark_gray, 0);
    lv_obj_set_style_bg_opa(controls_container, LV_OPA_80, 0);
    lv_obj_set_style_radius(controls_container, 10, 0);
    lv_obj_set_style_border_width(controls_container, 1, 0);
    lv_obj_set_style_border_color(controls_container, palette_gold, 0);
    lv_obj_set_style_pad_all(controls_container, 10, 0);

    // Create label for manual time display
    manual_time_label = lv_label_create(controls_container);
    lv_label_set_text(manual_time_label, "Manual: 00:00");
    lv_obj_align(manual_time_label, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_color(manual_time_label, palette_cosmic_latte, 0);
    lv_obj_set_style_text_font(manual_time_label, LV_FONT_DEFAULT, 0);

    // Create text area for time input
    time_textarea = lv_textarea_create(controls_container);
    lv_obj_set_size(time_textarea, 80, 25);
    lv_obj_align(time_textarea, LV_ALIGN_TOP_MID, -30, 25);
    lv_textarea_set_placeholder_text(time_textarea, "HH:MM");
    lv_textarea_set_one_line(time_textarea, true);
    lv_obj_add_event_cb(time_textarea, textarea_event_cb,
                        LV_EVENT_VALUE_CHANGED, NULL);

    // Add textarea to keyboard input group and set focus
    if (input_group) {
        lv_group_add_obj(input_group, time_textarea);
        lv_group_focus_obj(time_textarea);
    }

    // Style the text area
    lv_obj_set_style_bg_color(time_textarea, palette_black, 0);
    lv_obj_set_style_bg_opa(time_textarea, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(time_textarea, palette_cosmic_latte, 0);
    lv_obj_set_style_border_width(time_textarea, 1, 0);
    lv_obj_set_style_border_color(time_textarea, palette_gold, 0);
    lv_obj_set_style_radius(time_textarea, 5, 0);
    lv_obj_set_style_pad_all(time_textarea, 5, 0);

    // Create set time button
    set_time_btn = lv_btn_create(controls_container);
    lv_obj_set_size(set_time_btn, 60, 25);
    lv_obj_align(set_time_btn, LV_ALIGN_TOP_MID, 30, 25);
    lv_obj_add_event_cb(set_time_btn, set_time_btn_cb, LV_EVENT_CLICKED, NULL);

    // Add button to input group
    if (input_group) {
        lv_group_add_obj(input_group, set_time_btn);
    }

    // Style the button
    lv_obj_set_style_bg_color(set_time_btn, palette_gold, 0);
    lv_obj_set_style_bg_opa(set_time_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(set_time_btn, 5, 0);

    // Create button label
    lv_obj_t *set_btn_label = lv_label_create(set_time_btn);
    lv_label_set_text(set_btn_label, "Set");
    lv_obj_center(set_btn_label);
    lv_obj_set_style_text_color(set_btn_label, palette_black, 0);

    // Create label for range display
    manual_range_label = lv_label_create(controls_container);
    lv_label_set_text(manual_range_label, "Range: --");
    lv_obj_align(manual_range_label, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_set_style_text_color(manual_range_label, palette_cosmic_latte, 0);
    lv_obj_set_style_text_font(manual_range_label, LV_FONT_DEFAULT, 0);

    // Create text area for range input
    range_textarea = lv_textarea_create(controls_container);
    lv_obj_set_size(range_textarea, 80, 25);
    lv_obj_align(range_textarea, LV_ALIGN_TOP_MID, -30, 80);
    lv_textarea_set_placeholder_text(range_textarea, "0-144");
    lv_textarea_set_one_line(range_textarea, true);
    lv_obj_add_event_cb(range_textarea, textarea_event_cb,
                        LV_EVENT_VALUE_CHANGED, NULL);

    // Add range textarea to keyboard input group
    if (input_group) {
        lv_group_add_obj(input_group, range_textarea);
    }

    // Style the range text area
    lv_obj_set_style_bg_color(range_textarea, palette_black, 0);
    lv_obj_set_style_bg_opa(range_textarea, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(range_textarea, palette_cosmic_latte, 0);
    lv_obj_set_style_border_width(range_textarea, 1, 0);
    lv_obj_set_style_border_color(range_textarea, palette_gold, 0);
    lv_obj_set_style_radius(range_textarea, 5, 0);
    lv_obj_set_style_pad_all(range_textarea, 5, 0);

    // Create set range button
    set_range_btn = lv_btn_create(controls_container);
    lv_obj_set_size(set_range_btn, 60, 25);
    lv_obj_align(set_range_btn, LV_ALIGN_TOP_MID, 30, 80);
    lv_obj_add_event_cb(set_range_btn, set_range_btn_cb, LV_EVENT_CLICKED,
                        NULL);

    // Add range button to input group
    if (input_group) {
        lv_group_add_obj(input_group, set_range_btn);
    }

    // Style the range button
    lv_obj_set_style_bg_color(set_range_btn, palette_gold, 0);
    lv_obj_set_style_bg_opa(set_range_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(set_range_btn, 5, 0);

    // Create range button label
    lv_obj_t *set_range_btn_label = lv_label_create(set_range_btn);
    lv_label_set_text(set_range_btn_label, "Set");
    lv_obj_center(set_range_btn_label);
    lv_obj_set_style_text_color(set_range_btn_label, palette_black, 0);

    // Create reset button
    lv_obj_t *reset_btn = lv_btn_create(controls_container);
    lv_obj_set_size(reset_btn, 80, 25);
    lv_obj_align(reset_btn, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_add_event_cb(reset_btn, reset_time_btn_cb, LV_EVENT_CLICKED, NULL);

    // Style the reset button
    lv_obj_set_style_bg_color(reset_btn, palette_dark_gray, 0);
    lv_obj_set_style_bg_opa(reset_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(reset_btn, 1, 0);
    lv_obj_set_style_border_color(reset_btn, palette_cosmic_latte, 0);
    lv_obj_set_style_radius(reset_btn, 5, 0);

    // Create reset button label
    lv_obj_t *reset_btn_label = lv_label_create(reset_btn);
    lv_label_set_text(reset_btn_label, "Reset");
    lv_obj_center(reset_btn_label);
    lv_obj_set_style_text_color(reset_btn_label, palette_cosmic_latte, 0);
}
#endif

static void timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);

#ifdef ENABLE_MANUAL_CONTROLS
    /* Use manual time only */
    hour = manual_hour;
    minute = manual_minute;
#else
    /* Get current system time */
    time_t now;
    struct tm *timeinfo;

    time(&now);
    timeinfo = localtime(&now);

    hour = timeinfo->tm_hour;
    minute = timeinfo->tm_min;
#endif

    int32_t tick_position =
        (hour * MINUTES_IN_HOUR_PER_INTERVAL) + (minute / INTERVAL);
    printf("Time: %02d:%02d, Pos: %d (should point to hour %d)\n", hour, minute,
           tick_position, hour);
    lv_scale_set_image_needle_value(
        actual_scale, saturn_needle, (tick_position + TICK_OFFSET) % TOTAL_TICKS);
}

#ifdef ENABLE_MANUAL_CONTROLS
static void set_time_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);

    const char *time_str = lv_textarea_get_text(time_textarea);

    // Parse HH:MM format
    int h, m;
    if (sscanf(time_str, "%d:%d", &h, &m) == 2) {
        // Validate time values
        if (h >= HOUR_BEGIN && h <= HOUR_END && m >= MINUTE_BEGIN &&
            m <= MINUTE_END) {
            manual_hour = h;
            manual_minute = m;

            // Update the clock display
            hour = manual_hour;
            minute = manual_minute;

            int32_t tick_position =
                (hour * MINUTES_IN_HOUR_PER_INTERVAL) + (minute / INTERVAL);
            lv_scale_set_image_needle_value(actual_scale, saturn_needle,
                                            (tick_position + TICK_OFFSET) %
                                                TOTAL_TICKS);

            printf("Manual time set to: %02d:%02d\n", manual_hour,
                   manual_minute);

            // Update label
            lv_label_set_text_fmt(manual_time_label, "Manual: %02d:%02d",
                                  manual_hour, manual_minute);
        }
        else {
            printf("Invalid time format. Use HH:MM (00:00 to 23:59)\n");
        }
    }
    else {
        printf("Invalid time format. Use HH:MM\n");
    }
}
#endif

#ifdef ENABLE_MANUAL_CONTROLS
static void set_range_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);

    const char *range_str = lv_textarea_get_text(range_textarea);

    // Parse range value
    int range_val;
    if (sscanf(range_str, "%d", &range_val) == 1) {
        // Validate range values
        manual_range = range_val;

        // Update the clock display directly with range value
        lv_scale_set_image_needle_value(
            actual_scale, saturn_needle, (range_val + TICK_OFFSET) % TOTAL_TICKS);

        printf("Manual range set to: %d\n", manual_range);

        // Update label
        lv_label_set_text_fmt(manual_range_label, "Range: %d", manual_range);
    }
    else {
        printf("Invalid range format. Use numbers\n");
    }
}
#endif

#ifdef ENABLE_MANUAL_CONTROLS
static void reset_time_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);

    lv_textarea_set_text(time_textarea, "");
    lv_textarea_set_text(range_textarea, "");
    lv_label_set_text(manual_time_label, "Manual: 00:00");
    lv_label_set_text(manual_range_label, "Range: --");

    // Reset to 00:00
    hour = 0;
    minute = 0;
    manual_hour = 0;
    manual_minute = 0;

    int32_t tick_position =
        (hour * MINUTES_IN_HOUR_PER_INTERVAL) + (minute / INTERVAL);
    lv_scale_set_image_needle_value(
        actual_scale, saturn_needle, (tick_position + TICK_OFFSET) % TOTAL_TICKS);

    printf("Reset to 00:00\n");
}
#endif

#ifdef ENABLE_MANUAL_CONTROLS
static void textarea_event_cb(lv_event_t *e)
{
    // Empty callback - removed auto-formatting and validation
    LV_UNUSED(e);
}
#endif

static void make_clock_dial(void)
{
    // Create the actual scale (interval 1) - this will be the functional scale
    actual_scale = lv_scale_create(lv_screen_active());
    lv_obj_set_size(actual_scale, 460, 460);
    lv_scale_set_mode(actual_scale, LV_SCALE_MODE_ROUND_INNER);
    lv_obj_set_style_bg_opa(actual_scale, LV_OPA_0, 0);
    lv_obj_set_style_radius(actual_scale, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(actual_scale, true, 0);
    lv_obj_center(actual_scale);

    // Hide labels for actual scale since we only want the display scale to show them
    lv_scale_set_label_show(actual_scale, false);

    lv_scale_set_total_tick_count(actual_scale, TOTAL_TICKS_PLUS_1);
    lv_scale_set_major_tick_every(actual_scale, MINUTES_IN_HOUR_PER_INTERVAL);

    // Style the actual scale (invisible ticks for functionality)
    static lv_style_t actual_indicator_style;
    lv_style_init(&actual_indicator_style);
    lv_style_set_line_color(&actual_indicator_style, lv_color_black());
    lv_style_set_length(&actual_indicator_style, 0);    /* invisible ticks */
    lv_style_set_line_width(&actual_indicator_style, 0); /* invisible ticks */
    lv_obj_add_style(actual_scale, &actual_indicator_style, LV_PART_INDICATOR);

    static lv_style_t actual_minor_ticks_style;
    lv_style_init(&actual_minor_ticks_style);
    lv_style_set_line_color(&actual_minor_ticks_style, lv_color_black());
    lv_style_set_length(&actual_minor_ticks_style, 0);     /* invisible ticks */
    lv_style_set_line_width(&actual_minor_ticks_style, 0); /* invisible ticks */
    lv_obj_add_style(actual_scale, &actual_minor_ticks_style, LV_PART_ITEMS);

    static lv_style_t actual_main_line_style;
    lv_style_init(&actual_main_line_style);
    lv_style_set_arc_color(&actual_main_line_style, lv_color_black());
    lv_style_set_arc_width(&actual_main_line_style, 0); /* invisible main line */
    lv_obj_add_style(actual_scale, &actual_main_line_style, LV_PART_MAIN);

    lv_scale_set_range(actual_scale, 0, TOTAL_TICKS);
    lv_scale_set_angle_range(actual_scale, TOTAL_ANGLE);
    lv_scale_set_rotation(actual_scale, SCALE_ROTATION);

    // Create the display scale (interval 10) - this will be the visible scale
    display_scale = lv_scale_create(lv_screen_active());
    lv_obj_set_size(display_scale, 460, 460);
    lv_scale_set_mode(display_scale, LV_SCALE_MODE_ROUND_INNER);
    lv_obj_set_style_bg_opa(display_scale, LV_OPA_0, 0);
    lv_obj_set_style_radius(display_scale, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(display_scale, true, 0);
    lv_obj_center(display_scale);

    lv_scale_set_label_show(display_scale, true);

    lv_scale_set_total_tick_count(display_scale, TOTAL_DISPLAY_TICKS_PLUS_1);
    lv_scale_set_major_tick_every(display_scale, MINUTES_IN_HOUR_PER_DISPLAY_INTERVAL);

    static const char *hour_ticks[] = {"0",  "1",  "2",  "3",  "4",  "5",  "6",
                                       "7",  "8",  "9",  "10", "11", "12", "13",
                                       "14", "15", "16", "17", "18", "19", "20",
                                       "21", "22", "23", NULL};

    lv_scale_set_text_src(display_scale, hour_ticks);

    static lv_style_t indicator_style;
    lv_style_init(&indicator_style);

    /* Label style properties */
    lv_style_set_text_font(&indicator_style, LV_FONT_DEFAULT);
    lv_style_set_text_color(&indicator_style, palette_cosmic_latte);

    /* Major tick properties */
    lv_style_set_line_color(&indicator_style, palette_cosmic_latte);
    lv_style_set_length(&indicator_style, 10);    /* tick length */
    lv_style_set_line_width(&indicator_style, 2); /* tick width */
    lv_obj_add_style(display_scale, &indicator_style, LV_PART_INDICATOR);

    /* Minor tick properties */
    static lv_style_t minor_ticks_style;
    lv_style_init(&minor_ticks_style);
    lv_style_set_line_color(&minor_ticks_style, palette_cosmic_latte);
    lv_style_set_length(&minor_ticks_style, 5);     /* tick length */
    lv_style_set_line_width(&minor_ticks_style, 1); /* tick width */
    lv_obj_add_style(display_scale, &minor_ticks_style, LV_PART_ITEMS);

    /* Main line properties */
    static lv_style_t main_line_style;
    lv_style_init(&main_line_style);
    lv_style_set_arc_color(&main_line_style, lv_color_black());
    lv_style_set_arc_width(&main_line_style, 5);
    lv_obj_add_style(display_scale, &main_line_style, LV_PART_MAIN);

    lv_scale_set_range(display_scale, 0, TOTAL_DISPLAY_TICKS);
    lv_scale_set_angle_range(display_scale, TOTAL_ANGLE);
    lv_scale_set_rotation(display_scale, SCALE_ROTATION);

    /* Create Saturn needle image on the actual scale */
    saturn_needle = lv_image_create(actual_scale);
    lv_image_set_src(saturn_needle, &saturn_v_colored);
    int32_t saturn_needle_h = lv_image_get_src_height(saturn_needle);
    int32_t saturn_needle_w = lv_image_get_src_width(saturn_needle);

    /* Calculate pivot point - you can adjust these ratios as needed */
    int32_t pivot_x = (int32_t)((float)saturn_needle_w * 0.5f);
    int32_t pivot_y = (int32_t)((float)saturn_needle_h * 0.8f);
    /* Calculate alignment offset to keep image centered on screen */
    /* Offset = (pivot - center) to compensate for pivot position */
    int32_t align_offset_x = (saturn_needle_w / 2) - pivot_x;
    int32_t align_offset_y = (saturn_needle_h / 2) - pivot_y;

    printf("Image: w=%d, h=%d\n", saturn_needle_w, saturn_needle_h);
    printf("Pivot: x=%d, y=%d\n", pivot_x, pivot_y);
    printf("Align offset: x=%d, y=%d\n", align_offset_x, align_offset_y);

    lv_obj_align(saturn_needle, LV_ALIGN_CENTER, align_offset_x,
                 align_offset_y);
    lv_image_set_pivot(saturn_needle, pivot_x, pivot_y);

    /* Create center axis/cap */
    lv_obj_t *center_cap = lv_obj_create(actual_scale);
    lv_obj_set_size(center_cap, 20, 20);
    lv_obj_align(center_cap, LV_ALIGN_CENTER, 0, 0);

    /* Style the center cap */
    lv_obj_set_style_radius(center_cap, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(center_cap, palette_cosmic_latte, 0);
    lv_obj_set_style_bg_opa(center_cap, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(center_cap, 2, 0);
    lv_obj_set_style_border_color(center_cap, palette_gold, 0);
    lv_obj_set_style_border_opa(center_cap, LV_OPA_COVER, 0);

    /* Remove any default padding/margin */
    lv_obj_set_style_pad_all(center_cap, 0, 0);
    lv_obj_set_style_margin_all(center_cap, 0, 0);

#ifdef ENABLE_MANUAL_CONTROLS
    /* Initialize with 00:00 for manual mode */
    hour = 0;
    minute = 0;
    manual_hour = 0;
    manual_minute = 0;
    manual_time_mode = true;
#else
    /* Initialize with current system time */
    time_t now;
    struct tm *timeinfo;
    time(&now);
    timeinfo = localtime(&now);
    hour = timeinfo->tm_hour;
    minute = timeinfo->tm_min;
#endif

    lv_timer_t *timer =
        lv_timer_create(timer_cb, 10 * 1000, NULL); // 10 seconds
    lv_timer_ready(timer);
}

static void setup_gradient_background(void)
{
    static const lv_color_t grad_colors[4] = {
        LV_COLOR_MAKE(0x0B, 0x0C, 0x1A), // Deep indigo-blue center
        LV_COLOR_MAKE(0x0E, 0x10, 0x33), // Dark navy blue
        LV_COLOR_MAKE(0x08, 0x0D, 0x1A), // Almost black with blue undertone
        LV_COLOR_MAKE(0x00, 0x00, 0x00)  // Pure black
    };

    static lv_style_t style;
    lv_style_init(&style);

    /* Define the gradient */
    static lv_grad_dsc_t grad;
    lv_grad_init_stops(&grad, grad_colors, NULL, NULL,
                       sizeof(grad_colors) / sizeof(lv_color_t));

    /* Radial gradient from center to furthest corner */
    lv_grad_radial_init(&grad, LV_GRAD_CENTER,
                        LV_GRAD_CENTER, // Center
                        LV_GRAD_RIGHT,
                        LV_GRAD_BOTTOM,      // Outer edge
                        LV_GRAD_EXTEND_PAD); // Pad outwards if needed

    lv_style_set_bg_grad(&style, &grad);
    lv_obj_add_style(scr, &style, 0);
}

static void print_ui_constants(void)
{
    printf("\n=== UI CONSTANTS ===\n");
    printf("DIMENSION: %d\n", DIMENSION);
    printf("TOTAL_HOURS: %d\n", TOTAL_HOURS);
    printf("TOTAL_ANGLE: %d\n", TOTAL_ANGLE);
    printf("TOTAL_MINUTES: %d\n", TOTAL_MINUTES);
    printf("INTERVAL: %d\n", INTERVAL);
    printf("DISPLAY_INTERVAL: %d\n", DISPLAY_INTERVAL);
    printf("TOTAL_TICKS: %d\n", TOTAL_TICKS);
    printf("TOTAL_DISPLAY_TICKS: %d\n", TOTAL_DISPLAY_TICKS);
    printf("MINUTES_IN_HOUR_PER_INTERVAL: %d\n", MINUTES_IN_HOUR_PER_INTERVAL);
    printf("MINUTES_IN_HOUR_PER_DISPLAY_INTERVAL: %d\n", MINUTES_IN_HOUR_PER_DISPLAY_INTERVAL);
    printf("ANGLE_PER_TICK: %.2f\n", ANGLE_PER_TICK);
    printf("ANGLE_PER_DISPLAY_TICK: %.2f\n", ANGLE_PER_DISPLAY_TICK);
    printf("TOTAL_TICKS_PLUS_1: %d\n", TOTAL_TICKS_PLUS_1);
    printf("TOTAL_DISPLAY_TICKS_PLUS_1: %d\n", TOTAL_DISPLAY_TICKS_PLUS_1);
    printf("SCALE_ROTATION: %d\n", SCALE_ROTATION);
    printf("TICK_OFFSET: %d\n", TICK_OFFSET);
    printf("DISPLAY_TICK_OFFSET: %d\n", DISPLAY_TICK_OFFSET);
    printf("==================\n\n");
}
