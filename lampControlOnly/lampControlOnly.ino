/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */
 char val;
int lampPin = 4;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(lampPin, OUTPUT);
   Serial.begin(9600);
  
}

// the loop function runs over and over again forever
void loop() {
  
  if (Serial.available()) 
   { // If data is available to read,
     val = Serial.read(); // read it and store it in val
   }
   
   digitalWrite(lampPin, HIGH); // turn the LED on
   
   
   if (val == '1') 
   { // If 1 was received
     digitalWrite(lampPin, HIGH); // turn the LED on
     delay(500); 
   } else {
     digitalWrite(lampPin, LOW); // otherwise turn it off
     delay(500); 
   }
   
   
//   delay(10);
//  digitalWrite(4, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(1000);              // wait for a second
//  digitalWrite(4, LOW);    // turn the LED off by making the voltage LOW
//  delay(1000);              // wait for a second
}
