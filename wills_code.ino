/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wills project code
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int ledPin =  2;      // the number of the LED pin
 
// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long previousMillis = 0;        // will store last time LED was updated
long interval = 1000;           // interval at which to blink (milliseconds)
 
const int analogInPin = A0;
int backgroundValue = 0; 
int foregroundValue = 0; 
 
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
                Serial.write(bytes[m]);//sends the command
    }
}
 
void setup() {
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);      
  Serial.begin(19200);
  send_bytes((byte *)mode2);
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
 
  if(currentMillis - previousMillis > interval) {
    // save the last time you told the ipod to be controlled
    previousMillis = currentMillis;   
    // once per second send a request command -- you might need to remove the //s
    // send_bytes(mode2);
  }
  
  backgroundValue = 0;
  foregroundValue = 0;
  for (int i=0;i<4;i++)
  {
    digitalWrite(ledPin, HIGH);
    delay(10); 
    backgroundValue += analogRead(analogInPin);
    digitalWrite(ledPin, LOW);
    delay(10); 
    foregroundValue += analogRead(analogInPin);
  }
  Serial.print("fg = " );                       
  Serial.print(foregroundValue);      
  Serial.print("\t bg = ");      
  Serial.print(backgroundValue);
 
  if (foregroundValue > backgroundValue+4)
  {
    if (!sent_press)
    {
      sent_press = 1;
      sent_release = 0;
      send_command(playxpause); // could put other values - look above where playxpause is
  Serial.print(" Hand");
    }
  }
  else
  {
    if (!sent_release)
    {
      sent_press = 0;
      sent_release = 1;
      send_bytes((byte *)nobutton);
  Serial.print(" No Hand");
  }}
  Serial.println();
  
  delay(50);
}
  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The end!
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
