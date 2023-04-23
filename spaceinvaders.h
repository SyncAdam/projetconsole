#include "Adafruit_ILI9341.h"

#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 11
#define UP 4    //up button pin
#define DOWN 6  //down button pin
#define RIGHT 7 //right button pin
#define LEFT 3  //left button pin
#define BACK 5  //back button
#define SIaliendistance 20
#define shipspeed 50
#define shipUpdateSpeed 100
#define shootUpdateSpeed  600
#define bulletUpdateSpeed 50
#define bulletSpeed 10
#define projectilew 3
#define projectileh 10
#define alienBurstUpdateTime 100
#define alienBurstUpdateFactor 11
#define rightBoundary 228
#define leftBoundary 0
#define topBoundary 0
#define bottomBoundary 300
#define alienHStep 5
#define alienVStep 5

//Space invaders

typedef struct SpaceShip
{
  short x;
  short dx;
  short y;
  uint16_t color;
  bool texture[8][13];
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
  short type;     //0, 1 or 2
  uint16_t color;
  bool texture[8][12];
} alien;

void drawSpaceShip(Adafruit_ILI9341 screen, spaceShip *ship)
{

  if(ship->dx != ship->x)
  { 

    for(int i = 0; i < 13; i++)
    {
      for(int j = 0; j < 8; j++)
      {
        if(ship->texture[j][i])
        {
          screen.drawPixel(ship->dx + i, ship->y + j, ILI9341_BLACK);
        }     
      }
    }

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

void moveSpaceShip(SpaceShip* ship, bool left, bool right, float timeDelta)
{
  ship->dx = ship->x;
  if(left && ship->x - shipspeed * timeDelta >= leftBoundary) ship->x -= shipspeed * timeDelta;
  else if (right && ship->x + shipspeed * timeDelta <= rightBoundary) ship->x += shipspeed * timeDelta;
}

void moveProjectiles(Projectile **ps, int *index)
{
    for(int i = 0; i < *index; i++)
    {
      ps[i]->dy = ps[i]->y;
      if(ps[i]->y < 0)
      {
        delete(ps[i]);
        for(int j = i; j < *index; j++)
        {
          ps[j] = ps[j+1];
        }
        (*index)--;
      }  
      else
      {
        ps[i]->y -= bulletSpeed;
      }
    }
}

void printProjectiles(Projectile **ps, int index, Adafruit_ILI9341 screen)
{
  for(int i = 0; i < index; i++)
  {
    screen.fillRect(ps[i]->x, ps[i]->dy, projectilew, projectileh, ILI9341_BLACK);
  }
  for(int i = 0; i < index; i++)
  {
    screen.fillRect(ps[i]->x, ps[i]->y, projectilew, projectileh, ps[i]->color);
  }
}

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

void drawAliens(bool daliens[6][8][12], Alien **aliens, int start, int finish, bool s, Adafruit_ILI9341 screen)
{
     
    for(int k = start; k < finish; k++)
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
    for(int k = start; k < finish; k++)
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

void moveAliens(Alien **aliens, int num, bool *movingRight)
{
  bool movedown = false;
  for(int i = 0; i < num; i++)
  {
    if(aliens[i]->x + alienHStep >= rightBoundary && *movingRight || aliens[i]->x - alienHStep <= leftBoundary && !*movingRight) movedown = true;
  }
  for(int i = 0; i < num; i++)
  {
    if(movedown)
    {
      aliens[i]->dx = aliens[i]->x;
      aliens[i]->dy = aliens[i]->y;
      aliens[i]->y += alienVStep;
      *movingRight = !*movingRight;
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

void drawMenu(Adafruit_ILI9341 screen)
{
  screen.fillScreen(ILI9341_BLACK);
}

void SI(Adafruit_ILI9341 screen)
{

  drawMenu(screen);

  bool up = digitalRead(UP);
  bool left = digitalRead(LEFT);
  bool right = digitalRead(RIGHT);
  bool back = digitalRead(BACK);

  Projectile **projectiles = new Projectile*[10];
  int ptrindex = 0;

  unsigned int aliensNumber = 55;
  Alien **aliens = new Alien*[aliensNumber];
  for(int i = 0; i < aliensNumber; i++)
  {
    aliens[i] = new Alien;
  }

  generateAliens(aliens, aliensNumber);


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

    unsigned long start = millis();
    unsigned long currentT = millis();
    unsigned long shipUpdate = 0;
    unsigned long shootUpdate = 0;
    unsigned long bulletUpdate = 0;
    unsigned long alienUpdate = 0;
    unsigned long alienBurstUpdate = 0;
    unsigned int alienUpdateTime = 1500;
    unsigned short alienBurstUpdateIndex = 0;
    bool enableBurstUpdate = false;
    bool aliensMovingRight = true;

    bool shiptexture[8][13]  = {{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
                                {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
                                {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
                                {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
                                {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
                                {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
                                {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
                                {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}};

    spaceShip* ship = static_cast<spaceShip*> (malloc(sizeof(spaceShip)));
    if(ship != NULL)
    {
      ship->x = 50;
      ship->dx = 10;
      ship->y = 300;
      ship->color = ILI9341_GREEN;
      for(int i = 0; i < 8; i++)
      {
        for(int j = 0; j < 13; j++)
        {
          ship->texture[i][j] = shiptexture[i][j];
        }
      }
    }
    

  while(!back){

      up = digitalRead(UP);
      left = digitalRead(LEFT);
      right = digitalRead(RIGHT);
      back = digitalRead(BACK);

      drawSpaceShip(screen, ship);

      // Check if it's time to update the position
      currentT = millis();
      if (currentT - shipUpdate >= shipUpdateSpeed)
      {
          moveSpaceShip(ship, left, right, (currentT - shipUpdate) / 1000.0);
          drawSpaceShip(screen, ship);
          screen.fillRect(0, 0, 60, 60, ILI9341_BLACK);
          screen.setTextColor(ILI9341_WHITE);
          screen.setCursor(0, 30);
          screen.print(ship->x);
          screen.setCursor(0, 60);
          screen.print(aliens[10]->x);
          shipUpdate = currentT;
      }

      //check if we can shoot, if so add a projectile to projectiles vector
      if (currentT - shootUpdate >= shootUpdateSpeed)
      {
        if(up)
        {
          Projectile* p = new projectile;
          p->x = ship->x + 5;
          p->y = 290;
          p->dy = 0;
          p->color = ILI9341_WHITE;
          projectiles[ptrindex] = p;
          ptrindex++;
          shootUpdate = millis();
        }
      }

      //Moving and drawing projectiles
      if (currentT - bulletUpdate >= bulletUpdateSpeed)
      {
        moveProjectiles(projectiles, &ptrindex);   //passed as reference
        printProjectiles(projectiles, ptrindex, screen);
        bulletUpdate = millis();
      }

      //Every second and a half, aliens are moved and redrawn
      if(currentT - alienUpdate >= alienUpdateTime)
      {
        alienUpdate = millis();
        moveAliens(aliens, aliensNumber, &aliensMovingRight);
        enableBurstUpdate = true;
      }
      if(enableBurstUpdate && currentT - alienBurstUpdate >= alienBurstUpdateTime)
      {
        if(alienBurstUpdateIndex == (aliensNumber % alienBurstUpdateFactor == 0 ? aliensNumber / alienBurstUpdateFactor : aliensNumber / alienBurstUpdateFactor + 1))
        {
          alienBurstUpdateIndex = 0;
          enableBurstUpdate = false;
          alienBurstUpdate = millis();
        }
        else
        {
          alienBurstUpdate = millis();
          drawAliens(daliens, aliens, aliensNumber - (alienBurstUpdateIndex + 1) * alienBurstUpdateFactor,aliensNumber - alienBurstUpdateIndex * alienBurstUpdateFactor, 0, screen);
          alienBurstUpdateIndex++;
        }
      }

      /*drawAlien();
      moveAlien();
      fireBullet();
      moveBullet();
      checkCollisions();
      */
  }
  free (ship);
}