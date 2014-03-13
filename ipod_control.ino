/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wills project code
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <SoftwareSerial.h>
SoftwareSerial ipodSerial(4,3); // Rx, Tx
#define DEBUGGING 1

const int ledPin =  2;      // the number of the LED pin
 
// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long previousMillis = 0;        // will store last time LED was updated
long interval = 1000;           // interval at which to blink (milliseconds)
 
const int analogInPin = A0;
int backgroundValue = 0; 
int foregroundValue = 0; 
const int threshold_hand = 80; // difference between foreground and background when hand is there.
const int threshold_nohand = 50; // difference between foreground and background when hand is there.



// These two are used to make sure we only send the command once when the hand is held 
// in front of the sensor
int sent_press = 0;
int sent_release = 0;
 
// these can be used with the send_bytes function to set the ipod into remote mode and to 
// tell it there are no buttons pressed.
const byte mode2[]={0xFF,0x55,0x03,0x00,0x01,0x02,0xFA};//mode 2 command
const byte nobutton[]={0xFF,0x55,0x03,0x02,0x00,0x00,0xFB};//button release command
void send_bytes(byte *bytes)
{
    for(int m=0; m<7; m++)
    {
                ipodSerial.write(bytes[m]);//sends the command
    }
}
 
void setup() {
  
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);      
  Serial.begin(19200);
  ipodSerial.begin(19200);
  
  send_bytes((byte *)mode2);
Serial.print("Starting");  
}
 
// some values which can be used with the send_command function to send commands
const byte playxpause = 0x01;
const byte nextsong = 0x08;
const byte prevsong = 0x10;
 
void send_command(int select)
{
  byte checksum=0x00-0x03-0x02-0x00-select;//checksum of lenght, mode and command
  byte inst[]={0xFF,0x55,0x03,0x02,0x00,select,checksum};//general structure
  send_bytes(inst);
}
 
 
void loop()
{
  // here is where you'd put code that needs to be running all the time.
 
  // check to see if it's time to blink the LED; that is, if the 
  // difference between the current time and last time you blinked 
  // the LED is bigger than the interval at which you want to 
  // blink the LED.
  unsigned long currentMillis = millis();
  static 
  float filtered = 0.0; 
  if(currentMillis - previousMillis > interval) {
    // save the last time you told the ipod to be controlled
    previousMillis = currentMillis;   
    // once per second send a request command -- you might need to remove the //s
    send_bytes((byte *)mode2);
    //sent_release = 1;
    //send_bytes((byte *)nobutton);
  }
  
  backgroundValue = 0;
  foregroundValue = 0;
  for (int i=0;i<64;i++)
  {
    digitalWrite(ledPin, HIGH);
    delay(2); 
    backgroundValue += analogRead(analogInPin);
    digitalWrite(ledPin, LOW);
    delay(2
    ); 
    foregroundValue += analogRead(analogInPin);
  }
  
  signed delta = foregroundValue-backgroundValue;
  if (filtered == 0.0)
  {
    filtered = delta;
  }
  else
  {
    const float alpha = 0.2f;
    filtered = alpha*filtered + (1.0f-alpha)*delta;
  }
  #ifdef DEBUGGING
  Serial.print("fg = " );                       
  Serial.print(foregroundValue);      
  Serial.print("\t bg = ");      
  Serial.print(backgroundValue);
  Serial.print("\t delta= ");
  Serial.print(delta);
  Serial.print(" filtered = ");
 Serial.print(filtered);

 #endif
  {
    if (!sent_press && (filtered > threshold_hand))
    {
      sent_press = 1;
      sent_release = 0;
      send_command(playxpause); // could put other values - look above where playxpause is
      #ifdef DEBUGGING
      Serial.print(" Hand");
      #endif
    }
  }
  {
    if (!sent_release && (filtered < threshold_nohand))

    {
      sent_press = 0;
      sent_release = 1;
      send_bytes((byte *)nobutton);
      #ifdef DEBUGGING
      Serial.print(" No Hand");
      #endif
    }
  }
  #ifdef DEBUGGING
  Serial.println();
  #endif
  
  delay(50);
}
  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The end!
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
