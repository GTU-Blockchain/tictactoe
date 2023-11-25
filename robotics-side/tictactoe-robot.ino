// UMUT SATIR BLOCKCHAIN

#define DRAW_HUMAN_MOVE false

// If you don't have a remote control or IR receiver you can enable serial monitor instead
// When using serial monitor please choose 'No line ending' from the dropdown next to the boundrate instead of 'new line'
#define SERIAL_MONITOR_MODE true
#include <Wire.h>
#include <ESP32Servo.h>
#include <IRremote.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
const int LEFT_SERVO_PIN = 12;
const int RIGHT_SERVO_PIN = 18;
const int LIFT_SERO_PIN = 15;

Servo servo_lift;
Servo servo_left;
Servo servo_right;

// LCD Pins
#define TFT_CS 10
#define TFT_RST 8 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 9
// *** If the pen is not touching the board, this is the value you should play with ***
const int Z_OFFSET = 550; // Lower value will lift the pen higher

//*** Other servo configurations, usually you will not need to touch those
int servoLift = 1500;
const int LIFT0 = 950 + Z_OFFSET; // On drawing surface
const int LIFT1 = 625 + Z_OFFSET; // Between numbers
const int LIFT2 = 435 + Z_OFFSET; // Going towards sweeper
const int LIFT_SPEED = 1800;      // Speed of lifting arm, lower number will increase speed.
// Side servos calibration
const int SERVO_LEFT_FACTOR = 690;
const int SERVO_RIGHT_FACTOR = 690;
// Zero-position
const int SERVO_LEFT_NULL = 1950; // eski değer 1950
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
const volatile double ERASER_X = 0;
const volatile double ERASER_Y = 47;
volatile double lastX = ERASER_X; // 75;
volatile double lastY = ERASER_Y; // 47.5;

// We will be using an array that will hold the current state of all our game cells
//  -1-> Empty cell
//  0 -> 0
//  1 -> X

int board_values[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
int empty_places = 9;

int winner = -1;
void setup()
{
  Serial.begin(115200);

  startGame();
}

void loop()
{
  // put your main code here, to run repeatedly:
}
void startGame()
{
  Serial.println("====GAME IS ON====");
  tone(4, 3000, 250);

  // Clean text area
  Serial.println("Erasing");
  erase();

  Serial.println("Drawing Frame");
  drawFrame();

  delay(1000);

  int coordinate, counter = 0;

  while ((winner == -1) && (empty_places > 0))
  {
    while (!Serial.available())
      ;
    coordinate = Serial.readString().toInt();
    delay(50);
    Serial.println(coordinate);
    if (counter % 2 == 1)
      coordinate = coordinate + 10;
    drawMove(coordinate);
    recordMove(coordinate);
    checkWinner(counter % 2);
    counter++;
  }
  goHome();
}

void checkWinner(int player)
{
  int currplayer;
  if (player == 0)
    currplayer = 1;
  else
    currplayer = 0;

  for (int i = 1; i < 4; i++)
  {
    checkWinnerCol(i, currplayer);
    checkWinnerRow(i, currplayer);
  }
  checkWinnerDiag(1, currplayer);
}

void checkWinnerCol(int col, int player)
{
  // Row
  if ((board_values[(col - 1) * 3] == player) && (board_values[(col - 1) * 3 + 1] == player) && (board_values[(col - 1) * 3 + 2] == player))
  {
    attachServos();
    Serial.println("--== Winner COL==--");
    Serial.println(player);

    drawTo(55 - 20 * (4 - col - 1), 10);
    // Draw
    lift(LIFT0);
    drawTo(55 - 20 * (4 - col - 1), 50);
    lift(LIFT2);

    winner = player;
  }
}
void checkWinnerRow(int row, int player)
{

  // Row
  if ((board_values[row - 1] == player) && (board_values[row + 3 - 1] == player) && (board_values[row + 6 - 1] == player))
  {
    attachServos();
    Serial.println("--== Winner ROW==--");
    Serial.println(player);

    drawTo(10, 43 - 14 * (row - 1));
    // Draw
    lift(LIFT0);
    drawTo(60, 43 - 14 * (row - 1));
    lift(LIFT2);

    winner = player;
  }
}

void checkWinnerDiag(int diag, int player)
{
  attachServos();
  // Check which diagonal
  if (diag == 1)
  {
    if ((board_values[1 - 1] == player) && (board_values[5 - 1] == player) && (board_values[9 - 1] == player))
    {
      Serial.println("--== Winner DIAGONAL 1==--");
      Serial.println(player);

      drawTo(60, 10);
      // Draw
      lift(LIFT0);
      drawTo(15, 45);
      lift(LIFT2);

      winner = player;
    }
  }
  else if (diag == 2)
  {
    if ((board_values[2] == player) && (board_values[5 - 1] == player) && (board_values[6] == player))
    {
      Serial.println("--== Winner DIAGONAL 2==--");
      Serial.println(player);

      drawTo(10, 10);
      // Draw
      lift(LIFT0);
      drawTo(60, 50);
      lift(LIFT2);

      winner = player;
    }
  }
}

void recordMove(int move)
{
  if ((move >= 1) && (move <= 9))
  {
    board_values[move - 1] = 1;
    empty_places--;
  }
  if ((move >= 11) && (move <= 19))
  {
    board_values[move - 11] = 0;
    empty_places--;
  }
}

void drawMove(int move)
{
  attachServos();
  switch (move)
  {
  case 0:
    drawFrame();
    break;
  case 1:
    drawX(50, 20);
    break;

  case 2:
    drawX(34.5, 25);
    break;

  case 3:
    drawX(15, 25);
    break;

  case 4:
    drawX(50, 35);
    break;

  case 5:
    drawX(34.5, 38);
    break;

  case 6:
    drawX(15, 35);
    break;

  case 7:
    drawX(48, 47);
    break;

  case 8:
    drawX(34.5, 50);
    break;

  case 9:
    drawX(15, 50);
    break;

  case 11:
    drawZero(50, 10);
    break;

  case 12:
    drawZero(30, 10);
    break;

  case 13:
    drawZero(15, 10);
    break;

  case 14:
    drawZero(50, 25);
    break;

  case 15:
    drawZero(30, 25);
    break;

  case 16:
    drawZero(15, 25);
    break;

  case 17:
    drawZero(50, 40);
    break;

  case 18:
    drawZero(30, 40);
    break;

  case 19:
    drawZero(15, 40);
    break;

  case 99:
    drawTo(5, 0);
    break;
  }
  // Get out of the way
  lift(LIFT2);
  drawTo(10, 10);
  detachServos();
}

void erase()
{
  goHome();
  attachServos();
  lift(LIFT0); // Go down, just before doing the erase movements.
  drawTo(70, ERASER_Y);
  drawTo(5, ERASER_Y);

  drawTo(70, 34);
  drawTo(0, 34);
  drawTo(70, 34);

  drawTo(0, 26);
  drawTo(70, 20);

  drawTo(0, 20);
  drawTo(70, 5);

  drawTo(10, 15);
  drawTo(40, 30);

  drawTo(ERASER_X, ERASER_Y);
  lift(LIFT2 + 800);

  detachServos();
}

void drawX(float bx, float by)
{
  bx = bx - 1;
  by = by + 1;
  // Go
  drawTo(bx, by + 1);
  // Draw
  lift(LIFT0);
  drawTo(bx + 10, by + 10);
  delay(100);
  //=====
  // Go
  lift(LIFT2);
  drawTo(bx + 10, by);
  // Drawdelay(100);
  delay(100);
  lift(LIFT0);
  delay(100);
  drawTo(bx, by + 10);
  lift(LIFT1);
}

void drawZero(float bx, float by)
{
  drawTo(bx + 6, by + 3);
  lift(LIFT0);
  bogenGZS(bx + 3.5, by + 5, 7, 0, 7, 1);
  lift(LIFT1);
}

void lift(int lift)
{
  if (servoLift >= lift)
  {
    while (servoLift >= lift)
    {
      servoLift--;
      servo_lift.writeMicroseconds(servoLift);
      delayMicroseconds(LIFT_SPEED);
    }
  }
  else
  {
    while (servoLift <= lift)
    {
      servoLift++;
      servo_lift.writeMicroseconds(servoLift);
      delayMicroseconds(LIFT_SPEED);
    }
  }
}

void bogenUZS(float bx, float by, float radius, int start, int ende, float sqee)
{
  float inkr = -0.05;
  float count = 0;

  do
  {
    drawTo(sqee * radius * cos(start + count) + bx,
           radius * sin(start + count) + by);
    count += inkr;
  } while ((start + count) > ende);
}

void bogenGZS(float bx, float by, float radius, int start, int ende, float sqee)
{
  float inkr = 0.05;
  float count = 0;

  do
  {
    drawTo(sqee * radius * cos(start + count) + bx,
           radius * sin(start + count) + by);
    count += inkr;
  } while ((start + count) <= ende);
}

void drawTo(double pX, double pY)
{
  double dx, dy, c;
  int i;

  // dx dy of new point
  dx = pX - lastX;
  dy = pY - lastY;
  // path lenght in mm, times 4 equals 4 steps per mm
  c = floor(7 * sqrt(dx * dx + dy * dy));

  if (c < 1)
    c = 1;

  for (i = 0; i <= c; i++)
  {
    // draw line point by point
    set_XY(lastX + (i * dx / c), lastY + (i * dy / c));
  }

  lastX = pX;
  lastY = pY;
}

double return_angle(double a, double b, double c)
{
  // cosine rule for angle between c and a
  return acos((a * a + c * c - b * b) / (2 * a * c));
}

void set_XY(double Tx, double Ty)
{
  delay(1);
  double dx, dy, c, a1, a2, Hx, Hy;

  // calculate triangle between pen, servoLeft and arm joint
  // cartesian dx/dy
  dx = Tx - O1X;
  dy = Ty - O1Y;

  // polar lemgth (c) and angle (a1)
  c = sqrt(dx * dx + dy * dy); //
  a1 = atan2(dy, dx);          //
  a2 = return_angle(L1, L2, c);
  // Serial.print("servo_left:");
  // Serial.println(empty_places);
  servo_left.writeMicroseconds(floor(((a2 + a1 - M_PI) * SERVO_LEFT_FACTOR) + SERVO_LEFT_NULL));

  // calculate joinr arm point for triangle of the right servo arm
  a2 = return_angle(L2, L1, c);
  Hx = Tx + L3 * cos((a1 - a2 + 0.621) + M_PI); // 36,5°
  Hy = Ty + L3 * sin((a1 - a2 + 0.621) + M_PI);

  // calculate triangle between pen joint, servoRight and arm joint
  dx = Hx - O2X;
  dy = Hy - O2Y;

  c = sqrt(dx * dx + dy * dy);
  a1 = atan2(dy, dx);
  a2 = return_angle(L1, L4, c);
  // Serial.print("servo_right:");
  // Serial.println(floor(((a1 - a2) * SERVO_RIGHT_FACTOR) + SERVO_RIGHT_NULL));
  servo_right.writeMicroseconds(floor(((a1 - a2) * SERVO_RIGHT_FACTOR) + SERVO_RIGHT_NULL));
}

void drawFrame()
{

  attachServos();
  lift(LIFT2);
  delay(1000);

  //===VERTICAL
  // Go
  drawTo(21, 15);
  delay(500);
  // Draw
  lift(LIFT0);
  drawTo(23, 60);
  delay(250);
  lift(LIFT2);
  // Go
  drawTo(47, 15);
  delay(500);
  // Draw
  lift(LIFT0);
  drawTo(45, 60);
  delay(250);
  lift(LIFT2);

  //===HORIZONTAL

  // Go
  delay(250);
  drawTo(5, 28);

  // Draw
  lift(LIFT0);
  delay(300);
  drawTo(67, 32);
  delay(300);
  lift(LIFT2);

  // Go
  drawTo(5, 45);

  // Draw
  lift(LIFT0);
  delay(300);
  drawTo(67, 45);
  delay(300);
  lift(LIFT2);

  detachServos();
}
void attachServos()
{
  servo_lift.attach(LIFT_SERO_PIN);
  servo_left.attach(LEFT_SERVO_PIN);
  servo_right.attach(RIGHT_SERVO_PIN);
}
void goHome()
{
  // initial servo location

  servo_lift.writeMicroseconds(800);
  servo_left.writeMicroseconds(1633);
  servo_right.writeMicroseconds(2289);
  servo_lift.attach(LIFT_SERO_PIN);
  servo_left.attach(LEFT_SERVO_PIN);
  servo_right.attach(RIGHT_SERVO_PIN);

  lift(LIFT2 - 100); // Lift all the way up.
  drawTo(ERASER_X, ERASER_Y);
  delay(1000);
  lift(LIFT0);
  delay(500);
  // lift(LIFT2);
  detachServos();
}

void detachServos()
{
  servo_lift.detach();
  servo_left.detach();
  servo_right.detach();
}