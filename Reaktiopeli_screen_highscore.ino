#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* OLED-määritykset */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/*
  Reaktiopeli - 3 LEDiä ja 3 nappia
*/

const int ledPins[] = {2, 3, 4};
const int buttonPins[] = {8, 9, 10};

int score = 0;
float gameDelay = 1000.0;
const float speedUpFactor = 0.98;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // OLED käynnistys
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("OLED ei löytynyt!");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("Peli alkaa");
  display.display();

  randomSeed(analogRead(0));
  delay(2000);
}

void loop() {
  int targetLed = random(0, 3);
  digitalWrite(ledPins[targetLed], HIGH);

  unsigned long startTime = millis();
  bool correctPressed = false;
  bool gameOver = false;

  while (millis() - startTime < gameDelay) {
    for (int i = 0; i < 3; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        if (i == targetLed) {
          correctPressed = true;
        } else {
          gameOver = true;
        }
        break;
      }
    }
    if (correctPressed || gameOver) break;
  }

  if (!correctPressed) gameOver = true;

  digitalWrite(ledPins[targetLed], LOW);

  if (gameOver) {
    handleGameOver();
  } else {
    score++;
    gameDelay *= speedUpFactor;
    updateDisplay();
    delay(200);
  }
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("PISTEET");
  display.setTextSize(3);
  display.setCursor(30, 30);
  display.println(score);
  display.display();
}

void handleGameOver() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 0);
  display.println("GAME OVER");
  display.setTextSize(2);
  display.setCursor(20, 30);
  display.print("Pts: ");
  display.println(score);
  display.display();

  // LED-vilkutus
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 3; j++) digitalWrite(ledPins[j], HIGH);
    delay(200);
    for (int j = 0; j < 3; j++) digitalWrite(ledPins[j], LOW);
    delay(200);
  }

  score = 0;
  gameDelay = 1000.0;
  delay(2000);
}
