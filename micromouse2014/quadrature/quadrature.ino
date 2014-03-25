#include <Encoder.h>
#include <cmath.h>
const int RADIUS = 1.75 /*in cm (if we use mm then its 175*/;
const int CIRCUM = (PI*RADIUS*RADIUS);
const int DIAMETER = 2*RADIUS;
const int SEC = 60;
const int x = 2;
const int N = 1000; /*number of pulses generated by the encoder per shaft revolution*/ // not sure what value to try 
long positionLeft  = -999;
long positionRight = -999;
const int FTI = 60; //in seconds 
String datastream = "";
boolean stringComplete = false;

Encoder MotorLeft(2,3);
Encoder MotorRight(4,5);

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
datastream.reserve(200);
int velocity();
int acceleration();
int positionm();
void UART();
void serialEvent();
}



void loop() {
  // put your main code here, to run repeatedly:
long newLeft, newRight;
  newLeft = MotorLeft.read();
  newRight = MotorRight.read();
  if (newLeft != positionLeft || newRight != positionRight) {
    Serial.print("Left = ");
    Serial.print(newLeft);
    Serial.print(", Right = ");
    Serial.print(newRight);
    Serial.println();
    positionLeft = newLeft;
    positionRight = newRight;
  }
  // if a character is sent from the serial monitor,
  // reset both back to zero.
  if (Serial.available()) {
    Serial.read();
    Serial.println("Reset both Motors to zero");
    MotorLeft.write(0);
    MotorRight.write(0);
}
UART(); //may need a delay somewhere in the loop
}

int velocity()
{
  /* velocity in terms of RPM */
  /* velocity is taken over the span of a minute (60 sec) this can be altered if needed */
  int encoder_pulses; // number can be found by figuring out how many pulses occur during a fixed time interval
  int PPV;
  int velocity;
  velocity = SEC*((encoder_pulses)/PPV)/FTI;
  
  return velocity;
}

int acceleration()
{
  int encoder_pulses0 = MotorLeft.read();
  int encoder_pulses1 = MotorLeft.read();
 int accel =  (encoder_pulses1 - encoder_pulses0/(PPV*PPV))/((FTI)*(FTI));
 
  return accel;
}
//change

int positionm()
{
  /*rotational position*/
  int Edge_Count = MotorLeft.read(); /*this is starting position */
  int rotations = (360*(Edge_Count))/(N*x);
  
  
  return rotations;
}

void UART()
{
  if(stringComplete) {
  int p = positionm();
  int a = acceleration();  
  int v = velocity();
  Serial.write(p,a,v,MotorLeft.read(),MotorRight.read());  
  }
}

 void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}


