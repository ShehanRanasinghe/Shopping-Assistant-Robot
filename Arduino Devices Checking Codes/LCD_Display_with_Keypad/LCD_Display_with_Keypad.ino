#include <Key.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> //Code for LCD 16X2 with I2C Module

LiquidCrystal_I2C lcd(0x27, 16, 2);  //Code for LCD 16X2 with I2C Module

int i=0;
const char number_of_rows = 4;
const char number_of_columns = 4;

char row_pins[number_of_rows] = {9, 8, 7, 6};
char column_pins[number_of_columns] = {5, 4, 3, 2};

char key_array[number_of_rows][number_of_columns] = {  
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
  };

Keypad k = Keypad(makeKeymap(key_array),row_pins , column_pins, number_of_rows, number_of_columns);

void setup()
{
  //Code for LCD 16X2 with I2C Module
  Serial.begin(9600);
  lcd.init();                      
  lcd.backlight();


}

void loop()
{
  char key_pressed = k.getKey();
  if(key_pressed)
  {
    Serial.println(key_pressed);
    lcd.setCursor(i,0);
    lcd.print(key_pressed);
    i=i+1;
  }
}
