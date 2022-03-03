
#include<FastLED.h>
#include<Servo.h>

#define NUM_LEDS 2
#define DATA_PIN 9

#define ML 5
#define MR 6

#define CHANNELS 8
#define TIME_AVERAGE 3
#define X 4
#define Y 6
#define THR 5
#define READY 3
#define READY_ENABLE 1000
#define READY_DISABLE 1900

#define ZERO_CALIBRATION_DELAY 1000

Servo mL, mR;

int ch_pins[CHANNELS] = {10, 16, 14, 15, A0, A1, A2, A3};
int* ch_vals[CHANNELS];

int x_mid, y_mid, thr_low;

CRGB leds[NUM_LEDS];

int channelAverage(int channel){

  int sum = 0;
  
  for(int i = 0; i < TIME_AVERAGE; i++){
    sum += ch_vals[channel][i];
  }

  sum /= TIME_AVERAGE;

  return sum;
}

void attachESC(){
  mL.attach(ML);
  mR.attach(MR);

  leds[0] = CRGB::Red;
  leds[1] = CRGB::Yellow;

  FastLED.show();
}

void waitReady(){
  //Ensure that the driver is ready to begin the automatic calibration
  int enable_begin = millis();
  
  do{
    ch_vals[READY][0] = pulseIn(ch_pins[READY], HIGH);

    if(((millis() - enable_begin) % 100) < 50){
      leds[1] = CRGB::Black;
    } else {
      leds[1] = CRGB::Red;
    }

    FastLED.show();
  } while(ch_vals[READY][0] > READY_ENABLE);

}

void calibrate(){
  //Begin the calibration of the robot.

  leds[1] = CRGB::Red;
  
  int zero_pos_calc = millis();

  long x_sum = 0, y_sum = 0, thr_sum = 0;
  int pulses = 0;
  
  while(millis() - zero_pos_calc < ZERO_CALIBRATION_DELAY){
    x_sum += pulseIn(ch_pins[X], HIGH);
    y_sum += pulseIn(ch_pins[Y], HIGH);
    thr_sum += pulseIn(ch_pins[THR], HIGH);

    //Serial.println(y_sum);
    
    pulses++;

    if(((millis() - zero_pos_calc) % 100) < 50){
      leds[0] = CRGB::Black;
    } else {
      leds[0] = CRGB::Yellow;
    }

    FastLED.show();
  }
  
  x_mid = x_sum/pulses;
  y_mid = y_sum/pulses;
  thr_low = thr_sum/pulses;
}

void setup() {
  // put your setup code here, to run once:

  //Enable FastLED
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  
  
  leds[0] = CRGB::Red;
  leds[1] = CRGB::Red;

  FastLED.show();
  delay(250);

  // Attach ESCs;
  attachESC();
  delay(250);

  // Allot memory for the time average of the channel values
  for(int i = 0; i < CHANNELS; i++){
    pinMode(ch_pins[i], INPUT);
    ch_vals[i] = (int*) malloc(sizeof(int) * TIME_AVERAGE);
  }

  waitReady();
  delay(500);
  
  calibrate();

  delay(50);

  leds[0] = CRGB::Green;
  leds[1] = CRGB::Green;

  FastLED.show();

  delay(250);
  
}

void loop() {
  // put your main code here, to run repeatedly:

  for(int i = 1; i < TIME_AVERAGE; i++){
    
    ch_vals[X][i - 1] = ch_vals[X][i];
    ch_vals[Y][i - 1] = ch_vals[Y][i];
    ch_vals[THR][i - 1] = ch_vals[THR][i];
  }

  ch_vals[X][TIME_AVERAGE - 1] = pulseIn(ch_pins[X], HIGH);
  ch_vals[Y][TIME_AVERAGE - 1] = pulseIn(ch_pins[Y], HIGH);
  ch_vals[THR][TIME_AVERAGE - 1] = pulseIn(ch_pins[THR], HIGH);

  int thr = channelAverage(THR), x = channelAverage(X), y = channelAverage(Y);
  
  mL.writeMicroseconds(y);
  mR.writeMicroseconds(y);
  
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
