/*
    DIY Arduino based RC Transmitter Project
   == Receiver Code - ESC and Servo Control ==

  by Dejan Nedelkovski, www.HowToMechatronics.com
  Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

RF24 radio(10, 9);  // nRF24L01 (CE, CSN)
const byte address[6] = "00001";

unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

Servo esc;  // create servo object to control the ESC
Servo servo1;
Servo servo2;
int escValue, servo1Value, servo2Value;

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  byte j1PotX;
  byte j1PotY;
  byte j1Button;
  byte j2PotX;
  byte j2PotY;
  byte j2Button;
  byte pot1;
  byte pot2;
  byte tSwitch1;
  byte tSwitch2;
  byte button1;
  byte button2;
  byte button3;
  byte button4;
};

Data_Package data;  //Create a variable with the above structure

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();  //  Set the module as receiver
  resetData();
  esc.attach(9);
  servo1.attach(3);
  servo2.attach(4);
}
void loop() {
  // Check whether we keep receving data, or we have a connection between the two modules
  currentTime = millis();
  if (currentTime - lastReceiveTime > 1000) {  // If current time is more then 1 second since we have recived the last data, that means we have lost connection
    resetData();                               // If connection is lost, reset the data. It prevents unwanted behavior, for example if a drone jas a throttle up, if we lose connection it can keep flying away if we dont reset the function
  }
  // Check whether there is data to be received
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package));  // Read the whole data and store it into the 'data' structure
    lastReceiveTime = millis();               // At this moment we have received the data
  }
  // Controlling servos
  servo1Value = map(data.j2PotX, 0, 255, 0, 180);
  servo2Value = map(data.j2PotY, 0, 255, 0, 180);
  servo1.write(servo1Value);
  servo2.write(servo2Value);
  // Controlling brushless motor with ESC
  escValue = map(data.pot1, 0, 255, 1000, 2000);  // Map the receiving value form 0 to 255 to 0 1000 to 2000, values used for controlling ESCs
  esc.writeMicroseconds(escValue);                // Send the PWM control singal to the ESC
}

void resetData() {
  // Reset the values when there is no radio connection - Set initial default values
  data.j1PotX = 127;
  data.j1PotY = 127;
  data.j2PotX = 127;
  data.j2PotY = 127;
  data.j1Button = 1;
  data.j2Button = 1;
  data.pot1 = 1;
  data.pot2 = 1;
  data.tSwitch1 = 1;
  data.tSwitch2 = 1;
  data.button1 = 1;
  data.button2 = 1;
  data.button3 = 1;
  data.button4 = 1;
}