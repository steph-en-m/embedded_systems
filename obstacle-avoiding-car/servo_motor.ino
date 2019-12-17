#include <LiquidCrystal_I2C.h>
#include <NewPing.h>
#include <AFMotor.h>
#include <Servo.h>

#define servosignalpin 11
#define echoPin A0
#define trigPin A1
#define MinDistance 10
#define MaxDistance 200

boolean goForward = false;
long distance = 50;
int speedInit = 0;
long duration;

Servo Servo1;
LiquidCrystal_I2C lcd(0x27, 16, 2);
NewPing sonar(trigPin, echoPin, MaxDistance);

AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(2, MOTOR12_1KHZ);

void setup()
{
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
  
}

long Distance() {
   digitalWrite(trigPin, LOW);
   delayMicroseconds(2);
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPin, LOW);
   distance = sonar.ping_cm();
   lcd.print("distance:");
   lcd.setCursor(9,0);
   lcd.print(distance);
   lcd.print("cm");
   delay(1000);
   lcd.clear();
}

void loop()
{
   Distance();
   Servo1.write(110);
   if(distance <= MinDistance){
    Servo1.write(0);
    
   }
   else{
    Servo1.write(115);
    }

}
