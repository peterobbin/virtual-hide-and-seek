//Arduino 1.0+ only

#include <Wire.h>

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24

int L3G4200D_Address = 105; //I2C address of the L3G4200D

int x;
int y;
int z;
boolean lastpressed = false;
boolean pressed;
int trigger;

int state = 0;


const int buttonPin = 2;     // the number of the shutterButton pin
int buttonState = 0;

void setup(){

  Wire.begin();
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);


//  Serial.println("starting up L3G4200D");
  setupL3G4200D(2000); // Configure L3G4200  - 250, 500 or 2000 deg/sec

  delay(1500); //wait for the sensor to be ready 
}

void loop(){
   getGyroValues();  // This will update x, y, and z with new values
   buttonState = digitalRead(buttonPin);
   
   

   
   
   if (buttonState == 0){
     pressed = false;  
   }else if (buttonState == 1){
      pressed = true;
   }
   
   
    if(lastpressed == true && pressed == true){
      trigger = 0;

   }
   
   if(lastpressed == false && pressed == true){
      trigger = 1;
     
   }
   
     lastpressed = pressed;
   
   

   
   
//
//  Serial.print("rotX:");
  Serial.print(x);
  Serial.print(",");
//  Serial.print("rotY:");
  Serial.print(y);
  Serial.print(",");
 // Serial.print("rotZ:");
  Serial.print(z);
  Serial.print(",");
  Serial.print(trigger);
  Serial.println(",");

//  Serial.print(" Y:");
//  Serial.print(y);

//  Serial.print(" Z:");
//  Serial.println(z);

  delay(10); //Just here to slow down the serial to make it more readable
}

void getGyroValues(){

  byte xMSB = readRegister(L3G4200D_Address, 0x29);
  byte xLSB = readRegister(L3G4200D_Address, 0x28);
  x = ((xMSB << 8) | xLSB);
  x = map(x, 0, 1024, 0, 50);

  byte yMSB = readRegister(L3G4200D_Address, 0x2B);
  byte yLSB = readRegister(L3G4200D_Address, 0x2A);
  z = ((yMSB << 8) | yLSB);
  z = map(z, 0, 1024, 0, 50);

  byte zMSB = readRegister(L3G4200D_Address, 0x2D);
  byte zLSB = readRegister(L3G4200D_Address, 0x2C);
  y = ((zMSB << 8) | zLSB);
  y = - map(y, 0, 1024, 0, 30);
}

int setupL3G4200D(int scale){
  //From  Jim Lindblom of Sparkfun's code

  // Enable x, y, z and turn off power down:
  writeRegister(L3G4200D_Address, CTRL_REG1, 0b00001111);

  // If you'd like to adjust/use the HPF, you can edit the line below to configure CTRL_REG2:
  writeRegister(L3G4200D_Address, CTRL_REG2, 0b00000000);

  // Configure CTRL_REG3 to generate data ready interrupt on INT2
  // No interrupts used on INT1, if you'd like to configure INT1
  // or INT2 otherwise, consult the datasheet:
  writeRegister(L3G4200D_Address, CTRL_REG3, 0b00001000);

  // CTRL_REG4 controls the full-scale range, among other things:

  if(scale == 250){
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00000000);
  }else if(scale == 500){
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00010000);
  }else{
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00110000);
  }

  // CTRL_REG5 controls high-pass filtering of outputs, use it
  // if you'd like:
  writeRegister(L3G4200D_Address, CTRL_REG5, 0b00000000);
}

void writeRegister(int deviceAddress, byte address, byte val) {
    Wire.beginTransmission(deviceAddress); // start transmission to device 
    Wire.write(address);       // send register address
    Wire.write(val);         // send value to write
    Wire.endTransmission();     // end transmission
}

int readRegister(int deviceAddress, byte address){

    int v;
    Wire.beginTransmission(deviceAddress);
    Wire.write(address); // register to read
    Wire.endTransmission();

    Wire.requestFrom(deviceAddress, 1); // read a byte

    while(!Wire.available()) {
        // waiting
    }

    v = Wire.read();
    return v;
}
