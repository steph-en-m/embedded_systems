//L293D
//left motor
const int motorPin1  = 9;  
const int motorPin2  = 10;
//right motor
const int motorPin3  = 6;
const int motorPin4  = 5;

//This will run only one time.
void setup(){
 
    //Set pins as outputs
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(motorPin3, OUTPUT);
    pinMode(motorPin4, OUTPUT);
    
    //Motor Control:
    //left motor: motorPin1,motorpin2 &
    //right motor: motorpin3,motorpin4

    //turn left motor clockwise for 2 sec.
    analogWrite(motorPin1, 180);
    analogWrite(motorPin2, 0);
    analogWrite(motorPin3, 180);
    analogWrite(motorPin4, 0);
    delay(2000); 
    //turn left motor counter-clockwise for 2 sec.
    analogWrite(motorPin1, 0);
    analogWrite(motorPin2, 180);
    analogWrite(motorPin3, 0);
    analogWrite(motorPin4, 180);
    delay(2000);
     
    //turn right motor clockwise for 2 sec.
    analogWrite(motorPin1, 0);
    analogWrite(motorPin2, 180);
    analogWrite(motorPin3, 180);
    analogWrite(motorPin4, 0);
    delay(2000); 
    //turn right motor counter-clockwise for 2 sec.
    analogWrite(motorPin1, 180);
    analogWrite(motorPin2, 0);
    analogWrite(motorPin3, 0);
    analogWrite(motorPin4, 180);
    delay(2000);    
    
    //stop motors
    analogWrite(motorPin1, 0);
    analogWrite(motorPin2, 0);
    analogWrite(motorPin3, 0);
    analogWrite(motorPin4, 0);
  
}


void loop(){
  analogWrite(motorPin1, 360);
    analogWrite(motorPin2, 0);
    analogWrite(motorPin3, 0);
    analogWrite(motorPin4, 180);
    
  }
