#include "ui.h"
#include <lvgl.h>
#include <stdio.h>

#define SPEED_WIDTH 80
#define SPEED_HEIGHT 80
#define TEMP_WIDTH 40
#define TEMP_HEIGHT 40
#define BATTERY_WIDTH 40
#define BATTERY_HEIGHT 40
#define MILE_WIDTH 60
#define MILE_HEIGHT 60

// main speed
lv_draw_label_dsc_t speed_label_dsc;
static lv_color_t speed_cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(SPEED_WIDTH, SPEED_HEIGHT)];
static lv_color_t speed_cbuf_tmp[SPEED_WIDTH * SPEED_HEIGHT];
lv_obj_t *speed_canvas;

// tempature
lv_draw_label_dsc_t temp_label_dsc;
static lv_color_t tempature_cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(TEMP_WIDTH, TEMP_HEIGHT)];
static lv_color_t tempature_cbuf_tmp[LV_CANVAS_BUF_SIZE_TRUE_COLOR(TEMP_WIDTH, TEMP_HEIGHT)];
lv_obj_t *tempature_canvas;

// battery
lv_draw_label_dsc_t battery_label_dsc;
lv_obj_t *battery_canvas;
static lv_color_t battery_cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(BATTERY_WIDTH, BATTERY_HEIGHT)];
static lv_color_t battery_cbuf_tmp[LV_CANVAS_BUF_SIZE_TRUE_COLOR(BATTERY_WIDTH, BATTERY_HEIGHT)];

// mileage
lv_draw_label_dsc_t mileage_label_dsc;
lv_obj_t *mileage_canvas;
static lv_color_t mileage_cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(MILE_WIDTH, MILE_HEIGHT)];
static lv_color_t mileage_cbuf_tmp[LV_CANVAS_BUF_SIZE_TRUE_COLOR(MILE_WIDTH, MILE_HEIGHT)];

void updateSpeed(int speed)
{
    char data[10];
    sprintf(data, "%d", speed);
    lv_canvas_fill_bg(speed_canvas, lv_color_black(), LV_OPA_COVER);
    lv_canvas_draw_text(speed_canvas, 15, 20, 100, &speed_label_dsc, data);

    memcpy(speed_cbuf_tmp, speed_cbuf, sizeof(speed_cbuf_tmp));
    lv_img_dsc_t img;
    img.data = (const uint8_t *)speed_cbuf_tmp;
    img.header.cf = LV_IMG_CF_TRUE_COLOR;
    img.header.w = SPEED_WIDTH;
    img.header.h = SPEED_HEIGHT;

    lv_canvas_fill_bg(speed_canvas, lv_color_black(), LV_OPA_COVER);
    lv_canvas_transform(speed_canvas, &img, 900, 256, 0, 0, SPEED_WIDTH / 2, SPEED_HEIGHT / 2, true);
}

void lv_draw_main_speed_by_canvas()
{
    lv_draw_label_dsc_init(&speed_label_dsc);
    speed_label_dsc.color = lv_palette_main(LV_PALETTE_LIGHT_GREEN);
    speed_label_dsc.font = &lv_font_montserrat_48;

    speed_canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(speed_canvas, speed_cbuf, SPEED_WIDTH, SPEED_HEIGHT, LV_IMG_CF_TRUE_COLOR);

    lv_obj_align_to(speed_canvas, ui_mainSpeed, LV_ALIGN_CENTER, 0, 0);

    updateSpeed(25);
}

void updateTempature(int tempature)
{
    char data[10];
    sprintf(data, "%d°C", tempature);
    lv_canvas_fill_bg(tempature_canvas, lv_color_black(), LV_OPA_COVER);
    lv_canvas_draw_text(tempature_canvas, 0, 10, 100, &temp_label_dsc, data);
    memcpy(tempature_cbuf_tmp, tempature_cbuf, sizeof(tempature_cbuf_tmp));
    lv_img_dsc_t img;
    img.data = (const uint8_t *)tempature_cbuf_tmp;
    img.header.cf = LV_IMG_CF_TRUE_COLOR;
    img.header.w = TEMP_WIDTH;
    img.header.h = TEMP_HEIGHT;
    lv_canvas_fill_bg(tempature_canvas, lv_color_black(), LV_OPA_COVER);
    lv_canvas_transform(tempature_canvas, &img, 900, 256, 0, 0, TEMP_WIDTH / 2, TEMP_HEIGHT / 2, true);
}

void lv_draw_tempature_by_canvas()
{
    lv_draw_label_dsc_init(&temp_label_dsc);
    temp_label_dsc.color = lv_color_white();
    temp_label_dsc.font = &lv_font_montserrat_16;

    tempature_canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(tempature_canvas, tempature_cbuf, TEMP_WIDTH, TEMP_HEIGHT, LV_IMG_CF_TRUE_COLOR);

    lv_obj_align_to(tempature_canvas, ui_tempature, LV_ALIGN_CENTER, 0, 0);

    updateTempature(30);
}

void updateBattery(int battery)
{
    char data[10];
    sprintf(data, "%d%%", battery);
    lv_canvas_fill_bg(battery_canvas, lv_color_black(), LV_OPA_COVER);
    lv_canvas_draw_text(battery_canvas, 0, 10, 100, &temp_label_dsc, data);
    memcpy(battery_cbuf_tmp, battery_cbuf, sizeof(battery_cbuf_tmp));
    lv_img_dsc_t img;
    img.data = (const uint8_t *)battery_cbuf_tmp;
    img.header.cf = LV_IMG_CF_TRUE_COLOR;
    img.header.w = BATTERY_WIDTH;
    img.header.h = BATTERY_HEIGHT;

    lv_canvas_fill_bg(battery_canvas, lv_color_black(), LV_OPA_COVER);
    lv_canvas_transform(battery_canvas, &img, 900, 256, 0, 0, BATTERY_WIDTH / 2, BATTERY_HEIGHT / 2, true);
}

void lv_draw_battery_by_canvas()
{
    lv_draw_label_dsc_init(&battery_label_dsc);
    battery_label_dsc.color = lv_color_white();
    battery_label_dsc.font = &lv_font_montserrat_16;

    battery_canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(battery_canvas, battery_cbuf, BATTERY_WIDTH, BATTERY_HEIGHT, LV_IMG_CF_TRUE_COLOR);

    lv_obj_align_to(battery_canvas, ui_batteryLevel, LV_ALIGN_CENTER, 0, 0);

    updateBattery(100);
}

void updateMileage(int distance)
{
    char data[10];
    sprintf(data, "%dkm", distance);
    lv_canvas_fill_bg(mileage_canvas, lv_color_black(), LV_OPA_COVER);
    lv_canvas_draw_text(mileage_canvas, 0, 10, 100, &mileage_label_dsc, data);
    memcpy(mileage_cbuf_tmp, mileage_cbuf, sizeof(mileage_cbuf_tmp));
    lv_img_dsc_t img;
    img.data = (const uint8_t *)mileage_cbuf_tmp;
    img.header.cf = LV_IMG_CF_TRUE_COLOR;
    img.header.w = MILE_WIDTH;
    img.header.h = MILE_HEIGHT;

    lv_canvas_fill_bg(mileage_canvas, lv_color_black(), LV_OPA_COVER);
    lv_canvas_transform(mileage_canvas, &img, 900, 256, 0, 0, MILE_WIDTH / 2, MILE_HEIGHT / 2, true);
}

void lv_draw_mileage_by_canvas()
{
    lv_draw_label_dsc_init(&mileage_label_dsc);
    mileage_label_dsc.color = lv_color_white();
    mileage_label_dsc.font = &lv_font_montserrat_16;

    mileage_canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(mileage_canvas, mileage_cbuf, MILE_WIDTH, MILE_HEIGHT, LV_IMG_CF_TRUE_COLOR);

    lv_obj_align_to(mileage_canvas, ui_mileAge, LV_ALIGN_CENTER, 0, 0);

    updateMileage(100);
}

void initLabelsByCanvas()
{
    lv_draw_main_speed_by_canvas();
    lv_draw_tempature_by_canvas();
    lv_draw_battery_by_canvas();
    lv_draw_mileage_by_canvas();
}