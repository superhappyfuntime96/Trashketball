#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

#define detect 2

#define FADESPEED 5     // make this higher to slow down

#define MSG_MAX 3
int msg_num = 0;

char msg0[] = "GREAT JOB!";
char msg1[] = "THANK YOU!";
char msg2[] = "WOO   HOO!";

char* msgs[MSG_MAX] = { msg0, msg1, msg2 };

//////////////////OLED//////////////////////
uint8_t w = 128;
uint8_t h = 64;
TwoWire* twi = &Wire;
int8_t rst_pin = -1;

Adafruit_SSD1306 m_display(w, h, twi, rst_pin);

static const uint16_t white = SSD1306_WHITE;
static const uint16_t black = SSD1306_BLACK;

uint8_t switchvcc = SSD1306_SWITCHCAPVCC;
uint8_t i2caddr = 0x3C;
////////////////////////////////////////

/*
Code structure:
0. check if reset has been pressed
  a. if so, then reset the trash collected count
1. check for change in distance sensor
2. if change, then a trash has been thrown!
3. change OLED display to congratulate!
4. update amt of trash collected count
5. change LED display to new collected count
6. if larger than high score, update high score
*/

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN 6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 60

// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 50  // Set BRIGHTNESS to about 1/5 (max = 255)

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

void setup() {

  pinMode(detect, INPUT);

  //First, make sure you call the begin() function
  //if screen doesn't work from the start its likely because you forgot display.begin(), or because you are using too much memory

  beginScreen();

  Serial.begin(9600);

  delay(1000);

  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();   // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);
}

void loop() {

  // check for signal from IR
  if (digitalRead(detect) == HIGH) { 
    Serial.println("help");
    //clearScreen();
    drawLargeText(msgs[msg_num]);

    if (msg_num == MSG_MAX - 1) {
      msg_num = 0;
    } else {
      msg_num++;
    }

    rainbowFade(2, 2);
    
    //keep in mind that whenever you draw text, it automatically erases whatever text was there beforehand
    flashScreen();
    delay(500);

    //if we want to clear and erase the entire screen, we can use clearScreen()
    clearScreen();
    delay(1000);
    
  }
}

void beginScreen() {
  m_display.begin(switchvcc, i2caddr);
  m_display.clearDisplay();
  m_display.setTextColor(white);
  m_display.setTextSize(2);
}

void drawSmallText(String text) {
  m_display.fillRect(0, 0, 66, 64, black);
  m_display.setTextSize(1);
  m_display.setCursor(0, 0);
  if (text.length() > 88) {
    m_display.print(F("Too many\ncharacters!"));
    m_display.display();
    m_display.setTextSize(2);
    return;
  }
  for (int i = 0; i < text.length(); i++) {
    m_display.write(text.charAt(i));
    if ((i + 1) % 11 == 0) {
      m_display.write('\n');
    }
  }
  m_display.display();
  m_display.setTextSize(2);
}

void drawLargeText(String text) {
  m_display.fillRect(0, 0, 66, 64, black);
  m_display.setTextSize(4);
  m_display.setCursor(0, 0);
  if (text.length() > 40) {
    m_display.print(F("Too\nmany\nchars\n!!!!!"));
    m_display.display();
    m_display.setTextSize(2);
    return;
  }
  for (int i = 0; i < text.length(); i++) {
    m_display.write(text.charAt(i));
    if ((i + 1) % 5 == 0) {
      m_display.write('\n');
    }
  }
  m_display.display();
}

void flashScreen() {
  bool toggle = false;
  for (byte i = 0; i < 6; i++) {
    toggle = !toggle;
    m_display.invertDisplay(toggle);
    delay(150);
  }
}

void clearScreen() {
  m_display.clearDisplay();
  m_display.display();
}

void rainbowFade(int wait, int rainbowLoops) {
  int fadeVal = 0, fadeMax = 100;

  // Hue of first pixel runs 'rainbowLoops' complete loops through the color
  // wheel. Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to rainbowLoops*65536, using steps of 256 so we
  // advance around the wheel at a decent clip.
  for (uint32_t firstPixelHue = 0; firstPixelHue < rainbowLoops * 65536;
       firstPixelHue += 256) {

    for (int i = 0; i < strip.numPixels(); i++) {  // For each pixel in strip...

      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      uint32_t pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());

      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the three-argument variant, though the
      // second value (saturation) is a constant 255.
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue, 255,
                                                          255 * fadeVal / fadeMax)));
    }

    strip.show();
    delay(wait);

    if (firstPixelHue < 65536) {                                 // First loop,
      if (fadeVal < fadeMax) fadeVal++;                          // fade in
    } else if (firstPixelHue >= ((rainbowLoops - 1) * 65536)) {  // Last loop,
      if (fadeVal > 0) fadeVal--;                                // fade out
    } else {
      fadeVal = fadeMax;  // Interim loop, make sure fade is at max
    }
  }
}