#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "spaceinvaders.h"
#include "pong.h"

#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 11
#define UP 4    //up button pin
#define DOWN 6  //down button pin
#define RIGHT 7 //right button pin
#define LEFT 3  //left button pin
#define BACK 5

/*
  We define a structure for a menu text element containing all the information needed to place it on screen
  !!! Try to make a static function that calculates the x to center the text on screen
*/

typedef struct menutext{
  String text;
  int x;
  int y;
  int size;
  uint16_t color;
} MenuText;

//  ||
//  || Use hardware SPI for DC and CS
//  \/

Adafruit_ILI9341 screen = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);    //TFT_CS / TFT_DC / TFT_RST
uint16_t backgroundColor = ILI9341_RED;

void setup()
{

  //Deselescting all SPI devices
  //Doing this is for some reason necessary, otherwise the screen bugs sometimes.
  pinMode(TFT_CS, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  digitalWrite(TFT_CS, HIGH);


  //Setting the buttons pins to input and the buzzers pin to output 
  pinMode(UP, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(BACK, INPUT);

  //Serial connection to computer for debugging
  //Serial connection to screen
  Serial.begin(9600);
  screen.begin(44000000);

  screen.fillScreen(ILI9341_BLACK);
  diag();

}


//function for screen diagnostics
void diag()
{

  uint8_t x = screen.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = screen.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = screen.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = screen.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = screen.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
   
}


//method that draws the selection rectangle on the main menu
void drawMenuRect(MenuText t)
{
  screen.drawRect(t.x - 4, t.y -4, t.text.length() * 10 + (t.text.length()-1)*2 + 8, 22, ILI9341_WHITE);
}
//Maybe this isnt efficient but how else to do it?
void undrawMenuRect(MenuText t)
{
  screen.drawRect(t.x - 4, t.y -4, t.text.length() * 10 + (t.text.length()-1)*2 + 8, 22, ILI9341_BLACK);
}


//Menu texts
void createMenuElements(MenuText t[])
{
  /*
  t[0].x = 93;
  t[0].y = 140;
  t[0].color = ILI9341_BLUE;
  t[0].size = 2;
  t[0].text = "Snake";

  t[1].x = 100;
  t[1].y = 170;
  t[1].color = ILI9341_BLUE;
  t[1].size = 2;
  t[1].text = "Dino";ű
  */

  t[2].x = 40;
  t[2].y = 100;
  t[2].color = ILI9341_BLUE;
  t[2].size = 2;
  t[2].text = "Space Invaders";

  t[3].x = 100;
  t[3].y = 130;
  t[3].color = ILI9341_BLUE;
  t[3].size = 2;
  t[3].text = "Pong";
}


//Function prints all MenuText elements on screen.
//(Maybe do a dynamic function for this for expandability)
void printMenuElements(MenuText t[])
{
  for(int i = 2; i < 4; i++)
  {
    screen.setTextColor(t[i].color);
    screen.setCursor(t[i].x, t[i].y);
    screen.setTextSize(t[i].size);
    screen.print(t[i].text);
  }
}

//main menu method
int mainMenu()
{
  int mybool = true;
  int selected = 0;  //selector index
  int hoverindex = 2;
  int dhoverindex = hoverindex;

  //Erase screen
  screen.fillScreen(ILI9341_BLACK);

  //Create and store menu elements
  MenuText Texts[4];
  createMenuElements(Texts);

  //Print "Choose your game!" on screen
  screen.setCursor(20,60);
  screen.setTextColor(ILI9341_WHITE);
  screen.setTextSize(2);
  screen.print("Choose your game!");
  
  //Print all the MenuText elements
  //Note: the rectangle is drawn on the MenuText element which is currently chosen 
  printMenuElements(Texts);
  drawMenuRect(Texts[hoverindex]);

  //Waiting until user selects a game
  //Until then if a button is pressed, the selection rectangle is moved in function of which button which is pressed 
    while(!selected){
    dhoverindex = hoverindex;
    if(digitalRead(DOWN))
    {
      hoverindex++;
      if(hoverindex == 4) hoverindex = 2;
      undrawMenuRect(Texts[dhoverindex]);
      drawMenuRect(Texts[hoverindex]);
      delay(250);
    } 
    else if(digitalRead(UP)){
      hoverindex--;
      if(hoverindex < 2) hoverindex = 3;
      undrawMenuRect(Texts[dhoverindex]);
      drawMenuRect(Texts[hoverindex]);
      delay(150);
    }
    if(digitalRead(RIGHT)){
      delay(200);
      if(digitalRead(RIGHT)) return hoverindex;
    }
  }
  
}


//Main loop for the whole thing
void loop()
{
  switch(mainMenu())
  {
    case 0:
       //game0();
      break;
    case 1:
      break;
    case 2:
      SI(screen);
      break;
    case 3:
      pong(screen);
      break;
    default:
      break;
  }

}
