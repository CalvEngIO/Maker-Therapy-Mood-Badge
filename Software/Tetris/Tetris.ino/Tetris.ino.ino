// Is Tetris Patented???

/*
 * FILENAME:  Tetris.ino
 * AUTHOR:  Trevor D. Meyer
 * EMAIL: mail2trevorm@gmail.com
 * VERSION: 0.0
 *
 *
 * AFFILIATIONS
 * Calvary Engineering Family Group, USA
 *  A group of electronics and science enthusiasts who enjoy building things
 *
 *
 * DESCRIPTION
 * This source file creates the classic Tetris game for use on the
 * Maker-Therapy-Mood-Badge by Calvary Engineering Family Group, USA
 *
 * UPDATES
 * Version 0.0
 * 2017/07/07:2000>
 *      Started writing the code. 
 *
 *
 * DISCLAIMER
 * This code is in the public domain. Please feel free to use, modify, distribute,
 * etc. as needed, but please give reference to original author as a courtesy to
 * open source developing/-ers.
 *
 * If you find any bugs in the code, or have any questions, please feel free to
 * contact me.
 */

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

//const int pixel[8][8] = { rows, columns }
long lastTime = 0;

int left = A0;
int right = A7;
int rotate = A6;
long moveTime;
boolean firstTime;

int numShapes = 7;
//char shape[numShapes]   = { 'I', 'O', 'T', 'S', 'Z', 'J', 'L' };
int shapeProb[7] = {  1 ,  1 ,  1 ,  1 ,  1 ,  1 ,  1  }; //All equal rate of occurance currently

int activeShape[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // {ShapeID, Anchor Coordinate(1 2 3 or 4), r1, c1, r2, c2, r3, c3, r4, c4}
boolean playField[8][8];


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

  pinMode(left, INPUT);
  pinMode(right, INPUT);
  pinMode(rotateShape, INPUT);

  for(int i = 0; i < 8; i++)
  {
    for(int j = 0; j < 8; j++)
    {
      playField[i][j] = 0;
    }
  }
  
  lastTime = millis();
  moveTime = millis();
  firstTime = true;
}


void loop() {
  
  if(analogRead(left) > 700 && (millis() - lastTime) > 250)
  {
    lastTime = millis();
    moveLeft();
  }
  if(analogRead(right) > 700 && (millis() - lastTime) > 250)
  {
    lastTime = millis();
    moveRight();
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

// The {} block is the anchor block (for rotation)
// I        O          T           S
/*                                 
 *  []      {}[]      []{}[]        {}[]
 *  {}      [][]        []        [][] 
 *  []      
 *  []      
 *
 *
 * Z         J         L
 *                     
 * []{}       []      []
 *   [][]     []      []
 *            []      []
 *          []{}      {}[]
 */

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
  for(int i = 0; i < 8; i++)
  {
    digitalWrite(rows[i],HIGH);
    digitalWrite(columns[i],LOW);
  }

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
    digitalWrite(rows[ activeShape[i] ],LOW);
    digitalWrite(columns[ activeShape[i+1] ],HIGH);
    delayMicroseconds(300);
    digitalWrite(rows[ activeShape[i] ],HIGH);
    digitalWrite(columns[ activeShape[i+1] ],LOW);
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

bool moveLeft()
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

bool moveRight()
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
