#include <Wire.h>
#include <LiquidCrystal_I2C.h> //Code for LCD 16X2 with I2C Module

LiquidCrystal_I2C lcd(0x27, 16, 2);  //Code for LCD 16X2 with I2C Module

void setup()
{
  //Code for LCD 16X2 with I2C Module
  lcd.init();                      
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Shopping");
  lcd.setCursor(0,1);
  lcd.print("Assistant Robot");

}


void loop()
{
  //No Void Loop Code for LCD 16X2 with I2C Module for testing (print Hello World!)

}
