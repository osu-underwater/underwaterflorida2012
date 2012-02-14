//header files for TCP/IP
#include <SPI.h>
#include <Ethernet.h>

//network info
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,169,0,177);
IPAddress gateway(192,169,0,1);
IPAddress subnet(255,255,255,0);

EthernetServer server(23);
EthernetClient client = 0;

int pin, val;

void setup(){
    Serial.begin(9600);
    Ethernet.begin(mac, ip, gateway, subnet);
    server.begin();
    client = server.available();
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
            } else {
              Serial.print("Unknown opcode: ");
              Serial.println(char(mybyte));
            }
        }
    }
}
