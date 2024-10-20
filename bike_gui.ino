#include "src/ui/ui.h"
#include <lvgl.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define CANVAS_WIDTH  80
#define CANVAS_HEIGHT  80

static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 320;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];


#define TFT_CS   10 //34 //     10 or 34 (FSPI CS0) 
#define TFT_MOSI 11 //35 //     11 or 35 (FSPI D)
#define TFT_SCLK 12 //36 //     12 or 36 (FSPI CLK)
#define TFT_MISO 13 //37 //     13 or 37 (FSPI Q)
#define TFT_DC    3
#define TFT_RST   8

Adafruit_ST7789 *gfx = new Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


// Display flush function for LVGL
void my_disp_flush2(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t w = (area->x2 - area->x1 + 1);
    int32_t h = (area->y2 - area->y1 + 1);

    // Use draw16bitRGBBitmap() to draw the pixel data
    gfx->startWrite();
    gfx->drawRGBBitmap(area->x1, area->y1, (uint16_t *)color_p, w, h);
    gfx->endWrite();

    lv_disp_flush_ready(disp); // Indicate that flushing is done
}

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);


  gfx->drawRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);


  lv_disp_flush_ready(disp);
}

void setup() {
  // put your setup code here, to run once:
   Serial.begin( 115200 ); /* prepare for possible serial debug */

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println( LVGL_Arduino );
    Serial.println( "I am LVGL_Arduino" );
    
    gfx->init(240, 320);
    gfx->fillScreen(ST77XX_BLACK);
//    gfx->Display_Brightness(255);

  //  gfx->setRotation(3);  // Try different rotations: 0, 1, 2, 3

    // Initialize LVGL
    Serial.println("Initializing LVGL...");

    lv_init();

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

     // Initialize display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    //disp_drv.full_refresh = 1;
    lv_disp_drv_register(&disp_drv);
    Serial.println("Display driver registered.");



    // Initialize the user interface
      ui_init();
lv_draw_main_speed_by_canvas();
lv_draw_tempature_by_canvas();
    
    Serial.println( "Setup done" );

}

void loop() {
  // put your main code here, to run repeatedly:
    lv_timer_handler(); /* let the GUI do its work */
    delay( 5 );
}


void lv_draw_main_speed_by_canvas(void)
{
    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_palette_main(LV_PALETTE_LIGHT_GREEN);
    label_dsc.font = &lv_font_montserrat_48;

    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT)];

    lv_obj_t * canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
 
    lv_obj_align_to(canvas, ui_mainSpeed, LV_ALIGN_CENTER, 0, 0);

    lv_canvas_draw_text(canvas, 15, 20, 100, &label_dsc, "25");

    /*Test the rotation. It requires another buffer where the original image is stored.
     *So copy the current image to buffer and rotate it to the canvas*/
    static lv_color_t cbuf_tmp[CANVAS_WIDTH * CANVAS_HEIGHT];
    memcpy(cbuf_tmp, cbuf, sizeof(cbuf_tmp));
    lv_img_dsc_t img;
    img.data = (const uint8_t *)cbuf_tmp;
    img.header.cf = LV_IMG_CF_TRUE_COLOR;
    img.header.w = CANVAS_WIDTH;
    img.header.h = CANVAS_HEIGHT;

    //lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_NONE, 3), LV_OPA_0);
    lv_canvas_transform(canvas, &img, 900, 256, 0, 0, CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, true);
}

void lv_draw_tempature_by_canvas()
{
    lv_draw_label_dsc_t temp_label_dsc;
    lv_draw_label_dsc_init(&temp_label_dsc);
    temp_label_dsc.color = lv_color_white();
    temp_label_dsc.font = &lv_font_montserrat_12;
    int width = 40;
    int height = 40;

    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(width, height)];

    lv_obj_t * canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(canvas, cbuf, width, height, LV_IMG_CF_TRUE_COLOR);
 
    lv_obj_align_to(canvas, ui_tempature, LV_ALIGN_CENTER, 0, 0);

    lv_canvas_draw_text(canvas, 15, 20, 100, &temp_label_dsc, "30°C");

    /*Test the rotation. It requires another buffer where the original image is stored.
     *So copy the current image to buffer and rotate it to the canvas*/
    static lv_color_t cbuf_tmp[width * height];
    memcpy(cbuf_tmp, cbuf, sizeof(cbuf_tmp));
    lv_img_dsc_t img;
    img.data = (const uint8_t *)cbuf_tmp;
    img.header.cf = LV_IMG_CF_TRUE_COLOR;
    img.header.w = width;
    img.header.h = height;

    lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_NONE, 3), LV_OPA_0);
    lv_canvas_transform(canvas, &img, 900, 256, 0, 0, width / 2, height / 2, true);
}
