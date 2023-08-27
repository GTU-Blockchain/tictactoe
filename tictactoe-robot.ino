void setup() {
  // put your setup code here, to run once:
drawFrame();
}

void loop() {
  // put your main code here, to run repeatedly:

}
/**********************/
// Tico - Tic-Tac-Toe playing robot
// Tico is an open source 3D printed robot designed by PlayRobotics
// Tico was designed in order to inspire kids to learn coding while teaching Tico to play Tic-Tac-Toe
// Full documentation can be found here: https://playrobotics.com/blog/tico-tic-tac-toe-arduino-robot-documentation

// Attribution: Parts of this code are based on the popular Plotclock by Joo (https://www.thingiverse.com/thing:248009)
//**********************//

//Should playro draw the move made by the human, or the human will draw it himself?


// Include libraries
#include <Wire.h>
#include <Servo.h>
#include <SPI.h>
// Servo pins
const int LEFT_SERVO_PIN = 12;
const int RIGHT_SERVO_PIN = 18;
const int LIFT_SERO_PIN = 15;

Servo servo_lift;
Servo servo_left;
Servo servo_right;




// * If the pen is not touching the board, this is the value you should play with *
const int Z_OFFSET = 395;  // Lower value will lift the pen higher

//* Other servo configurations, usually you will not need to touch those
int servoLift = 1500;
const int LIFT0 = 1000 + Z_OFFSET;  // On drawing surface
const int LIFT1 = 925 + Z_OFFSET;   // Between numbers
const int LIFT2 = 735 + Z_OFFSET;   // Going towards sweeper
const int LIFT_SPEED = 1000;  // Speed of liftimg arm, lower number will increase speed.
// Side servos calibration
const int SERVO_LEFT_FACTOR = 690;
const int SERVO_RIGHT_FACTOR = 690;
// Zero-position
const int SERVO_LEFT_NULL = 1950;
const int SERVO_RIGHT_NULL = 815;
// Length of arms
const float L1 = 35;
const float L2 = 55.1;
const float L3 = 13.2;
const float L4 = 45;
// Origin points of left and right servos.
const int O1X = 24;
const int O1Y = -25;
const int O2X = 49;
const int O2Y = -25;
// Home coordinates, where the eraser is.
const volatile double ERASER_X = -11;
const volatile double ERASER_Y = 45.5;
volatile double lastX = ERASER_X;  // 75;
volatile double lastY = ERASER_Y;  // 47.5;

void drawFrame() {

  attachServos();
  lift(LIFT2);
  
  //===VERTICAL

  //Go
  
  drawTo(60, 5);
  delay(500);
  //Draw 
 lift(LIFT0);
 drawTo(67, 45);
 lift(LIFT2);
 
  //Go
   drawTo(45, 5);
  delay(500);
  //Draw
  lift(LIFT0);
 
  drawTo(52, 45);
   lift(LIFT2);


  //===HORIZONTAL

  //Go
  drawTo(72, 15);
delay(500);
  //Draw
  lift(LIFT0);

  drawTo(32, 21);
    lift(LIFT2);

 
  //Go
  drawTo(78, 30);

delay(500);
  //Draw
  lift(LIFT0);
  
  drawTo(30, 36);
  
  
  

  detachServos();
}
void attachServos() {
  servo_lift.attach(LIFT_SERO_PIN);
  servo_left.attach(LEFT_SERVO_PIN);
  servo_right.attach(RIGHT_SERVO_PIN);
}
void lift(int lift) {
  if (servoLift >= lift) {
    while (servoLift >= lift) {
      servoLift--;
      servo_lift.writeMicroseconds(servoLift);
      delayMicroseconds(LIFT_SPEED);
    }
  }
  else {
    while (servoLift <= lift) {
      servoLift++;
      servo_lift.writeMicroseconds(servoLift);
      delayMicroseconds(LIFT_SPEED);
    }
  }
}
void drawTo(double pX, double pY) {
  double dx, dy, c;
  int i;

  // dx dy of new point
  dx = pX - lastX;
  dy = pY - lastY;
  //path lenght in mm, times 4 equals 4 steps per mm
  c = floor(7 * sqrt(dx * dx + dy * dy));

  if (c < 1) c = 1;

  for (i = 0; i <= c; i++) {
    // draw line point by point
    set_XY(lastX + (i * dx / c), lastY + (i * dy / c));
  }

  lastX = pX;
  lastY = pY;

}
void set_XY(double Tx, double Ty) {
  delay(1);
  double dx, dy, c, a1, a2, Hx, Hy;

  // calculate triangle between pen, servoLeft and arm joint
  // cartesian dx/dy
  dx = Tx - O1X;
  dy = Ty - O1Y;

  // polar lemgth (c) and angle (a1)
  c = sqrt(dx * dx + dy * dy); //
  a1 = atan2(dy, dx); //
  a2 = return_angle(L1, L2, c);
  //Serial.print("servo_left:");
  //Serial.println(empty_places);
  servo_left.writeMicroseconds(floor(((a2 + a1 - M_PI) * SERVO_LEFT_FACTOR) + SERVO_LEFT_NULL));

  // calculate joinr arm point for triangle of the right servo arm
  a2 = return_angle(L2, L1, c);
  Hx = Tx + L3 * cos((a1 - a2 + 0.621) + M_PI); //36,5°
  Hy = Ty + L3 * sin((a1 - a2 + 0.621) + M_PI);

  // calculate triangle between pen joint, servoRight and arm joint
  dx = Hx - O2X;
  dy = Hy - O2Y;

  c = sqrt(dx * dx + dy * dy);
  a1 = atan2(dy, dx);
  a2 = return_angle(L1, L4, c);
  //Serial.print("servo_right:");
  //Serial.println(floor(((a1 - a2) * SERVO_RIGHT_FACTOR) + SERVO_RIGHT_NULL));
  servo_right.writeMicroseconds(floor(((a1 - a2) * SERVO_RIGHT_FACTOR) + SERVO_RIGHT_NULL));
}
double return_angle(double a, double b, double c) {
  // cosine rule for angle between c and a
  return acos((a * a + c * c - b * b) / (2 * a * c));
}
void detachServos() {
  servo_lift.detach();
  servo_left.detach();
  servo_right.detach();
}