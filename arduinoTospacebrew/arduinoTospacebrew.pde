import spacebrew.*;
import processing.serial.*;

String server = "sandbox.spacebrew.cc";
String name = " Arduino Simulated Camera Controller " + floor(random(1000));
String description = "Reading Accelerometer data and button trigger";

int rotX = 0;   // the value of xyz
int rotY = 0;
int rotZ = 0;
int balanceZ = 0;
int magRotation = 0;
boolean shutter = false;
boolean bottleTriggered = false;

Spacebrew spacebrewConnection;  // Spacebrew connection object
Serial myPort;          // Serial port object 

void setup() {
  size(400, 200);

  // instantiate the spacebrew object
  spacebrewConnection = new Spacebrew( this );

  // add each thing you publish to
  spacebrewConnection.addPublish( "rotX", rotX );
  spacebrewConnection.addPublish( "rotZ", rotZ );
  spacebrewConnection.addPublish( "rotY", rotY );  
  spacebrewConnection.addPublish( "balance", balanceZ );  
  spacebrewConnection.addPublish( "magRotation", magRotation );  
  spacebrewConnection.addPublish( "shutterTrigger", shutter ); 


  // add subscriber
  spacebrewConnection.addSubscribe("bottleTrigger", "boolean"); 

  // connect to spacebrew 
  spacebrewConnection.connect(server, name, description );




  // print list of serial devices to console
  println(Serial.list());
  myPort = new Serial(this, Serial.list()[7], 9600); // CONFIRM the port that your arduino is connect to
  myPort.bufferUntil('\n');
}

void draw() {
  // set backgroun color based on brightness


  if (shutter == false) {
    background( rotX / 2, rotY / 2, rotZ / 2 );
  }

  if (shutter == true) {
    background( 255 );
  }

  if (bottleTriggered == true) 
  {                           //if we clicked in the window
    myPort.write('1');         //send a 1
    println("1");
  } else 
  {                           //otherwise
    //  myPort.write('0');          //send a 0
  } 



  // if background is light then make text black
  if (rotX < 512) { 
    fill(225, 225, 225);
  }
  // otherwise make text white
  else { 
    fill(25, 25, 25);
  }

  // set text alignment and font size
  textAlign(LEFT);
  textSize(16);

  if (spacebrewConnection.connected()) {
    // print client name to screen
    text("Connected as: " + name, 20, 25 );  

    // print current bright value to screen
    textSize(20);
    text("rotationX"+ rotX, 10, 60);  
    text("rotationY"+ rotZ, 10, 80);  
    text("rotationZ"+ rotY, 10, 100); 
    text("balanceZ" + balanceZ, 10, 120);
    text("magRotation" + magRotation, 10, 140);
    text("shutterTrigger"+ shutter, 10, 160);
  } else {
    text("Not Connected to Spacebrew", width/2, 25 );
  }
}

void onBooleanMessage( String name, boolean value ) {
  println("got bool message " + name + " : " + value); 

  // update background color
  if (value == true) {
    bottleTriggered = true;
  } else {
    // bottleTriggered = false;
  }
}







void serialEvent (Serial myPort) {



  //  // read data as an ASCII string:
  String inString = myPort.readStringUntil('\n');
  // 
  if (inString != null) {
    //    
    // read a byte from the serial port:
    // String inString = myPort.readStringUntil(',');
    // split the string into multiple strings
    // where there is a ":"
    inString = trim(inString);
    String items[] = split(inString, ',');

    // remove any whitespace from the label
    //String label = trim(items[0]);
    // remove the ',' off the end
    // String val = split(items[1], ',')[0];
    // check what the label was and update the appropriate variable
    //    if (label.equals("rotX")) {
    //      rotX = int(val);
    //    }
    //    if (label.equals("rotY")) {
    //      rotY = int(val);
    //    }
    //    
    //     if (label.equals("rotZ")) {
    //      rotZ = int(val);
    //    }

    rotX =  int (items[0]); 
    rotY =  int (items[1]); 
    rotZ =  int (items[2]); 
    balanceZ  =  int (items[3]); 
   

    int items4 = int (items[4]); 
    if (items4 == 0) {
      shutter = false;
    }
    if (items4 == 1) {
      shutter = true;
    }
    
     magRotation = int (items[5]);
  }


  //    // trim off whitespace
  //    inString = trim(inString);
  // 
  //    // convert value from string to an integer
  //    rotX = int(inString); 

  // publish the value to spacebrew if app is connected to spacebrew
  if (spacebrewConnection.connected()) {
    spacebrewConnection.send( "rotX", rotX );
    spacebrewConnection.send( "rotY", rotZ );
    spacebrewConnection.send( "rotZ", rotY );
    spacebrewConnection.send( "balance", balanceZ  );
    spacebrewConnection.send( "magRotation", magRotation ); 
    spacebrewConnection.send( "shutterTrigger", shutter );
  }
}


