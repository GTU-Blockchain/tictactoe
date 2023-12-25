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
const int LEFT_SERVO_PIN = 26;
const int RIGHT_SERVO_PIN = 27;
const int LIFT_SERO_PIN = 25;

Servo servo_lift;
Servo servo_left;
Servo servo_right;

// LCD Pins
#define TFT_CS 19
#define TFT_RST 21 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 3
// * If the pen is not touching the board, this is the value you should play with *
#define TFT_MOSI 23 // Data out
#define TFT_SCLK 18 // Clock out

// For ST7735-based displays, we will use this call
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

const int Z_OFFSET = 800; // Lower value will lift the pen higher

//* Other servo configurations, usually you will not need to touch those
int servoLift = 1500;
const int LIFT0 = 950 + Z_OFFSET; // On drawing surface
const int LIFT1 = 625 + Z_OFFSET; // Between numbers
const int LIFT2 = 435 + Z_OFFSET; // Going towards sweeper
const int LIFT_SPEED = 1600;      // Speed of liftimg arm, lower number will increase speed.
// Side servos calibration
const int SERVO_LEFT_FACTOR = 690;
const int SERVO_RIGHT_FACTOR = 690;
// Zero-position
const int SERVO_LEFT_NULL = 1950; // eski değer 1950
const int SERVO_RIGHT_NULL = 815;
// Length of arms
const float L1 = 58;   // 35
const float L2 = 101;  // 55.1
const float L3 = 13.2; // 13.2
const float L4 = 91;   // 45
// Origin points of left and right servos.
const int O1X = 24;
const int O1Y = -25;
const int O2X = 49;
const int O2Y = -25;
// Home coordinates, where the eraser is.
const volatile double ERASER_X = -50;
const volatile double ERASER_Y = 98;
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
  Serial.println("====GAME IS ON====");
  // LCD Setup and clear
  // LCD Setup and clear
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);

  delay(1000); // Ekranda bir gecikme süresi ekleyin

  // Daire çizimlerini kaldırıldı

  // Yazıyı ekrana yazdır
  tft.fillCircle(30, 50, 22, ST77XX_WHITE);
  tft.fillCircle(95, 50, 22, ST77XX_WHITE);
  delay(500);

  tft.fillCircle(30, 50, 9, ST77XX_BLACK);
  tft.fillCircle(95, 50, 9, ST77XX_BLACK);
  delay(500);

  tft.fillCircle(30, 50, 15, ST77XX_WHITE);
  tft.fillCircle(95, 50, 15, ST77XX_WHITE);
  delay(250);

  tft.fillCircle(30, 50, 15, ST77XX_BLACK);
  tft.fillCircle(95, 50, 15, ST77XX_BLACK);
  delay(250);

  tft.fillCircle(95, 50, 25, ST77XX_WHITE);
  delay(250);

  tft.fillCircle(95, 50, 15, ST77XX_BLACK);
  delay(250);

  tft.fillCircle(30, 50, 15, ST77XX_WHITE);
  tft.fillCircle(95, 50, 15, ST77XX_WHITE);
  delay(250);

  tft.fillCircle(30, 50, 15, ST77XX_BLACK);
  tft.fillCircle(95, 50, 15, ST77XX_BLACK);
  delay(250);

  delay(1000);

  tft.fillScreen(ST77XX_BLACK); // Clear the screen

  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setTextWrap(true);

  tft.setCursor(37, 80);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.print("BEN");
  tft.setTextSize(2);
  tft.setCursor(10, 110);
  tft.print("ROBOCHAIN");
  tft.fillCircle(30, 40, 22, ST77XX_WHITE);
  tft.fillCircle(95, 40, 22, ST77XX_WHITE);
  tft.fillCircle(30, 40, 9, ST77XX_BLACK);
  tft.fillCircle(95, 40, 9, ST77XX_BLACK);

  /* tft.fillCircle(30, 40, 22, ST77XX_WHITE);
   tft.fillCircle(95, 40, 22, ST77XX_WHITE);
  tft.fillCircle(30, 30, 9, ST77XX_BLACK);
   tft.fillCircle(95, 30, 9, ST77XX_BLACK); yukarı efekti*/
  /*tft.fillCircle(30, 40, 22, ST77XX_WHITE);
  tft.fillCircle(95, 40, 22, ST77XX_WHITE);
 tft.fillCircle(30, 50, 9, ST77XX_BLACK);
  tft.fillCircle(95, 50, 9, ST77XX_BLACK); aşağı efekti*/
  Serial.end();

  // put your setup code here, to run once:
  if (SERIAL_MONITOR_MODE)
  {
    Serial.begin(115200);
    Serial.println("--==MAIN MENU==--");
    Serial.println("==1== PvE");
    Serial.println("==2== AI vs AI");

    // We just wait until there is an input
    while (Serial.available() == 0)
      ;

    // Get the value user entered
    int user_input = Serial.readString().toInt();

    // Ignore the hidden line end character the monitor is adding to the received character
    // React to user input
    if (user_input == 1)
      startGamePvE();
    else if (user_input == 2)
      startGameAIvsAI();
    else
      Serial.println("Wrong input!");
  }
  Serial.end();
}

void loop()
{
  // put your main code here, to run repeatedly:
}

void startGameAIvsAI()
{
  initializeGame();

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

void startGamePvE()
{
  initializeGame();

  int coordinate;

  while ((winner == -1) && (empty_places > 0))
  {
    while (!Serial.available())
      ;
    coordinate = Serial.readString().toInt();
    delay(50);
    Serial.println(coordinate);
    drawMove(coordinate);
    recordMove(coordinate);
    checkWinner(0);
    delay(2000);
    yourTurnLCD();

    while (!Serial.available())
      ;
    coordinate = Serial.readString().toInt();
    delay(50);
    drawMove(coordinate + 10);
    recordMove(coordinate + 10);
    checkWinner(1);
  }
  goHome();
}

void initializeGame()
{
  Serial.begin(115200);
  Serial.println("====GAME IS ON====");

  // Clean text area

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.fillCircle(30, 40, 22, ST77XX_WHITE);
  tft.fillCircle(95, 40, 22, ST77XX_WHITE);
  tft.fillCircle(30, 40, 9, ST77XX_BLACK);
  tft.fillCircle(95, 40, 9, ST77XX_BLACK);
  // Print
  tft.setCursor(40, 90);
  tft.print("OYUN");
  tft.setCursor(20, 115);
  tft.print("BASLASIN");

  delay(2000);
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  // ERESE
  tft.fillCircle(30, 40, 22, ST77XX_WHITE);
  tft.fillCircle(95, 40, 22, ST77XX_WHITE);
  tft.fillCircle(30, 50, 9, ST77XX_BLACK);
  tft.fillCircle(95, 50, 9, ST77XX_BLACK);
  tft.setCursor(35, 90);
  tft.print("TAHTA");
  tft.setCursor(13, 115);
  tft.print("SILINIYOR");
  delay(500);
  Serial.println("Erasing");
  erase();
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  // ERESE
  tft.fillCircle(30, 40, 22, ST77XX_WHITE);
  tft.fillCircle(95, 40, 22, ST77XX_WHITE);
  tft.fillCircle(30, 50, 9, ST77XX_BLACK);
  tft.fillCircle(95, 50, 9, ST77XX_BLACK);
  tft.setCursor(35, 90);
  tft.print("CIZIM");
  tft.setCursor(13, 115);
  tft.print("YAPILIYOR");
  delay(500);
  Serial.println("Drawing Frame");
  drawFrame();

  delay(1000);
  Serial.end();
  Serial.begin(115200);
}

void yourTurnLCD()
{
  Serial.begin(115200);
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.fillCircle(30, 40, 22, ST77XX_WHITE);
  tft.fillCircle(95, 40, 22, ST77XX_WHITE);
  tft.fillCircle(30, 50, 9, ST77XX_BLACK);
  tft.fillCircle(95, 50, 9, ST77XX_BLACK);
  tft.setCursor(35, 90);
  tft.print("SENIN");
  tft.setCursor(35, 115);
  tft.print("SIRAN");
  delay(500);
  Serial.end();
  Serial.begin(115200);
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

void replyMove()
{
  //========= Reply move ======
  // We will generate a random number from 1 to the number of empty places
  // We will then go over the array and count the empty places we meet until we get to the needed place
  // If there are 3 empty places and the trandom number will be 2 , this means we will make a move at the second empty place we find

  int randEmptyPlace = random(empty_places) + 1;
  // Debugging
  /*
    Serial.println("============================");
    Serial.print("Empty Spaces:");
    Serial.println(empty_places);
    Serial.print("Replying to randEmptyPlace: ");
    Serial.println(randEmptyPlace);
    Serial.println("============================");
  */
  // Loop until we find an empty place
  int emptyPlacesFound = 0;

  for (int i = 0; i < 9; i++)
  {
    if (board_values[i] == -1)
    {
      // We found an empty place
      emptyPlacesFound++;
      if (emptyPlacesFound == randEmptyPlace)
      {
        drawMove(i + 1);
        recordMove(i + 1);
        Serial.print("Replying to: ");
        Serial.println(i + 1);
      }
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
    drawX(60, 40);
    break;

  case 2:
    drawX(38, 38);
    break;

  case 3:
    drawX(7, 40);
    break;

  case 4:
    drawX(65, 60);
    break;

  case 5:
    drawX(40, 60);
    break;

  case 6:
    drawX(7, 60);
    break;

  case 7:
    drawX(65, 80);
    break;

  case 8:
    drawX(40, 80);
    ;
    break;

  case 9:
    drawX(7, 80);
    break;
  case 11:
    drawZero(65, 40);
    break;

  case 12:
    drawZero(33, 38);
    break;

  case 13:
    drawZero(5, 40);
    break;

  case 14:
    drawZero(65, 60);
    break;

  case 15:
    drawZero(35, 60);
    break;

  case 16:
    drawZero(5, 60);
    break;

  case 17:
    drawZero(65, 80);
    break;

  case 18:
    drawZero(35, 80);
    break;

  case 19:
    drawZero(5, 80);
    break;

  case 99:
    drawTo(ERASER_X + 20, ERASER_Y - 10);
    break;
  }
  // Get out of the way
  lift(LIFT2);
  drawTo(ERASER_X, ERASER_Y);
  detachServos();
}

void erase()
{
  goHome();
  attachServos();

  lift(LIFT0 + 200); // Go down, just before doing the erase movements.

  drawTo(-25, 90); // bunuu elleme
  drawTo(100, 90);

  drawTo(-45, 80);
  drawTo(100, 80);

  drawTo(-45, 70);
  drawTo(100, 70);

  drawTo(-45, 60);
  drawTo(100, 60);

  drawTo(-45, 50);
  drawTo(100, 50);

  drawTo(-45, 40);
  drawTo(100, 40);

  drawTo(-45, 40);
  drawTo(100, 40);

  drawTo(-25, 90);
  drawTo(ERASER_X, ERASER_Y);
  lift(LIFT2);

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
  /*
  drawTo(-25,90);//bunuu elleme
  drawTo(100,90);

  drawTo(-45,80);
  drawTo(100,80);
  */
  //===VERTICAL
  // Go
  drawTo(50, 90);
  delay(500);
  // Draw
  lift(LIFT0 + 200);
  drawTo(50, 40);
  delay(250);
  lift(LIFT2);
  // Go
  drawTo(20, 90);
  delay(500);
  // Draw
  lift(LIFT0 + 200);
  drawTo(20, 40);
  delay(250);
  lift(LIFT2);

  //===HORIZONTAL

  // Go
  delay(250);
  drawTo(0, 73);

  // Draw
  lift(LIFT0 + 200);
  delay(300);
  drawTo(80, 73);
  delay(300);
  lift(LIFT2);

  // Go
  drawTo(0, 53);

  // Draw
  lift(LIFT0 + 200);
  delay(300);
  drawTo(80, 53);
  delay(300);
  lift(LIFT2);
  drawTo(ERASER_X, ERASER_Y);

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
  attachServos();

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