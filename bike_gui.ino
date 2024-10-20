#include "src/ui/ui.h"
#include <lvgl.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include "src/ui/labelsByCanvas.h"


static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 320;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 4 ];


#define TFT_CS   10 //34 //     10 or 34 (FSPI CS0) 
#define TFT_MOSI 11 //35 //     11 or 35 (FSPI D)
#define TFT_SCLK 12 //36 //     12 or 36 (FSPI CLK)
#define TFT_MISO 13 //37 //     13 or 37 (FSPI Q)
#define TFT_DC    3
#define TFT_RST   8

Adafruit_ST7789 *gfx = new Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


// Display flush function for LVGL
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t w = (area->x2 - area->x1 + 1);
    int32_t h = (area->y2 - area->y1 + 1);

    // Use draw16bitRGBBitmap() to draw the pixel data
    gfx->startWrite();
    gfx->drawRGBBitmap(area->x1, area->y1, (uint16_t *)color_p, w, h);
    gfx->endWrite();

    lv_disp_flush_ready(disp); // Indicate that flushing is done
}

void setup() {
  // put your setup code here, to run once:
   Serial.begin( 115200 ); /* prepare for possible serial debug */

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println( LVGL_Arduino );
    Serial.println( "I am LVGL_Arduino" );
    
    //gfx->init(240, 320);
    gfx->init(240, 320, SPI_MODE3);
    gfx->setSPISpeed(80000000);   // 80 MHz
    gfx->fillScreen(ST77XX_BLACK);

    // Initialize LVGL
    Serial.println("Initializing LVGL...");

    lv_init();

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 4 );

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
      initLabelsByCanvas();
 
    updateSpeedAndNeedle(0);
      
    Serial.println( "Setup done" );

}

void loop() {
  // put your main code here, to run repeatedly:
    lv_timer_handler(); /* let the GUI do its work */
    delay( 50 );
    for (int i = 0; i <= 25; i ++) {
      delay(20);
      lv_timer_handler();
      updateSpeedAndNeedle(i);
    }
    for (int i = 25; i >= 0; i --) {
      delay(20);
      lv_timer_handler();
      updateSpeedAndNeedle(i);
    }

}
