#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

//  ||
//  || Use hardware SPI (on Uno, #13, #12, #11) for DC and CS
//  \/

#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8
#define game_speed 3
#define UP 4    //up button pin
#define DOWN 3  //down button pin
#define RIGHT 5 //right button pin
#define LEFT 6  //left button pin

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC

Adafruit_ILI9341 screen = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
uint16_t backgroundColor = ILI9341_RED;

typedef struct menutext{
  String text;
  int x;
  int y;
  int size;
  uint16_t color;
} MenuText;


//method for screen diagnostics
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
void undrawMenuRect(MenuText t)
{
  screen.drawRect(t.x - 4, t.y -4, t.text.length() * 10 + (t.text.length()-1)*2 + 8, 22, ILI9341_BLACK);
}

void createMenuElements(MenuText t[])
{
  t[0].x = 93;
  t[0].y = 140;
  t[0].color = ILI9341_BLUE;
  t[0].size = 2;
  t[0].text = "Snake";

  t[1].x = 100;
  t[1].y = 170;
  t[1].color = ILI9341_BLUE;
  t[1].size = 2;
  t[1].text = "Dino";

  t[2].x = 40;
  t[2].y = 200;
  t[2].color = ILI9341_BLUE;
  t[2].size = 2;
  t[2].text = "Space Invaders";

  t[3].x = 100;
  t[3].y = 230;
  t[3].color = ILI9341_BLUE;
  t[3].size = 2;
  t[3].text = "Pong";
}

void printMenuElements(MenuText t[])
{
  for(int i = 0; i < 4; i++)
  {
    screen.setTextColor(t[i].color);
    screen.setCursor(t[i].x, t[i].y);
    screen.setTextSize(t[i].size);
    screen.print(t[i].text);
  }
}

void pong(){

    int play = 1;
    int inp0 = 0;
    int inp1 = 0;
    int inp2 = 0;
    int inp3 = 0;

    screen.fillScreen(ILI9341_BLACK);

    screen.setTextColor(ILI9341_YELLOW);
    screen.setCursor(73, 130);
    screen.setTextSize(2);
    screen.print("Jeu sous\nconstruction");

    while(play){
      inp0 = digitalRead(LEFT);
      inp1 = digitalRead(RIGHT);
      inp2 = digitalRead(UP);
      inp3 = digitalRead(DOWN);
      delay(200);
      if(inp0 && inp1 && inp2 && inp3) play = !play;
    }
}

//main menu method
int mainMenu()
{

  screen.fillScreen(ILI9341_BLACK);

  int mybool = true;
  int selected = 0;  //selector index
  int hoverindex = 0;
  int dhoverindex = hoverindex;

  MenuText Texts[4];
  createMenuElements(Texts);

  Serial.println(Texts[0].text);

  screen.setCursor(20,60);
  screen.setTextColor(ILI9341_WHITE);
  screen.setTextSize(2);
  screen.print("Choisissez un jeu");
  printMenuElements(Texts);
  drawMenuRect(Texts[hoverindex]);

  while(!selected){
    dhoverindex = hoverindex;
    if(digitalRead(DOWN))
    {
      hoverindex++;
      if(hoverindex == 4) hoverindex = 0;
      undrawMenuRect(Texts[dhoverindex]);
      drawMenuRect(Texts[hoverindex]);
      delay(150);
    } 
    else if(digitalRead(UP)){
      hoverindex--;
      if(hoverindex < 0) hoverindex = 3;
      undrawMenuRect(Texts[dhoverindex]);
      drawMenuRect(Texts[hoverindex]);
      delay(150);
    }
    if(digitalRead(RIGHT)){
      delay(10);
      if(digitalRead(RIGHT)) return hoverindex;
    }
  }
  
}

void game0()
{

  //game variables

  int x, y, xd, yd;
  x = 30;
  y = 30;

  xd = 0;
  yd = 0;
  int px = x;
  int py = y;
  int inp0 = 0;
  int inp1 = 0;
  int inp2 = 0;
  int inp3 = 0;
  int play = 1;

  screen.fillScreen(backgroundColor);
  render_figure(x, y);

  //game loop
  while(play)
  {
    inp0 = digitalRead(6);
    inp1 = digitalRead(5);
    inp2 = digitalRead(4);
    inp3 = digitalRead(3);

    if(inp0 && inp1 && inp2 && inp3) play = !play;
    
    px = x;
    py = y;

    if(inp0 == 1) x-=game_speed;
    else if(inp1 == 1) x+=game_speed;
    if(inp2 == 1) y-=game_speed;
    else if(inp3 == 1)y+=game_speed;
    if(x >= screen.width()-30) x = screen.width()-30;
    if(x <= 0+30) x = 30;
    if(y >= screen.height()-30) y = screen.height()-30;
    if(y <= 0+30) y = 30;
   
    if(py == y && px == x) continue;
    else{
      delete_figure(px, py);
      render_figure(x, y);
      delay(20);
      Serial.println(x);
      Serial.println(py);
      Serial.println(y);
      Serial.println(py);
      Serial.println();
    }
  }
}

void game1()
{
  //game variables

  int x, y, xd, yd;
  x = 30;
  y = 30;

  xd = 0;
  yd = 0;
  int px = x;
  int py = y;
  int inp0 = 0;
  int inp1 = 0;
  int inp2 = 0;
  int inp3 = 0;
  int play = 1;

  screen.fillScreen(backgroundColor);
  render_figure(x, y);

  //game loop
  while(play)
  {
    inp0 = digitalRead(6);
    inp1 = digitalRead(5);
    inp2 = digitalRead(4);
    inp3 = digitalRead(3);

    if(inp0 && inp1 && inp2 && inp3) play = !play;

    px = x;
    py = y;
    
    if(inp0) xd = 1;
    if(inp1) xd = 0;
    if(inp2) yd = 1;
    if(inp3) yd = 0;
    if(x >= screen.width()-30) xd = 1;
    if(x <= 0+30) xd = 0;
    if(y >= screen.height()-30) yd = 1;
    if(y <= 0+30) yd = 0;
    if(xd == 0) x+=game_speed;
    else x-=game_speed;
    if(yd == 0) y+=game_speed;
    else y-=game_speed;
    
   
   
    if(py == y && px == x) continue;
    else{
      delete_figure(px, py);
      render_figure(x, y);
      delay(20);
      Serial.println(x);
      Serial.println(py);
      Serial.println(y);
      Serial.println(py);
      Serial.println();
    }
  }
}

void setup()
{

  //Deselescting all SPI devices
  pinMode(TFT_CS, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  digitalWrite(TFT_CS, HIGH);

  pinMode(6, INPUT);
  pinMode(5, INPUT);
  pinMode(4, INPUT);
  pinMode(3, INPUT);

  Serial.begin(9600);
  screen.begin();

  screen.fillScreen(ILI9341_BLACK);
  diag();

}

void render_figure(int posx, int posy)
{
  screen.drawCircle(posx, posy, 30, ILI9341_WHITE);
}

void delete_figure(int posx, int posy)
{
  screen.drawCircle(posx, posy, 30, backgroundColor);
}
void loop()
{

  switch(mainMenu())
  {
    case 0:
      game0();
      break;
    case 1:
      game1();
      break;
    case 2:
      game1();
      break;
    case 3:
      pong();
      break;
    default:
      break;
  }

}
