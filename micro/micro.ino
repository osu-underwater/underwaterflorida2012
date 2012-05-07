//header files for TCP/IP
#include <SPI.h>
#include <Ethernet.h>

#define SERVO_PIN_1 11
#define SERVO_PIN_2 11

//network info
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,169,0,177);
IPAddress gateway(192,169,0,1);
IPAddress subnet(255,255,255,0);

EthernetServer server(23);
EthernetClient client = 0;

Servo servo1, servo2;

int pin, val;

/*
Pins 11 and 3: controlled by timer 2

Setting     Divisor     Frequency
0x01        1           31250
0x02        8           3906.25
0x03        32          976.5625
0x04        64          488.28125
0x05        128         244.140625
0x06        256         122.0703125
0x07        1024        30.517578125
TCCR2B = TCCR2B & 0b11111000 | <Setting>;
*/

void setup(){
    TCCR2B = TCCR2B & 0b11111000 | 0x05;  //see above
    Serial.begin(9600);
    Ethernet.begin(mac, ip, gateway, subnet);
    server.begin();
    client = server.available();
    servo1.attach(SERVO_PIN_1, SERVO_MIN, SERVO_MAX);
    servo1.write(90);                    //0 to 180 degrees, 90 is stopped
    servo2.attach(SERVO_PIN_2, SERVO_MIN, SERVO_MAX);
    servo2.write(90);
    Serial.println("Setup complete.");
}

void loop(){
    test_comm();
}

void test_comm(){
    uint8_t mybyte;
    EthernetClient client = server.available();
    if (client.connected()){
        if(client.available()){
            mybyte = client.read();
            if (mybyte == 'a'){
                pin = client.read();
                val = client.read();
                analogWrite(pin, val);

            } else if (mybyte == 'd'){
                pin = client.read();
                val = client.read();
                if (val == 'H'){
                    digitalWrite(pin, HIGH);
                } else if (val == 'L'){
                    digitalWrite(pin, LOW);
                }

            } else if (mybyte == 's'){
                pin = client.read();
                val = client.read();
                if (pin == SERVO_PIN_1){
                    servo1.write(val);
                } else if (pin == SERVO_PIN_2){
                    servo2.write(val);
                }

            } else {
              Serial.print("Unknown opcode: ");
              Serial.println(char(mybyte));
            }
        }
    }
}
