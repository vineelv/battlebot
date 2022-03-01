
#include<FastLED.h>
#include<Servo.h>

#define NUM_LEDS 2
#define DATA_PIN 9

#define M1 5
#define M2 6

#define CHANNELS 8
#define X 4
#define Y 6
#define THR 5

Servo m1, m2;

int ch_pins[CHANNELS] = {10, 16, 14, 15, A0, A1, A2, A3};
int ch_vals[CHANNELS] = {0, 0, 0, 0, 0, 0, 0, 0};

CRGB leds[NUM_LEDS];

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  

  m1.attach(M1);
  m2.attach(M2);
   
  for(int i = 0; i < CHANNELS; i++){
    pinMode(ch_pins[i], INPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  
  ch_vals[X] = pulseIn(ch_pins[X], HIGH);
  ch_vals[Y] = pulseIn(ch_pins[Y], HIGH);
  ch_vals[THR] = pulseIn(ch_pins[THR], HIGH);

  m1.writeMicroseconds(ch_vals[X]);
  m2.writeMicroseconds(ch_vals[Y]);
  
  int mappedX = map(ch_vals[X], 950, 1950, 0, 255);
  int mappedY = map(ch_vals[Y], 1000, 1980, 0, 255); 
  int mappedThr = map(ch_vals[THR], 980, 1960, 0, 255); 

  leds[0] = CRGB(mappedX, mappedY, mappedThr);
  leds[1] = CRGB(mappedThr, mappedThr, mappedThr);

  FastLED.show();
  
  /*char buff[255];

  sprintf(buff, "CH %s: %d\t", "X", ch_vals[X]);
  Serial.print(buff);

  sprintf(buff, "CH %s: %d\t", "Y", ch_vals[Y]);
  Serial.print(buff);

  sprintf(buff, "CH %s: %d\t", "THR", ch_vals[THR]);
  Serial.print(buff);

  Serial.println("");*/
}
