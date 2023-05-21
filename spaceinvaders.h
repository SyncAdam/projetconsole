/*
  An implementation of Space Invaders for a screen with the ILI9341 lcd driver and arduino created by Adam Sinkovics
*/

//Include directories
#include "Adafruit_ILI9341.h"

//Defining constants maybe define them as const ints, names shall imply the use
#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 11
#define UP 4                            //up button pin
#define DOWN 6                          //down button pin
#define RIGHT 7                         //right button pin
#define LEFT 3                          //left button pin
#define BACK 5                          //back button
#define shipspeed 50
#define shipUpdateSpeed 100             //Update speed in milliseconds, meaning the period of refreshing the element
#define shootUpdateSpeed 600
#define alienShootUpdateSpeed 1200
#define bulletUpdateSpeed 50
#define collisionUpdateSpeed 50          //For some reason I changed the name from speed to time
#define alienBurstUpdateTime 100        //I dont know why, it could lead to confusion later
#define alienQuickUpdateTime 200
#define bulletSpeed 10
#define projectilew 3
#define projectileh 10
#define alienBurstUpdateFactor 11       //An interesting implementation to update aliens will be explained later
#define rightBoundary 228               //screen boundaries
#define leftBoundary 0
#define topBoundary 0
#define bottomBoundary 320              
#define alienHStep 5                    //aliens horizontal and vertical step size in pixels
#define alienVStep 5 
#define nAliens 55
 

//Defining 3 structs as types SpaceShip, Projectile and Aliens
//These are the 3 main objects in the game
//Each one of them has x and y positions as well as dx and/or dy to indicate the last position
typedef struct SpaceShip
{
  short x;
  short dx;
  short y;
  uint16_t color;
  short lives;            //It do be short btw
  bool texture[8][13];    //To store spaceShip texture
} spaceShip;

typedef struct Projectile
{
  short x;
  short y;
  short dy;
  uint16_t color;
} projectile;

typedef struct Alien
{
  short x;
  short dx;
  short y;
  short dy;
  short type;               //0, 1 or 2
  uint16_t color;
  bool texture[8][12];      //To store alien texture
} alien;

//======================================
//Screen printing functions
//======================================

//Print any text to the top left corner, used mostly for debugging since Arduino Due does not support debugging by default
void printText(String text, Adafruit_ILI9341 screen)
{
  screen.setTextSize(1);
  screen.fillRect(0, 0, 230, 30, ILI9341_BLACK);
  screen.setCursor(0, 0);
  screen.print(text);
}

void printGameOverScreen(Adafruit_ILI9341 screen)
{
  screen.fillScreen(ILI9341_BLACK);
  screen.setCursor(50, 150);
  screen.setTextSize(3);
  screen.setTextColor(ILI9341_RED);
  screen.print("GAME OVER");
}

//Print the remaining lives to the top right corner
void printLives(SpaceShip * ship, Adafruit_ILI9341 screen)
{
  screen.fillRect(150, 0, 120, 10, ILI9341_BLACK);

  screen.setCursor(180, 0);
  screen.setTextSize(2);
  screen.setTextColor(ILI9341_WHITE);
  screen.print("HP:");
  screen.print(ship->lives);
}

//Refresh the players score, called each time a collision is detected between an alien and a projectile shot by the player
void refreshScore(int score, Adafruit_ILI9341 screen)
{
  screen.setTextSize(2);
  screen.setCursor(0, 0);
  screen.print("SCORE :");
  screen.fillRect(90, 0, 60, 14, ILI9341_BLACK);
  screen.setCursor(90, 0);
  screen.print(score);
}

//======================================
//Screen object drawing functions
//======================================


void drawSpaceShip(Adafruit_ILI9341 screen, spaceShip *ship)
{

  if(ship->dx != ship->x)
  { 

    for(int i = 0; i < 13; i++)
    {
      for(int j = 0; j < 8; j++)
      {
        if(ship->texture[j][i])
        {                                                               //In this loop the spaceships old position is redrawn in black, so it disappears,
          screen.drawPixel(ship->dx + i, ship->y + j, ILI9341_BLACK);   //this only happens if the new position if different from the old one
        }                                                               //Maybe drawing a filled rectangle is faster but i have no efficient way to mesure it
      }
    }

    //These two loops are identical, however it is impossible to combine them, because they might overlap, meaning that I draw
    //a colored pixel which is then deleted
    //this is true for every place where I redraw something

    for(int i = 0; i < 13; i++)
    {
      for(int j = 0; j < 8; j++)
      {
        if(ship->texture[j][i])
        {
          screen.drawPixel(ship->x + i, ship->y + j, ship->color);
        }     
      }
    }
  }
}

//Same principle as drawSpaceShip
//I feel that major optimisation is possible here

void drawAliens(bool daliens[6][8][12], Alien **aliens, int start, int finish, bool s, Adafruit_ILI9341 screen)
{
  
    for(int k = start; k < finish; k++)
    {
      if(aliens[k]->dx != aliens[k]->x || aliens[k]->dy != aliens[k]->y)
      {
        for(int i = 0; i < 12; i++)
        {
          for(int j = 0; j < 8; j++)
          {
            
            if(s == 0) 
            {
              if(daliens[(aliens[k]->type*2)][j][i])
              {
                screen.drawPixel(aliens[k] -> dx + i, aliens[k] -> dy + j, ILI9341_BLACK);
              }
            }
            else if(s == 1)
            {
              if(daliens[(aliens[k]->type*2)+1][j][i])
              {
                screen.drawPixel(aliens[k] -> dx + i, aliens[k] -> dy + j, ILI9341_BLACK);
              } 
            }
          }
        }
      }
    }
    for(int k = start; k < finish; k++)
    {
      if(aliens[k]->dx != aliens[k]->x || aliens[k]->dy != aliens[k]->y)
      {
        for(int i = 0; i < 12; i++)
        {
          for(int j = 0; j < 8; j++)
          {
            if(s == 0) 
            {
              if(daliens[(aliens[k]->type*2)][j][i])
              {
                screen.drawPixel(aliens[k] -> x + i, aliens[k] -> y + j, aliens[k] -> color);
              }
            }
            else if(s == 1)
            {
              if(daliens[(aliens[k]->type*2)+1][j][i])
              {
                screen.drawPixel(aliens[k] -> x + i, aliens[k] -> y + j, aliens[k] -> color);
              } 
            }
          }
        }
      }
    }
}



void printProjectiles(Projectile **ps, int index, Projectile **aps, int aindex, Adafruit_ILI9341 screen)
{
  for(int i = 0; i < index; i++)
  {
    if(ps[i]!= NULL) screen.fillRect(ps[i]->x, ps[i]->dy, projectilew, projectileh, ILI9341_BLACK);
  }
  for(int i = 0; i < index; i++)
  {
    if(ps[i]!= NULL) screen.fillRect(ps[i]->x, ps[i]->y, projectilew, projectileh, ps[i]->color);
  }
  for(int i = 0; i < aindex; i++)
  {
    if(ps[i]!= NULL) screen.fillRect(aps[i]->x, aps[i]->dy, projectilew, projectileh, ILI9341_BLACK);
  }
  for(int i = 0; i < aindex; i++)
  {
    if(ps[i]!= NULL) screen.fillRect(aps[i]->x, aps[i]->y, projectilew, projectileh, aps[i]->color);
  }
}

void drawMenu(Adafruit_ILI9341 screen)
{
  screen.fillScreen(ILI9341_BLACK);
}

//======================================
//Object moving functions
//======================================

//Moving the spaceship based on user input
void moveSpaceShip(SpaceShip* ship, bool left, bool right, float timeDelta)
{
  ship->dx = ship->x;
  if(left && ship->x - shipspeed * timeDelta >= leftBoundary) ship->x -= shipspeed * timeDelta;
  else if (right && ship->x + shipspeed * timeDelta <= rightBoundary) ship->x += shipspeed * timeDelta;
}

//Moving the projectiles
void moveProjectiles(Projectile **ps, int *index, Projectile **aps, int* aindex)
{                              //ps for player projectiles and aps for alien projectiles
    for(int i = 0; i < *index; i++)
    {
      if(ps[i] != NULL)
      {
        ps[i]->dy = ps[i]->y;
        if(ps[i]->y < topBoundary)            //if goes offscreen
        {
          delete(ps[i]);                      //pointer deleted
          for(int j = i; j < *index; j++)
          {
            ps[j] = ps[j+1];                  //array shifted
          }
          (*index)--;                         //Never forget the paranthesis - index decremented
        }  
        else
        {
          ps[i]->y -= bulletSpeed;            //bullet moved up
        }
      }
    }

    for(int i = 0; i < *aindex; i++)        //Same deal for alien projectiles
    {
      aps[i]->dy = aps[i]->y;
      if(aps[i]->y > bottomBoundary)
      {
        delete(aps[i]);
        for(int j = i; j < *aindex; j++)
        {
          aps[j] = aps[j+1];
        }
        (*aindex)--;                        //Never forget the paranthesis
      }  
      else
      {
        aps[i]->y += bulletSpeed;           //bullet moved down
      }
    }
}

void moveAliens(Alien **aliens, int num, bool *movingRight)
{
  bool movedown = false;
  for(int i = 0; i < num; i++)
  {
    if(aliens[i] != NULL)
    {
      if(aliens[i]->x + alienHStep >= rightBoundary && *movingRight || aliens[i]->x - alienHStep <= leftBoundary && !*movingRight) movedown = true;
    }
  }
  if(movedown) *movingRight = !*movingRight;
  for(int i = 0; i < num; i++)
  {
    if(movedown)
    {
      aliens[i]->dx = aliens[i]->x;
      aliens[i]->dy = aliens[i]->y;
      aliens[i]->y += alienVStep;
    }
    else if(*movingRight)
    {
      aliens[i]->dy = aliens[i]->y;
      aliens[i]->dx = aliens[i]->x;
      aliens[i]->x += alienHStep;
    }
    else if(!*movingRight)
    {
      aliens[i]->dy = aliens[i]->y;
      aliens[i]->dx = aliens[i]->x;
      aliens[i]->x -= alienHStep;
    }
  }
}

//======================================
//Alien generator function
//======================================

void generateAliens(Alien **aliens, int aliensNumber)
{
  short nrows = ((aliensNumber % 11 == 0) ? aliensNumber/11 : (aliensNumber/11)+1);
  for(int i = 0; i < nrows; i++)
  {
    for(int j = 0; j < (i == nrows-1 ? (aliensNumber - i*11) : 11); j++)
    {
      aliens[i * 11 + j] -> x = j * 20 + 10;
      aliens[i * 11 + j] -> dx = 0;
      aliens[i * 11 + j] -> y = i * 20 + 20;
      aliens[i * 11 + j] -> dy = 0;
      aliens[i * 11 + j] -> type = i % 3;
      if(i % 3 == 0) aliens[i * 11 + j] -> color = ILI9341_GREEN;
      else if(i % 3 == 1) aliens[i * 11 + j] -> color = ILI9341_YELLOW;
      else aliens[i * 11 + j] -> color = ILI9341_PURPLE;
     
    }
  }
}

//======================================
//Collision detection function
//======================================



void checkCollisions(SpaceShip * ship, Projectile ** ps, int *pind, Projectile **aps, int *apind, Alien ** aliens, bool daliens[6][8][12], int *aliensNumber, Adafruit_ILI9341 screen, bool debug,unsigned int * score, bool * gameover)
{ 
  //First check for projectile and alien collisions
  if(ps != NULL && aliens != NULL)
  { 
    if(debug) printText("if cond good", screen);
    for(short i = 0; i < *pind; i++)
    {
      if(debug) printText("for 1 good", screen);
      for(short j = 0; j < *aliensNumber; j++)
      {
        if(debug)
        {
          printText("for 2 good", screen);
          screen.fillRect(0, 0, 15, 90, ILI9341_BLACK);
          screen.setCursor(0, 30);
          screen.print(i);
          screen.setCursor(0, 60);
          screen.print(j);
        } 

        if((((ps[i]->x) + projectilew > (aliens[j]->x)) && ((aliens[j]->x) + 13 > ps[i]->x)) && (((ps[i]->y) + projectileh > aliens[j]->y) && ((aliens[j]->y) + 8 > ps[i]->y)))
        {
          if(debug)
          {
            printText("Collision detected!", screen);
            delay(1000);
          }
          //delete the projectile from screen
          screen.fillRect(ps[i]->x, ps[i]->y, projectilew, projectileh, ILI9341_BLACK);
          //delete(ps[i]);
          for(int k = i; k < (*pind)-1; k++)
          {
            ps[k] = ps[k+1];
          }

          //delete alien from screen
          screen.fillRect(aliens[j]->x, aliens[j]->y, 13, 8, ILI9341_BLACK);
          screen.fillRect(aliens[j]->dx, aliens[j]->dy, 13, 8, ILI9341_BLACK);

          if(aliens[j]->type == 0) *score += 10;
          else if(aliens[j]->type == 1) *score += 20;
          else if(aliens[j]->type == 2) *score += 30;
          //delete(aliens[j]);
          for(int k = j; k < (*aliensNumber)-1; k++)
          {
            aliens[k] = aliens[k+1];
          }
          (*pind)--;                        //Forgot the paranthesis
          (*aliensNumber)--;
          continue;
        }
      }
    }
  }

  if(aps != NULL)
  {
    for(int i = 0; i < *apind; i++)
    {
      if((((aps[i]->x) + projectilew > (ship->x)) && ((ship->x) + 13 > aps[i]->x)) && (((aps[i]->y) + projectileh > ship->y) && ((ship->y) + 8 > aps[i]->y)))
      {
        ship->lives--;
        printLives(ship, screen);
        if(ship->lives == 0)
        {
          *gameover = true;
          printGameOverScreen(screen);
          delay(2000);
        }
        screen.fillRect(aps[i]->x, aps[i]->y, projectilew, projectileh, ILI9341_BLACK);

        //delete(aps[i]);
        for(int j = i; j < (*apind)-1; j++)
        {
          aps[j] = aps[j+1];
        }
        (*apind)--;                       //Same
        continue;
      }
    }
  }
      //Second check for aliens and spaceship collision
}

void SI(Adafruit_ILI9341 screen)
{

  //Game variables
  bool debugging = false;

  drawMenu(screen);

  bool up = digitalRead(UP);
  bool left = digitalRead(LEFT);
  bool right = digitalRead(RIGHT);
  bool back = digitalRead(BACK);
  unsigned long start = millis();
  unsigned long currentT = millis();
  unsigned long shipUpdate = 0;
  unsigned long shootUpdate = 0;
  unsigned long alienShootUpdate = 0;
  unsigned long alienQuickUpdate = 0;             //Aliens are updated frequently if possible, so that when an other aliens projectiles go through them there isnt a gaping hole in them
  unsigned long bulletUpdate = 0;
  unsigned long alienUpdate = 0;
  unsigned long collisionUpdate = 0;
  unsigned long alienBurstUpdate = 0;
  unsigned int alienUpdateTime = 1500;
  unsigned short alienBurstUpdateIndex = 0;       //aliens are updated in a weird way to avoid major refresh lag
  unsigned int score = 0;
  bool enableBurstUpdate = false;
  bool aliensMovingRight = true;
  bool gameover = false;

  Projectile **projectiles = new Projectile*[10];
  int ptrindex = 0;     //player projectile pointer array size
  Projectile **alienProjectiles = new Projectile*[15];
  int aptrindex = 0;    //alien projectile pointer array size

  int aliensNumber = nAliens;
  int aliensNumberRefreshing = aliensNumber;
  short originalAliensNumber = aliensNumber;

  Alien **aliens = new Alien*[aliensNumber];

  for(int i = 0; i < aliensNumber; i++)
  {
    aliens[i] = new Alien;
  }

  generateAliens(aliens, aliensNumber);

  if(debugging) printText("Aliens ok", screen);
  
  bool daliens[6][8][12] = {
                          {{0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0},
                           {0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
                           {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
                           {0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0},
                           {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                           {0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1},
                           {0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
                           {0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0}},

                          {{0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0},
                           {0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
                           {0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1},
                           {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1},
                           {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
                           {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
                           {0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0},
                           {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1}},

                          {{0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
                           {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
                           {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                           {1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1},
                           {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                           {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
                           {0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0},
                           {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1}},
                           
                          {{0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
                           {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
                           {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                           {1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1},
                           {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                           {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
                           {0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0},
                           {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0}},

                          {{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
                           {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
                           {0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0},
                           {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
                           {0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
                           {0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0},
                           {0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0}},

                          {{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
                           {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
                           {0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0},
                           {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
                           {0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
                           {0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
                           {0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0}}

    };

    bool shiptexture[8][13]  = {{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
                                {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
                                {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
                                {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
                                {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
                                {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
                                {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
                                {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}};

    if(debugging) printText("Vars ok", screen);

    spaceShip* ship = static_cast<spaceShip*> (malloc(sizeof(spaceShip)));
    if(ship != NULL)
    {
      ship->x = 50;
      ship->dx = 10;
      ship->y = 300;
      ship->color = ILI9341_GREEN;
      ship->lives = 3;
      for(int i = 0; i < 8; i++)
      {
        for(int j = 0; j < 13; j++)
        {
          ship->texture[i][j] = shiptexture[i][j];
        }
      }
    }

    if(debugging) printText("Ready", screen);
    screen.setTextSize(2);
    screen.setTextColor(ILI9341_WHITE);
    screen.setCursor(0, 0);
    screen.print("SCORE :");
    printLives(ship, screen);

    drawSpaceShip(screen, ship);
    

  while(!back && !gameover){

      //Read user inputs
      up = digitalRead(UP);
      left = digitalRead(LEFT);
      right = digitalRead(RIGHT);
      back = digitalRead(BACK);

      // Check if it's time to update the ships position
      currentT = millis();      //read current time
      if (currentT - shipUpdate >= shipUpdateSpeed)       //if time to update
      {
          if(debugging)
          {
            screen.fillRect(0, 0, 60, 60, ILI9341_BLACK);
            screen.setTextColor(ILI9341_WHITE);
            screen.setCursor(0, 30);
            printText("ShipUpdating", screen);
            screen.print(ship->x);
          } 

          moveSpaceShip(ship, left, right, (currentT - shipUpdate) / 1000.0);   //move ship
          drawSpaceShip(screen, ship);                                          //redraw ship
          shipUpdate = currentT;                                                //refrst last update time

          if(debugging) printText("ShipUpdated", screen);
      }

      //check if we can shoot, if so add a projectile to projectiles vector ATTENTION MAX 10
      if (currentT - shootUpdate >= shootUpdateSpeed)
      {
        if(up)
        {
          if(debugging) printText("ShootUpdating", screen);
          Projectile* p = new projectile;                       //new projectile pointer
          p->x = ship->x + 5;     
          p->y = 290;
          p->dy = 0;
          p->color = ILI9341_WHITE;
          projectiles[ptrindex] = p;                            //add projectile pointer to projectile pointer array
          ptrindex++;                                           //increment the arrays index/size (its more like a size)
          shootUpdate = millis();
          if(debugging) printText("ShootUpdated", screen);
        }
      }

      //Check if aliens can shoot or not ATTENTION MAX 15
      if (currentT - alienShootUpdate >= alienShootUpdateSpeed)
      {
        if(debugging) printText("AlienShootUpdating", screen);
        if(aliensNumber > 0)
        {
          Projectile* p = new projectile;                         //Same principle as for the ships projectile, except that the projectile is
          int foobar = (int)random(aliensNumber);                 //<==shot from a random ship
          p->x = aliens[foobar]->x + 6;
          p->y = aliens[foobar]->y;
          p->dy = 0;
          p->color = ILI9341_WHITE;
          alienProjectiles[aptrindex] = p;                        //alien and player projectiles are handled separately
          aptrindex++;
          alienShootUpdate = millis();
        } 
        if(debugging) printText("AlienShootUpdated", screen);
      }

      //Moving and drawing projectiles
      if (currentT - bulletUpdate >= bulletUpdateSpeed)
      {
        if(debugging) printText("BulletUpdating", screen);
        moveProjectiles(projectiles, &ptrindex, alienProjectiles, &aptrindex);   //passed as reference ptrindex and aptrindex
        printProjectiles(projectiles, ptrindex, alienProjectiles, aptrindex, screen);
        bulletUpdate = millis();
        if(debugging) printText("BulletUpdate", screen);
      }

      //Every second and a half, aliens are moved and redrawn
      if(currentT - alienUpdate >= alienUpdateTime)
      {
        if(debugging) printText("AlienGlobalUpdating", screen);
        alienUpdate = millis();
        moveAliens(aliens, aliensNumber, &aliensMovingRight);
        
        if(aliensNumber < 2 * originalAliensNumber / 3 && aliensNumber > originalAliensNumber / 3) alienUpdateTime = 750;
        else if(aliensNumber < originalAliensNumber / 3) alienUpdateTime = 500;
        if(debugging) printText("AlienGlobalUpdated", screen);
      }

      if(currentT - alienQuickUpdate >= alienQuickUpdateTime)
      {
        alienQuickUpdate = millis();
        enableBurstUpdate = true;
        aliensNumberRefreshing = aliensNumber;
      }

      //For an alien burst update I take the number of aliens and basically divide it into parts based on a burstupdate factor
      //for exemple if there are 50 aliens and the factor is 10, the 50 aliens are divided into groups of ten, and groups are 
      //refreshed in different game loop iterations. in our exemple first we update the last 10 (from 40 to 50) then the next 10 (30 to 40) until
      //the last one is refreshed. thus between the aliens being refreshed the player can move or shoot, so the update is less noticable in the gameplay
      if(enableBurstUpdate && currentT - alienBurstUpdate >= alienBurstUpdateTime)
      {
        if(debugging)
        {
          printText("AlieniUpdating", screen);
          screen.fillRect(0, 30, 10, 10, ILI9341_BLACK);
          screen.setCursor(0, 30);
          screen.print(alienBurstUpdateIndex);
        }

        
        //if the burstUpdateIndex is the largest it can get we set it back to 0 and disable burst updating
        if(alienBurstUpdateIndex == (aliensNumberRefreshing % alienBurstUpdateFactor == 0 ? aliensNumberRefreshing / alienBurstUpdateFactor : aliensNumberRefreshing / alienBurstUpdateFactor + 1))
        {
          if(aliensNumber <= alienBurstUpdateFactor)
          {
            drawAliens(daliens, aliens, 0, aliensNumber, 0, screen);
            enableBurstUpdate = false;
            alienBurstUpdate = millis();
            alienBurstUpdateIndex = 0;
            continue;
          }
          else
          {
            alienBurstUpdateIndex = 0;
            enableBurstUpdate = false;
            alienBurstUpdate = millis();
            aliensNumberRefreshing = aliensNumber;
          }
        }
        //else we update each part, by incrementing the index
        else
        {
          alienBurstUpdate = millis();
          drawAliens(daliens, aliens, aliensNumberRefreshing - (alienBurstUpdateIndex + 1) * alienBurstUpdateFactor < 0 ? 0 : aliensNumberRefreshing - (alienBurstUpdateIndex + 1) * alienBurstUpdateFactor,aliensNumberRefreshing - alienBurstUpdateIndex * alienBurstUpdateFactor, 0, screen);
          alienBurstUpdateIndex++;
        }

    
        if(debugging)
        {
          printText("AlieniUpdated", screen);
          screen.fillRect(0, 30, 10, 10, ILI9341_BLACK);
          screen.setCursor(0, 30);
          screen.print(alienBurstUpdateIndex);
        }
      }

      //Check for collisions between aliens projectiles and the spaceShip
      if(currentT - collisionUpdate >= collisionUpdateSpeed)
      {
        
        checkCollisions(ship, projectiles, &ptrindex, alienProjectiles, &aptrindex, aliens, daliens, &aliensNumber, screen, debugging, &score, &gameover);
        collisionUpdate = millis();
        printLives(ship, screen);
        refreshScore(score, screen);
        
      }

      if(aliensNumber == 0)
      {
        gameover = true;
      }
      
  }
  free (ship);

}