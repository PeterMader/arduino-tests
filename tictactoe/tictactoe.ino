#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>


// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     53
#define TFT_RST    9  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     8

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
#define TFT_SCLK 52   // set these to be whatever pins you like!
#define TFT_MOSI 51   // set these to be whatever pins you like!
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

const byte FIELD_SIZE = 4;
const byte BOARD_SIZE = 31;

const uint16_t BACKGROUND_COLOR = ST7735_BLACK;
const uint16_t CIRCLE_COLOR     = ST7735_GREEN;
const uint16_t CROSS_COLOR      = ST7735_RED;
const uint16_t TEXT_COLOR       = ST7735_WHITE; 
const uint16_t WINNER_COLOR     = ST7735_CYAN;

const byte PIN_LEFT  = 5;
const byte PIN_DOWN  = 4;
const byte PIN_UP    = 3;
const byte PIN_RIGHT = 2;
const byte PIN_ENTER = 6;

byte states[9];

byte selectedX = 1;
byte selectedY = 1;

byte activePlayer = 1;

byte countingMoves = 0;

void drawCircle (byte x, byte y) {
      tft.drawCircle(21 + 42 * x,21 + 42 * y, 15, CIRCLE_COLOR);  
 }

 void drawCross (byte x, byte y) {
    tft.drawLine(42 * x, 42 * y, 42 * (x + 1), 42 * (y + 1), CROSS_COLOR);
    tft.drawLine(42 * x, 42 * y + 42, 42 * (x + 1), 42 * y,  CROSS_COLOR);     
 }

void setup(void) { 
    // Use this initializer if you're using a 1.8" TFT
    tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

    reset();

    pinMode(PIN_LEFT,  INPUT);
    pinMode(PIN_DOWN,  INPUT);
    pinMode(PIN_UP,    INPUT);
    pinMode(PIN_RIGHT, INPUT);
    pinMode(PIN_ENTER, INPUT);

    // Pullup-Widerstand aktivieren
    digitalWrite(PIN_LEFT,  HIGH);
    digitalWrite(PIN_DOWN,  HIGH);
    digitalWrite(PIN_UP,    HIGH);
    digitalWrite(PIN_RIGHT, HIGH);
    digitalWrite(PIN_ENTER, HIGH);
    tft.setRotation(2);
}

// resets the board and the game data
void reset() {
    for (byte i = 0; i < 9; i++) {
      states[i] = 0;
    }
    countingMoves = 0;
}

// conditionally sets the direction to a new one
void setSelectedField() {
    bool pressedLeft = false;
    bool pressedDown  = false;
    bool pressedUp    = false;
    bool pressedRight = false;
    bool pressedEnter = false;
  
    do { 
      delay(50);
      pressedLeft  = digitalRead(PIN_LEFT)  == LOW;
      pressedDown  = digitalRead(PIN_DOWN)  == LOW;
      pressedUp    = digitalRead(PIN_UP)    == LOW;
      pressedRight = digitalRead(PIN_RIGHT) == LOW;
      pressedEnter = digitalRead(PIN_ENTER) == LOW;
    } while (!pressedLeft && !pressedDown && !pressedUp && !pressedRight && !pressedEnter);

    if (pressedUp && selectedY != 0) {
      selectedY --;
    }
    if(pressedLeft && selectedX != 0) {
      selectedX --;
    }
    if(pressedDown && selectedY != 2) {
      selectedY ++;
    }
    if(pressedRight && selectedX != 2) {
      selectedX ++;
    }
    if(pressedEnter && !states[selectedX * 3 + selectedY]) {
      states[selectedX * 3 + selectedY] = activePlayer;
      draw();

      countingMoves ++;

      if (activePlayer == 1) {
        activePlayer = 2;
      } 
      else {
        activePlayer = 1;
      }
    }
    delay(200);
}

void drawField( byte x, byte y, byte state) {
  if (state == 1) {
    drawCircle(x, y);
  }
  if (state == 2) {
    drawCross(x, y);
  }
}

// redraws the whole board
void draw() {

    tft.fillRect(0, 0, tft.width(), tft.height(), BACKGROUND_COLOR);
  
    //draw the vertical lines
    tft.drawFastVLine(42, 0, 128, TEXT_COLOR);
    tft.drawFastVLine(84, 0, 128, TEXT_COLOR);
  
    // draw the horizontal lines
    tft.drawFastHLine(0, 42, 128, TEXT_COLOR);
    tft.drawFastHLine(0, 84, 128, TEXT_COLOR);

    tft.drawCircle(21 + 42 * selectedX, 21 + 42 * selectedY, 2, CIRCLE_COLOR); 

    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            drawField(x, y, states[x * 3 + y]);
        }
    }
   
}

void winner() {
    const char* text = "Winner!";
    int16_t  x1, y1;
    uint16_t w, h;

    tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    
    tft.setCursor(
        (tft.width() - w) / 2,
        (tft.height()-h) / 2
    );

    tft.setTextColor(WINNER_COLOR);
    tft.print(text);
    delay(1000);
}


void loop() {
    draw();
    setSelectedField();

  if (countingMoves == 9) {
    delay(1000);
    reset();
  }

  if (states[0] && states[0] == states[1] && states[1] == states[2] ||
      states[3] && states[3] == states[4] && states[4] == states[5] || 
      states[6] && states[6] == states[7] && states[7] == states[8] || 
      states[0] && states[0] == states[3] && states[3] == states[6] || 
      states[1] && states[1] == states[4] && states[4] == states[7] || 
      states[2] && states[2] == states[5] && states[5] == states[8] || 
      states[0] && states[0] == states[4] && states[4] == states[8] || 
      states[6] && states[6] == states[4] && states[4] == states[2]) {
      winner();
      delay (2000);
      reset();    
      }   
}
