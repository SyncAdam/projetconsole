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
#define shootUpdateSpeed 600
#define bulletUpdateSpeed 50
#define bulletSpeed 10
#define projectilew 3
#define projectileh 10
#define alienBurstUpdateTime 75
#define alienBurstUpdateFactor 11
#define rightBoundary 228
#define leftBoundary 0
#define topBoundary 0
#define bottomBoundary 300
#define alienHStep 5
#define alienVStep 5
#define collisionUpdateTime 50

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

void printText(String text, Adafruit_ILI9341 screen)
{
  screen.setTextSize(1);
  screen.fillRect(0, 0, 230, 30, ILI9341_BLACK);
  screen.setCursor(0, 0);
  screen.print(text);
}

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

void drawMenu(Adafruit_ILI9341 screen)
{
  screen.fillScreen(ILI9341_BLACK);
}

void checkCollisions(SpaceShip * ship, Projectile ** ps, int *pind, Alien ** aliens, bool daliens[6][8][12], int *aliensNumber, Adafruit_ILI9341 screen, bool debug)
{ 
  short* delProj = NULL;
  if(debug) printText("1 var created", screen);
  short* delAlien = NULL;

            short temp1;
          short temp2;

  if(debug) printText("2 vars created", screen);
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
          temp1 = i;
          temp2 = j;
          delProj = &temp1;
          delAlien = &temp2;
          //delete the projectile from screen
          screen.fillRect(ps[i]->x, ps[i]->y, projectilew, projectileh, ILI9341_BLACK);

          //delete alien from screen
          screen.fillRect(aliens[j]->x, aliens[j]->y, 13, 8, ILI9341_BLACK);
          screen.fillRect(aliens[j]->dx, aliens[j]->dy, 13, 8, ILI9341_BLACK);
          continue;
        }
      }
    }
  }

  if(delAlien != NULL && delProj != NULL)
  {
    delete(ps[*delProj]);
    delete(aliens[*delAlien]);

    for(int k = *delProj; k < *pind; k++)
    {
      ps[k] = ps[k+1];
    }
    for(int k = *delAlien; k < *aliensNumber; k++)
    {
      aliens[k] = aliens[k+1];
    }

    (*pind)--;
    (*aliensNumber)--;
  }
      //Second check for aliens and spaceship collision
} 

void SI(Adafruit_ILI9341 screen)
{

  bool debugging = false;

  drawMenu(screen);

  bool up = digitalRead(UP);
  bool left = digitalRead(LEFT);
  bool right = digitalRead(RIGHT);
  bool back = digitalRead(BACK);

  Projectile **projectiles = new Projectile*[10];
  int ptrindex = 0;

  int aliensNumber = 55;
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

    unsigned long start = millis();
    unsigned long currentT = millis();
    unsigned long shipUpdate = 0;
    unsigned long shootUpdate = 0;
    unsigned long bulletUpdate = 0;
    unsigned long alienUpdate = 0;
    unsigned long collisionUpdate = 0;
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

    if(debugging) printText("Vars ok", screen);

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

    if(debugging) printText("Ready", screen);
    

  while(!back){

    //printText("looped", screen);

      up = digitalRead(UP);
      left = digitalRead(LEFT);
      right = digitalRead(RIGHT);
      back = digitalRead(BACK);

      drawSpaceShip(screen, ship);

      // Check if it's time to update the position
      currentT = millis();
      if (currentT - shipUpdate >= shipUpdateSpeed)
      {
          if(debugging)
          {
            printText("ShipUpdating", screen);
            screen.fillRect(0, 0, 60, 60, ILI9341_BLACK);
            screen.setTextColor(ILI9341_WHITE);
            screen.setCursor(0, 30);
            screen.print(ship->x);
          } 

          moveSpaceShip(ship, left, right, (currentT - shipUpdate) / 1000.0);
          drawSpaceShip(screen, ship);
          shipUpdate = currentT;

          if(debugging) printText("ShipUpdated", screen);
      }



      //check if we can shoot, if so add a projectile to projectiles vector
      if (currentT - shootUpdate >= shootUpdateSpeed)
      {
        if(up)
        {
          if(debugging) printText("ShootUpdating", screen);
          Projectile* p = new projectile;
          p->x = ship->x + 5;
          p->y = 290;
          p->dy = 0;
          p->color = ILI9341_WHITE;
          projectiles[ptrindex] = p;
          ptrindex++;
          shootUpdate = millis();
          if(debugging) printText("ShootUpdate", screen);
        }
      }

      //Moving and drawing projectiles
      if (currentT - bulletUpdate >= bulletUpdateSpeed)
      {
        if(debugging) printText("BulletUpdating", screen);
        moveProjectiles(projectiles, &ptrindex);   //passed as reference
        printProjectiles(projectiles, ptrindex, screen);
        bulletUpdate = millis();
        if(debugging) printText("BulletUpdate", screen);
      }

      //Every second and a half, aliens are moved and redrawn
      if(currentT - alienUpdate >= alienUpdateTime)
      {
        if(debugging) printText("AlienGlobalUpdating", screen);
        alienUpdate = millis();
        moveAliens(aliens, aliensNumber, &aliensMovingRight);
        enableBurstUpdate = true;
        aliensNumberRefreshing = aliensNumber;
        if(aliensNumber < 2 * originalAliensNumber / 3 && aliensNumber > originalAliensNumber / 3) alienUpdateTime = 750;
        else if(aliensNumber < originalAliensNumber / 3) alienUpdateTime = 500;
        if(debugging) printText("AlienGlobalUpdated", screen);

      }
      if(enableBurstUpdate && currentT - alienBurstUpdate >= alienBurstUpdateTime)
      {
        if(debugging)
        {
          printText("AlieniUpdating", screen);
          screen.fillRect(0, 30, 10, 10, ILI9341_BLACK);
          screen.setCursor(0, 30);
          screen.print(alienBurstUpdateIndex);
        } 
        if(alienBurstUpdateIndex == (aliensNumber % alienBurstUpdateFactor == 0 ? aliensNumber / alienBurstUpdateFactor : aliensNumber / alienBurstUpdateFactor + 1))
        {
          alienBurstUpdateIndex = 0;
          enableBurstUpdate = false;
          alienBurstUpdate = millis();
          aliensNumberRefreshing = aliensNumber;
        }
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
      if(currentT - collisionUpdate >= collisionUpdateTime)
      {
        if(debugging) printText("CollisionUpdating", screen);
        checkCollisions(ship, projectiles, &ptrindex, aliens, daliens, &aliensNumber, screen, false);
        collisionUpdate = millis();
        if(debugging) printText("CollisionUpdated", screen);
      }
      
  }
  free (ship);
}