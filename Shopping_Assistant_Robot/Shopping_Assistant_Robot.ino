#include <Wire.h>    //I2C Module
#include <LiquidCrystal_I2C.h>//I2C Module with 16x2 LCD Library
#include <Keypad.h>  //Keypad Library
#include <SoftwareSerial.h>
#include "GM65_scanner.h" // GM65 barcode scanner custom library from github
SoftwareSerial mySerial(10, 11); // RX, TX

GM65_scanner scanner(&mySerial);

LiquidCrystal_I2C lcd(0x27, 16, 2); //I2C address 0x27

#define IR_LEFT   A13 //IR Sensor Right (Grey)
#define IR_CENTER A14 //IR Sensor Center (Orange)
#define IR_RIGHT  A15 //IR Sensor Left  (Black)

//L298N Motor Control Pins
#define ENA 4   // PWM speed control (left motor)
#define IN1 5   // Left motor forward
#define IN2 6   // Left motor backward
#define IN3 7   // Right motor forward
#define IN4 8   // Right motor backward
#define ENB 9   // PWM speed control (right motor)
int motorSpeed = 100; //Motor Speed

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
byte rowPins[ROWS] = {30, 32, 34, 36};
byte colPins[COLS] = {38, 40, 42, 44};

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

//Global Variables
int selectedItem = 0;       // Holds the item numbers 1 to 7
String correctCode = "";    // To matche the selected item using barcode
String scannedCode = "";    // Store the scanned barcodes from GM65
bool isAuthorized = false;  // Until item is selected the system Starts False
bool itemFound = false;     // When item is found the condition is set to True

void setup() {
  // IR Sensors
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_CENTER, INPUT);
  pinMode(IR_RIGHT, INPUT);

  // Motor Pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Begin communication with GM65 barcode scanner
  mySerial.begin(9600);

  lcd.init();      // Initialize LCD
  lcd.backlight(); // Turn on LCD backlight
  lcd.setCursor(0, 0);
  lcd.print("Select Item 1-7");

  scanner.init();
  scanner.enable_setting_code();

}

void loop() {
    char key = keypad.getKey(); //Get the current pressed key

  //Item Selection 1 to 7
  if (!isAuthorized && key >= '1' && key <= '7') 
  {
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
  int left = digitalRead(IR_LEFT);
  int center = digitalRead(IR_CENTER);
  int right = digitalRead(IR_RIGHT);

  if (center == 1 && left == 0 && right == 0) {
    MoveForward();  // Line is centered
  }
  else if (left == 0 && center == 1 && right == 1) {
    TurnLeft();     // Line detected on left only
  }
  else if (right == 0 && center == 1 && left == 1) {
    TurnRight();    // Line detected on right only
  }
  else if (center == 0 && left == 0 && right == 1) {
    // Line slightly left, adjust left
    TurnLeft();
  }
  else if (center == 0 && right == 0 && left == 1) {
    // Line slightly right, adjust right
    TurnRight();
  }
  else if (center == 0 && left == 0 && right == 0) {
    StopMotors();   // All sensors on white — Stop
  }
  else if (center == 1 && left == 1 && right == 1) {
    StopMotors();   // All sensors on Black — Stop
  }
  else{
    StopMotors(); //Stop
  }
}

//Movements

void MoveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
}

void TurnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
}

void TurnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
}

void StopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

