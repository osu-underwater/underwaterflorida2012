//header files for TCP/IP
#include <SPI.h>
#include <Ethernet.h>

//network info
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,0,177);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
//const byte gateway[]  = {192,168,0,1};
//const byte subnet[]   = {255,255,255,0};

EthernetServer server(23);
EthernetClient client = 0;

void setup(){
    Serial.begin(9600);
    Ethernet.begin(mac,ip,gateway, subnet);
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
    /*
    if (client){
              Serial.println("1");
        while (client.connected()){
              Serial.println("Fuckin finally");
           if (client.available()){
              Serial.println("Fuckin finally");
           } 
        }
    }*/
    if (client.connected()){
        if(client.available()){
            mybyte = client.read();
            Serial.print(mybyte);
        }
    }
}
