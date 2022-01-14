#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <SPI.h>

// Assume we'll be using the default hardware SPI, so the code doesn't need to know SCK/MOSI pins
// These match the pin assignments on a couple of breadboard projects I have lying around. 
// Adjust to taste.
#if defined(ARDUINO_QTPY_M0) || defined(SEEED_XIAO_M0)
  #define CS_PIN   2
  #define DC_PIN   3
  #define RST_PIN  4
#elif defined(ARDUINO_TINYPICO)
  #define CS_PIN   5
  #define DC_PIN   21
  #define RST_PIN  22
#endif

//// Uncomment this to run the tests at all 4 rotations.
// #define TEST_ROTATIONS

// Turn on one of these, depending on the connected display.

#if 1
///////////////////////////////////////////////////////////
//// 0.96" 128x96 color OLED, SSD1331, with acceleration)
// https://www.amazon.com/gp/product/B08C4M5D23
#include <Adafruit_SSD1331.h>
Adafruit_SSD1331 display(&SPI, CS_PIN, DC_PIN, RST_PIN);
#define DISPLAY_DEPTH 16
void display_begin() {
  display.begin();
}
#elif 0
///////////////////////////////////////////////////////////
//// GMT130 - 1.3" 240x240 color TFT, SD7789, with NO CHIP SELECT for some reason >:( 
// https://www.amazon.com/dp/B07P9X3L7M
#include <Adafruit_ST7789.h>
Adafruit_ST7789 display(&SPI, -1, DC_PIN, RST_PIN);
#define DISPLAY_DEPTH 16
void display_begin() {
  display.init(240, 240, SPI_MODE3);
  display.setRotation(2);
}
#elif 0
///////////////////////////////////////////////////////////
//// GMT144-10 - 1.4" 128x128 color TFT, ST7735
// https://www.alibaba.com/product-detail/1-44-inch-TFT-LCD-display_62410646764.html
#include <Adafruit_ST7735.h>
Adafruit_ST7735 display(CS_PIN, DC_PIN, RST_PIN);
#define DISPLAY_DEPTH 16
void display_begin() {
  display.initR(INITR_144GREENTAB);
}
#elif 0
///////////////////////////////////////////////////////////
//// 1.14" 135x240 color TFT, SD7789)
// https://www.amazon.com/dp/B095SY163C
#include <Adafruit_ST7789.h>
Adafruit_ST7789 display(&SPI, CS_PIN, DC_PIN, RST_PIN);
#define DISPLAY_DEPTH 16
void display_begin() {
  display.init(135, 240);
  display.setRotation(3);
}
#elif 0
///////////////////////////////////////////////////////////
//// 1.3" and 0.96" 128x64 monochrome OLED, SH1106
// https://www.amazon.com/gp/product/B01N1LZT8L
// https://www.amazon.com/gp/product/B01N1LZQA8
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(128, 64, &SPI, DC_PIN, RST_PIN, CS_PIN, 8000000UL, true);
#define DISPLAY_DEPTH 1
#define DISPLAY_NEEDS_FLUSH
void display_begin() {
  display.begin();
}
#elif 0
///////////////////////////////////////////////////////////
//// 3.12" 256x64 4 bit grayscale OLED, SSD1322
// https://www.amazon.com/dp/B09NVJWN7G
#include <Adafruit_SSD1322.h>
Adafruit_SSD1322 display(&SPI, DC_PIN, RST_PIN, CS_PIN);
#define DISPLAY_DEPTH 4
#define DISPLAY_NEEDS_FLUSH
void display_begin() {
  display.begin();
}
#elif 0
///////////////////////////////////////////////////////////
//// 2.08" 256x64 4 bit grayscale OLED, SSH1122
// https://www.alibaba.com/product-detail/2-08-inch-OLED-display-module_62427607341.html
#include <Adafruit_SSD1322.h>
Adafruit_SSD1322 display(&SPI, DC_PIN, RST_PIN, CS_PIN, Adafruit_SSD1322::VARIANT_SSH1122);
#define DISPLAY_DEPTH 4
#define DISPLAY_NEEDS_FLUSH
void display_begin() {
  display.begin();
}
#elif 0
///////////////////////////////////////////////////////////
//// 1.5" 128x128 16 bit color OLED, SSD1351
// https://www.amazon.com/dp/B07DBXMFSN
#include <Adafruit_SSD1351.h>
Adafruit_SSD1351 display(128, 128, &SPI, CS_PIN, DC_PIN, RST_PIN);
#define DISPLAY_DEPTH 16
void display_begin() {
  display.begin();
}
#endif

#if DISPLAY_DEPTH == 16
// Color definitions
// (conveniently, the defs for BLACK and WHITE also work for 1 and 4 bit monochrome displays...)
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define GRAY            0x9492
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define BROWN           0xABC8
#elif DISPLAY_DEPTH == 4
#define	BLACK           0x00
#define WHITE           0x0F
#endif

const uint16_t assorted_colors[] = {
  // BLUE, RED, GREEN, GRAY, CYAN, MAGENTA, YELLOW, WHITE
  // Generated by clicking around in: 
  // http://www.barth-dev.de/online/rgb565-color-picker/
  0x051A, 0x031F, 0x4916, 0x9957,
  0xB96B, 0xFA02, 0xFB40, 0xFD40,
  0xFE20, 0xFFE8, 0xDF46, 0x75C8
};
const int assorted_colors_size = sizeof(assorted_colors) / sizeof(assorted_colors[0]);

void flush() {
#if defined(DISPLAY_NEEDS_FLUSH)
  display.display();
#endif
}
void clearDisplay()
{
#if DISPLAY_DEPTH == 16
  display.fillScreen(BLACK);
#else
  display.clearDisplay();
#endif
}

uint16_t randomcolor(int index)
{
  #if DISPLAY_DEPTH == 16
    return assorted_colors[index % assorted_colors_size];
  #elif DISPLAY_DEPTH == 4
    // for grayscale, cycle through pixel values that are not black.
    return (index % 15) + 1;
  #else
    // for monochrome, it's always white.
    return WHITE;
  #endif
}

#define NUMFLAKES 32
#define XPOS 0
#define YPOS 1
#define DELTAY 2

unsigned long testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  unsigned long result = 0;
  clearDisplay();
  uint8_t icons[NUMFLAKES][3];
  randomSeed(666);     // whatever seed
 
  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random(display.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;
  }

  unsigned long start = millis();

  // Run this for 5 seconds
  while(millis() - start <  5000) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, randomcolor(f));
    }
    flush();
    result += NUMFLAKES;
    // delay(10);
    
    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS],  bitmap, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display.height()) {
      	icons[f][XPOS] = random(display.width());
      	icons[f][YPOS] = 0;
      	icons[f][DELTAY] = random(5) + 1;
      }
    }
   }
  return result;
}

unsigned long testdrawbitmap() {
  const uint8_t height = 16;
  const uint8_t width = 16;
  static const unsigned char PROGMEM logo16[] =
  { B00000000, B11000000,
    B00000001, B11000000,
    B00000001, B11000000,
    B00000011, B11100000,
    B11110011, B11100000,
    B11111110, B11111000,
    B01111110, B11111111,
    B00110011, B10011111,
    B00011111, B11111100,
    B00001101, B01110000,
    B00011011, B10100000,
    B00111111, B11100000,
    B00111111, B11110000,
    B01111100, B11110000,
    B01110000, B01110000,
    B00000000, B00110000 
  };

  return testdrawbitmap(logo16, width, height);
}

void testdrawchar(void) {
  clearDisplay();
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }    
  flush();
  display.setTextWrap(true);
}

void testlines() {
   int color = 0;
   clearDisplay();
   for (int16_t x=0; x < display.width()-1; x+=6) {
     display.drawLine(0, 0, x, display.height()-1, randomcolor(color));
     color++;
     flush();
   }
   for (int16_t y=0; y < display.height()-1; y+=6) {
     display.drawLine(0, 0, display.width()-1, y, randomcolor(color));
     color++;
     flush();
   }
   delay(250);

   clearDisplay();
   for (int16_t x=0; x < display.width()-1; x+=6) {
     display.drawLine(display.width()-1, 0, x, display.height()-1, randomcolor(color));
     color++;
     flush();
   }
   for (int16_t y=0; y < display.height()-1; y+=6) {
     display.drawLine(display.width()-1, 0, 0, y, randomcolor(color));
     color++;
     flush();
   }
   delay(250);

   // To avoid ESP8266 watchdog timer resets when not using the hardware SPI pins
   delay(0);

   clearDisplay();
   for (int16_t x=0; x < display.width()-1; x+=6) {
     display.drawLine(0, display.height()-1, x, 0, randomcolor(color));
     color++;
     flush();
   }
   for (int16_t y=0; y < display.height()-1; y+=6) {
     display.drawLine(0, display.height()-1, display.width()-1, y, randomcolor(color));
     color++;
     flush();
   }
   delay(250);

   clearDisplay();
   for (int16_t x=0; x < display.width()-1; x+=6) {
     display.drawLine(display.width()-1, display.height()-1, x, 0, randomcolor(color));
     color++;
     flush();
   }
   for (int16_t y=0; y < display.height()-1; y+=6) {
     display.drawLine(display.width()-1, display.height()-1, 0, y, randomcolor(color));
     color++;
     flush();
   }
   delay(250);

}

void testdrawtext(char *text, uint16_t color) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
    {
      display.println();
      flush();
    }
  }
}

void testfastlines() {
    int color = 0;
    clearDisplay();
    for (int16_t y=0; y < display.height()-1; y+=5) {
      display.drawFastHLine(0, y, display.width()-1, randomcolor(color));
      color++;
      flush();
    }
    for (int16_t x=0; x < display.width()-1; x+=5) {
      display.drawFastVLine(x, 0, display.height()-1, randomcolor(color));
      color++;
      flush();
    }
}

void testdrawrects() {
  int color = 0;
  clearDisplay();
  int size = ((display.height() > display.width())?display.width():display.height()) - 1;
  for (int16_t x=0; x < size; x+=6) {
    display.drawRect((display.width()-1)/2 -x/2, (display.height()-1)/2 -x/2 , x, x, randomcolor(color));
    color++;
    flush();
  }
}

void testfillrects() {
  int color = 0;
  clearDisplay();
  int size = ((display.height() > display.width())?display.width():display.height()) - 1;
  for (int16_t x=size; x > 6; x-=6) {
    display.fillRect((display.width()-1)/2 -x/2, (display.height()-1)/2 -x/2 , x, x, randomcolor(color));
    display.drawRect((display.width()-1)/2 -x/2, (display.height()-1)/2 -x/2 , x, x, randomcolor(-color));
    color++;
    flush();
  }
}

void testdrawcircle(void) {
  for (uint8_t i=0; i<display.height(); i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, randomcolor(i));
    flush();
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (uint8_t x=radius; x < display.width()-1; x+=radius*2) {
    for (uint8_t y=radius; y < display.height()-1; y+=radius*2) {
      display.fillCircle(x, y, radius, color);
      flush();
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < display.width()-1+radius; x+=radius*2) {
    for (int16_t y=0; y < display.height()-1+radius; y+=radius*2) {
      display.drawCircle(x, y, radius, color);
      flush();
    }
  }
}

void testtriangles() {
  clearDisplay();
  int color = 0;
  int t;
  int w = display.width()/2;
  int x = display.height();
  int y = 0;
  int z = display.width();
  for (t = 0 ; t <= 15; t+=1) {
    display.drawTriangle(w, y, y, x, z, x, randomcolor(color));
    flush();
    x-=4;
    y+=4;
    z-=4;
    color+=1;
  }
}

void testroundrects() {
   clearDisplay();
  int color = 0;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = display.width();
    int h = display.height();
    for(i = 0 ; i <= 8; i+=1) {
      display.drawRoundRect(x, y, w, h, 5, randomcolor(color));
      flush();
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1;
    }
    color+=1;
  }
}

#if DISPLAY_DEPTH == 16
void tftPrintTest() {
  clearDisplay();
  display.setCursor(0, 5);
  display.setTextColor(RED);
  display.setTextSize(1);
  display.println("Hello World!");
  display.setTextColor(YELLOW, GREEN);
  display.setTextSize(2);
  display.print("Hello Wo");
  display.setTextColor(BLUE);
  display.setTextSize(3);
  display.print(1234.567);
  flush();
  delay(1500);
  display.setCursor(0, 5);
  clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(0);
  display.println("Hello World!");
  display.setTextSize(1);
  display.setTextColor(GREEN);
  display.print(float(PI), 5);
  display.println(" Want pi?");
  display.print(8675309, HEX); // print 8,675,309 out in HEX!
  display.print(" Print HEX");
  display.setTextColor(WHITE);
  display.println("Sketch has been");
  display.println("running for: ");
  display.setTextColor(MAGENTA);
  display.print(millis() / 1000);
  display.setTextColor(WHITE);
  display.print(" seconds.");
  flush();
}

/**************************************************************************/
/*!
    @brief  Renders a simple test pattern on the LCD
*/
/**************************************************************************/
void lcdTestPattern(void)
{
  uint8_t w,h;
  display.startWrite();
  int stripe_width = (display.width() / 8);
  for (h = 0; h < display.height() - 1; h++) {
    for (w = 0; w < display.width() - 1; w++) {
      if (w > stripe_width * 7) {
        display.writePixel(w, h, WHITE);
      } else if (w > stripe_width * 6) {
        display.writePixel(w, h, BLUE);
      } else if (w > stripe_width * 5) {
        display.writePixel(w, h, GREEN);
      } else if (w > stripe_width * 4) {
        display.writePixel(w, h, CYAN);
      } else if (w > stripe_width * 3) {
        display.writePixel(w, h, RED);
      } else if (w > stripe_width * 2) {
        display.writePixel(w, h, MAGENTA);
      } else if (w > stripe_width) {
        display.writePixel(w, h, YELLOW);
      } else {
        display.writePixel(w, h, BLACK);
      }
    }
  }
  display.endWrite();
}
#endif

void scroll_off()
{
#ifdef SSD1331_EXTRAS
  int width = display.width();
  int height = display.height();
  int scroll_amount = 1;
  int xscroll = 0;
  int yscroll = 0;
  int count = 0;
  int clearx = 0;
  int cleary = 0;
  int clearw = width;
  int clearh = height;
  static int direction = 0;

  switch(direction)
  {
  case 0:
    // scroll up
    yscroll = -scroll_amount;
    count = height / scroll_amount;
    cleary = (height - 1) - scroll_amount;
    clearh = scroll_amount;
  break;
  case 1:
    // scroll right
    xscroll = scroll_amount;
    count = width / scroll_amount;
    clearw = scroll_amount;
  break;
  case 2:
    // scroll down
    yscroll = scroll_amount;
    count = height / scroll_amount;
    clearh = scroll_amount;
  break;
  case 3:
    // scroll left
    xscroll = -scroll_amount;
    count = width / scroll_amount;
    clearx = (width - 1) - scroll_amount;
    clearw = scroll_amount;
  break;
  }

  for (int i = 0; i < count; i++)
  {
    display.copyBits(0, 0, width, height, xscroll, yscroll);
    display.fillRect(clearx, cleary, clearw, clearh, 0);
  }

  // Increment the scroll direction each time.
  direction += 1;
  direction &= 0x03;
  delay(500);
#endif
}

void setup(void) {
  Serial.begin(9600);
  Serial.print("hello!");

  display_begin();

  Serial.println("init");
}


void all_tests() {
  Serial.printf("Running all tests at rotation %d\r\n", display.getRotation());
  unsigned long time = millis();
  unsigned long count, end;
  clearDisplay();
  Serial.printf("Clear screen took %ldms\r\n", millis() - time);
  delay(500);

#if DISPLAY_DEPTH == 16
  time = millis();
  lcdTestPattern();
  Serial.printf("Test pattern took %ldms\r\n", millis() - time);
  delay(1000);
  scroll_off();
#endif

  // drawchar test
  time = millis();
  testdrawchar();
  Serial.printf("drawchar test took %ldms\r\n", (millis() - time));
  delay(1000);
  scroll_off();

#if DISPLAY_DEPTH == 16
  // scrolling small text
  time = millis();
  clearDisplay();
  display.setCursor(0,0);
#ifdef SSD1331_EXTRAS
  display.setTextScroll(true);
#endif
  for (int i = 0; i < 10; i++) {
    display.print("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa");
  }
  time = millis() - time;
  Serial.printf("Small text scrolling test took %ldms\r\n", time);
  display.setTextColor(GREEN);
  display.printf("\r\n-> %ldms", time);
#ifdef SSD1331_EXTRAS
  display.setTextScroll(false);
#endif
  delay(2000);
  scroll_off();

  // scrolling big text
  time = millis();
  clearDisplay();
  display.setCursor(0,0);
#ifdef SSD1331_EXTRAS
  display.setTextScroll(true);
#endif
  display.setTextSize(2);
  display.setTextColor(BLUE);
  for (int i = 0; i < 10; i++) {
    display.print("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa");
  }
  time = millis() - time;
  Serial.printf("Large text scrolling test took %ldms\r\n", time);
  display.setTextColor(GREEN);
  display.printf("\r\n%ldms", time);
#ifdef SSD1331_EXTRAS
  display.setTextScroll(false);
#endif
  display.setTextSize(1);
  delay(2000);
  scroll_off();

  // tft print function!
  time = millis();
  tftPrintTest();
  // The print test contains a 1500ms delay.
  Serial.printf("Print test took %ldms\r\n", (millis() - time) - 1500);
  delay(2000);
  scroll_off();
#endif
  
  //a single pixel
  // display.drawPixel(display.width()/2, display.height()/2, GREEN);
  // delay(500);
  // scroll_off();

  // line draw test
  time = millis();
  testlines();
  // The lines test contains four 250ms delays.
  Serial.printf("Lines test took %ldms\r\n", (millis() - time) - 1000);
  delay(500);
  scroll_off();

  // optimized lines
  time = millis();
  testfastlines();
  Serial.printf("Optimized Lines test took %ldms\r\n", millis() - time);
  delay(500);
  scroll_off();

  time = millis();
  testdrawrects();
  Serial.printf("Drawrects test took %ldms\r\n", millis() - time);
  delay(500);
  scroll_off();

  time = millis();
  testfillrects();
  Serial.printf("Fillrects test took %ldms\r\n", millis() - time);
  delay(500);
  scroll_off();

  time = millis();
  clearDisplay();
  testdrawcircle();
#if DISPLAY_DEPTH == 16
  testfillcircles(10, BLUE);
  testdrawcircles(10, WHITE);
#endif
  Serial.printf("Circles test took %ldms\r\n", millis() - time);
  delay(500);
  scroll_off();

  time = millis();
  testroundrects();
  Serial.printf("Round rects test took %ldms\r\n", millis() - time);
  delay(500);
  scroll_off();

  time = millis();
  testtriangles();
  Serial.printf("Triangles test took %ldms\r\n", millis() - time);
  delay(500);
  scroll_off();

  time = millis();
  count = testdrawbitmap();
  end = millis();
  Serial.printf("Bitmap test drew %ld bitmaps in %ldms, for %ld bitmaps/sec\r\n", 
    count, end - time,
    long((double(count) / (end - time)) * 1000) );
  delay(500);
  scroll_off();

  // Of the displays here, I think only the SSD1322/SH1122 have settable contrast.
#if DISPLAY_DEPTH == 4
  for (int i = 0; i < 5; i++) {
    display.setContrast(0x00);
    delay(500);
    display.setContrast(0xFF);
    delay(500);
  }
  display.setContrast(0x80);
#endif

  Serial.printf("*** done ***\r\n\r\n");
  delay(1000);
}

void loop(){
  for (int i = 0; i < 4; i++)
  {
#if defined(TEST_ROTATIONS)
    display.setRotation(i);
#endif
    all_tests();
  }
}