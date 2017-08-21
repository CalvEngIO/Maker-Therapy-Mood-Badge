#include "Stack.h"
#include "ASCII.h"
#include "Emoji.h"

//Arduino Pro Mini 3.3V 8MHz
const int r8 = 13;
const int r7 = 8;
const int r6 = A1;
const int r5 = 10;
const int r4 = 4;
const int r3 = A2;
const int r2 = 3;
const int r1 = A4;

const int c8 = 9;
const int c7 = 2;
const int c6 = A5;
const int c5 = 12;
const int c4 = A3;
const int c3 = 11;
const int c2 = 7;
const int c1 = 6;

const int rows[8] = { r1, r2, r3, r4, r5, r6, r7, r8 };
const int columns[8] = { c1, c2, c3, c4, c5, c6, c7, c8 };

int ship[] = {2,3,4};

long lastTime = 0;
long holdTime = 0;

int hitMatrix[8][8] = {0};

int matrix[8][8] = {0};

int enemies[8][8] = {0};

int left = A0;
int right = A7;
int action = A6;
int rotate = A6;
long moveTime;
boolean firstTime;

const int mode = 5;
boolean gameSelect = 0;
boolean actionHold = true;

long lastSpawn = 0;

int numShapes = 7;
//char shape[numShapes]   = { 'I', 'O', 'T', 'S', 'Z', 'J', 'L' };
int shapeProb[7] = {  1 ,  1 ,  1 ,  1 ,  1 ,  1 ,  1  }; //All equal rate of occurance currently

int activeShape[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // {ShapeID, Anchor Coordinate(1 2 3 or 4), r1, c1, r2, c2, r3, c3, r4, c4}
boolean playField[8][8];

int emoji = 0;

void setup() 
{
  Serial.begin(9600);
  for(int i = 0; i < 8; i++)
  {
    pinMode(rows[i], OUTPUT);
    pinMode(columns[i], OUTPUT);
    digitalWrite(rows[i],HIGH);
    digitalWrite(columns[i],LOW);
  }

  pinMode(mode, INPUT);
  pinMode(left, INPUT);
  pinMode(right, INPUT);
  pinMode(action, INPUT);

  lastTime = millis();

  print("MakerTherapy Mood Badge!", 30);

  newGame();
}

void loop()
{
  if(digitalRead(mode))
  {
    if (analogRead(action) > 700)
    {
      if (actionHold == false)
      {
        actionHold = true;
        holdTime = millis();
      }
      else if (millis() - holdTime > 4000)
      {
        gameSelect = !(gameSelect);
        delay(1000);
        holdTime = 0;
      }
    }
    else
    {
      actionHold = false;
    }

  // GALAGA **********************************************************************
  if (gameSelect)
  {
    //if(millis() - lastSpawn > random(1,2)*1000+(random(0,3)*random(250,750)))
    if(millis() - lastSpawn > random(1,2)*1000+(random(0,2)*random(100,500)))
    {
      lastSpawn = millis();
      addEnemies();
    }
    
    //if enemies matrix is empty you win!!
    if(win())
    {
      cheer();
      newGame();
    }
      
    loadEnemies();
  
    resetMatrix();
    
    if(analogRead(left) > 700 && (millis() - lastTime) > 125)
    {
      lastTime = millis();
      moveLeft();
    }
    if(analogRead(right) > 700 && (millis() - lastTime) > 125)
    {
      lastTime = millis();
      moveRight();
    }
    if(analogRead(action) > 700 && (millis() - lastTime) > 150)
    {
      lastTime = millis();
      fire();
    }
    loadBattleShip();
    resetMatrix();
  }
  // TETRIS ************************************************************
  else
  {
    if(analogRead(left) > 700 && (millis() - lastTime) > 250)
    {
      lastTime = millis();
      moveLeftTETRIS();
    }
    if(analogRead(right) > 700 && (millis() - lastTime) > 250)
    {
      lastTime = millis();
      moveRightTETRIS();
    }
    if(analogRead(rotate) > 700 && (millis() - lastTime) > 250)
    {
      lastTime = millis();
      rotateShape();
    }
  
    refreshScreen();
  
    // Will move shape down every 2 sec
    if (millis() - moveTime > 700)
    {
      // Will move shape down every 2 sec
      moveTime = millis();
      if ( moveDown() == false || firstTime == true)
      {
        firstTime = false;
        cancelBlocks();
        addShape();
      }
      refreshScreen();
    }
  }
  }
  // EMOJI **********************************************************
  else
  {
    if(analogRead(left) > 700 && (millis() - lastTime) > 200)
    {
      lastTime = millis();
      if(emoji > 0) emoji--;
    }
    if(analogRead(right) > 700 && (millis() - lastTime) > 200)
    {
      lastTime = millis();
      if(emoji < 9) emoji++;
    }
    print(emoji);
  }
}



void resetMatrix()
{
  for(int i = 0; i < 8; i++)
  {
    digitalWrite(rows[i],HIGH);
    digitalWrite(columns[i],LOW);
  }
}

void write(int r, int c)
{
  digitalWrite(rows[r], LOW);
  digitalWrite(columns[c], HIGH);
}


void loadEnemies()
{
  for(int i = 0; i < 3; i++)
  {
    for(int j = 1; j < 7; j++)
    {
      if((hitMatrix[i][j]) == 0)
      {
        write(i,j);
        delayMicroseconds(300);
        resetMatrix();
        enemies[i][j] = {1};
      }
      else
      {
        enemies[i][j] = {0};
      }
    }
  }
}

void loadBattleShip()
{
  for (int i = 0; i < 3; i++)
  {
    write(7,ship[i]);
    delayMicroseconds(300);
    resetMatrix();
  }
  write(6,ship[1]);
  delayMicroseconds(300);
  resetMatrix();
}


void moveRight()
{
  if (ship[2] < 7)
  {
    for(int i = 0; i < 3; i++)
    {
      ship[i]++;
    }
  }
}


void moveLeft()
{
  if (ship[0] > 0)
  {
    for(int i = 0; i < 3; i++)
    {
      ship[i]--;
    }
  }
}

void fire()
{
  for (int i = 5; i > -1; i--)
  {
    write(i,ship[1]);
    if((enemies[i][ship[1]]) == 1)
    {
//      Serial.print("Enemey hit: ");
//      Serial.print("enemies[i][ship[1]]): ");
//      Serial.println(i);
      hitMatrix[i][ship[1]] = 1;
      i = -1; //can only hit one enemy block at once
    }
    resetMatrix();
    //Serial.println(ship[1]);
    loadEnemies();
    resetMatrix();
    loadBattleShip();
    resetMatrix();
  }
  
//  for (int i = 0; i < 8; i++)
//  {
//    for (int j = 0; j < 8; j++)
//    {
//      Serial.print(hitMatrix[i][j]);
//      Serial.print("\t");
//    }
//    Serial.println();
//  }
}


void hit()
{
//  matrix[curRow][curColumn] = 1;
}


void enemyFire()
{
  int row = random(0,6)+1;
  int col = random(0,6)+1;
}


void addEnemies()
{
  for(int i = 0; i < 3; i++)
  {
    for(int j = 1; j < 7; j++)
    {
      if(enemies[i][j] == 0 && (random(1,100)%7) == 0)
      {
        enemies[i][j] = {1};
        hitMatrix[i][j] = 0;
      }
      else
      {
        //do nothing
      }
    }
  }
}

bool win()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (enemies[i][j] != 0) return 0;
        }
    }
    return 1;
}


void cheer()
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      write(i,j);
      delay(10);
    }
  }
}


void newGame()
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      hitMatrix[i][j] = 0;
    }
  }

  ship[0] = 2;
  ship[1] = 3;
  ship[2] = 4;
}



void print(String str, uint8_t wait)
{
  int arr[] = {7, 6, 5, 4, 3, 2, 1, 0};
  for (int k = 0; k < str.length(); k++)
  {
    for (int q = 0; q < wait; q++)
    {
      for (int i = 0; i < 8; i++)
      {
        for (int m = 0; m < 8; m++)
        {
          if(bitRead((ASCII[str.charAt(k) - ' '])[i], m))
          {
            write(arr[m],i);
            delayMicroseconds(550);
            resetMatrix();
          }
        }
      }
    }
  }
}


void print(int num)
{
  int arr[] = {7, 6, 5, 4, 3, 2, 1, 0};
  for (int i = 0; i < 8; i++)
  {
    for (int m = 0; m < 8; m++)
    {
      if(bitRead((Emoji[num])[i], m))
      {
        write(arr[m],i);
        delayMicroseconds(200);
        resetMatrix();
      }
    }
  }
}



void addShape()
{
  // check if user has lost game
  // Won't add a shape if the user has lost
  for(int i = 0; i < 8; i++)
  {
    if (playField[0][i] == 1)
    {
      endGame();
      break;
    }
  }
  
  //selectShape
  /*
  double selection = 0;
  int sumProb = 0;
  for (int i = 0; i < numShapes; i++)
  {
    selection += ( (double)random(0,100) / 100.0) * shapeProb[i];
    sumProb += shapeProb[i];
  }
  int shape = (int) (selection / sumProb);
  */

  int shape = random(0, 7);
  switch(shape) {
    case 0:
      //Shape 'I'
      activeShape[0] = 0; activeShape[1] = 2;  //ShapeID; Anchor Coordinate(1 2 3 or 4)
      activeShape[2] = 0; activeShape[3] = 2;  //Block 1 (r1; c1)
      activeShape[4] = 0; activeShape[5] = 3;  //Block 2 (r2; c2)
      activeShape[6] = 0; activeShape[7] = 4;  //Block 3 (r3; c3)
      activeShape[8] = 0; activeShape[9] = 5;  //Block 4 (r4; c4)
      break;
    case 1:
      //Shape 'O'
      activeShape[0] = 1; activeShape[1] = 1;  //ShapeID; Anchor Coordinate(1 2 3 or 4)
      activeShape[2] = 0; activeShape[3] = 3;  //Block 1 (r1; c1)
      activeShape[4] = 0; activeShape[5] = 4;  //Block 2 (r2; c2)
      activeShape[6] = 1; activeShape[7] = 3;  //Block 3 (r3; c3)
      activeShape[8] = 1; activeShape[9] = 4;  //Block 4 (r4; c4)
      break;
    case 2:
      //Shape 'T'
      activeShape[0] = 2; activeShape[1] = 2;  //ShapeID; Anchor Coordinate(1 2 3 or 4)
      activeShape[2] = 0; activeShape[3] = 2;  //Block 1 (r1; c1)
      activeShape[4] = 0; activeShape[5] = 3;  //Block 2 (r2; c2)
      activeShape[6] = 0; activeShape[7] = 4;  //Block 3 (r3; c3)
      activeShape[8] = 1; activeShape[9] = 3;  //Block 4 (r4; c4)
      break;
    case 3:
      //Shape 'S'
      activeShape[0] = 3; activeShape[1] = 1;  //ShapeID; Anchor Coordinate(1 2 3 or 4)
      activeShape[2] = 0; activeShape[3] = 3;  //Block 1 (r1; c1)
      activeShape[4] = 0; activeShape[5] = 4;  //Block 2 (r2; c2)
      activeShape[6] = 1; activeShape[7] = 2;  //Block 3 (r3; c3)
      activeShape[8] = 1; activeShape[9] = 3;  //Block 4 (r4; c4)
      break;
    case 4:
      //Shape 'Z'
      activeShape[0] = 4; activeShape[1] = 2;  //ShapeID; Anchor Coordinate(1 2 3 or 4)
      activeShape[2] = 0; activeShape[3] = 2;  //Block 1 (r1; c1)
      activeShape[4] = 0; activeShape[5] = 3;  //Block 2 (r2; c2)
      activeShape[6] = 1; activeShape[7] = 3;  //Block 3 (r3; c3)
      activeShape[8] = 1; activeShape[9] = 4;  //Block 4 (r4; c4)
      break;
    case 5:
      //Shape 'J'
      activeShape[0] = 5; activeShape[1] = 2;  //ShapeID; Anchor Coordinate(1 2 3 or 4)
      activeShape[2] = 0; activeShape[3] = 2;  //Block 1 (r1; c1)
      activeShape[4] = 0; activeShape[5] = 3;  //Block 2 (r2; c2)
      activeShape[6] = 0; activeShape[7] = 4;  //Block 3 (r3; c3)
      activeShape[8] = 1; activeShape[9] = 4;  //Block 4 (r4; c4)
      break;
    case 6:
      //Shape 'L'
      activeShape[0] = 6; activeShape[1] = 2;  //ShapeID; Anchor Coordinate(1 2 3 or 4)
      activeShape[2] = 0; activeShape[3] = 2;  //Block 1 (r1; c1)
      activeShape[4] = 0; activeShape[5] = 3;  //Block 2 (r2; c2)
      activeShape[6] = 0; activeShape[7] = 4;  //Block 3 (r3; c3)
      activeShape[8] = 1; activeShape[9] = 2;  //Block 4 (r4; c4)
      break;
    default:
      Serial.println("ERROR in addShape");
  }
  refreshScreen();
}

void refreshScreen()
{
  resetMatrix();

  // Updates the play field
/*
  for(int i = 0; i < 8; i++)
  {
    // Updates row i
    digitalWrite(rows[i],LOW);
    
    for(int j = 0; j < 8; j++)  {
      if (playField[i][j] == 1)   {
        digitalWrite(columns[j],HIGH);
      }
    }

    // Display active shape - Option 1
    //**************************************************
    /*
    for(int k = 2; k < 10; k += 2)  {
      if (activeShape[k] == i)
      {
        digitalWrite(columns[ activeShape[k + 1] ],HIGH);
      }
    }
    
    //**************************************************
    
    delayMicroseconds(300);

    digitalWrite(rows[i],HIGH);
    for(int j = 0; j < 8; j++)
      digitalWrite(columns[j], LOW);

  }
*/
  // Display active, shape - Option 2
  //***************************************************
  for(int i = 2; i < 10; i += 2)
  {
    if (activeShape[i] < 0)
      continue;
      
    write(activeShape[i], activeShape[i+1]);
    delayMicroseconds(300);
    resetMatrix();
  }
  //***************************************************

  //Display playField - Option 2
  //***************************************************
  for(int i = 0; i < 8; i++)
  {
    for(int j = 0; j < 8; j++)
    {
      if (playField[i][j] == 1)
      {
        digitalWrite(rows[i],LOW);
        digitalWrite(columns[j],HIGH);
        delayMicroseconds(300);
        digitalWrite(rows[i],HIGH);
        digitalWrite(columns[j],LOW);
        
      }
    }
  }
}

boolean moveDown()
{
  boolean move = true;
  for(int i = 2; i < 10; i += 2)
  {
    if ( activeShape[i] < 0)
      continue;
    if ( (playField[ activeShape[i] + 1 ][ activeShape[i + 1] ] == 1) ||
         (activeShape[i] + 1 == 8) )
    {
      move = false;
    }
  }
  
  if (move == true)
  {
    for(int i = 2; i < 10; i += 2)
    {
      activeShape[i] += 1;
    }
  }
  else
  {
    for(int i = 2; i < 10; i += 2)
    {
      playField[ activeShape[i] ][ activeShape[i + 1] ] = 1;
    }
  }
  return move;
}

void cancelBlocks()
{
  int i = 7;
  while (i >= 0)
  {
    boolean rowDelete = true;
    for (int j = 0; j < 8; j++)
    {
      if (playField[i][j] == 0)
      {
        rowDelete = false;
        break;
      }
    }
    if (rowDelete == true)
    {
      for(int m = i; m > 0; m--)
      {
        for(int n = 0; n < 8; n++)
        {
          playField[m][n] = playField[m - 1][n];
        }
      }
      playField[0][0] = 0; playField[0][1] = 0; playField[0][2] = 0;
      playField[0][3] = 0; playField[0][4] = 0; playField[0][5] = 0;
      playField[0][6] = 0; playField[0][7] = 0;
    }
    else
    {
      i--;
    }
  }
}

bool moveLeftTETRIS()
{
  boolean move = true;
  for(int i = 2; i < 10; i += 2)
  {
    if ( activeShape[i] < 0 )
      continue;
    if ( (activeShape[i + 1] - 1 == -1) || (playField[ activeShape[i] ][ activeShape[i + 1] - 1 ] == 1) )
    {
      move = false;
    }
  }
  if (move == true)
  {
    for(int i = 2; i < 10; i += 2)
    {
      activeShape[i + 1] -= 1;
    }
    return true;
  }
  else
    return false;
}

bool moveRightTETRIS()
{
  boolean move = true;
  for(int i = 2; i < 10; i += 2)
  {
    if ( activeShape[i] < 0 )
      continue;
    if ( (activeShape[i + 1] + 1 == 8) || (playField[ activeShape[i] ][ activeShape[i + 1] + 1 ] == 1) )
    {
      move = false;
    }
  }
  if (move == true)
  {
    for(int i = 2; i < 10; i += 2)
    {
      activeShape[i + 1] += 1;
    }
    return true;
  }
  else
    return false;
}

void rotateShape()
{
  if (activeShape[0] == 1)
    return;
  
  // Will rotateShape clockwise relative to anchor block
  int anchorRow = activeShape[activeShape[1] * 2];
  int anchorCol = activeShape[activeShape[1] * 2 + 1];
  int temp, level = 0;

  for(int i = 2; i < 10; i+= 2)
  {
    if (activeShape[i] > level)
      level = activeShape[i];
  }
  
  int tempShape[10];
  tempShape[0] = activeShape[0];
  tempShape[1] = activeShape[1];
  tempShape[2] = activeShape[2];
  tempShape[3] = activeShape[3];
  tempShape[4] = activeShape[4];
  tempShape[5] = activeShape[5];
  tempShape[6] = activeShape[6];
  tempShape[7] = activeShape[7];
  tempShape[8] = activeShape[8];
  tempShape[9] = activeShape[9];
  
  // Loop through and check each block
  for (int i = 2; i < 10; i += 2)
  {
    if (i != tempShape[1] * 2)  // If not the anchor block
    {
      // This logic simply flips the x/y coordinates realtive to the anchor block
      temp = (tempShape[i + 1] - anchorCol) + anchorRow;
      tempShape[i + 1] = (anchorRow - tempShape[i]) + anchorCol;
      tempShape[i]     = temp;
    }

    
    if ((tempShape[i + 1] < 0) || (tempShape[i + 1] > 7) || (tempShape[i] > 7) ||
        (tempShape[i] >= 0 && (playField[ tempShape[i] ][ tempShape[i + 1] ] == 1)))
      {
        // Shape rotateShape invalid if it will end up out of bounds or contact another block
        return;
      }
  }
     
  int k = 2;
  while (k < 10)
  {
    if (tempShape[k] > level)
    {
      for (int j = 2; j < 10; j += 2)  {
        tempShape[j]--;
      }
    }
    else
      k += 2;
  }
  
  for (int i = 0; i < 10; i++)
  {
    activeShape[i] = tempShape[i];
  }
}

void endGame()
{
  delay(2000);
  
  for(int i = 0; i < 8; i++)
  {
    for(int j = 0; j < 8; j++)
    {
      playField[i][j] = 0;
    }
  }
  activeShape[0] = 0; activeShape[1] = 0;  //ShapeID; Anchor Coordinate(1 2 3 or 4)
  activeShape[2] = 0; activeShape[3] = 0;  //Block 1 (r1; c1)
  activeShape[4] = 0; activeShape[5] = 0;  //Block 2 (r2; c2)
  activeShape[6] = 0; activeShape[7] = 0;  //Block 3 (r3; c3)
  activeShape[8] = 0; activeShape[9] = 0;  //Block 4 (r4; c4)
  
  refreshScreen();

  delay(2000);
}
