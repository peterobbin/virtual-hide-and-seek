#include <Servo.h> 
Servo myservo;

char val; // Data received from the serial port
int ledPin = 4; // Set the pin to digital I/O 13
int tvPin = 12;
int pos = 0;



void setup() {
  pinMode(ledPin, OUTPUT); // Set pin as OUTPUT
  pinMode(tvPin, OUTPUT); // Set pin as OUTPUT
  myservo.attach(9); 
  myservo.write(0);
  Serial.begin(9600); // Start serial communication at 9600 bps
}


void loop() {
  if (Serial.available()) 
  { // If data is available to read,
    val = Serial.read(); // read it and store it in val
  }
  if (val == '2') 
  { // If 1 was received
    digitalWrite(ledPin, HIGH); // turn the LED on
   // myservo.write(180);
  } 
  else if(val == '3') {
    digitalWrite(ledPin, LOW); // otherwise turn it off
  }
  //delay(10); // Wait 10 milliseconds for next reading


  if (val == '4') 
  { // If 1 was received
    digitalWrite(tvPin, LOW); // turn the LED on
  } 
  else if (val == '5'){
    digitalWrite(tvPin, HIGH); // otherwise turn it off
  }


  if  (val == '1'){

    myservo.write(180);
//    for(pos = 0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
//    {                                  // in steps of 1 degree 
//      myservo.write(pos);              // tell servo to go to position in variable 'pos' 
//      delay(15);                       // waits 15ms for the servo to reach the position 
//    } 
  }
  else if (val =='0'){
    myservo.write(0);

  }




  // delay(10); // Wait 10 milliseconds for next reading
}




