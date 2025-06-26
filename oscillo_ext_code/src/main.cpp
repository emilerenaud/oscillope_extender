#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_DotStar.h>

Adafruit_NeoPixel strip(8,38, NEO_GRB + NEO_KHZ800);
Adafruit_DotStar strip1(8, 34, 33, DOTSTAR_BRG);

void setup() {
 Serial.begin(115200);

 strip.begin();           
 strip.show();            
 strip.setBrightness(60);

 strip1.begin();           
 strip1.show();            
 strip1.setBrightness(60); 
}

void loop() {
  static unsigned long last_time = 0;
  if(last_time - millis() > 1000)
  {
    Serial.println("Test" + millis());
    last_time = millis(); 
  }

  strip.clear(); // Set all pixel colors to 'off'
  strip1.clear(); // Set all pixel colors to 'off'
  strip.show();
  strip1.show();
  delay(200);
  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for(int i=0; i<8; i+=2) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    int r = random(0,255);
    int g = random(0,255);
    int b = random(0,255);
    strip.setPixelColor(i, r, g, b);
    strip.setPixelColor(i+1, r, g, b);
    strip1.setPixelColor(i, r, g, b);
    strip1.setPixelColor(i+1, r, g, b);
    
    strip.show();   // Send the updated pixel colors to the hardware.
    strip1.show();   // Send the updated pixel colors to the hardware.

    delay(200); // Pause before next pass through loop
  }
  delay(500);
}
