
#include<FastLED.h>
#include<Servo.h>

#define NUM_LEDS 2
#define DATA_PIN 9

#define MF 5
#define MB 6

#define CHANNELS 8
#define TIME_AVERAGE 3
#define X 4
#define Y 6
#define THR 5

Servo mf, mb;

int ch_pins[CHANNELS] = {10, 16, 14, 15, A0, A1, A2, A3};
int* ch_vals[CHANNELS];

CRGB leds[NUM_LEDS];

int index = 0;

int channelAverage(int channel){

  int sum = 0;
  
  for(int i = 0; i < TIME_AVERAGE; i++){
    sum += ch_vals[channel][i];
  }

  sum /= TIME_AVERAGE;

  return sum;
}

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  

  mf.attach(MF);
  mb.attach(MB);
   
  for(int i = 0; i < CHANNELS; i++){
    pinMode(ch_pins[i], INPUT);
    ch_vals[i] = (int*) malloc(sizeof(int) * TIME_AVERAGE);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  if(index < TIME_AVERAGE){
    ch_vals[X][index] = pulseIn(ch_pins[X], HIGH);
    ch_vals[Y][index] = pulseIn(ch_pins[Y], HIGH);
    ch_vals[THR][index++] = pulseIn(ch_pins[THR], HIGH);
  } else {
    for(int i = 1; i < TIME_AVERAGE; i++){
      
      ch_vals[X][i - 1] = ch_vals[X][i];
      ch_vals[Y][i - 1] = ch_vals[Y][i];
      ch_vals[THR][i - 1] = ch_vals[THR][i];
    }

    ch_vals[X][TIME_AVERAGE - 1] = pulseIn(ch_pins[X], HIGH);
    ch_vals[Y][TIME_AVERAGE - 1] = pulseIn(ch_pins[Y], HIGH);
    ch_vals[THR][TIME_AVERAGE - 1] = pulseIn(ch_pins[THR], HIGH);
  }
 
  mf.writeMicroseconds(channelAverage(THR));
  mb.writeMicroseconds(channelAverage(Y));
  
  int mappedX = map(channelAverage(X), 900, 2000, 0, 255);
  int mappedY = map(channelAverage(Y), 900, 2000, 0, 255); 
  int mappedThr = map(channelAverage(THR), 900, 2000, 0, 255); 

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
