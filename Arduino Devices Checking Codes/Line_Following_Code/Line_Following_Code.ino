#include <AFMotor.h>

#define left A0
#define right A1

AF_DCMotor motor1(1, MOTOR12_1KHZ); 
AF_DCMotor motor2(2, MOTOR12_1KHZ);
AF_DCMotor motor3(3, MOTOR34_1KHZ);
AF_DCMotor motor4(4, MOTOR34_1KHZ);

void setup() {
  
  pinMode(left,INPUT);
  pinMode(right,INPUT);
  
}

void loop(){
  
  if(digitalRead(left)==0 && digitalRead(right)==0){
    // Move forward 
    motor1.run(FORWARD);
    motor1.setSpeed(100);
    motor2.run(FORWARD); 
    motor2.setSpeed(100);
    motor3.run(FORWARD);
    motor3.setSpeed(100);
    motor4.run(FORWARD);
    motor4.setSpeed(100);
  }
  else if(digitalRead(left)==0 && !analogRead(right)==0){
    //turn right 
    motor1.run(FORWARD);
    motor1.setSpeed(100);
    motor2.run(FORWARD);
    motor2.setSpeed(100);
    motor3.run(BACKWARD);
    motor3.setSpeed(100);
    motor4.run(BACKWARD);
    motor4.setSpeed(100);
    
  }
  else if(!digitalRead(left)==0 && digitalRead(right)==0){
    //Turn left
    motor1.run(BACKWARD);
    motor1.setSpeed(100);
    motor2.run(BACKWARD);
    motor2.setSpeed(100);
    motor3.run(FORWARD);
    motor3.setSpeed(100);
    motor4.run(FORWARD);
    motor4.setSpeed(100);
   
  }

  else if(!digitalRead(left)==0 && !digitalRead(right)==0){
    //Stop 
    motor1.run(RELEASE);
    motor1.setSpeed(0);
    motor2.run(RELEASE);
    motor2.setSpeed(0);
    motor3.run(RELEASE);
    motor3.setSpeed(0);
    motor4.run(RELEASE);
    motor4.setSpeed(0);
   
  }
  
}

