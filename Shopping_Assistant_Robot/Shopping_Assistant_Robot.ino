#include <AFMotor.h> //L293D Motor Driver Library
#include <Wire.h>    //I2C Module
#include <LiquidCrystal_I2C.h>//I2C Module with 16x2 LCD Library
#include <Keypad.h>  //Keypad Library
#include "GM65_scanner.h" // GM65 barcode scanner custom library from github

LiquidCrystal_I2C lcd(0x27, 16, 2); //I2C address 0x27

#define left A0 //IR Sensor 01
#define right A1 //IR Sensor 02 

AF_DCMotor motor1(1, MOTOR12_1KHZ); // Motor 1 connected to M1 (first driver chip - MOTOR12), 1 kHz PWM
AF_DCMotor motor2(2, MOTOR12_1KHZ); // Motor 2 connected to M2 (first driver chip - MOTOR12), 1 kHz PWM
AF_DCMotor motor3(3, MOTOR34_1KHZ); // Motor 3 connected to M3 (second driver chip - MOTOR34), 1 kHz PWM
AF_DCMotor motor4(4, MOTOR34_1KHZ); // Motor 4 connected to M4 (second driver chip - MOTOR34), 1 kHz PWM

//Dimensions of the 4X4 KeyPad
const byte ROWS = 4;   
const byte COLS = 4;                   

//Maps Keypad buttons to characters
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

//Physical pin connections of the KeyPad.
byte rowPins[ROWS] = {44, 42, 40, 38};
byte colPins[COLS] = {36, 34, 32, 30};

//Creates the KeyPad object for input reading
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//Array for Barcodes assigned to each item number (1 to 7)
String itemBarcodes[8] = {
  "",           // index 0 unused
  "3857291046", // 1 → Pringles
  "7902431857", // 2 → Snickers Chocolate
  "6415072389", // 3 → Anchor Butter
  "1246893057", // 4 → IMORICH Ice Cream
  "9083761542", // 5 → Ramen Noodles
  "3569817204", // 6 → Swiss roll
  "7024189365"  // 7 → Raffaello
};

// Barcode scanner object using Arduino Mega Serial1 (18 and 19 pins)
GM65_scanner scanner(&Serial1);

//Global Variables
int selectedItem = 0;       // Holds the item numbers 1 to 7
String correctCode = "";    // To matche the selected item using barcode
String scannedCode = "";    // Store the scanned barcodes from GM65
bool isAuthorized = false;  // Until item is selected the system Starts False
bool itemFound = false;     // When item is found the condition is set to True

void setup() 
{
  //IR Sensors
  pinMode(left, INPUT);              
  pinMode(right, INPUT);             

  // Begin communication with GM65 barcode scanner
  Serial1.begin(9600);     

  lcd.init();      // Initialize LCD
  lcd.backlight(); // Turn on LCD backlight
  lcd.setCursor(0, 0);
  lcd.print("Select Item 1-7");

  scanner.init();                // Initialize the GM65 scanner
  scanner.enable_setting_code(); // Allow GM65 to read barcodes
}

void loop() {
  char key = keypad.getKey(); //Get the current pressed key

  //Item Selection 1 to 7
  if (!isAuthorized && key >= '1' && key <= '7') {
    selectedItem = key - '0';                 // Convert char to int
    correctCode = itemBarcodes[selectedItem]; // Get matching barcode from Array

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Item Selected:");
    lcd.setCursor(0, 1);
    lcd.print(correctCode);   // Show the barcode for selected item by customer
    delay(1500);

    lcd.clear();
    lcd.print("Start Searching");  // Start message
    isAuthorized = true;        //Start the line following
  }

  //When Press D in keypad the robot reset and allow to search new item
  if (key == 'D') 
  {
    StopMotors();                     
    isAuthorized = false;   //Set False to stop the robot       
    itemFound = false;     
    correctCode = "";  // Clear barcode
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Select Item 1-7"); // Show message again for selecting the items
  }

  //When Press C in keypad the robot stop like a emergency stop
  if (key == 'C') 
  {
    StopMotors();                        
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Stopped");             
  }

  //If authorized and item not found, follow the line and scanning
  if (isAuthorized && !itemFound) 
  {
    LineFollowing();                     
    scannedCode = scanner.get_info();    // Read barcodes from GM65
    scannedCode.trim();                  // Clean up extra characters like \n

    // If scanned barcode matches customer selected barcode stop and show result
    if (scannedCode.length() > 0) 
    {
      if (scannedCode == correctCode) 
      {
        itemFound = true;              
        StopMotors();                  

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Item Found!");
        lcd.setCursor(0, 1);
        lcd.print(scannedCode);  // Display the matched barcode
      }
    }
  }
}


void LineFollowing() 
{
  if (digitalRead(left) == 0 && digitalRead(right) == 0) 
  {
    MoveForward();
  } 
  else if (digitalRead(left) == 0 && digitalRead(right) == 1) 
  {
    TurnRight();
  } 
  else if (digitalRead(left) == 1 && digitalRead(right) == 0) 
  {
    TurnLeft();
  } 
  else 
  {
    StopMotors();
  }
}

// Motors
void MoveForward() 
{
  motor1.run(FORWARD); motor1.setSpeed(80);
  motor2.run(FORWARD); motor2.setSpeed(80);
  motor3.run(FORWARD); motor3.setSpeed(80);
  motor4.run(FORWARD); motor4.setSpeed(80);
}

void TurnRight() 
{
  motor1.run(FORWARD); motor1.setSpeed(80);
  motor2.run(FORWARD); motor2.setSpeed(80);
  motor3.run(BACKWARD); motor3.setSpeed(80);
  motor4.run(BACKWARD); motor4.setSpeed(80);
}

void TurnLeft() 
{
  motor1.run(BACKWARD); motor1.setSpeed(80);
  motor2.run(BACKWARD); motor2.setSpeed(80);
  motor3.run(FORWARD); motor3.setSpeed(80);
  motor4.run(FORWARD); motor4.setSpeed(80);
}

void StopMotors() 
{
  motor1.run(RELEASE); motor1.setSpeed(0);
  motor2.run(RELEASE); motor2.setSpeed(0);
  motor3.run(RELEASE); motor3.setSpeed(0);
  motor4.run(RELEASE); motor4.setSpeed(0);
}
