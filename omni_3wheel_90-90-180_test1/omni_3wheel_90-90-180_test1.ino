/*
 * This code is for prototyping a 3-wheel holonomic drive
 * where each of the wheels are directly driven by a DC motor
 * and have varrying angular spacing between them
 * 
 * 3-wheel holonomic drive with 90-90-180 degree spacing
 */

#include <PS2X_lib.h> //PS2 Controller interface library

PS2X ps2x; //create ps2x object of PS2X Class

int error;
byte vibrate = 0;

//define logic pins for each motor
//motors are numbered from the front of the robot counter-clockwise
int enable1 = 11;
int pow1 = 12;
int dir1 = 13;

int enable2 = 8;
int pow2 = 9;
int dir2 = 10;

int enable3 = 5;
int pow3 = 6;
int dir3 = 7;


void setup() {
    Serial.begin(250000);

    //check controller status
    error = ps2x.config_gamepad(50, 51, 52, 53, true, true); //setup pins and settings:  
    //GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error

    if (error == 0) {
      Serial.println("Found Controller, configured successful");
      Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
      Serial.println("holding L1 or R1 will print out the analog stick values.");
      Serial.println("Go to www.billporter.info for updates and to report bugs.");
    }

    else if (error == 1)
      Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");

    else if (error == 2)
      Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

    else if (error == 3)
      Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

    Serial.print(ps2x.Analog(1), HEX);
  
  int type = ps2x.readType();
  switch (type) {
    case 0:
      Serial.println("Unknown Controller type");
      break;
    case 1:
      Serial.println("DualShock Controller Found");
      break;
    case 2:
      Serial.println("GuitarHero Controller Found");
      break;
  }


  //setup motor driver logic pins as outputs
  pinMode(enable1, OUTPUT);
  pinMode(pow1, OUTPUT);
  pinMode(dir1, OUTPUT);
  
  pinMode(enable2, OUTPUT);
  pinMode(pow2, OUTPUT);
  pinMode(dir2, OUTPUT);

  pinMode(enable3, OUTPUT);
  pinMode(pow3, OUTPUT);
  pinMode(dir3, OUTPUT);

}

void loop() {
  //Serial.print("running");  //for debugging
  
  //skip loop if no controller found
  if (error != 0)
  {
    Serial.println("\t\t no controller");
    //disable driers
    digitalWrite(enable1, LOW);
    digitalWrite(enable2, LOW);
    digitalWrite(enable3, LOW);
    return;
  }
  
  //enable drivers
  digitalWrite(enable1, HIGH);
  digitalWrite(enable2, HIGH);
  digitalWrite(enable3, HIGH); 

  //read controller
  ps2x.read_gamepad(false, vibrate);

  //get joystick readings, 0-255 centered on 128
  int LX = ps2x.Analog(PSS_LX);   //0=left   255=right
  int LY = ps2x.Analog(PSS_LY);   //0=forward   255=backward
  int RX = ps2x.Analog(PSS_RX);   //0=left   255=right
  int RY = ps2x.Analog(PSS_RY);   //0=forward   255=backward

  //define factors that influence each motor's power
  int straight = 0;
  int side = 0;
  int turn = 0;

  //for debugging
  //display joystick readings
  Serial.print(LX);
  Serial.print("\t");
  Serial.print(LY);
  Serial.print("\t");
  Serial.print(RX);
  Serial.print("\t");
  Serial.print(RY);
  Serial.print("\t");

  //Determine straight factor
  if(LY <= 130 && LY >= 125)  //joystick is centered verticaly
  {
      //do not change straight factor
  }
  else if(LY < 125)   //joystick is forward
  {
    straight = (128 - LY) * 100/127;   //straight factor scaled to 100 +
  }
  else if(LY > 130)   //joystick is backward
  {
    straight = -(LY - 128) *  100/127;  //straight factor scaled to 100 -
  }

  //Determine side factor
  if(LX <= 130 && LX >= 125)    //joystick is centered horizontally
  {
      //do not change side factor
  }
  else if(LX < 125)   //joystick is left
  {
    side = -(127 - LX) * 100/125;   //side factor scaled to 100 -
  }
  else if(LX > 130)   //joystick is right
  {
    side = (LX - 127) *  100/127;   //side factor scaled to 100 +
  }

  //Determine turning factor
  if(RX <= 130 && RX >= 125)    //joystick is centered horizontally
  {
      //do not change turn factor
  }
  else if(RX < 125)   //joystick is left
  {
    turn = (127 - RX) *  100/127;    //turn factor scaled to 100 +
  }
  else if(RX > 130)   //joystick is right
  {
    turn = -(RX - 127) *  100/127;    //side factor scaled to 100 -
  }

  //for debugging
  //display factors
  Serial.print(straight);
  Serial.print("\t");
  Serial.print(side);
  Serial.print("\t");
  Serial.print(turn);
  Serial.print("\n");

  //set each motor's direction and speed
  //speed1
  int speed1 = -straight - (0.1*side) + turn;  //signs for terms in this equation will vary for each motor
  //determine direction
  if(speed1 >= 0)
  {
    digitalWrite(dir1, HIGH);
  }
  else
  {
    digitalWrite(dir1, LOW);
  }
  
  //determine power by taking absolute value and scalling speed to use in arduino analogWrite()
  speed1 = speed1 * 2.55;
  if(speed1 < 0)
  {
    speed1 = -speed1;
  }
  if(speed1 > 255)
  {
    analogWrite(pow1, 255);
  }
  else
  {
    analogWrite(pow1, speed1);
  }
  
  //speed2
  int speed2 = side + turn;    //signs for terms in this equation will vary for each motor
  //determine direction
  if(speed2 >= 0)
  {
    digitalWrite(dir2, HIGH);
  }
  else
  {
    digitalWrite(dir2, LOW);
  }

  //determine power by taking absolute value and scalling speed to use in arduino analogWrite()
  speed2 = speed2 * 2.55;
  if(speed2 < 0)
  {
    speed2 = -speed2;
  }
  if(speed2 > 255)
  {
    analogWrite(pow2, 255);
  }
  else
  {
    analogWrite(pow2, speed2);
  }
  
  //speed3
  int speed3 = straight - (0.1*side) + turn;   //signs for terms in this equation will vary for each motor
  //determine direction
  if(speed3 >= 0)
  {
    digitalWrite(dir3, HIGH);
  }
  else
  {
    digitalWrite(dir3, LOW);
  }

  //determine power by taking absolute value and scalling speed to use in arduino analogWrite()
  speed3 = speed3 * 2.55;
  if(speed3 < 0)
  {
    speed3 = -speed3;
  }
  if(speed3 > 255)
  {
    analogWrite(pow3, 255);
  }
  else
  {
    analogWrite(pow3, speed3);
  }

}
