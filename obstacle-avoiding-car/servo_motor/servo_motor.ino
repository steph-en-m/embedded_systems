#include <LiquidCrystal_I2C.h>
#include <NewPing.h>
#include <AFMotor.h>
#include <Servo.h>

#define servosignalpin 11
#define echoPin A0
#define trigPin A1
#define MinDistance 10
#define MaxDistance 200

//left motor
const int motorPin1  = 9;  
const int motorPin2  = 10;
//right motor
const int motorPin3  = 6;
const int motorPin4  = 5;

Servo Servo1;
LiquidCrystal_I2C lcd(0x27, 16, 2);
NewPing sonar(trigPin, echoPin, MaxDistance);

void setup()
{
  //Set pins as outputs
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
    
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  Servo1.attach(servosignalpin);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  Servo1.write(110);
  lcd.print("Initializing..");
  delay(1000);
  lcd.clear();
  
  digitalWrite(trigPin, LOW);
  delay(10);
  digitalWrite(trigPin, HIGH);
  delay(10);
  digitalWrite(trigPin, LOW);
  delay(2000);
  distance = readPing();
  delay(100);
  lcd.print("distance:");
  lcd.setCursor(9,0);
  lcd.print(distance);
  lcd.print("cm");
  delay(1000);
  lcd.clear();
}
  
void loop()
{
  int distanceR = 0;
  int distanceL =  0;
  delay(40);
 
  if(distance<=15)
  {
   moveStop();
   delay(100);
   moveBackward();
   delay(300);
   moveStop();
   delay(200);
   distanceR = lookRight();
   delay(200);
   distanceL = lookLeft();
   delay(200);

   if(distanceR>=distanceL)
   {
     turnRight();
     moveStop();
   }else
   {
     turnLeft();
     moveStop();
   }
  }else
  {
   moveForward();
  }
 }

int lookRight()
{
    Servo1.write(50); 
    delay(500);
    int distance = readPing();
    delay(100);
    Servo1.write(115); 
    return distance;
}

int lookLeft()
{
    Servo1.write(170); 
    delay(500);
    int distance = readPing();
    delay(100);
    Servo1.write(115); 
    return distance;
    delay(100);
}

void moveStop() {
  //stop motors
    analogWrite(motorPin1, 0);
    analogWrite(motorPin2, 0);
    analogWrite(motorPin3, 0);
    analogWrite(motorPin4, 0);
  } 
  
void moveForward() {

}

void moveBackward() {
 
}  

void turnRight() {
  analogWrite(motorPin1, 180);
  analogWrite(motorPin2, 0);
  analogWrite(motorPin3, 180);
  analogWrite(motorPin4, 0);
    
  analogWrite(motorPin1, 0);
  analogWrite(motorPin2, 180);
  analogWrite(motorPin3, 180);
  analogWrite(motorPin4, 0);   
} 
 
void turnLeft() {
  //right motors counter-clockwise
  analogWrite(motorPin1, 180);
  analogWrite(motorPin2, 0);
  analogWrite(motorPin3, 0);
  analogWrite(motorPin4, 180);
}  
