#include <TouchScreen.h>
#include "BallPositionFilter.h"
#include <TimerOne.h>

//== == == == == == == == == == == == == == == == == == == ==

#define encoder1PinA 2  // Khai báo chân đọc channel B
#define encoder1PinB 3  // Khai báo chân đọc channel A

#define encoder2PinA 18
#define encoder2PinB 19

#define encoder3PinA 20
#define encoder3PinB 21


volatile double encoder1Count = 0;  // Khai báo biến đếm xung của encoder 0
volatile double encoder2Count = 0;
volatile double encoder3Count = 0;

//== == == == == == == == == == == == == == == == == == == ==

#define YP A0  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM A2  // can be a digital pin
#define XP A1  // can be a digital pin

// Resistance between X+ and X- is 330 ohms
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 330);
BallPositionFilter ballPosition = BallPositionFilter(&ts);

//== == == == == == == == == == == == == == == == == == == ==

#define PWM_1 4
#define DC1_LEN 5
#define DC1_XUONG 6

#define PWM_2 10
#define DC2_LEN 11
#define DC2_XUONG 12

#define PWM_3 7
#define DC3_LEN 8
#define DC3_XUONG 9

//== == == == == == == == == == == == == == == == == == == ==
volatile double error_x = 0;
volatile double error_y1 = 0;
volatile double error_y2 = 0;

volatile double x_previous = 0;
volatile double y1_previous = 0;
volatile double y2_previous = 0;

volatile double x = 0;
volatile double y1 = 0;
volatile double y2 = 0;

volatile double x_dot = 0;
volatile double y1_dot = 0;
volatile double y2_dot = 0;

volatile double t1_previous = 0;
volatile double t2_previous = 0;
volatile double t3_previous = 0;

volatile double t1 = 0;
volatile double t2 = 0;
volatile double t3 = 0;

volatile double t1_dot = 0;
volatile double t2_dot = 0;
volatile double t3_dot = 0;

volatile double u1 = 0;
volatile double u2 = 0;
volatile double u3 = 0;

volatile double Output1 = 0;
volatile double Output2 = 0;
volatile double Output3 = 0;

//== == == == == == == == == == == == == == == == == == == ==

//lqr
double k11 = -95;
double k12 = -100;
double k13 = 90;
double k14 = 2;

double k21 = -95;
double k22 = -100;
double k23 = 90;
double k24 = 2;

double k31 = -95;
double k32 = -100;
double k33 = 90;
double k34 = 2;

//== == == == == == == == == == == == == == == == == == == ==

#define CENTRE_X 500
#define CENTRE_Y 525
volatile double setPointX = CENTRE_X, setPointY = CENTRE_Y;
int mode = 1;
int setPointDegrees = 0;
int setPointDegrees_1=0;

void setup() {

  Serial.begin(115200);

  //== == == == == == == == == == == == == == == == == == == ==

  //interrupt
  pinMode(encoder1PinA, INPUT_PULLUP);
  pinMode(encoder1PinB, INPUT_PULLUP);

  pinMode(encoder2PinA, INPUT_PULLUP);
  pinMode(encoder2PinB, INPUT_PULLUP);

  pinMode(encoder3PinA, INPUT_PULLUP);
  pinMode(encoder3PinB, INPUT_PULLUP);

  attachInterrupt(0, doencoder1A, CHANGE);
  attachInterrupt(1, doencoder1B, CHANGE);

  attachInterrupt(5, doencoder2A, CHANGE);
  attachInterrupt(4, doencoder2B, CHANGE);

  attachInterrupt(3, doencoder3A, CHANGE);
  attachInterrupt(2, doencoder3B, CHANGE);

  //== == == == == == == == == == == == == == == == == == == ==

  TSPoint p = ballPosition.getPoint();
  x = (p.x - CENTRE_X) * 0.023;

  y1 = 0.866 * (p.y - CENTRE_Y) * 0.028 - (p.x - CENTRE_X) * 0.023 * 0.5;

  y2 = -0.866 * (p.y - CENTRE_Y) * 0.028 - (p.x - CENTRE_X) * 0.023 * 0.5;
  x_previous = x;
  y1_previous = y1;
  y2_previous = y2;

  //== == == == == == == == == == == == == == == == == == == ==

  analogWrite(PWM_1, 0);
  analogWrite(PWM_2, 0);
  analogWrite(PWM_3, 0);

  Timer1.initialize(10000);  //don vi us
  Timer1.attachInterrupt(timer_interrupt);
}



void loop() {
  computeSetPoint(mode);
  TSPoint p = ballPosition.getPoint();
  // Assume the ball is removed if we have 10 contiguous missed readings
  if (ballPosition.isPresent()) {
#ifdef RAW_POSITION_DEBUG
    // Serial.print(", ");
#endif
    x = (p.x - CENTRE_X) * 0.023/ 100;
  
    y1 = 0.866 * (p.y - CENTRE_Y) * 0.028/ 100 - (p.x - CENTRE_X) * 0.023 * 0.5/ 100;
 
    y2 = -0.866 * (p.y - CENTRE_Y) * 0.028/ 100 - (p.x - CENTRE_X) * 0.023 * 0.5/ 100;

    error_x = (p.x - setPointX) * 0.023/ 100;
 
    error_y1 = 0.866 * (p.y - setPointY) * 0.028/ 100 - (p.x - setPointX) * 0.023 * 0.5/ 100;

    error_y2 = -0.866 * (p.y - setPointY) * 0.028/ 100 - (p.x - setPointX) * 0.023 * 0.5/ 100;

    Serial.print("fX:");
    Serial.print(x);
    Serial.print(", fY1:");
    Serial.print(y1);
    Serial.print(", fY2:");
    Serial.print(y2);
    // Serial.print(' ');
    // Serial.print(t1);
    // Serial.print(' ');
    // Serial.print(t2);
    // Serial.print(' ');
    // Serial.print(t3);
    Serial.print(' ');
    Serial.print(Output1);
    Serial.print(' ');
    Serial.print(Output2);
    Serial.print(' ');
    Serial.print(Output3);
    // Serial.print(' ');
    // Serial.print(setPointX);
    // Serial.print(' ');
    // Serial.print(setPointY);
    Serial.println();
  }
}
