Welcome! This is an offline-capable Notepad which is a Progressive Web App.

The app serves the following features:

- Your notes are saved in real-time as you type.
- Installable on supported browsers for offline usage.
- "Add To Home Screen" feature on Android-supported devices to launch the app from the home screen.
- Dark mode.
- Privacy-focused - Never collects your precious data.
- Light-weight - Loads almost instantly.
- Writing timer.
- View Note Statistics.
- Snapshots.
- Ability to mimic typewriter sound when typing.
- Optional break reminders after long writing sessions.
- Keyboard shortcuts for common actions.
- Focus mode to leave you with a barebones and pristine editor.
- Full-screen mode for a distraction-free writing experience.
- Floating window (in supported browsers) to effectively take notes across other apps.
- Download notes as plain text, PDF, HTML, and DOCX file.
- Ability to play ambient noise to help you focus.
- It's proudly open-source!

CAUTION: Since the app uses the browser's localStorage to store your notes, 
it's recommended that you take a backup of your notes more often using the 
"Download Notes" button or by pressing the "ctrl/command + s" keys.

Lastly, if you're using Notepad, and want to support the development, 
you can buy me a coffee — the link of which is available in the About section.

** Delete this text and start writing your notes **#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Buttons
#define UP_BUTTON     D6
#define DOWN_BUTTON   D7
#define OK_BUTTON     D5
#define LEFT_BUTTON   D3
#define RIGHT_BUTTON  D4

// Menu
const char* games[] = {
  "1. Dino Run",
  "2. Snake",
  "3. Flappy stone",
  "4. Tic Tac Toe",
  "5. Pong",
  "6. Mario Mini",
  "7. Car Dodger",
  "8. Tappy Tap",
  "9. Space Invaders", 
  "10. tetris lite",
  "11. brick killer",
  "12. play 2048",
  "13. doodle jump "
}; 

const int totalGames = sizeof(games) / sizeof(games[0]);
int selectedGame = 0;
int menuStartIndex = 0;
bool inGame = false;
bool unlocked = false;
unsigned long lastDebounce = 0;

// Password (UP, UP, UP, UP)
const int passwordSequence[] = {UP_BUTTON, UP_BUTTON, UP_BUTTON, UP_BUTTON};
const int passwordLength = sizeof(passwordSequence) / sizeof(passwordSequence[0]);
int passwordIndex = 0;

// Bitmaps
static const unsigned char PROGMEM dino1[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 0x00, 0x00, 0x06, 0xff, 0x00, 0x00, 0x0e, 0xff, 0x00,
  0x00, 0x0f, 0xff, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x0f, 0xc0, 0x00,
  0x00, 0x0f, 0xfc, 0x00, 0x40, 0x0f, 0xc0, 0x00, 0x40, 0x1f, 0x80, 0x00, 0x40, 0x7f, 0x80, 0x00,
  0x60, 0xff, 0xe0, 0x00, 0x71, 0xff, 0xa0, 0x00, 0x7f, 0xff, 0x80, 0x00, 0x7f, 0xff, 0x80, 0x00,
  0x7f, 0xff, 0x80, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00,
  0x03, 0xfc, 0x00, 0x00, 0x01, 0xdc, 0x00, 0x00, 0x01, 0x8c, 0x00, 0x00, 0x01, 0x8c, 0x00, 0x00,
  0x01, 0x0c, 0x00, 0x00, 0x01, 0x8e, 0x00, 0x00
};
static const unsigned char PROGMEM tree1[] = {
  0x1e, 0x00, 0x1f, 0x00, 0x1f, 0x40, 0x1f, 0xe0, 0x1f, 0xe0, 0xdf, 0xe0, 0xff, 0xe0, 0xff, 0xe0,
  0xff, 0xe0, 0xff, 0xe0, 0xff, 0xe0, 0xff, 0xe0, 0xff, 0xc0, 0xff, 0x00, 0xff, 0x00, 0x7f, 0x00,
  0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00
};
static const unsigned char PROGMEM tree2[] = {
  0x1e, 0x01, 0xe0, 0x1f, 0x03, 0xe0, 0x1f, 0x4f, 0xe8, 0x1f, 0xff, 0xfc, 0x1f, 0xff, 0xfc, 0xdf,
  0xff, 0xfc, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xfc, 0xff, 0xef,
  0xfc, 0xff, 0x83, 0xfc, 0xff, 0x03, 0xfc, 0xff, 0x03, 0xf8, 0x7f, 0x03, 0xe0, 0x1f, 0x03, 0xe0,
  0x1f, 0x03, 0xe0, 0x1f, 0x03, 0xe0, 0x1f, 0x03, 0xe0, 0x1f, 0x03, 0xe0
};

void setup() {
  Wire.begin(D1, D2);
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(OK_BUTTON, INPUT_PULLUP);
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 25);
  display.println("calculator");
  display.display();
  delay(1000);
}

void loop() {
  if (!unlocked) {
    handlePassword();
    return;
  }

  if (!inGame) {
    drawMenu();
    handleMenuInput();
  } else {
    showSelectedGame();
  }
}

void handlePassword() {
  static int lastRead = HIGH;
  int currentButton = readButton();

  if (currentButton != -1 && lastRead == HIGH) {
    if (currentButton == passwordSequence[passwordIndex]) {
      passwordIndex++;
      if (passwordIndex >= passwordLength) {
        unlocked = true;
        display.clearDisplay();
        display.setCursor(20, 25);
        display.println(" 2 + 2 =5.!");
        display.display();
        delay(3000);
      }
    } else {
      passwordIndex = 0;
    }
  }

  lastRead = (currentButton == -1) ? HIGH : LOW;
}

int readButton() {
  if (digitalRead(UP_BUTTON) == LOW) return UP_BUTTON;
  if (digitalRead(DOWN_BUTTON) == LOW) return DOWN_BUTTON;
  if (digitalRead(OK_BUTTON) == LOW) return OK_BUTTON;
  return -1;
}

void drawMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Select Game:");
  for (int i = 0; i < 6; i++) {
    int index = menuStartIndex + i;
    if (index >= totalGames) break;
    if (index == selectedGame) display.print("> ");
    else display.print("  ");
    display.println(games[index]);
  }
  display.display();
}

void handleMenuInput() {
  if (millis() - lastDebounce > 200) {
    if (digitalRead(UP_BUTTON) == LOW) {
      selectedGame = (selectedGame - 1 + totalGames) % totalGames;
      lastDebounce = millis();
    }
    if (digitalRead(DOWN_BUTTON) == LOW) {
      selectedGame = (selectedGame + 1) % totalGames;
      lastDebounce = millis();
    }
    if (digitalRead(OK_BUTTON) == LOW) {
      inGame = true;
      lastDebounce = millis();
    }

    if (selectedGame < menuStartIndex) menuStartIndex = selectedGame;
    else if (selectedGame >= menuStartIndex + 6) menuStartIndex = selectedGame - 5;
  }
}

void showSelectedGame() {
  switch (selectedGame) {
    case 0: dinoRun(); break;
    case 1: snakeGame(); break;
    case 2: flappystone(); break;
    case 3: ticTacToe(); break;
    case 4: pongGame(); break;
   case 5: marioMiniGame(); break;
    case 6: carDodgerGame(); break;
    case 7: //tappyTapGame(); break;
    case 8: spaceInvaderGame(); break;
    case 9: tetrislite();break;
    case 10:  brickkillerGame();break;
   case 11: play2048();break;
case 12: playDoodleJump(); break;



    break; default: inGame = false; break;
}
}

void dinoRun() {
  int dinoY = 38;
  float velocity = 0;
  float gravity = 0.6;
  float jump = -7.5;
  int obstacleX = SCREEN_WIDTH;
  int score = 0;
  bool useAltTree = false;

  int baseSpeed = 3;
  int delayTime = 50;

  while (true) {
    if (digitalRead(OK_BUTTON) == LOW && dinoY >= 38)
      velocity = jump;

    gravity = 0.6 + (score * 0.05);
    velocity += gravity;
    dinoY += velocity;

    if (dinoY > 38) {
      dinoY = 38;
      velocity = 0;
    }

    int currentSpeed = baseSpeed + (score / 5);
    delayTime = 50 - (score * 2);
    if (delayTime < 20) delayTime = 20;

    obstacleX -= currentSpeed;
    if (obstacleX < -22) {
      obstacleX = SCREEN_WIDTH;
      score++;
      useAltTree = !useAltTree;
    }

    if ((obstacleX < 25 && obstacleX + (useAltTree ? 22 : 11) > 10) && dinoY > 26)
      break;

    display.clearDisplay();
    display.drawBitmap(10, dinoY, dino1, 25, 26, SSD1306_WHITE);
    if (useAltTree)
      display.drawBitmap(obstacleX, 38, tree2, 22, 23, SSD1306_WHITE);
    else
      display.drawBitmap(obstacleX, 41, tree1, 11, 23, SSD1306_WHITE);

    display.setCursor(90, 0);
    display.print("S:");
    display.print(score);
    display.display();
    delay(delayTime);
  }

  gameOver(score);
  inGame = false;
}

void snakeGame() {
  const int cellSize = 4;
  const int gridWidth = SCREEN_WIDTH / cellSize;
  const int gridHeight = SCREEN_HEIGHT / cellSize;

  struct Point { int x, y; };
  Point snake[128];
  int snakeLength = 3;
  snake[0] = {gridWidth / 2, gridHeight / 2};
  snake[1] = {gridWidth / 2 - 1, gridHeight / 2};
  snake[2] = {gridWidth / 2 - 2, gridHeight / 2};

  int dx = 1, dy = 0;
  bool running = true;
  int score = 0;
  Point food = {random(0, gridWidth), random(0, gridHeight)};

  while (running) {
    if (digitalRead(UP_BUTTON) == LOW && dy != 1) { dx = 0; dy = -1; }
    else if (digitalRead(DOWN_BUTTON) == LOW && dy != -1) { dx = 0; dy = 1; }
    else if (digitalRead(LEFT_BUTTON) == LOW && dx != 1) { dx = -1; dy = 0; }
    else if (digitalRead(RIGHT_BUTTON) == LOW && dx != -1) { dx = 1; dy = 0; }

    Point newHead = {snake[0].x + dx, snake[0].y + dy};
    if (newHead.x < 0 || newHead.x >= gridWidth || newHead.y < 0 || newHead.y >= gridHeight)
      break;

    for (int i = 0; i < snakeLength; i++) {
      if (snake[i].x == newHead.x && snake[i].y == newHead.y) {
        running = false;
        break;
      }
    }

    for (int i = snakeLength; i > 0; i--) {
      snake[i] = snake[i - 1];
    }
    snake[0] = newHead;

    if (newHead.x == food.x && newHead.y == food.y) {
      snakeLength++;
      score++;
      food = {random(0, gridWidth), random(0, gridHeight)};
    }

    display.clearDisplay();
    for (int i = 0; i < snakeLength; i++) {
      display.fillRect(snake[i].x * cellSize, snake[i].y * cellSize, cellSize, cellSize, SSD1306_WHITE);
    }
    display.fillRect(food.x * cellSize, food.y * cellSize, cellSize, cellSize, SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("S:");
    display.print(score);
    display.display();

    delay(150 - min(score * 5, 100));
  }

  gameOver(score);
  inGame = false;
}

void gameOver(int score) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(20, 20);
  display.println("Game Over!");
  display.setCursor(20, 35);
  display.print("Score: ");
  display.print(score);
  display.display();
  delay(2000);
}
void flappystone() {
  const int birdX = 20;
  float birdY = 30;
  float birdVelocity = 0;
  float gravity = 0.6;
  float jumpStrength = -3.5;

  const int pipeWidth = 15;
  int pipeX = SCREEN_WIDTH;
  int pipeGapY = random(10, 40);
  const int gapHeight = 20;

  int flappyScore = 0;
  bool playing = true;

  while (playing) {
    // Jump if OK button is pressed
    if (digitalRead(OK_BUTTON) == LOW) {
      birdVelocity = jumpStrength;
    }

    // Gravity and bird position update
    birdVelocity += gravity;
    birdY += birdVelocity;
    if (birdY < 0) birdY = 0;
    if (birdY > SCREEN_HEIGHT - 1) {
      break; // Hits ground
    }

    // Pipe movement
    pipeX -= 2;
    if (pipeX + pipeWidth < 0) {
      pipeX = SCREEN_WIDTH;
      pipeGapY = random(10, 40);
      flappyScore++;
    }

    // Collision with pipe
    if (pipeX < birdX + 6 && pipeX + pipeWidth > birdX) {
      if (birdY < pipeGapY || birdY > pipeGapY + gapHeight) {
        break; // Collision with pipe
      }
    }

    // Draw everything
    display.clearDisplay();
    display.fillCircle(birdX, (int)birdY, 3, SSD1306_WHITE); // Bird
    display.fillRect(pipeX, 0, pipeWidth, pipeGapY, SSD1306_WHITE); // Top pipe
    display.fillRect(pipeX, pipeGapY + gapHeight, pipeWidth, SCREEN_HEIGHT - (pipeGapY + gapHeight), SSD1306_WHITE); // Bottom pipe
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print("S:");
    display.print(flappyScore);
    display.display();

    delay(30);
  }

  gameOver(flappyScore);
  inGame = false;
}
void ticTacToe() {
  char board[3][3];
  memset(board, ' ', sizeof(board));
  int cursorX = 0, cursorY = 0;
  bool isXturn = true;
  bool playing = true;

  unsigned long startTime = millis();  // For 1 second button lock
  bool inputEnabled = false;

  while (playing) {
    display.clearDisplay();

    // Draw grid
    display.drawLine(44, 0, 44, 64, WHITE);
    display.drawLine(84, 0, 84, 64, WHITE);
    display.drawLine(0, 21, 128, 21, WHITE);
    display.drawLine(0, 43, 128, 43, WHITE);

    // Draw current board state
    for (int y = 0; y < 3; y++) {
      for (int x = 0; x < 3; x++) {
        int px = x * 42 + 15;
        int py = y * 21 + 7;
        if (board[y][x] == 'X') {
          display.drawLine(px - 5, py - 5, px + 5, py + 5, WHITE);
          display.drawLine(px - 5, py + 5, px + 5, py - 5, WHITE);
        } else if (board[y][x] == 'O') {
          display.drawCircle(px, py, 6, WHITE);
        }
      }
    }

    // Highlight cursor
    display.drawRect(cursorX * 42 + 1, cursorY * 21 + 1, 42, 21, WHITE);

    display.display();

    // Enable input after 1 second
    if (!inputEnabled && millis() - startTime >= 1000) {
      inputEnabled = true;
    }

    if (!inputEnabled) continue;

    // Handle button input
    if (digitalRead(LEFT_BUTTON) == LOW && cursorX > 0) {
      cursorX--;
      delay(200);
    }
    if (digitalRead(RIGHT_BUTTON) == LOW && cursorX < 2) {
      cursorX++;
      delay(200);
    }
    if (digitalRead(UP_BUTTON) == LOW && cursorY > 0) {
      cursorY--;
      delay(200);
    }
    if (digitalRead(DOWN_BUTTON) == LOW && cursorY < 2) {
      cursorY++;
      delay(200);
    }
    if (digitalRead(OK_BUTTON) == LOW && board[cursorY][cursorX] == ' ') {
      board[cursorY][cursorX] = isXturn ? 'X' : 'O';
      isXturn = !isXturn;
      delay(200);
    }

    // Check win
    char winner = checkWinner(board);
    if (winner != ' ') {
      displayGameResult(winner);
      playing = false;
    }

    // Check draw
    if (isBoardFull(board) && winner == ' ') {
      displayGameResult('D');
      playing = false;
    }
  }

  delay(1000);
  inGame = false;
}

// ✅ Check winner
char checkWinner(char b[3][3]) {
  for (int i = 0; i < 3; i++) {
    if (b[i][0] != ' ' && b[i][0] == b[i][1] && b[i][1] == b[i][2])
      return b[i][0];
    if (b[0][i] != ' ' && b[0][i] == b[1][i] && b[1][i] == b[2][i])
      return b[0][i];
  }
  if (b[0][0] != ' ' && b[0][0] == b[1][1] && b[1][1] == b[2][2])
    return b[0][0];
  if (b[0][2] != ' ' && b[0][2] == b[1][1] && b[1][1] == b[2][0])
    return b[0][2];
  return ' ';
}

// ✅ Check draw
bool isBoardFull(char b[3][3]) {
  for (int y = 0; y < 3; y++) {
    for (int x = 0; x < 3; x++) {
      if (b[y][x] == ' ') return false;
    }
  }
  return true;
}

// ✅ Display winner or draw
void displayGameResult(char result) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(15, 20);
  if (result == 'X') display.println("Player X Wins!");
  else if (result == 'O') display.println("Player O Wins!");
  else display.println("It's a Draw!");
  display.display();
  delay(2000);
}
// Pong Game with Mode Selection (AI / Friend)

void pongGame() {
  int mode = 0; // 0 = AI, 1 = Friend
  bool modeSelected = false;
  unsigned long entryTime = millis();  // time when mode page opened

  // Mode Selection Page
  while (!modeSelected) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(20, 0);
    display.print("Select Mode:");

    display.setCursor(15, 20);
    display.print(mode == 0 ? "> Play with AI" : "  Play with AI");

    display.setCursor(15, 35);
    display.print(mode == 1 ? "> Play with Friend" : "  Play with Friend");

    display.display();

    // Allow cursor movement
    if (digitalRead(UP_BUTTON) == LOW || digitalRead(DOWN_BUTTON) == LOW) {
      mode = 1 - mode;
      delay(200);
    }

    // Block OK for 300ms after entry
    if (millis() - entryTime > 300) {
      if (digitalRead(OK_BUTTON) == LOW) {
        delay(300); // slight hold before entering game
        modeSelected = true;
      }
    }
  }

  // Game variables
  int paddleHeight = 22;
  int paddleWidth = 3;
  int ballSize = 3;
  int leftPaddleY = SCREEN_HEIGHT / 2 - paddleHeight / 2;
  int rightPaddleY = SCREEN_HEIGHT / 2 - paddleHeight / 2;
  int ballX = SCREEN_WIDTH / 2;
  int ballY = SCREEN_HEIGHT / 2;
  float ballSpeedX = 2.0;
  float ballSpeedY = 1.0;
  int score = 0;
  bool playing = true;

  // Game Loop
  while (playing) {
    // Controls
    if (digitalRead(UP_BUTTON) == LOW && leftPaddleY > 0) leftPaddleY -= 2;
    if (digitalRead(DOWN_BUTTON) == LOW && leftPaddleY + paddleHeight < SCREEN_HEIGHT) leftPaddleY += 2;

    if (mode == 1) {
      // Friend mode
      if (digitalRead(LEFT_BUTTON) == LOW && rightPaddleY > 0) rightPaddleY -= 2;
      if (digitalRead(RIGHT_BUTTON) == LOW && rightPaddleY + paddleHeight < SCREEN_HEIGHT) rightPaddleY += 2;
    } else {
      // AI mode
      if (ballY > rightPaddleY + paddleHeight / 2 && rightPaddleY + paddleHeight < SCREEN_HEIGHT) rightPaddleY += 1;
      if (ballY < rightPaddleY + paddleHeight / 2 && rightPaddleY > 0) rightPaddleY -= 1;
    }

    // Ball movement
    ballX += ballSpeedX;
    ballY += ballSpeedY;

    if (ballY <= 0 || ballY + ballSize >= SCREEN_HEIGHT) ballSpeedY *= -1;

    // Paddle collision
    if (ballX <= paddleWidth && ballY + ballSize >= leftPaddleY && ballY <= leftPaddleY + paddleHeight) {
      ballSpeedX *= -1;
      ballSpeedX *= 1.05;
      score++;
    }

    if (ballX + ballSize >= SCREEN_WIDTH - paddleWidth &&
        ballY + ballSize >= rightPaddleY && ballY <= rightPaddleY + paddleHeight) {
      ballSpeedX *= -1;
      ballSpeedX *= 1.05;
      score++;
    }

    // Game over
    if (ballX < 0 || ballX > SCREEN_WIDTH) {
      playing = false;
    }

    // Draw everything
    display.clearDisplay();
    display.fillRect(0, leftPaddleY, paddleWidth, paddleHeight, SSD1306_WHITE);
    display.fillRect(SCREEN_WIDTH - paddleWidth, rightPaddleY, paddleWidth, paddleHeight, SSD1306_WHITE);
    display.fillRect(ballX, ballY, ballSize, ballSize, SSD1306_WHITE);
    display.setCursor(48, 0);
    display.setTextSize(1);
    display.print("S:");
    display.print(score);
    display.display();

    delay(20);
  }

  gameOver(score);
  inGame = false;
}
// space inventor //

const unsigned char playerShip[] PROGMEM = {
  0b00001100, 0b00110000,
  0b00011110, 0b01111000,
  0b00111111, 0b11111100,
  0b01111111, 0b11111110,
  0b11111111, 0b11111111,
  0b11111111, 0b11111111,
  0b00000110, 0b01100000,
  0b00000110, 0b01100000
};
const unsigned char invader[] PROGMEM = {
  0b00011100, 0b00000000,
  0b00111110, 0b00000000,
  0b01111111, 0b00000000,
  0b11011011, 0b10000000,
  0b11111111, 0b10000000,
  0b00100100, 0b00000000,
  0b01000010, 0b00000000,
  0b10000001, 0b00000000
};
void spaceInvaderGame() {
  int playerX = SCREEN_WIDTH / 2 - 8;
  int playerSpeed = 2;

  // Multiple bullets
  const int MAX_BULLETS = 25;
  int bulletX[MAX_BULLETS];
  int bulletY[MAX_BULLETS];
  int bulletDX[MAX_BULLETS];
  bool bulletActive[MAX_BULLETS];
  unsigned long lastBulletTime = 0;
  unsigned long bulletInterval = 200;
  int bulletSpeed = 3;

  // Multiple invaders
  const int MAX_INVADERS_TOTAL = 15;
  int currentInvaders = 6;
  int invaderX[MAX_INVADERS_TOTAL];
  int invaderY[MAX_INVADERS_TOTAL];
  int invaderDir[MAX_INVADERS_TOTAL];
  int invaderSpeed[MAX_INVADERS_TOTAL];

  int score = 0;
  bool playing = true;

  // Init bullets
  for (int i = 0; i < MAX_BULLETS; i++) {
    bulletActive[i] = false;
    bulletDX[i] = 0;
  }

  // Init invaders at top rows only
  for (int i = 0; i < MAX_INVADERS_TOTAL; i++) {
    invaderX[i] = random(0, SCREEN_WIDTH - 12);
    invaderY[i] = (random(0, 2) == 0) ? 10 : 22; // Only top two rows
    invaderDir[i] = (random(0, 2) == 0) ? 1 : -1;
    invaderSpeed[i] = 1;
  }

  while (playing) {
    display.clearDisplay();

    // Difficulty progression
    if (score >= 10 && currentInvaders < 10) currentInvaders = 10;
    if (score >= 20) { bulletSpeed = 7; playerSpeed = 3; }
    if (score >= 30) { bulletInterval = 120; playerSpeed = 4; }
    if (score >= 40) { bulletSpeed = 10; playerSpeed = 5; }
    if (score >= 50 && currentInvaders < 15) currentInvaders = 15;
    if (score >= 80 && currentInvaders < MAX_INVADERS_TOTAL) currentInvaders = MAX_INVADERS_TOTAL;

    // Increase enemy speed with score
    for (int i = 0; i < currentInvaders; i++) {
      if (score >= 10) invaderSpeed[i] = 2;
      if (score >= 30) invaderSpeed[i] = 3;
      if (score >= 50) invaderSpeed[i] = 4;
      if (score >= 80) invaderSpeed[i] = 5;
    }

    // Controls
    if (digitalRead(LEFT_BUTTON) == LOW && playerX > 0) playerX -= playerSpeed;
    if (digitalRead(RIGHT_BUTTON) == LOW && playerX < SCREEN_WIDTH - 16) playerX += playerSpeed;

    // Auto shooting
    if (millis() - lastBulletTime > bulletInterval) {
      if (score < 30) {
        // Single bullet
        for (int i = 0; i < MAX_BULLETS; i++) {
          if (!bulletActive[i]) {
            bulletX[i] = playerX + 7;
            bulletY[i] = SCREEN_HEIGHT - 10;
            bulletDX[i] = 0;
            bulletActive[i] = true;
            break;
          }
        }
      }
      else if (score < 150) {
        // Triple bullets
        for (int b = -1; b <= 1; b++) {
          for (int i = 0; i < MAX_BULLETS; i++) {
            if (!bulletActive[i]) {
              bulletX[i] = playerX + 7;
              bulletY[i] = SCREEN_HEIGHT - 10;
              bulletDX[i] = b;
              bulletActive[i] = true;
              break;
            }
          }
        }
      }
      else {
        // 4 bullets with spread
        int bulletAngles[4] = { -3, -1, 1, 3 };
        for (int a = 0; a < 4; a++) {
          for (int i = 0; i < MAX_BULLETS; i++) {
            if (!bulletActive[i]) {
              bulletX[i] = playerX + 7;
              bulletY[i] = SCREEN_HEIGHT - 10;
              bulletDX[i] = bulletAngles[a];
              bulletActive[i] = true;
              break;
            }
          }
        }
      }
      lastBulletTime = millis();
    }

    // Bullet logic
    for (int i = 0; i < MAX_BULLETS; i++) {
      if (bulletActive[i]) {
        display.drawPixel(bulletX[i], bulletY[i], SSD1306_WHITE);
        bulletX[i] += bulletDX[i];
        bulletY[i] -= bulletSpeed;
        if (bulletY[i] < 0 || bulletX[i] < 0 || bulletX[i] >= SCREEN_WIDTH) bulletActive[i] = false;

        // Collision check
        for (int j = 0; j < currentInvaders; j++) {
          if (bulletX[i] > invaderX[j] && bulletX[i] < invaderX[j] + 12 &&
              bulletY[i] > invaderY[j] && bulletY[i] < invaderY[j] + 8) {
            score++;
            bulletActive[i] = false;

            // Respawn at top rows only
            invaderX[j] = random(0, SCREEN_WIDTH - 12);
            invaderY[j] = (random(0, 2) == 0) ? 10 : 22;
            invaderDir[j] = (random(0, 2) == 0) ? 1 : -1;
          }
        }
      }
    }

    // Invader movement (no vertical drop)
    for (int i = 0; i < currentInvaders; i++) {
      invaderX[i] += invaderDir[i] * invaderSpeed[i];
      if (invaderX[i] <= 0 || invaderX[i] >= SCREEN_WIDTH - 12) {
        invaderDir[i] *= -1; // Only change direction
      }

      display.drawBitmap(invaderX[i], invaderY[i], invader, 12, 8, SSD1306_WHITE);
    }

    // Draw player
    display.drawBitmap(playerX, SCREEN_HEIGHT - 8, playerShip, 16, 8, SSD1306_WHITE);

    // Score display
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print("S:");
    display.print(score);

    display.display();
    delay(30);
  }

  gameOver(score);
  inGame = false;
}



// ==== Car Dodger Sprites (SSD1306, 1-bit) ====
// Dimensions note: bytes = ((W+7)/8) * H

// Player Car (W=12, H=12)
static const unsigned char PROGMEM carSprite12x12[] = {
  0x18, 0x00,  // ..11 0000 0000
  0x3C, 0x00,  // .111 1000 0000
  0x7E, 0x00,  // 1111 1100 0000
  0xFF, 0x00,  // 1111 1111 0000
  0xE7, 0x00,  // 1110 0111 0000
  0xE7, 0x00,  // 1110 0111 0000
  0xFF, 0x00,  // 1111 1111 0000
  0xBD, 0x00,  // 1011 1101 0000
  0xBD, 0x00,  // 1011 1101 0000
  0xFF, 0x00,  // 1111 1111 0000
  0x7E, 0x00,  // 0111 1110 0000
  0x3C, 0x00   // 0011 1100 0000
};

// Obstacle: Truck (W=12, H=10)
static const unsigned char PROGMEM truck12x10[] = {
  0x3C, 0x00,  // ..111100....
  0x7E, 0x00,  // .1111110....
  0xFF, 0x00,  // 11111111....
  0xDB, 0x00,  // 11011011....
  0xFF, 0x00,  // 11111111....
  0xE7, 0x00,  // 11100111....
  0xFF, 0x00,  // 11111111....
  0xBD, 0x00,  // 10111101....
  0xFF, 0x00,  // 11111111....
  0x7E, 0x00   // .1111110....
};

// Obstacle: Cone (W=8, H=8)
static const unsigned char PROGMEM cone8x8[] = {
  0x18, // ..11....
  0x3C, // .1111...
  0x7E, // 111111..
  0xFF, // 11111111
  0x7E, // 111111..
  0x3C, // .1111...
  0x18, // ..11....
  0x18  // ..11....
};

// Obstacle: Block (W=10, H=10)
static const unsigned char PROGMEM block10x10[] = {
  0xFF, 0x03,  // 1111111111
  0x81, 0x01,  // 1........1
  0xBD, 0x01,  // 1.111101.1
  0xA5, 0x01,  // 1.0.0.0.01
  0xBD, 0x01,  // 1.111101.1
  0xA5, 0x01,  // 1.0.0.0.01
  0xBD, 0x01,  // 1.111101.1
  0xA5, 0x01,  // 1.0.0.0.01
  0x81, 0x01,  // 1........1
  0xFF, 0x03   // 1111111111
};
void carDodgerGame() {
  // ---- Road layout ----
  const int ROAD_LEFT   = 8;
  const int ROAD_RIGHT  = 120;
  const int ROAD_TOP    = 0;
  const int ROAD_BOTTOM = 63;

  // ---- Player car ----
  const int CAR_W = 12, CAR_H = 12;
  int carX = (SCREEN_WIDTH - CAR_W) / 2;
  const int carY = ROAD_BOTTOM - CAR_H - 2;

  // ---- Speed system ----
  int worldSpeed = 2;             // starting speed
  const int WORLD_MAX_SPD = 3;    // thoda hi max rakha

  // ---- Obstacles ----
  struct Obs {
    int x, y, w, h, type;
  };
  const int MAX_OBS = 3;
  Obs obs[MAX_OBS];

  auto resetObstacle = [&](int i, int baseY) {
    int t = random(0, 3);
    obs[i].type = t;
    if (t == 0) { obs[i].w = 12; obs[i].h = 10; }
    else if (t == 1) { obs[i].w = 8; obs[i].h = 8; }
    else { obs[i].w = 10; obs[i].h = 10; }
    int minX = ROAD_LEFT + 2;
    int maxX = ROAD_RIGHT - obs[i].w - 2;
    obs[i].x = random(minX, maxX + 1);
    obs[i].y = baseY - random(14, 40);
  };

  for (int i = 0; i < MAX_OBS; i++) resetObstacle(i, -(i * 18) - 20);

  // ---- Countdown ----
  for (int c = 3; c > 0; c--) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor((SCREEN_WIDTH / 2) - 6, SCREEN_HEIGHT / 2 - 8);
    display.print(c);
    display.display();
    delay(1000);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor((SCREEN_WIDTH / 2) - 12, SCREEN_HEIGHT / 2 - 8);
  display.print("GO!");
  display.display();
  delay(800);

  // ---- Game vars ----
  int dashOffset = 0;
  unsigned long lastFrame = 0;
  const uint8_t FRAME_MS = 20;
  int score = 0;
  bool running = true;
  bool speedBoosted = false; // speed increase flag

  auto clamp = [&](int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); };

  // ---- Main loop ----
  while (running) {
    unsigned long now = millis();
    if (now - lastFrame < FRAME_MS) continue;
    lastFrame = now;

    // ---- Controls ----
    if (digitalRead(LEFT_BUTTON) == LOW)  carX -= 2;
    if (digitalRead(RIGHT_BUTTON) == LOW) carX += 2;

    carX = clamp(carX, ROAD_LEFT + 2, ROAD_RIGHT - CAR_W - 2);

    // ---- Speed increase once at score 50 ----
    if (score >= 50 && !speedBoosted) {
      worldSpeed++;
      if (worldSpeed > WORLD_MAX_SPD) worldSpeed = WORLD_MAX_SPD;
      speedBoosted = true; // ek hi bar speed badhegi
    }

    // ---- Move obstacles ----
    for (int i = 0; i < MAX_OBS; i++) {
      obs[i].y += worldSpeed;
      if (obs[i].y > ROAD_BOTTOM + 8) {
        score++;
        resetObstacle(i, -10);
      }
    }

    // ---- Collision ----
    for (int i = 0; i < MAX_OBS; i++) {
      int ax1 = carX, ay1 = carY, ax2 = carX + CAR_W, ay2 = carY + CAR_H;
      int bx1 = obs[i].x, by1 = obs[i].y, bx2 = obs[i].x + obs[i].w, by2 = obs[i].y + obs[i].h;
      bool overlap = !(ax2 <= bx1 || ax1 >= bx2 || ay2 <= by1 || ay1 >= by2);
      if (overlap) { running = false; break; }
    }

    // ---- Road scroll ----
    dashOffset = (dashOffset + worldSpeed) % 8;

    // ---- Draw ----
    display.clearDisplay();
    display.drawFastVLine(ROAD_LEFT,  ROAD_TOP, ROAD_BOTTOM - ROAD_TOP + 1, SSD1306_WHITE);
    display.drawFastVLine(ROAD_RIGHT, ROAD_TOP, ROAD_BOTTOM - ROAD_TOP + 1, SSD1306_WHITE);

    int centerX = (ROAD_LEFT + ROAD_RIGHT) / 2;
    for (int y = -dashOffset; y < SCREEN_HEIGHT; y += 8) {
      display.drawFastVLine(centerX, y, 4, SSD1306_WHITE);
    }

    for (int i = 0; i < MAX_OBS; i++) {
      if (obs[i].type == 0)      display.drawBitmap(obs[i].x, obs[i].y, truck12x10, 12, 10, SSD1306_WHITE);
      else if (obs[i].type == 1) display.drawBitmap(obs[i].x, obs[i].y, cone8x8, 8, 8, SSD1306_WHITE);
      else                       display.drawBitmap(obs[i].x, obs[i].y, block10x10, 10, 10, SSD1306_WHITE);
    }

    display.drawBitmap(carX, carY, carSprite12x12, CAR_W, CAR_H, SSD1306_WHITE);

    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print("S:");
    display.print(score);

    display.display();
  }

  gameOver(score);
  inGame = false;
}
// ===================== MARIO MINI: BITMAPS =====================
// Sizes commented next to each sprite

// 12x14 - Mario standing
static const uint8_t PROGMEM mario_stand_12x14[] = {
  0x00,0x00,0x18,0x00,0x3C,0x00,0x3C,0x00,0x7E,0x00,0x66,0x00,0x7E,0x00,
  0x18,0x00,0x7E,0x00,0xDB,0x00,0x99,0x00,0x18,0x00,0x24,0x00,0x42,0x00
};

// 12x14 - Mario run frame 1
static const uint8_t PROGMEM mario_run1_12x14[] = {
  0x00,0x00,0x18,0x00,0x3C,0x00,0x3C,0x00,0x7E,0x00,0x66,0x00,0x7E,0x00,
  0x18,0x00,0x3C,0x00,0x5A,0x00,0x99,0x00,0x24,0x00,0x42,0x00,0x00,0x00
};

// 12x14 - Mario run frame 2
static const uint8_t PROGMEM mario_run2_12x14[] = {
  0x00,0x00,0x18,0x00,0x3C,0x00,0x3C,0x00,0x7E,0x00,0x66,0x00,0x7E,0x00,
  0x18,0x00,0x18,0x00,0xBD,0x00,0x18,0x00,0x24,0x00,0x42,0x00,0x00,0x00
};

// 12x14 - Mario jump
static const uint8_t PROGMEM mario_jump_12x14[] = {
  0x00,0x00,0x18,0x00,0x3C,0x00,0x3C,0x00,0x7E,0x00,0x66,0x00,0x7E,0x00,
  0x18,0x00,0x24,0x00,0x42,0x00,0x81,0x00,0x24,0x00,0x18,0x00,0x00,0x00
};

// 16x18 - Pipe
static const uint8_t PROGMEM pipe_16x18[] = {
  0xFF,0xFF,0x81,0x01,0xBD,0xBD,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0x81,0x01,0x81,0x01,
  0x81,0x01,0x81,0x01,0x81,0x01,0x81,0x01,0x81,0x01,0x81,0x01,0x81,0x01,0xFF,0xFF,
  0x00,0x00,0x00,0x00
};

// 12x10 - Goomba
static const uint8_t PROGMEM goomba_12x10[] = {
  0x1E,0x00,0x3F,0x00,0x7F,0x80,0x6D,0x80,0x7F,0x80,0x3F,0x00,0x3F,0x00,
  0x7F,0x80,0x41,0x80,0x41,0x80
};

// 8x8 - Coin
static const uint8_t PROGMEM coin_8x8[] = {
  0x3C,0x42,0xBD,0xA5,0xA5,0xBD,0x42,0x3C
};

// 16x8 - Cloud
static const uint8_t PROGMEM cloud_16x8[] = {
  0x0E,0x00,0x1F,0x80,0x3F,0xC0,0x7F,0xE0,
  0x7F,0xE0,0x3F,0xC0,0x1F,0x80,0x0E,0x00
};

// 8x8 - Brick block
static const uint8_t PROGMEM brick_8x8[] = {
  0xFF,0x81,0xBD,0x81,0xBD,0x81,0xFF,0x00
};

// Ground pattern (128x6 as repeated stripes via drawFastHLine, no bitmap needed)


// ===================== MARIO MINI: GAME =====================
void marioMiniGame() {
  // --- World / camera ---
  const int GROUND_Y = 54;          // top of ground band
  int camX = 0;                      // camera scroll
  int distance = 0;                  // for score

  // --- Player (Mario) ---
  const int MARIO_W = 12, MARIO_H = 14;
  int marioX = 20;                   // screen-space x
  int marioY = GROUND_Y - MARIO_H;
  float vy = 0;
  const float GRAV = 0.5;
  const float JUMP_VEL = -5.8;
  const int MOVE_SPD = 2;
  bool onGround = true;
  bool faceRight = true;

  // --- Animation ---
  uint8_t animTick = 0;
  uint8_t runPhase = 0;
  unsigned long lastFrame = 0;
  const uint8_t FRAME_MS = 20;       // 50 FPS

  // --- Obstacles/Coins/Bricks ---
  struct Obj { int x, y, w, h, type; bool active; };
  const int MAX_OBJ = 10;
  Obj objs[MAX_OBJ];

  auto spawnObject = [&](int idx, int worldX) {
    int t = random(0, 3); // 0=pipe,1=goomba,2=coin
    objs[idx].type = t;
    if (t == 0) { // pipe
      objs[idx].w = 16; objs[idx].h = 18;
      objs[idx].x = worldX; objs[idx].y = GROUND_Y - 18;
    } else if (t == 1) { // goomba
      objs[idx].w = 12; objs[idx].h = 10;
      objs[idx].x = worldX; objs[idx].y = GROUND_Y - 10;
    } else { // coin
      objs[idx].w = 8; objs[idx].h = 8;
      objs[idx].x = worldX; objs[idx].y = GROUND_Y - 24 - random(0, 10);
    }
    objs[idx].active = true;
  };

  for (int i = 0; i < MAX_OBJ; i++) {
    int gap = 40 + random(0, 40);
    spawnObject(i, 140 + i * gap);
  }

  // Fixed bricks data (constant position, not floating randomly)
  const int FIXED_BRICK_Y = GROUND_Y - 24; // fixed height
  int brickPositions[] = {60, 120, 180, 240}; // world positions

  int cloudX1 = 10, cloudX2 = 90;
  int score = 0;
  int coins = 0;
  bool running = true;

  auto overlap = [&](int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh) {
    return !(ax + aw <= bx || bx + bw <= ax || ay + ah <= by || by + bh <= ay);
  };

  // --- Game loop ---
  while (running) {
    unsigned long now = millis();
    if (now - lastFrame < FRAME_MS) continue;
    lastFrame = now;

    // -------- INPUT --------
    bool leftHeld  = (digitalRead(LEFT_BUTTON)  == LOW);
    bool rightHeld = (digitalRead(RIGHT_BUTTON) == LOW);
    bool jumpTap   = (digitalRead(OK_BUTTON)    == LOW);

    if (leftHeld && !rightHeld) {
      marioX -= MOVE_SPD;
      if (marioX < 8) { marioX = 8; camX -= MOVE_SPD; if (camX < 0) camX = 0; }
      faceRight = false;
    } else if (rightHeld && !leftHeld) {
      if (marioX < 48) marioX += MOVE_SPD;
      else camX += MOVE_SPD;
      faceRight = true;
    }

    if (jumpTap && onGround) {
      vy = JUMP_VEL;
      onGround = false;
    }

    // -------- PHYSICS --------
    vy += GRAV;
    marioY += (int)vy;

    // --- Land on bricks (top collision) ---
    onGround = false; // reset
    for (int bxIdx = 0; bxIdx < sizeof(brickPositions)/sizeof(int); bxIdx++) {
      int bxWorld = brickPositions[bxIdx];
      int bxScreen = bxWorld - camX;
      int by = FIXED_BRICK_Y;

      if (vy > 0 && overlap(marioX, marioY, MARIO_W, MARIO_H, bxScreen, by, 8, 8)) {
        marioY = by - MARIO_H;
        vy = 0;
        onGround = true;
      }
    }

    // ground collision
    if (marioY + MARIO_H >= GROUND_Y) {
      marioY = GROUND_Y - MARIO_H;
      vy = 0;
      onGround = true;
    }

    animTick++;
    if (animTick % 6 == 0) runPhase = (runPhase + 1) % 3;

    if ((animTick % 2) == 0) { cloudX1--; cloudX2--; }
    if (cloudX1 < -16) cloudX1 = 128;
    if (cloudX2 < -16) cloudX2 = 128;

    distance += MOVE_SPD * (rightHeld ? 1 : 0);
    if (distance % 16 == 0) score++;

    // -------- OBJECTS --------
    for (int i = 0; i < MAX_OBJ; i++) {
      if (!objs[i].active) continue;
      int sx = objs[i].x - camX;
      int sy = objs[i].y;

      if (objs[i].type == 1) { objs[i].x -= 1; sx = objs[i].x - camX; }

      if (sx + objs[i].w < -10) {
        int gap = 60 + random(0, 60);
        spawnObject(i, camX + 140 + gap);
        continue;
      }

      if (objs[i].type == 0) {
        if (overlap(marioX, marioY, MARIO_W, MARIO_H, sx, sy, objs[i].w, objs[i].h)) {
          running = false;
        }
      } else if (objs[i].type == 1) {
        if (overlap(marioX, marioY, MARIO_W, MARIO_H, sx, sy, objs[i].w, objs[i].h)) {
          if (vy > 0 && marioY + MARIO_H - sy <= 5) {
            vy = JUMP_VEL * 0.6;
            objs[i].active = false;
            score += 3;
          } else {
            running = false;
          }
        }
      } else if (objs[i].type == 2) {
        if (overlap(marioX, marioY, MARIO_W, MARIO_H, sx, sy, objs[i].w, objs[i].h)) {
          objs[i].active = false;
          coins++;
          score += 5;
        }
      }
    }

    // -------- DRAW --------
    display.clearDisplay();
    display.drawBitmap(cloudX1, 8,  cloud_16x8, 16, 8, SSD1306_WHITE);
    display.drawBitmap(cloudX2, 16, cloud_16x8, 16, 8, SSD1306_WHITE);

    // Draw fixed bricks
    for (int bxIdx = 0; bxIdx < sizeof(brickPositions)/sizeof(int); bxIdx++) {
      int bxScreen = brickPositions[bxIdx] - camX;
      if (bxScreen > -8 && bxScreen < 128) {
        display.drawBitmap(bxScreen, FIXED_BRICK_Y, brick_8x8, 8, 8, SSD1306_WHITE);
      }
    }

    for (int x = 0; x < 128; x += 8) {
      display.drawFastHLine(x, GROUND_Y, 8, SSD1306_WHITE);
      display.drawFastHLine(x, GROUND_Y+2, 8, SSD1306_WHITE);
    }

    for (int i = 0; i < MAX_OBJ; i++) {
      if (!objs[i].active) continue;
      int sx = objs[i].x - camX;
      int sy = objs[i].y;
      if (sx < -20 || sx > 140) continue;

      if (objs[i].type == 0)       display.drawBitmap(sx, sy, pipe_16x18, 16, 18, SSD1306_WHITE);
      else if (objs[i].type == 1)  display.drawBitmap(sx, sy, goomba_12x10, 12, 10, SSD1306_WHITE);
      else if (objs[i].type == 2)  display.drawBitmap(sx, sy, coin_8x8, 8, 8, SSD1306_WHITE);
    }

    const uint8_t* marioBmp = mario_stand_12x14;
    if (!onGround) marioBmp = mario_jump_12x14;
    else {
      if (leftHeld || rightHeld) {
        marioBmp = (runPhase == 0) ? mario_run1_12x14 :
                   (runPhase == 1) ? mario_run2_12x14 : mario_run1_12x14;
      }
    }
    display.drawBitmap(marioX, marioY, marioBmp, MARIO_W, MARIO_H, SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("S:");
    display.print(score);
    display.setCursor(40, 0);
    display.print("[");
    display.print(coins);
    display.print("]");

    display.display();
  }

  gameOver(score);
  inGame = false;
}
// ========================= T E T R I S  (128x64 SSD1306) =========================
// Controls: LEFT=D3, RIGHT=D4, UP=D6 (rotate), DOWN=D7 (soft drop), OK=D5 (hard drop/pause)

#ifndef LEFT_BUTTON
#define LEFT_BUTTON  D3
#endif
#ifndef RIGHT_BUTTON
#define RIGHT_BUTTON D4
#endif
#ifndef UP_BUTTON
#define UP_BUTTON    D6
#endif
#ifndef DOWN_BUTTON
#define DOWN_BUTTON  D7
#endif
#ifndef OK_BUTTON
#define OK_BUTTON    D5
#endif

// Fallbacks (uncomment if not present in your project)
// void gameOver(int score){ display.clearDisplay(); display.setTextSize(1); display.setCursor(20,24); display.print("GAME OVER S="); display.print(score); display.display(); delay(1500); }
// bool inGame = false;

static const uint8_t PROGMEM cell3x3[9] = {
  0xFF,0xFF,0xFF, // simple 3x3 filled cell, we will draw via fillRect for speed
  0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF
};

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

void tetrislite() {
  inGame = true;

  // ---------------- Display/Layout ----------------
  const int ORG_X = 2;         // playfield top-left x
  const int ORG_Y = 2;         // playfield top-left y
  const int CELL  = 3;         // 3x3 pixels per cell
  const int COLS  = 10;        // board width
  const int ROWS  = 20;        // board height
  const int W_PX  = COLS*CELL; // 30 px
  const int H_PX  = ROWS*CELL; // 60 px
  const int UI_X  = ORG_X + W_PX + 4; // sidebar start

  // ---------------- Buttons setup ----------------
  pinMode(LEFT_BUTTON,  INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(UP_BUTTON,    INPUT_PULLUP);
  pinMode(DOWN_BUTTON,  INPUT_PULLUP);
  pinMode(OK_BUTTON,    INPUT_PULLUP);

  // ---------------- Board ----------------
  uint8_t board[ROWS][COLS];
  for (int r=0;r<ROWS;r++) for (int c=0;c<COLS;c++) board[r][c]=0;

  // ---------------- Pieces ----------------
  const uint16_t SHAPES[7][4] = {
    { 0x0F00, 0x2222, 0x00F0, 0x4444 },
    { 0x8E00, 0x6440, 0x0E20, 0x44C0 },
    { 0x2E00, 0x4460, 0x0E80, 0xC440 },
    { 0x6600, 0x6600, 0x6600, 0x6600 },
    { 0x6C00, 0x4620, 0x06C0, 0x8C40 },
    { 0x4E00, 0x4640, 0x0E40, 0x4C40 },
    { 0xC600, 0x2640, 0x0C60, 0x4C80 }
  };

  auto cellAtMask = [&](uint16_t mask, int x, int y)->bool {
    int bit = 15 - (y*4 + x);
    return (mask >> bit) & 1;
  };

  // ---------------- RNG ----------------
  randomSeed(micros());
  uint8_t bag[7]; int bagIdx=7;
  auto refillBag = [&](){
    for(int i=0;i<7;i++) bag[i]=i;
    for(int i=6;i>0;i--){ int j=random(0,i+1); uint8_t t=bag[i]; bag[i]=bag[j]; bag[j]=t; }
    bagIdx=0;
  };
  refillBag();

  // ---------------- Piece State ----------------
  struct P { int type, rot, r, c; } cur, nxt;
  auto newPiece = [&]()->P {
    if (bagIdx>=7) refillBag();
    P p; p.type=bag[bagIdx++]; p.rot=0; p.r=0; p.c=3;
    return p;
  };
  cur = newPiece();
  nxt = newPiece();

  // ---------------- Collision ----------------
  auto collides = [&](const P& p)->bool{
    uint16_t m = SHAPES[p.type][p.rot];
    for (int y=0;y<4;y++){
      for(int x=0;x<4;x++){
        if (!cellAtMask(m,x,y)) continue;
        int rr = p.r + y;
        int cc = p.c + x;
        if (cc<0 || cc>=COLS || rr>=ROWS) return true;
        if (rr>=0 && board[rr][cc]) return true;
      }
    }
    return false;
  };

  // ---------------- Lock piece to board ----------------
  auto lockPiece = [&](const P& p){
    uint16_t m = SHAPES[p.type][p.rot];
    for(int y=0;y<4;y++){
      for(int x=0;x<4;x++){
        if (!cellAtMask(m,x,y)) continue;
        int rr=p.r+y, cc=p.c+x;
        if (rr>=0 && rr<ROWS && cc>=0 && cc<COLS) board[rr][cc]=1;
      }
    }
  };

  // ---------------- Line clear ----------------
  auto clearLines = [&]()->int{
    int cleared=0;
    for(int r=ROWS-1;r>=0;){
      bool full=true;
      for(int c=0;c<COLS;c++) if(!board[r][c]){ full=false; break; }
      if(full){
        for(int rr=r; rr>0; rr--) for(int cc=0;cc<COLS;cc++) board[rr][cc]=board[rr-1][cc];
        for(int cc=0;cc<COLS;cc++) board[0][cc]=0;
        cleared++;
      }else{
        r--;
      }
    }
    return cleared;
  };

  // ---------------- Ghost piece ----------------
  auto ghostRow = [&](P p)->int{
    while(!collides(p)) p.r++;
    return p.r-1;
  };

  // ---------------- Drawing ----------------
  auto drawCell = [&](int r,int c, bool fill=true){
    int x = ORG_X + c*CELL;
    int y = ORG_Y + r*CELL;
    if (fill) display.fillRect(x, y, CELL, CELL, SSD1306_WHITE);
    else      display.drawRect(x, y, CELL, CELL, SSD1306_WHITE);
  };

  auto drawBoard = [&](){
    display.drawRect(ORG_X-1, ORG_Y-1, W_PX+2, H_PX+2, SSD1306_WHITE);
    for(int r=0;r<ROWS;r++)
      for(int c=0;c<COLS;c++)
        if(board[r][c]) drawCell(r,c,true);
  };

  auto drawPiece = [&](const P& p, bool outline=false){
    uint16_t m = SHAPES[p.type][p.rot];
    for(int y=0;y<4;y++){
      for(int x=0;x<4;x++){
        if(!cellAtMask(m,x,y)) continue;
        int rr=p.r+y, cc=p.c+x;
        if(rr>=0) drawCell(rr,cc,!outline);
      }
    }
  };

  auto drawNext = [&](){
    display.setTextSize(1);
    display.setCursor(UI_X, 2);  display.print("NEXT");
    uint16_t m = SHAPES[nxt.type][0];
    int px = UI_X; int py = 10;
    display.drawRect(px-1, py-1, 16+2, 16+2, SSD1306_WHITE);
    for(int y=0;y<4;y++){
      for(int x=0;x<4;x++){
        if(!cellAtMask(m,x,y)) continue;
        display.fillRect(px + x*4, py + y*4, 4, 4, SSD1306_WHITE);
      }
    }
  };

  // ---------------- Scoring/Speed ----------------
  long score=0;
  int level=1;
  unsigned long gravityMs = 600;
  auto updateLevel = [&](){
    gravityMs = max(90UL, 600UL - (level-1)*60UL);
  };
  updateLevel();

  // ---------------- Input Repeat ----------------
  bool lastL=false,lastR=false,lastU=false,lastD=false,lastOK=false;
  unsigned long lastLRTime=0;
  const unsigned long DAS = 170;
  const unsigned long ARR = 40;
  int lrDir=0;

  auto readButtons = [&](){
    bool L = digitalRead(LEFT_BUTTON )==LOW;
    bool R = digitalRead(RIGHT_BUTTON)==LOW;
    bool U = digitalRead(UP_BUTTON   )==LOW;
    bool D = digitalRead(DOWN_BUTTON )==LOW;
    bool O = digitalRead(OK_BUTTON   )==LOW;

    if(U && !lastU){
      P test=cur; test.rot=(test.rot+1)&3;
      if(!collides(test)) cur=test;
    }

    static bool paused=false;
    if(O && !lastOK && !paused){
      int gr = ghostRow(cur);
      cur.r = gr;
      lockPiece(cur);
      int cleared = clearLines();
      if (cleared==1) score+=100;
      else if (cleared==2) score+=300;
      else if (cleared==3) score+=500;
      else if (cleared==4) score+=800;
      if (score/1000 + 1 > level){ level = score/1000 + 1; updateLevel(); }
      cur = nxt; nxt=newPiece();
      if(collides(cur)){ gameOver((int)score); inGame=false; return true; }
    }

    static unsigned long okHoldStart=0;
    if(O && !lastOK){ okHoldStart=millis(); }
    if(O && lastOK && millis()-okHoldStart>500){
      display.setTextSize(1);
      display.setCursor(UI_X, 40); display.print("PAUSE  ");
      display.display();
      while(digitalRead(OK_BUTTON)==LOW) delay(10);
      while(digitalRead(OK_BUTTON)==HIGH) delay(10);
    }

    unsigned long now=millis();
    if(L && !R){
      if(!lastL){
        P t=cur; t.c--; if(!collides(t)) cur=t;
        lrDir=-1; lastLRTime=now;
      } else {
        if (lrDir==-1){
          if (now-lastLRTime>DAS) {
            if (now%ARR < 2){
              P t=cur; t.c--; if(!collides(t)) cur=t;
            }
          }
        } else { lrDir=-1; lastLRTime=now; }
      }
    } else if(R && !L){
      if(!lastR){
        P t=cur; t.c++; if(!collides(t)) cur=t;
        lrDir=+1; lastLRTime=now;
      } else {
        if (lrDir==+1){
          if (now-lastLRTime>DAS) {
            if (now%ARR < 2){
              P t=cur; t.c++; if(!collides(t)) cur=t;
            }
          }
        } else { lrDir=+1; lastLRTime=now; }
      }
    } else {
      lrDir=0;
    }

    if(D){
      P t=cur; t.r++;
      if(!collides(t)){ cur=t; score+=1; }
    }

    lastL=L; lastR=R; lastU=U; lastD=D; lastOK=O;
    return false;
  };

  // ---------------- Gravity timer ----------------
  unsigned long lastFall=millis();

  // ---------------- Start prompt ----------------
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(4, 26); display.print("TETRIS  (UP=Rotate)");
  display.setCursor(4, 36); display.print("OK=HardDrop  Dn=Soft");
  display.display();
  delay(650);

  // ---------------- Game Loop ----------------
  bool running=true;
  while(running && inGame){
    if (readButtons()) return;

    unsigned long now=millis();
    if (now - lastFall >= gravityMs){
      lastFall = now;
      P t=cur; t.r++;
      if (!collides(t)){
        cur=t;
      } else {
        lockPiece(cur);
        int lines[4]; int ln=0;
        for(int r=0;r<ROWS;r++){
          bool full=true;
          for(int c=0;c<COLS;c++) if(!board[r][c]){ full=false; break; }
          if(full){ if(ln<4) lines[ln++]=r; }
        }
        if(ln>0){
          for(int k=0;k<2;k++){
            display.clearDisplay();
            drawBoard();
            for(int i=0;i<ln;i++){
              int r=lines[i];
              display.fillRect(ORG_X, ORG_Y + r*CELL, W_PX, CELL, (k&1)? SSD1306_WHITE: SSD1306_BLACK);
            }
            drawPiece(cur,true);
            drawNext();
            display.setCursor(UI_X, 30); display.print("S:"); display.print(score);
            display.setCursor(UI_X, 56); display.print("Lv:"); display.print(level);
            display.display();
            delay(60);
          }
        }
        int cleared = clearLines();
        if (cleared==1) score+=100;
        else if (cleared==2) score+=300;
        else if (cleared==3) score+=500;
        else if (cleared==4) score+=800;
        if (score/1000 + 1 > level){ level = score/1000 + 1; updateLevel(); }
        cur = nxt; nxt = newPiece();
        if (collides(cur)){ gameOver((int)score); inGame=false; break; }
      }
    }

    display.clearDisplay();
    drawBoard();

    P g=cur; g.r = ghostRow(g);
    uint16_t gm = SHAPES[g.type][g.rot];
    for(int y=0;y<4;y++){
      for(int x=0;x<4;x++){
        if(!cellAtMask(gm,x,y)) continue;
        int rr=g.r+y, cc=g.c+x;
        if(rr>=0) display.drawRect(ORG_X + cc*CELL, ORG_Y + rr*CELL, CELL, CELL, SSD1306_WHITE);
      }
    }

    drawPiece(cur,false);
    drawNext();
    display.setCursor(UI_X, 30); display.print("S:"); display.print(score);
    display.setCursor(UI_X, 56); display.print("Lv:"); display.print(level);

    display.display();
    delay(10);
  }
}
// ====== Brick killer Game (Game 11) ======
void brickkillerGame() {
  int paddleWidth = 24;  // default paddle
  const int paddleHeight = 4;
  int paddleX = (SCREEN_WIDTH - paddleWidth) / 2;
  const int paddleY = SCREEN_HEIGHT - 8;

  const int ballSize = 3;
  const int maxBalls = 5;
  struct Ball { float x, y; float vx, vy; bool active; };
  Ball balls[maxBalls];

  // Initialize first ball
  for (int i=0;i<maxBalls;i++) balls[i].active=false;
  balls[0] = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 1.5, -1.5, true};

  int brickRows = 4;
  int brickCols = 8;
  const int brickWidth = 14;
  const int brickHeight = 6;
  bool bricks[brickRows][brickCols];

  int score = 0;
  bool playing = true;

  // Initialize bricks
  for (int r = 0; r < brickRows; r++)
    for (int c = 0; c < brickCols; c++)
      bricks[r][c] = true;

  while (playing) {
    // --- Input: move paddle ---
    if (digitalRead(LEFT_BUTTON) == LOW && paddleX > 0) paddleX -= 2;
    if (digitalRead(RIGHT_BUTTON) == LOW && paddleX + paddleWidth < SCREEN_WIDTH) paddleX += 2;

    // --- Move balls ---
    for(int b=0;b<maxBalls;b++){
      if(!balls[b].active) continue;
      balls[b].x += balls[b].vx;
      balls[b].y += balls[b].vy;

      // --- Collisions: walls ---
      if (balls[b].x <= 0 || balls[b].x + ballSize >= SCREEN_WIDTH) balls[b].vx *= -1;
      if (balls[b].y <= 0) balls[b].vy *= -1;
      if (balls[b].y + ballSize >= SCREEN_HEIGHT) balls[b].active=false;

      // --- Collisions: paddle ---
      if (balls[b].y + ballSize >= paddleY && balls[b].y + ballSize <= paddleY + paddleHeight &&
          balls[b].x + ballSize >= paddleX && balls[b].x <= paddleX + paddleWidth) {
        balls[b].vy *= -1;
        float hitPos = (balls[b].x + ballSize/2) - (paddleX + paddleWidth/2);
        balls[b].vx = hitPos / (paddleWidth/2) * 2; // max ±2
      }

      // --- Collisions: bricks ---
      bool spawnedExtra = false;
      for (int r = 0; r < brickRows; r++) {
        for (int c = 0; c < brickCols; c++) {
          if (bricks[r][c]) {
            int brickX = c * (brickWidth + 2) + 2;
            int brickY = r * (brickHeight + 2) + 2;
            if (balls[b].x + ballSize > brickX && balls[b].x < brickX + brickWidth &&
                balls[b].y + ballSize > brickY && balls[b].y < brickY + brickHeight) {
              bricks[r][c] = false;
              balls[b].vy *= -1;
              score += 1;

              // --- Spawn extra balls only once per collision ---
              if(!spawnedExtra){
                int extra = random(2,4); // 2–3 extra balls
                for(int k=0;k<maxBalls && extra>0;k++){
                  if(!balls[k].active){
                    balls[k].x = balls[b].x;
                    balls[k].y = balls[b].y;
                    balls[k].vx = random(-15,16)/10.0;
                    balls[k].vy = -1.5;
                    balls[k].active = true;
                    extra--;
                  }
                }
                spawnedExtra = true;
              }
            }
          }
        }
      }
    }

    // --- Draw everything ---
    display.clearDisplay();
    // Bricks
    for (int r = 0; r < brickRows; r++)
      for (int c = 0; c < brickCols; c++)
        if (bricks[r][c]) display.fillRect(c*(brickWidth+2)+2, r*(brickHeight+2)+2, brickWidth, brickHeight, SSD1306_WHITE);

    // Paddle
    display.fillRect(paddleX, paddleY, paddleWidth, paddleHeight, SSD1306_WHITE);

    // Balls
    for(int b=0;b<maxBalls;b++)
      if(balls[b].active) display.fillRect(balls[b].x, balls[b].y, ballSize, ballSize, SSD1306_WHITE);

    // Score
    display.setCursor(0,0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.print("S:");
    display.print(score);

    display.display();
    delay(20);

    // --- Check end conditions ---
    bool anyBrickLeft=false;
    for(int r=0;r<brickRows;r++)
      for(int c=0;c<brickCols;c++)
        if(bricks[r][c]) anyBrickLeft=true;

    bool anyBallActive=false;
    for(int b=0;b<maxBalls;b++) if(balls[b].active) anyBallActive=true;

    if(!anyBrickLeft || !anyBallActive) playing=false;
  }

  // --- After game over: choose next level ---
  display.clearDisplay();
  display.setCursor(0, SCREEN_HEIGHT/2-8);
  display.setTextSize(1);
  display.print("Select Level: L-M/H");
  display.display();

  // Wait for input: LEFT=Medium, RIGHT=Hard
  bool levelChosen=false;
  while(!levelChosen){
    if(digitalRead(LEFT_BUTTON)==LOW){
      paddleWidth=20; // medium: slightly smaller
      brickRows=5; brickCols=9; // more bricks
      levelChosen=true;
    }
    if(digitalRead(RIGHT_BUTTON)==LOW){
      paddleWidth=16; // hard: small paddle
      brickRows=6; brickCols=10; // dense bricks
      levelChosen=true;
    }
    delay(100);
  }
// 2048 game //
}
#define GRID_SIZE 4
int grid[GRID_SIZE][GRID_SIZE];
int score2048 = 0;

const int TILE_SIZE = 14; // For 0.96 OLED
const int TILE_GAP = 2;

void init2048() {
  for (int i = 0; i < GRID_SIZE; i++)
    for (int j = 0; j < GRID_SIZE; j++)
      grid[i][j] = 0;

  score2048 = 0;
  addRandomTile();
  addRandomTile();
}

void addRandomTile() {
  int emptyCount = 0;
  for (int i = 0; i < GRID_SIZE; i++)
    for (int j = 0; j < GRID_SIZE; j++)
      if (grid[i][j] == 0) emptyCount++;

  if (emptyCount == 0) return;

  int r = random(emptyCount);
  int n = 0;
  for (int i = 0; i < GRID_SIZE; i++) {
    for (int j = 0; j < GRID_SIZE; j++) {
      if (grid[i][j] == 0) {
        if (n == r) {
          grid[i][j] = (random(0, 2) ? 2 : 4);
          return;
        }
        n++;
      }
    }
  }
}

// Move helpers
bool moveLeft() {
  bool moved = false;
  for (int i = 0; i < GRID_SIZE; i++) {
    int lastMerge = -1;
    for (int j = 1; j < GRID_SIZE; j++) {
      if (grid[i][j] == 0) continue;
      int k = j;
      while (k > 0 && grid[i][k-1] == 0) { // slide
        grid[i][k-1] = grid[i][k];
        grid[i][k] = 0;
        k--;
        moved = true;
      }
      if (k > 0 && grid[i][k-1] == grid[i][k] && lastMerge != k-1) { // merge
        grid[i][k-1] *= 2;
        score2048 += grid[i][k-1];
        grid[i][k] = 0;
        lastMerge = k-1;
        moved = true;
      }
    }
  }
  return moved;
}

bool moveRight() {
  rotateGrid180();
  bool moved = moveLeft();
  rotateGrid180();
  return moved;
}

bool moveUp() {
  rotateGridLeft();
  bool moved = moveLeft();
  rotateGridRight();
  return moved;
}

bool moveDown() {
  rotateGridRight();
  bool moved = moveLeft();
  rotateGridLeft();
  return moved;
}

// Rotate helpers
void rotateGridLeft() {
  int tmp[GRID_SIZE][GRID_SIZE];
  for (int i=0;i<GRID_SIZE;i++)
    for (int j=0;j<GRID_SIZE;j++)
      tmp[i][j] = grid[j][GRID_SIZE-1-i];
  memcpy(grid,tmp,sizeof(grid));
}

void rotateGridRight() {
  int tmp[GRID_SIZE][GRID_SIZE];
  for (int i=0;i<GRID_SIZE;i++)
    for (int j=0;j<GRID_SIZE;j++)
      tmp[i][j] = grid[GRID_SIZE-1-j][i];
  memcpy(grid,tmp,sizeof(grid));
}

void rotateGrid180() {
  int tmp[GRID_SIZE][GRID_SIZE];
  for (int i=0;i<GRID_SIZE;i++)
    for (int j=0;j<GRID_SIZE;j++)
      tmp[i][j] = grid[GRID_SIZE-1-i][GRID_SIZE-1-j];
  memcpy(grid,tmp,sizeof(grid));
}

// Check game over
bool isGameOver() {
  for(int i=0;i<GRID_SIZE;i++)
    for(int j=0;j<GRID_SIZE;j++)
      if(grid[i][j]==0) return false;
  // Check possible merges
  for(int i=0;i<GRID_SIZE;i++)
    for(int j=0;j<GRID_SIZE-1;j++)
      if(grid[i][j]==grid[i][j+1] || grid[j][i]==grid[j+1][i]) return false;
  return true;
}

// Draw grid
void draw2048() {
  display.clearDisplay();
  for(int i=0;i<GRID_SIZE;i++){
    for(int j=0;j<GRID_SIZE;j++){
      int x = j*(TILE_SIZE+TILE_GAP)+2;
      int y = i*(TILE_SIZE+TILE_GAP)+2;
      if(grid[i][j]!=0){
        display.fillRect(x,y,TILE_SIZE,TILE_SIZE,SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
        display.setCursor(x+3,y+3);
        display.setTextSize(1);
        display.print(grid[i][j]);
        display.setTextColor(SSD1306_WHITE);
      } else {
        display.drawRect(x,y,TILE_SIZE,TILE_SIZE,SSD1306_WHITE);
      }
    }
  }
  // Score
display.setCursor(0, SCREEN_HEIGHT-8);
display.setTextSize(1);
display.print("S:");
display.print(score2048);


  display.display();
}

// 2048 game loop
void play2048() {
  init2048();
  draw2048();

  while(true){
    bool moved=false;

    if(digitalRead(UP_BUTTON)==LOW) moved=moveUp();
    if(digitalRead(DOWN_BUTTON)==LOW) moved=moveDown();
    if(digitalRead(LEFT_BUTTON)==LOW) moved=moveLeft();
    if(digitalRead(RIGHT_BUTTON)==LOW) moved=moveRight();
    if(digitalRead(OK_BUTTON)==LOW) init2048(); // restart game

    if(moved) addRandomTile();
    draw2048();

    if(isGameOver()){
      display.clearDisplay();
      display.setCursor(10,SCREEN_HEIGHT/2-4);
      display.setTextSize(1);
      display.print("Game Over");
      display.display();
      delay(1500);
      break;
    }
    delay(150); // control speed
  }
}
// --- Doodle Jump Game Logic ---
const unsigned char doodlerSprite[] PROGMEM = {
  0b00011000,
  0b00111100,
  0b01111110,
  0b11111111,
  0b10111101,
  0b00100100,
  0b01000010,
  0b10000001
};

void playDoodleJump() {
  int doodlerX = SCREEN_WIDTH/2;
  int doodlerY = SCREEN_HEIGHT-10;
  int velocityY = 0;
  int gravity = 1;
  int jumpPower = -8;

  // ek platform fixed bottom pe
  int platformX = SCREEN_WIDTH/2 - 15;
  int platformY = SCREEN_HEIGHT-5;
  int platformW = 30;
  int platformH = 4;

  while(true){
    // control with buttons
    if(digitalRead(LEFT_BUTTON)==LOW)  doodlerX -= 2;
    if(digitalRead(RIGHT_BUTTON)==LOW) doodlerX += 2;
    if(digitalRead(OK_BUTTON)==LOW && doodlerY + 8 >= platformY && doodlerX+8>=platformX && doodlerX<=platformX+platformW){
      velocityY = jumpPower; // jump
    }

    // gravity
    velocityY += gravity;
    doodlerY += velocityY;

    // collision with platform
    if(doodlerY + 8 >= platformY && doodlerX+8>=platformX && doodlerX<=platformX+platformW && velocityY>0){
      doodlerY = platformY-8;
      velocityY = jumpPower;
    }

    // boundaries
    if(doodlerX<0) doodlerX=0;
    if(doodlerX>SCREEN_WIDTH-8) doodlerX=SCREEN_WIDTH-8;

    // game over if falls
    if(doodlerY > SCREEN_HEIGHT){
      display.clearDisplay();
      display.setCursor(10,SCREEN_HEIGHT/2);
      display.setTextSize(1);
      display.print("Game Over");
      display.display();
      delay(1500);
      break;
    }

    // draw
    display.clearDisplay();
    display.drawBitmap(doodlerX, doodlerY, doodlerSprite, 8, 8, 1);
    display.fillRect(platformX, platformY, platformW, platformH, 1);
    display.display();
    delay(50);
  }
}
