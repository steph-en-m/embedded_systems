#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <NewPing.h>
#include <Servo.h>

//our L298N control for motors
#define LeftMotorForward 9
#define LeftMotorBackward 10
#define RightMotorForward 6
#define RightMotorBackward 5

//sensor pins
#define trig_pin A1
#define echo_pin A0

#define maximum_distance 200
boolean goesForward = false;
int distance = 100;

NewPing sonar(trig_pin, echo_pin, maximum_distance); //sensor function
Servo servo_motor;  
LiquidCrystal_I2C lcd(0x27,16,2);


void setup() {
 
  pinMode(RightMotorForward, OUTPUT);
  pinMode(LeftMotorForward, OUTPUT);
  pinMode(LeftMotorBackward, OUTPUT);
  pinMode(RightMotorBackward, OUTPUT);

  servo_motor.attach(11); //our servo pin

  servo_motor.write(90);
  delay(2000);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);

  // lcd checker
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Initializing...");
  lcd.clear();
}

void loop() {
  int distanceRight = 0;
  int distanceLeft = 0;
  delay(50);

  if(distance <= 10){
    moveStop();
    delay(300);
    moveBackward();
    delay(400);
    moveStop();
    delay(300);
    distanceLeft = lookLeft();
    delay(300);

    if(distanceRight >= distanceLeft){
      turnRight();
      moveStop();
      
      }
      else{
        turnLeft();
        moveStop();
        }
    }
    else{
      moveForward();
      }
      distance = readPing();
}

int lookRight(){
  servo_motor.write(340);
  delay(500);
  int distance = readPing();
  delay(100);
  servo_motor.write(70);
  return distance;
  }
  
   int lookLeft(){
    servo_motor.write(170);
    delay(500);
    int distance = readPing();
    delay(100);
    servo_motor.write(80);
    return distance;
    delay(100);
    }

int readPing(){
  delay(70);
  int cm = sonar.ping_cm();
  if (cm==0){
    cm=250;
    }
    return cm;
  }

  void moveStop(){
    digitalWrite(RightMotorForward, LOW);
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(RightMotorBackward, LOW);
    digitalWrite(LeftMotorBackward, LOW);
    }

void moveForward(){
  if(!goesForward){
    goesForward=true;

    lcd.clear();
    lcd.print("Forward");
    lcd.setCursor(0,1);
    lcd.print("distance:");
    lcd.print(distance);
  
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(RightMotorForward, HIGH);

    digitalWrite(LeftMotorBackward, LOW);
    digitalWrite(RightMotorBackward, LOW);
    }
  }

 void moveBackward(){
  
    goesForward=false;

    lcd.clear();
    delay(200);
    lcd.print("Backward");
    lcd.setCursor(2,1);
    delay(200);
    lcd.print("distance:");
    lcd.print(distance);
    
    digitalWrite(LeftMotorBackward, HIGH);
    digitalWrite(RightMotorBackward, HIGH);
    
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(RightMotorForward, LOW);
    
  }

 void turnRight(){
  
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(RightMotorBackward, HIGH);
    
    digitalWrite(LeftMotorBackward, LOW);
    digitalWrite(RightMotorForward, LOW);

    delay(500);

    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(RightMotorForward, HIGH);
    
    digitalWrite(LeftMotorBackward, LOW);
    digitalWrite(RightMotorBackward, LOW);
  }

 void turnLeft(){
  
    digitalWrite(LeftMotorBackward, HIGH);
    digitalWrite(RightMotorForward, HIGH);
    
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(RightMotorBackward, LOW);

    delay(500);

    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(RightMotorForward, HIGH);
    
    digitalWrite(LeftMotorBackward, LOW);
    digitalWrite(RightMotorBackward, LOW);
  }






  






    
