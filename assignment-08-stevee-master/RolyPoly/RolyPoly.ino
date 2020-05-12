/*
  Library for the MMA8452Q
  By: Jim Lindblom and Andrea DeVore
  SparkFun Electronics

  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/products/14587

  This sketch uses the SparkFun_MMA8452Q library to initialize
  the accelerometer and stream calcuated x, y, z, acceleration
  values from it (in g units).

  Hardware hookup:
  Arduino --------------- MMA8452Q Breakout
    3.3V  ---------------     3.3V
    GND   ---------------     GND
  SDA (A4) --\/330 Ohm\/--    SDA
  SCL (A5) --\/330 Ohm\/--    SCL

  The MMA8452Q is a 3.3V max sensor, so you'll need to do some
  level-shifting between the Arduino and the breakout. Series
  resistors on the SDA and SCL lines should do the trick.

  License: This code is public domain, but if you see me
  (or any other SparkFun employee) at the local, and you've
  found our code helpful, please buy us a round (Beerware
  license).

  Distributed as is; no warrenty given.
*/

#include <Wire.h>                 // Must include Wire library for I2C
#include "SparkFun_MMA8452Q.h"    // Click here to get the library: http://librarymanager/All#SparkFun_MMA8452Q

MMA8452Q accel;                   // create instance of the MMA8452 class

boolean matrix[7][5];

const int row[7] = {2, 3, 4, 5, 6, 7, 8};
const int column[5] = {9, 10, 11, 12, 13};
int preyRow = 0;
int preyCol = 0;
int predatorRow = 5;
int predatorCol = 3;

const int frameLength = 15;

const int preySpeedDelta = 400;
unsigned long nextTime_prey = 400;

const int predatorSpeedDelta = 800;
unsigned long nextTime_predator = 800;

const int predatorSpeedDelta_blink = 250;
unsigned long nextTime_predator_blink = 250;

unsigned long initialTime = 0;

boolean predator_on = true;
boolean predatorWin = false;
boolean preyWin = false;
int predator_Win_Times = 0;
int prey_Win_Times = 0;
int winTimes_Difference;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 7; i++) {   // set up rows
    pinMode(row[i], OUTPUT);
    digitalWrite(row[i], LOW);
  }

  for (int i = 0; i < 5; i++) {   // set up columns
    pinMode(column[i], OUTPUT);
    digitalWrite(column[i], HIGH);
  }

  accel.init();
  randomPosition();
  Serial.println("ready to start: ");
}

void loop() {
  clearCanvas();
  randomPosition();

  initialTime = millis();

  while (!preyWin && !predatorWin) {
    // prey:
    if (millis() > nextTime_prey) {
      nextTime_prey = millis() + preySpeedDelta;

      // update prey position
      if (accel.available()) {
        Serial.println("available");

        accel.read();
        byte reading = accel.readPL(); // read portrait data
        matrix[preyRow][preyCol] = 0;  //erase position of last time
        switch (reading) {
          case PORTRAIT_U:
            Serial.print("Prey Going Up");
            preyRow = preyRow - 1;
            if (preyRow < 0) preyRow = 0;
            break;
          case PORTRAIT_D:
            Serial.print("Prey going Down");
            preyRow = preyRow + 1;
            if (preyRow > 6) preyRow = 6;
            break;
          case LANDSCAPE_R:
            Serial.print("Prey Going left");
            preyCol = preyCol - 1;
            if (preyCol < 0) preyCol = 0;
            break;
          case LANDSCAPE_L:
            Serial.print("Prey Going Right");
            preyCol = preyCol + 1;
            if (preyCol > 4) preyCol = 4;
            break;
          case LOCKOUT:
            Serial.print("Flat");
            break;
        }
        matrix[preyRow][preyCol] = 1;
      }
    }


    // predator:
    if (millis() > nextTime_predator) {
      nextTime_predator = millis() + predatorSpeedDelta;

      // update predator position
      if (Serial.available()) {
        matrix[predatorRow][predatorCol] = 0;
        matrix[predatorRow + 1][predatorCol] = 0;
        matrix[predatorRow][predatorCol + 1] = 0;
        matrix[predatorRow + 1][predatorCol + 1] = 0;

        byte reading = Serial.read();   // read input from JAVA side
        switch (reading) {
          case 0:
            predatorCol = predatorCol - 1;
            if (predatorCol < 0) predatorCol = 0;
            Serial.println("Predator going left");
            break;
          case 1:
            predatorCol = predatorCol + 1;
            if (predatorCol > 3) predatorCol = 3;
            Serial.println("Predator going right");
            break;
          case 2:
            predatorRow = predatorRow - 1;
            if (predatorRow < 0) predatorRow = 0;
            Serial.println("Predator going up");
            break;
          case 3:
            predatorRow = predatorRow + 1;
            if (predatorRow > 5) predatorRow = 5;
            Serial.println("Predator going down");
            break;
        }
        matrix[predatorRow][predatorCol] = 1;
        matrix[predatorRow][predatorCol + 1] = 1;
        matrix[predatorRow + 1][predatorCol] = 1;
        matrix[predatorRow + 1][predatorCol + 1] = 1;
      }
    }

    if (millis() > nextTime_predator_blink) {
      nextTime_predator_blink = millis() + predatorSpeedDelta_blink;
      matrix[predatorRow][predatorCol] = predator_on;
      matrix[predatorRow + 1][predatorCol] = predator_on;
      matrix[predatorRow][predatorCol + 1] = predator_on;
      matrix[predatorRow + 1][predatorCol + 1] = predator_on;
      predator_on = !predator_on;
    }

    updateCanvas();
    preyWin = (millis() - initialTime) > 15000; // if 15 secs have passed, prey wins

    // check if prey wins
    if ( (preyRow == predatorRow || preyRow == (predatorRow + 1)) && (preyCol == predatorCol  || preyCol == (predatorCol + 1))) {
      predatorWin = true;       
    }
    else {
      predatorWin = false;
    }
   
  }

  update_result();
  initialTime = millis();
  
  while (millis() - initialTime < 4000) { // display scores
    updateCanvas();
  }

  preyWin = false;
  predatorWin = false;
}


void clearCanvas() {
  for (int row = 0; row < 7; row++) {
    for (int col = 0; col < 5; col++) {
      matrix[row][col] = 0;
    }
  }
}

void randomPosition() {
  predatorRow = int(random(0, 6));
  predatorCol = int(random(0, 4));
  preyRow = int(random(0, 7));
  preyCol = int(random(0, 5));

  while (preyRow == (predatorRow + 1) || predatorRow == preyRow || predatorCol == preyCol || preyCol == (predatorCol + 1)) {
    preyRow = int(random(0, 6));
    preyCol = int(random(0, 4));
  }

  matrix[preyRow][preyCol] = 1;
  matrix[predatorRow][predatorCol] = 1;
  matrix[predatorRow + 1][predatorCol] = 1;
  matrix[predatorRow][predatorCol + 1] = 1;
  matrix[predatorRow + 1][predatorCol + 1] = 1;
}


void updateCanvas() {
  for (int c = 0; c < 5; c++) {
    digitalWrite(column[c], LOW);
    for (int r = 0; r < 7; r++) {
      digitalWrite(row[r], matrix[r][c]);
    }
    delay(frameLength / 5);
    digitalWrite(column[c], HIGH);
  }
}


void update_result() {
  clearCanvas();
  if (preyWin) {
    prey_Win_Times += 1;
    matrix[3][2] = 1;
    winTimes_Difference = prey_Win_Times - predator_Win_Times;
  }
  else if (predatorWin) {
    predator_Win_Times += 1;
    winTimes_Difference = predator_Win_Times - prey_Win_Times;
    matrix[3][2] = 1;
    matrix[3][3] = 1;
    matrix[4][2] = 1;
    matrix[4][3] = 1;
  }

  matrix[6][2] = 1;

  if (winTimes_Difference >= 3) {
    matrix[6][4] = 1;
  }
  if (winTimes_Difference > 0) {
    matrix[6][3] = 1;
  }
  if (winTimes_Difference <= -3) {
    matrix[6][0] = 1;
  }
  if (winTimes_Difference < 0) {
    matrix[6][1] = 1;
  }
}
