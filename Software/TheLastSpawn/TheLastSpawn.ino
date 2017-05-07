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

//const int pixel[8][8] = { rows, columns }
long lastTime = 0;

int hitMatrix[8][8] = {0};

int matrix[8][8] = {0};

int enemies[8][8] = {0};

int left = A0;
int right = A7;
int action = A6;

long lastSpawn = 0;


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
  pinMode(action, INPUT);

  lastTime = millis();
}

void loop()
{
  if(millis() - lastSpawn > random(1,2)*1000+(random(0,3)*random(100,750)))
  {
    lastSpawn = millis();
    addEnemies();
  }
  
  //if enemies matrix is empty you win!!
  
  loadEnemies();
  resetMatrix();
  if(analogRead(left) > 700 && (millis() - lastTime) > 100)
  {
    lastTime = millis();
    moveLeft();
  }
  if(analogRead(right) > 700 && (millis() - lastTime) > 100)
  {
    lastTime = millis();
    moveRight();
  }
  if(analogRead(action) > 700 && (millis() - lastTime) > 200)
  {
    lastTime = millis();
    fire();
  }
  loadBattleShip();
  resetMatrix();
//  loadFire();
//  resetMatrix();
  
  
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
  write(7,ship[0]);
  resetMatrix();
  write(7,ship[1]);
  resetMatrix();
  write(7,ship[2]);
  resetMatrix();
  write(6,ship[1]);
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

