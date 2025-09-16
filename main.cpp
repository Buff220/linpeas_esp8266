#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define PIN_D6 12
#define PIN_D7 13

bool SO = false;
char RED_YELLOW[100];
char RED[100];

void setup() {
  pinMode(PIN_D6, OUTPUT);
  pinMode(PIN_D7, OUTPUT);
  Serial.begin(115200);
  Wire.begin(D2, D1); // SDA, SCL for ESP8266
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  display.clearDisplay();
  display.display();
}

void red_yellow() {
  digitalWrite(PIN_D6, HIGH);
  digitalWrite(PIN_D7, LOW);
  delay(300);
  digitalWrite(PIN_D7, HIGH);
  digitalWrite(PIN_D6, LOW); 
  delay(280);
}

void loop() {
  if (SO) red_yellow();

  if (Serial.available() >= 2) {
    uint8_t length = Serial.read();
    uint8_t col = Serial.read();

    // Wait until full message arrives
    while (Serial.available() < length) delay(1);

    char buffer[101];  // max 100 chars + null
    uint8_t read_len = length > 100 ? 100 : length;

    for (uint8_t i = 0; i < read_len; i++) buffer[i] = Serial.read();
    buffer[read_len] = '\0';

    display.clearDisplay();
    if (buffer){
      if (col == 1) {  // RED/YELLOW
        SO = true;
        memset(RED_YELLOW, 0, sizeof(RED_YELLOW));
        strncpy(RED_YELLOW, buffer, read_len);
        RED_YELLOW[read_len] = '\0';
      } else {         // RED or normal
        if (!SO) digitalWrite(PIN_D7, HIGH);
        memset(RED, 0, sizeof(RED));
        strncpy(RED, buffer, read_len);
        RED[read_len] = '\0';
      }
    }

    // Always show previous RED/YELLOW if any
    if (RED_YELLOW) {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 24);
      display.print(RED_YELLOW);
    }
    if (RED) {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print(RED);
    }

    display.display();
  }

  delay(20);
}
