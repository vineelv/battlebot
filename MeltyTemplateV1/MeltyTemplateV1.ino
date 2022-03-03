#include <FastLED.h>
#include <Servo.h>
#include <Wire.h>
#include "LIS331.h"


/**
 * Indicator LED Parameters
 */
#define NUM_LEDS 2
#define LED_CONTROL_PIN 9

/**
 * Motor/ESC Parameters
 */
#define ML_PIN 5
#define MR_PIN 6

/**
 * Radio Parameters
 */
#define CHANNELS 8
#define X 4
#define Y 6
#define THR 5
#define READY 3

/**
 * Channel Parameters: Constants regarding specific channels and channel calibration
 */
#define READY_ENABLE 1000
#define READY_DISABLE 1900
#define ZERO_CALIBRATION_DELAY 1000


/*============================================*/

/**
 * Accelerometer
 */
LIS331 hls;


/**
 * LED array for fastLED library
 */
CRGB leds[2];

/**
 * Servo library allows us to write a microsecond value to a pin
 * and drive it using PWM.
 */
Servo mL, mR;

/**
 * Channel Pins and Values
 */
int ch_pins[CHANNELS] = {10, 16, 14, 15, A0, A1, A2, A3};
int ch_vals[CHANNELS];

/**
 * Calibrated zero positions for each driving channel (X, Y, THR)
 */
int x_mid, y_mid, thr_low;

/**
 * Function to light the leds.
 */
void showLED(CRGB led1, CRGB led2){
  leds[0] = led1;
  leds[1] = led2;

  FastLED.show();
}

void setup() {

  //==========Enable FastLED===========
  FastLED.addLeds<WS2812B, LED_CONTROL_PIN, GRB>(leds, NUM_LEDS);
  
  showLED(CRGB::Red, CRGB::RED);
  delay(250);

  //===========Attach ESCs==========
  mL.attach(ML_PIN);
  mR.attach(MR_PIN);

  showLED(CRGB::Red, CRGB::Yellow);
  delay(250);

  //===========Initialize Accelerometer===========
  Wire.begin();
  
  hla.setI2CAddr(0x18);
  hla.setODR(LIS331::DR_400HZ);
  hla.setFullScale(LIS331::HIGH_RANGE) //+- 400 g
  hla.begin(LIS331::USE_I2C);
  
  //===========Initialize Channel Pins============
  for(int i = 0; i < CHANNELS; i++){
    pinMode(ch_pins[i], INPUT);
    ch_vals[i] = 0;
  }

  showLED(CRGB::Yellow, CRGB::Yellow);
  delay(250);

  //===========Wait for User to indicate robot is ready to calibrate and drive================
  int start_wait = millis();
  
  do{
    ch_vals[READY] = pulseIn(ch_pins[READY], HIGH);
    
    if(((millis() - start_wait) % 500) < 250){
      showLED(CRGB::Yellow, CRGB::Yellow);
    } else {
      showLED(CRGB::Yellow, CRGB::Black);
    }  
  }

  //===========Calibrate============
  int start_calibrate = millis();
  
  long x_sum = 0, y_sum = 0, thr_sum = 0;
  int pulses = 0;
  
  while(millis() - zero_pos_calc < ZERO_CALIBRATION_DELAY){
    x_sum += pulseIn(ch_pins[X], HIGH);
    y_sum += pulseIn(ch_pins[Y], HIGH);
    thr_sum += pulseIn(ch_pins[THR], HIGH);
    
    pulses++;

    if(((millis() - start_wait) % 500) < 250){
      showLED(CRGB::Yellow, CRGB::Yellow);
    } else {
      showLED(CRGB::Black, CRGB::Yellow);
    } 
  }
  
  x_mid = x_sum/pulses;
  y_mid = y_sum/pulses;
  thr_low = thr_sum/pulses;

  showLED(CRGB::Red, CRGB::Red);
  delay(250);
  showLED(CRGB::Yellow, CRGB::Yellow);
  delay(250);
  showLED(CRGB::Green, CRGB::Green);
  delay(250);
}

void loop() {

  int16_t x, y, z, xg, yg, zg;
  hla.readAxes(x, y, z);
  xg = hla.convertToG(LIS331::HIGH_RANGE, x);
  yg = hla.convertToG(LIS331::HIGH_RANGE, y);
  zg = hla.convertToG(LIS331::HIGH_RANGE, z);
  
  
  ch_vals[X] = pulseIn(ch_pins[X], HIGH);
  ch_vals[Y] = pulseIn(ch_pins[Y], HIGH);
  ch_vals[THR] = pulseIn(ch_pins[THR], HIGH);

  //Perform channel math
  int mLDrive = ch_vals[Y];
  int mRDrive = ch_vals[Y];
  
  mL.writeMicroseconds(mLDrive);
  mR.writeMicroseconds(mRDrive);
  
}
