#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_DotStar.h>
#include <RotaryEncoder.h>
#include "pi4ioe.h"

#define NUM_ENCODERS 4

Adafruit_NeoPixel strip(8,38, NEO_GRB + NEO_KHZ800);
Adafruit_DotStar strip1(8, 34, 33, DOTSTAR_BGR);

struct Color {
    uint8_t r, g, b;
};
int mappingLED[8] = {0,7,1,6,2,5,3,4};
const Color buttonColors[8] = {
    {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 0},
    {255, 0, 255}, {0, 255, 255}, {255, 165, 0}, {255, 255, 255}
};

PI4IOE::PI4IOE io_expander;
void IRAM_ATTR ISR_button(void);
bool int_received = 0;

const int pinA[NUM_ENCODERS] = {37, 2, 10, 7};
const int pinB[NUM_ENCODERS] = {36, 1, 9, 6};
const int btnPin[NUM_ENCODERS] = {35, 4, 8, 5};

RotaryEncoder encoders[] = {
    RotaryEncoder(pinA[0], pinB[0], RotaryEncoder::LatchMode::FOUR3),
    RotaryEncoder(pinA[1], pinB[1], RotaryEncoder::LatchMode::FOUR3),
    RotaryEncoder(pinA[2], pinB[2], RotaryEncoder::LatchMode::FOUR3),
    RotaryEncoder(pinA[3], pinB[3], RotaryEncoder::LatchMode::FOUR3)
};
bool lastStateA[NUM_ENCODERS] = {0};
uint8_t lastAB[NUM_ENCODERS] = {0};
bool lastButtonState[NUM_ENCODERS] = {1};  // Assuming buttons use INPUT_PULLUP

int encoderPosition[NUM_ENCODERS] = {0};
unsigned long lastDebounceTime[NUM_ENCODERS] = {0};
unsigned long lastButtonDebounceTime[NUM_ENCODERS] = {0};

const unsigned long debounceDelay = 200;    // Debounce time for encoder edges (ms)
const unsigned long buttonDebounceDelay = 50;  // Debounce time for button presses (ms)


void pollButtons(void);
void initColor(void);


void setup() {
 Serial.begin(115200);

//  LED setup
 strip.begin();           
 strip.show();            
 strip.setBrightness(10);

 strip1.begin();           
 strip1.show();            
 strip1.setBrightness(10); 

 delay(500);
 initColor();

  for (int i = 0; i < NUM_ENCODERS; i++) {
    pinMode(pinA[i], INPUT_PULLUP);
    pinMode(pinB[i], INPUT_PULLUP);
    pinMode(btnPin[i], INPUT_PULLUP);
    lastStateA[i] = digitalRead(pinA[i]);
    lastAB[i] = (digitalRead(pinA[i]) << 1) | digitalRead(pinB[i]);
  }

  Wire.begin(11, 12, 100000);
  io_expander.begin(0x43, &Wire);
  io_expander.write_register(PI4IOE::PULL_UP_DOWN_SELECT,B11111111);
  io_expander.write_register(PI4IOE::PULL_UP_DOWN_ENABLE,B11111111);
  io_expander.write_register(PI4IOE::INPUT_DEFAULT_STATE,B11111111);
  io_expander.write_register(PI4IOE::INTERRUPT_MASK,B00000000);
  io_expander.read_register(PI4IOE::INTERRUPT_STATUS);
  pinMode(13,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(13),ISR_button,FALLING);
}

void loop() {
  static unsigned long last_time = 0;
  static int lastPos[NUM_ENCODERS] = {0};
  static unsigned long lastEncoderTime[NUM_ENCODERS] = {0};
  const unsigned long encoderDebounce = 8000;  // 200 ms

  if(millis() - last_time > 1000)
  {
    // Serial.print("Test  ");
    // Serial.println(millis());
    last_time = millis(); 
    // setColor();
  }

  
 for (int i = 0; i < 4; i++) 
 {
        encoders[i].tick();  // Important: mettre à jour l'état de l'encodeur

        static int lastPos[4] = {0};
        int newPos = encoders[i].getPosition();

        if (newPos != lastPos[i]) {
            int delta = newPos - lastPos[i];
            Serial.printf("Encoder %d %+d\n", i, delta);
            lastPos[i] = newPos;
        }
    }
  pollButtons();

  if(int_received)
  {
    int_received = 0;
    uint8_t switch_status = io_expander.read_register(PI4IOE::INTERRUPT_STATUS);
    int buttonIndex = -1;

    for (int i = 0; i < 8; i++) {
        if (switch_status & (1 << i)) {
            buttonIndex = i;
            strip.fill(strip.Color(buttonColors[mappingLED[i]].r,buttonColors[mappingLED[i]].g,buttonColors[mappingLED[i]].b));
            strip.show();
            Serial.printf("Button %d pressed\n", i);
            break;  // On prend le premier bouton actif
        }
    }

  }
}

void initColor()
{
  strip.fill(strip.Color(buttonColors[0].r,buttonColors[0].g,buttonColors[0].b));

  strip1.setPixelColor(0, strip1.Color(buttonColors[0].r, buttonColors[0].g, buttonColors[0].b));
  strip1.setPixelColor(1, strip1.Color(buttonColors[1].r, buttonColors[1].g, buttonColors[1].b));
  strip1.setPixelColor(2, strip1.Color(buttonColors[2].r, buttonColors[2].g, buttonColors[2].b));
  strip1.setPixelColor(3, strip1.Color(buttonColors[3].r, buttonColors[3].g, buttonColors[3].b));
  strip1.setPixelColor(4, strip1.Color(buttonColors[4].r, buttonColors[4].g, buttonColors[4].b));
  strip1.setPixelColor(5, strip1.Color(buttonColors[5].r, buttonColors[5].g, buttonColors[5].b));
  strip1.setPixelColor(6, strip1.Color(buttonColors[6].r, buttonColors[6].g, buttonColors[6].b));
  strip1.setPixelColor(7, strip1.Color(buttonColors[7].r, buttonColors[7].g, buttonColors[7].b));

  strip.show();
  strip1.show();
  
}


void pollButtons() {
    unsigned long now = millis();

    for (int i = 0; i < 4; i++) {
        bool btnState = digitalRead(btnPin[i]);

        if (btnState != lastButtonState[i] && (now - lastButtonDebounceTime[i]) > buttonDebounceDelay) {
            lastButtonDebounceTime[i] = now;
            lastButtonState[i] = btnState;

            if (btnState == LOW) {
                Serial.printf("Button %d pressed\n", i);
            }
        }
    }
}

void ISR_button()
{
  // Serial.println("xd");
  int_received = 1;
  // Serial.println(io_expander.read_register(PI4IOE::INTERRUPT_STATUS));
  // uint8_t switch_status = io_expander.read_register(PI4IOE::INTERRUPT_STATUS);
  // Serial.println(switch_status,BIN);
}


