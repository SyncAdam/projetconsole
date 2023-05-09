#include <Arduino_BuiltIn.h>// NOT SURE

#include <Adafruit_GFX.h>

// Écran screen
#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 11
#define UP 4    //up button pin
#define DOWN 6  //down button pin
#define RIGHT 7 //right button pin
#define LEFT 3  //left button pin
#define BACK 5
#define SCREENHEIGHT 320
#define SCREENWIDTH 240

#define BALLUPDATESPEED 600
#define PADDLEUPDATESPEED 50


// Taille de la balle et des raquettes
#define BALL_SIZE 8
#define PADDLE_WIDTH 5
#define PADDLE_HEIGHT 40

// Vitesse de la balle et des raquettes
#define BALL_SPEED 1
#define PADDLE_SPEED 1

// Position de départ de la balle
int ballX, ballY;
int ballDX, ballDY;
// Vitesse initiale de la balle
int ballVelocityX = BALL_SPEED, ballVelocityY = BALL_SPEED;

// Position de départ des raquettes
int paddle1X, paddle1Y, paddle2X, paddle2Y;
int paddle1DY, paddle2DY;
// Scores des joueurs
int player1Score = 0, player2Score = 0;


void initialise(Adafruit_ILI9341 screen)
{

  screen.fillScreen(ILI9341_BLACK);

  paddle1X = 0;
  paddle1Y = SCREENWIDTH / 2 - PADDLE_HEIGHT / 2;
  paddle2X = SCREENWIDTH - PADDLE_WIDTH;
  paddle2Y = SCREENWIDTH / 2 - PADDLE_HEIGHT / 2;

}

void updateScore(Adafruit_ILI9341 screen) {

  // Efface les anciens scores
  screen.fillRect((SCREENWIDTH / 2) - 20, 10, 100, 40, ILI9341_BLACK);

  // Affiche les scores
  screen.setCursor(SCREENWIDTH / 2 - 20, 10);
  screen.setTextColor(ILI9341_WHITE);
  screen.print(player1Score);
  screen.print(" - ");
  screen.print(player2Score);

}

void resetBall(Adafruit_ILI9341 screen) {
  // Place la balle au centre de l'écran
  ballX = screen.width() / 2 - BALL_SIZE / 2;
  ballY = screen.height() / 2 - BALL_SIZE / 2;

  // Donne une vitesse aléatoire à la balle
  ballVelocityX = random(0, 2) == 0 ? BALL_SPEED : -BALL_SPEED;
  ballVelocityY = random(0, 2) == 0 ? BALL_SPEED : -BALL_SPEED;

  delay(2000);

  updateScore(screen); // Met à jour le score
}

void moveBall() {
  // Déplace la balle en fonction de sa vitesse
  ballDX = ballX;
  ballDY = ballY;
  ballX += ballVelocityX;
  ballY += ballVelocityY;
}

void movePaddles() {
  // Déplace la raquette du joueur 1 vers le haut si le bouton est appuyé
  paddle1DY = paddle1Y;
  paddle2DY = paddle2Y;
  if (digitalRead(UP) == HIGH && paddle1Y >= PADDLE_SPEED) {
    paddle1Y -= PADDLE_SPEED;
  }
  // Déplace la raquette du joueur 1 vers le bas si le bouton est appuyé
  else if (digitalRead(RIGHT) == HIGH && paddle1Y <= SCREENHEIGHT - PADDLE_HEIGHT - PADDLE_SPEED) {
    paddle1Y += PADDLE_SPEED;
  }

  // Déplace la raquette du joueur 2 vers le haut si le bouton est appuyé
  if (digitalRead(LEFT) == HIGH && paddle2Y >= PADDLE_SPEED) {
    paddle2Y -= PADDLE_SPEED;
  }
  // Déplace la raquette du joueur 2 vers le bas si le bouton est appuy
  else if (digitalRead(DOWN) == HIGH && paddle2Y <= SCREENHEIGHT - PADDLE_HEIGHT - PADDLE_SPEED) {
    paddle2Y += PADDLE_SPEED;
  }
}

void drawBall(Adafruit_ILI9341 screen) {
  // Efface les anciens objets
  screen.fillRect(ballDX, ballDY, BALL_SIZE, BALL_SIZE, ILI9341_BLACK);
  // Dessine les nouveaux objets
  screen.fillRect(ballX, ballY, BALL_SIZE, BALL_SIZE, ILI9341_WHITE);
}

void undrawBall(Adafruit_ILI9341 screen)
{
  screen.fillRect(ballX, ballY, BALL_SIZE, BALL_SIZE, ILI9341_BLACK);
}

void checkCollisions(bool gameOver, Adafruit_ILI9341 screen) {
  // Vérifie s'il y a collision entre la balle et le bord supérieur ou inférieur de l'écran
  if (ballY <= 0 || ballY >= SCREENHEIGHT - BALL_SIZE) {
    ballVelocityY = -ballVelocityY;
  }

  // Vérifie s'il y a collision entre la balle et les raquettes
  if (ballX <= PADDLE_WIDTH && ballY >= paddle1Y && ballY <= paddle1Y + PADDLE_HEIGHT) {
    ballVelocityX = -ballVelocityX;
  }
  else if (ballX >= SCREENWIDTH - PADDLE_WIDTH - BALL_SIZE && ballY >= paddle2Y && ballY <= paddle2Y + PADDLE_HEIGHT) {
    ballVelocityX = -ballVelocityX;
  }

  // Vérifie si la balle est sortie de l'écran
  if (ballX <= 0) {

    player2Score++;
    if(player2Score > 10) gameOver = true;
    undrawBall(screen);
    resetBall(screen);
  }
  else if (ballX >= SCREENWIDTH - BALL_SIZE) {
    player1Score++;
    if(player1Score > 10) gameOver = true;
    undrawBall(screen);
    resetBall(screen);
  }
}

void drawPaddles(Adafruit_ILI9341 screen) {
  // Efface les anciens objets
  screen.fillRect(paddle1X, paddle1DY, PADDLE_WIDTH, PADDLE_HEIGHT, ILI9341_BLACK);
  screen.fillRect(paddle2X, paddle2DY, PADDLE_WIDTH, PADDLE_HEIGHT, ILI9341_BLACK);

  // Dessine les nouveaux objets
  screen.fillRect(paddle1X, paddle1Y, PADDLE_WIDTH, PADDLE_HEIGHT, ILI9341_GREEN);
  screen.fillRect(paddle2X, paddle2Y, PADDLE_WIDTH, PADDLE_HEIGHT, ILI9341_BLUE );
}

void pong(Adafruit_ILI9341 screen) {

  initialise(screen);
  bool gameOver = false;

  unsigned long ballUpdateTime = 0;
  unsigned long paddleUpdateTime = 0;
  unsigned long collisionUpdateTime = 0;

  unsigned long currentT = millis();

  updateScore(screen);

  while(!gameOver)
  {

    if(currentT - ballUpdateTime >= BALLUPDATESPEED)
    {
      moveBall(); // Déplace la balle
      drawBall(screen);
      ballUpdateTime = millis();
    }

    if(currentT - paddleUpdateTime >= PADDLEUPDATESPEED)
    {
      movePaddles(); // Déplace les raquettes
      drawPaddles(screen);
      paddleUpdateTime = millis();
    }

    if(currentT - collisionUpdateTime >= BALLUPDATESPEED)
    {
      checkCollisions(gameOver, screen); // Vérifie s'il y a collision entre la balle et les bords ou les raquettes
      collisionUpdateTime = millis();
    }

  }
}