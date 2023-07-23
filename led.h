
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif
#define PIN 13         // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 192  // Popular NeoPixel ring size
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setled(int R, int G, int B);

void setupled() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();  // Set all pixel colors to 'off'



  setled(0, 0, 22.9); //blue
  delay(1000);
  setled(0, 22.9, 0); //green
  delay(1000);
  setled(25.5, 25.5, 1); //red and green low intensity
  delay(1000);
  setled(25.5, 12.7, 0); // red and green moderate intensity
  delay(1000);
  setled(22.9, 0, 0); //red
  delay(1000);
  setled(15.3, 1, 7.8); // red and blue low intensity
  delay(1000);
  setled(12.7, 2, 3.6); //red and blue very low intensity
}

void setled(int R, int G, int B) {
  pixels.clear();
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(R, G, B));
  }
  pixels.show();
}