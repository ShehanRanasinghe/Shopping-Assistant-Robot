#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {44, 42, 40, 38};
byte colPins[COLS] = {36, 34, 32, 30};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String input = "";

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Enter input:");
  lcd.setCursor(0, 1);
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    if (key == '#') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("You typed:");
      lcd.setCursor(0, 1);
      lcd.print(input);

      Serial.println("Input Received: " + input);

      input = "";
      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter input:");
      lcd.setCursor(0, 1);
    }
    else if (key == '*') {
  
      if (input.length() > 0) {
        input.remove(input.length() - 1);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter input:");
        lcd.setCursor(0, 1);
        lcd.print(input);
      }
    }
    else {
      if (input.length() < 16) { 
        input += key;
        lcd.setCursor(0, 1);
        lcd.print(input);
      }
    }
  }
}
