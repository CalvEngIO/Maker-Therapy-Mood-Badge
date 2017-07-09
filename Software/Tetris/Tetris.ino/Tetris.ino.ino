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

char shape[7]   = { 'I', 'O', 'T', 'S', 'Z', 'J', 'L' };
int shapeProb[7] = {  1 ,  1 ,  1 ,  1 ,  1 ,  1 ,  1  }; //All equal rate of occurance currently

int activeShape[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // {ShapeID, Anchor Coordinate(1 2 3 or 4), r1, c1, r2, c2, r3, c3, r4, c4}
boolean playField[8][8] = {0};


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
  pinMode(rotate, INPUT);
  
  lastTime = millis();
  addShape();
}


void loop() {
  moveTime = millis();
  
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
  if(analogRead(action) > 700 && (millis() - lastTime) > 250)
  {
    lastTime = millis();
    rotate();
  }

  refreshScreen();

  // Will move shape down every 2 sec
  if (millis() - moveTime > 2000)
  {
    // Will move shape down every 2 sec
    moveTime = millis();
    if ( moveDown() == false )
    {
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
    if playField[0][i] == 1]
    {
      endGame();
      break;
    }
  }
  
  //selectShape
  double selection = 0;
  for (int i = 0; i < length(shape); i++)
  {
    selection += random() * shapeProb[i];
  }
  selection = (int) (selection / sum(shapeProb))

  switch(selection):
    case 0:
      //Shape 'I'
      activeShape = {0, 2, 0, 2, 0, 3, 0, 4, 0, 5};
      break;
    case 1:
      //Shape 'O'
      activeShape = {1, 1, 0, 3, 0, 4, 1, 3, 1, 4};
      break;
    case 2:
      //Shape 'T'
      activeShape = {2, 2, 0, 2, 0, 3, 0, 4, 1, 3};
      break;
    case 3:
      //Shape 'S'
      activeShape = {3, 1, 0, 3, 0, 4, 1, 2, 1, 3};
      break;
    case 4:
      //Shape 'Z'
      activeShape = {4, 2, 0, 2, 0, 3, 1, 3, 1, 4};
      break;
    case 5:
      //Shape 'J'
      activeShape = {5, 3, 0, 2, 0, 3, 0, 4, 1, 4};
      break;
    case 6:
      //Shape 'L'
      activeShape = {6, 1, 0, 2, 0, 3, 0, 4, 1, 2};
      break;
}

void refreshScreen
{
  for(int i = 0; i < 8; i++)
  {
    digitalWrite(rows[i],HIGH);
    digitalWrite(columns[i],LOW);
  }

  // Updates the play field
  for(int i = 0; i < 8; i++)
  {
    for(int j = 0; j < 8; j++)
    {
      if (playField[i][j] == 1)
      {
        digitalWrite(rows[i],LOW);
        digitalWrite(columns[i],HIGH);
      }
    }
  }

  // Updates the active shape
  for(int i = 2; i < 10; i += 2)
  {
    try
    {
      digitalWrite(rows[ activeShape[i] ],LOW);
      digitalWrite(columns[ activeShape[i + 1] ],HIGH);
    }
    // Do I need a catch block?
    // If index out of bounds, then just skip it.  The block still exists
    // it just could have gone off the top of the screen (due to rotation)
  }
}

boolean moveDown()
{
  boolean move = true;
  for(int i = 2; i < 10; i += 2)
  {
    if (playField[ activeShape[i] + 1 ][ activeShape[i + 1] ] == 1) ||
       (activeShape[i] + 1 == 8)
    {
      move = false;
    }
  }
  
  if (move == true)
  {
    for(int i = 3; i < 10; i += 2)
    {
      activeShape[i] += 1;
    }
  }
  else
  {
    for(int i = 3; i < 10; i += 2)
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
      if playField[i][j] == 0)
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
      playField[0] = {0, 0, 0, 0, 0, 0, 0, 0};
    }
    else
    {
      i--;
    }
  }
}

void moveLeft()
{
  boolean move = true;
  for(int i = 2; i < 10; i += 2)
  {
    if (activeShape[i + 1] - 1 == -1) ||
       (playField[ activeShape[i] ][ activeShape[i + 1] - 1 ] == 1)
    {
      move = false;
    }
  }
  if (move == true)
  {
    for(int i = 3; i < 10; i += 2)
    {
      activeShape[i + 1] -= 1;
    }
  }
}

void moveRight()
{
  boolean move = true;
  for(int i = 2; i < 10; i += 2)
  {
    if (activeShape[i + 1] + 1 == 8) ||
       (playField[ activeShape[i] ][ activeShape[i + 1] + 1 ] == 1)
    {
      move = false;
    }
  }
  if (move == true)
  {
    for(int i = 3; i < 10; i += 2)
    {
      activeShape[i + 1] += 1;
    }
  }
}

void rotate()
{
  // Will rotate clockwise relative to anchor block
  anchorRow = activeShape[activeShape[1] * 2];
  anchorCol = activeShape[activeShape[1] * 2 + 1];

  int tempShape[10] = activeShape;
  
  // Loop through and check each block
  for (int i = 2; i < 10, i+= 2)
  {
    if (i != tempShape[1] * 2)  // If not the anchor block
    {
      // This logic simply flips the x/y coordinates realtive to the anchor block
      temp             = (tempShape[i + 1] - anchorCol) + anchorRow;
      tempShape[i + 1] = (anchorRow - tempShape[i]) + anchorCol;
      tempShape[i]     = temp;
    }
    
    if ((tempShape[i + 1] == -1) || (tempShape[i + 1] == 8) ||
        (playField[ tempShape[i] ][ tempShape[i + 1] ] == 1))
    {
      // Shape turn invalid if it will end up out of bounds or contact another block
      return;
    }    
  }

  for (int i = 0; i < length(tempShape); i++)
  {
    activeShape[i] = tempShape[i]
  }
}

void endGame()
{
  delay(6000);
  
  for(int i = 0; i < 8; i++)
  {
    for(int j = 0; j < 8; j++)
    {
      playField[i][j] = 0;
    }
  }
  activeShape = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  
  refreshScreen();

  delay(2000);
}
