//Arduino 1.0+ only

#include <Wire.h>
#include <SFE_MMA8452Q.h> // Includes the SFE_MMA8452Q library
#include <HMC5883L.h>



#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24

int L3G4200D_Address = 105; //I2C address of the L3G4200D
HMC5883L compass;
MMA8452Q accel(0x1C); 

float x;
float y;
float z;

float radZ;
boolean lastpressed = false;
boolean pressed;
int trigger;

float accX;
float accY;
float accZ;

float accX2;
float accY2;
float accZ2;
float prevAccX;
float prevAccY;
float prevAccZ;
float x_val;
float y_val;
float z_val;
float acc_angle_x;
float acc_angle_y;
float acc_angle_z;

float filterAngle;
  float dt=0.01;



int state = 0;


const int buttonPin = 4;     // the number of the shutterButton pin
int buttonState = 0;

void setup(){

  Wire.begin();
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
 accel.init();


//  Serial.println("starting up L3G4200D");
  setupL3G4200D(500); // Configure L3G4200  - 250, 500 or 2000 deg/sec
  
    // Set measurement range
  compass.setRange(HMC5883L_RANGE_1_3GA);

  // Set measurement mode
  compass.setMeasurementMode(HMC5883L_CONTINOUS);

  // Set data rate
  compass.setDataRate(HMC5883L_DATARATE_30HZ);

  // Set number of samples averaged
  compass.setSamples(HMC5883L_SAMPLES_8);

  // Set calibration offset. See HMC5883L_calibration.ino
  compass.setOffset(0, 0);

  delay(1500); //wait for the sensor to be ready 
    prevAccX = accel.cx;
  prevAccY = accel.cy;
  prevAccZ = accel.cz;
}

void loop(){
   getGyroValues();  // This will update x, y, and z with new values
   buttonState = digitalRead(buttonPin);
   
   
   if (accel.available())
  {
    // First, use accel.read() to read the new variables:
    accel.read();
    
    // accel.read() will update two sets of variables. 
    // * int's x, y, and z will store the signed 12-bit values 
    //   read out of the accelerometer.
    // * floats cx, cy, and cz will store the calculated 
    //   acceleration from those 12-bit values. These variables 
    //   are in units of g's.
    // Check the two function declarations below for an example
    // of how to use these variables.
    //printCalculatedAccels();
    //printAccels(); // Uncomment to print digital readings
    printCalculatedAccels();
    
   
    
   // Serial.print(previousAngle);
    
    // The library also supports the portrait/landscape detection
    //  of the MMA8452Q. Check out this function declaration for
    //  an example of how to use that.
     //    printOrientation();
    
   
  } 
  
  
   Vector norm = compass.readNormalize();

  // Calculate heading
  float heading = atan2(norm.YAxis, norm.XAxis);

  // Set declination angle on your location and fix heading
  // You can find your declination on: http://magnetic-declination.com/
  // (+) Positive or (-) for negative
  // For Bytom / Poland declination angle is 4'26E (positive)
  // Formula: (deg + (min / 60.0)) / (180 / M_PI);
  float declinationAngle = (4.0 + (26.0 / 60.0)) / (180 / M_PI);
  heading += declinationAngle;

  // Correct for heading < 0deg and heading > 360deg
  if (heading < 0)
  {
    heading += 2 * PI;
  }

  if (heading > 2 * PI)
  {
    heading -= 2 * PI;
  }

  // Convert to degrees
  float headingDegrees = heading * 180/M_PI; 
//
// float calx = compFilter(acc_angle_x, x);
//    float caly = compFilter(acc_angle_y, y);
//    float calz = compFilter(acc_angle_z, z);


//-------------printing out filtered angles
  float calx = cFilter(acc_angle_x, x, accX)/14 * 1000 ;
//  Serial.print(calx);
//  Serial.print(",");
  float caly = cFilter(acc_angle_y, y, accY)/14 * 1000 ;
//  Serial.print(caly);
//  Serial.print(",");
  float calz = cFilter(acc_angle_z, z, accZ)/14 * 1000;
//  Serial.println(calz);

//     Serial.print(calx); // Print new line every time.
//    Serial.print(caly);
//    Serial.println(calz);
   
   
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
   
   radZ = radZ + y/17.5/4;
  

  Serial.print(calx);
  Serial.print(",");
  Serial.print(caly);
  Serial.print(",");
  Serial.print(radZ);
  Serial.print(",");
  Serial.print(calz);
  Serial.print(",");
  Serial.print(trigger);
  Serial.print(",");
  Serial.print(headingDegrees);
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

int compFilter(float _newAngle, float _newRate){
  float filterTerm0;
  float filterTerm1;
  float filterTerm2;
  float timeConstant = 0.5;
  
  
  
  filterTerm0 = (_newAngle - filterAngle) * timeConstant * timeConstant;
  filterTerm2 += filterTerm0 * dt;
  filterTerm1 = filterTerm2 + ((_newAngle - filterAngle) * 2 * timeConstant) + _newRate;
  filterAngle = (filterTerm1 * dt) + filterAngle;
  
   return filterAngle;


//  float angle = 0.98*(_newAngle + _newRate*d)
 
}


float cFilter(float _newAngle, float _newRate, float _acc){
   float pitch;
    //pitch += (_newRate/17.50)*dt;
    pitch += (_newRate/17.50)*dt;

   
   float angle = 0.98*(_newAngle + pitch) + 0.02*(_acc);
   
  
   return angle;


//  float angle = 0.98*(_newAngle + _newRate*d)
 
}

void printCalculatedAccels()
{ 
  accX = accel.x;
  accY = accel.y;
  accZ = accel.z;
  
  
 
  
  accX2 = accX*accX;
  accY2 = accY*accY;
  accZ2 = accZ*accZ;
  
  
  float result;
  
  
 
  
//  result = atan2(accY, accZ) * 180/PI;
//  acc_angle_x = result;
  
  //x axis
  result = sqrt(accY2+accZ2);
  result = accX/result;
  acc_angle_x = atan(result);
 
    //y axis
  result = sqrt(accX2+accZ2);
  result = accY/result;
  acc_angle_y = atan(result);

  
    //z axis
  result = sqrt(accX2+accY2);
  result = accZ/result;
  acc_angle_z = atan(result);
 
  
  
//  Serial.print(acc_angle_x);
//  Serial.print("\t");
//  Serial.print(acc_angle_y);
//  Serial.print("\t");
//  Serial.print(acc_angle_z);
//  Serial.println("\t");
  
  
  
  
  
  
//  Serial.print(accX - prevAccX, 3);
//  Serial.print("\t");
//  Serial.print(accY - prevAccY, 3);
//  Serial.print("\t");
//  Serial.print(accZ - prevAccZ, 3);
//  Serial.print("\t");
  
  prevAccX = accX;
  prevAccY = accY;
  prevAccZ = accZ;
  
  
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
