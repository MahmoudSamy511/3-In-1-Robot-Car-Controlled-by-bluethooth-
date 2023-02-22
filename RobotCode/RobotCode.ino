#include<Servo.h>
Servo myservo;
#define IN1 10   // Forward Pin For Left Motor
#define IN2 5    // Backward Pin For Left Motor
#define IN3 6    // Forward Pin For Right Motor
#define IN4 11   // Backward Pin For Right Motor
#define RS  2    // Right IR Sensor Pin
#define LS  3    // Left IR Sensor Pin
#define echo 9     //Echo pin
#define trigger 8  //Trigger pin
char mode;         //  Variable to Store mode value 
int distance_L, distance_F = 30, distance_R;
long distance;
int set = 20;   // Minimum Distance for Ultrasonic sensor to stop
int flag; 
int pos;        //  Variable to Store first angle of servo   
void setup() {
  // Begin Serial to take bluetooth commands
  Serial.begin(9600);
  // configrate Input And Output Pins
  pinMode(echo, INPUT );
  pinMode(trigger, OUTPUT);
  pinMode(RS, INPUT);
  pinMode(LS, INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  myservo.attach(A0);
}
// Function to return distance that ultrasonic sensor detect
long Ultrasonic_read() {
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  distance = pulseIn (echo, HIGH);
  return (distance  / 29 / 2);
}

void Forward() {
  // Forward
  digitalWrite(IN1, 1);
  digitalWrite(IN2, 0);
  digitalWrite(IN3, 1);
  digitalWrite(IN4, 0);
}
void Backward() {
  // Backward
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 1);
  digitalWrite(IN3, 0);
  digitalWrite(IN4, 1);
}
void Right() {
  // Turn Right
  digitalWrite(IN1, 1);
  digitalWrite(IN2, 0);
  digitalWrite(IN3, 0);
  digitalWrite(IN4, 1);
}
void Left() {
  // Turn Left
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 1);
  digitalWrite(IN3, 1);
  digitalWrite(IN4, 0);
}
void Stop() {
  // Stop
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 0);
  digitalWrite(IN3, 0);
  digitalWrite(IN4, 0);
}

void loop() {

  mode = Serial.read();   // Reading mode From HC-05
  if (mode == 'F' || mode == 'B' || mode == 'L' || mode == 'R') {
    flag = 0;   // Changing flag value to 0 in Manual Mode
  }
  else if ( mode == 'W') {
    flag = 1;    // Changing flag value to 1 in Line Folloer Mode
  }
  else if (mode == 'U') {
    flag = 2;    // Changing flag value to 2 in Obstacle Avoider Mode
  }
  if (flag == 0) {           // Manual Mode
    switch (mode) {
      case 'F':
        Forward();
        break;
      case 'B':
        Backward();
        break;
      case 'R':
        Right();
        break;
      case 'L':
        Left();
        break;
      case 'S':
        Stop();
        break;
    }
  }
  else if (flag == 1) {         // Line Follower Mode
    int V1 = digitalRead(RS);
    int V2 = digitalRead(LS);
    if (V1 == 0 && V2 == 0 )
    {
      // Go Forward
      analogWrite(IN1, 255);         
      analogWrite(IN2, 0);
      analogWrite(IN3, 255);
      analogWrite(IN4, 0);
      analogWrite(IN1, 70);
      analogWrite(IN3, 70);
    }
    // Turn Right
    else if (V1 == 1 && V2 == 0 )
    {
      analogWrite(IN1, 255);
      analogWrite(IN2, 0);
      analogWrite(IN3, 0);
      analogWrite(IN4, 255);
      analogWrite(IN1, 65);
      analogWrite(IN4, 65);
    }
    else if (V1 == 0 && V2 == 1 )
    {
      // Turn Left
      analogWrite(IN1, 0);
      analogWrite(IN2, 60);
      analogWrite(IN3, 60);
      analogWrite(IN4, 0);
    }
    else if (V1 == 1 && V2 == 1 )
    {
      // Go Forward
      analogWrite(IN1, 255);
      analogWrite(IN2, 0);
      analogWrite(IN3, 255);
      analogWrite(IN4, 0);
      analogWrite(IN1, 70);
      analogWrite(IN3, 70);
    }
  }
  else if (flag == 2) {            // Obstacle Avoider Mode
    distance_F = Ultrasonic_read();   //check initial distance
    if (distance_F > set) {
      Forward();
    }
    else {
      Stop();
      delay(100);
      // Distance calculation left and right Side
      for (pos = 90; pos <= 180; pos += 1) {
        myservo.write(pos);
        delay(15);
      }
      distance_L = Ultrasonic_read();
      delay(100);
      for (pos = 180; pos >= 0; pos -= 1) {
        myservo.write(pos);
        delay(15);
      }
      distance_R = Ultrasonic_read();
      delay(100);
      for (pos = 0; pos <= 90; pos += 1) { 
        myservo.write(pos);              
        delay(15);       
      }
      delay(100);
      if (distance_L > distance_R) {
        // Go Left Side if it is clear
        Left();
        delay(600);
      }
      else if (distance_R > distance_L) {
        // Go Right Side if it is clear
        Right();
        delay(600);
      }
      else {
        // Go back and turn right
        Backward();
        delay(600);
        Right();
        delay(600);
      }

    }

    delay(10);
  }
}
