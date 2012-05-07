#include <Servo.h>
#include <SPI.h>
#include <Ethernet.h>

Servo myservo;

/*
Pins 11 and 3: controlled by timer 2

Setting 	Divisor 	Frequency
0x01 	 	1  		31250
0x02 	 	8 	 	3906.25
0x03  		32  		976.5625
0x04 	 	64 	 	488.28125
0x05 	 	128  		244.140625
0x06  		256  		122.0703125
0x07 	 	1024  		30.517578125
TCCR2B = TCCR2B & 0b11111000 | <Setting>;
*/

void setup() {
  TCCR2B = TCCR2B & 0b11111000 | 0x05;  //see above
  myservo.attach(11, 1000, 2000);       //pin 11, 1 millisecond minimum, 2 ms maximum
  myservo.write(90);                    //0 to 180 degrees, 90 is stopped
  Serial.begin(9600);                   //baud rate
  Serial.println("Setup complete.");
}

void loop() {
  if (Serial.available() > 0){
    int mybyte = Serial.read() - 48;    //takes ascii 0-9 and converts to int 0-9
    mybyte = mybyte * 20;               //maps 0-9 to 0-180
    if (mybyte > 180) mybyte = 180;
    else if (mybyte < 0) mybyte = 0;
    myservo.write(mybyte);              //writes received byte to servo
  }
}
