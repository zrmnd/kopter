#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <stdint.h>
#include "string.h"





char *ssid = "PULT";
char *password = "pult7834";


WiFiUDP udpIPAutoconf;
unsigned int udpIPAutoconfPort = 4351;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  udpReplyPacekt[] = "$PUA00,PULT V1.0 in debug,\n";  // a reply string to send back

WiFiClient tabletTcpClient;

/* Soft AP network parameters */
IPAddress tabletIP(192, 168, 8, 31);   ////////////////39 PC, 34 - odroid
unsigned int tabletKeysHandlerPort = 4355;
IPAddress broadcastIP(192, 168, 8, 255);
IPAddress netMsk(255, 255, 255, 0);










uint8_t process1Sec() {
  static uint32_t nextTime = millis()+1000;
  if (millis() >= nextTime) {
    nextTime += 1000; 
    return 1;
  } 
  else {
    return 0;
  }
}


void checkAndRepairConnection() {
    if (!tabletTcpClient.connected()) {
        tabletTcpClient.stop();
        //Serial.print("Try connect...");
        tabletTcpClient.connect(tabletIP, tabletKeysHandlerPort);
        delay(100);   
    }
}


byte sendToKeysHandler(byte *data, uint16_t len) {
    checkAndRepairConnection();
    uint8_t res = 0;
    if (tabletTcpClient.connected()) {
        if (len == 0) 
            res = tabletTcpClient.write((char*)data);
        else
            res = tabletTcpClient.write((char*)data, len);
    }
    if (res == 0)
        tabletTcpClient.stop();
    else 
        tabletTcpClient.flush();
    return res;
}


void tryReceiveAndHandle() {
    checkAndRepairConnection();
    static byte data[1024];
    byte recv;
    static uint16_t nextFreeElementIndex = 0;
    while (tabletTcpClient.available()) {
        recv = tabletTcpClient.read();
        if (recv == '$') 
            nextFreeElementIndex = 0;
        else {
            data[nextFreeElementIndex++] = recv;
            if (recv == '\n') {
                Serial.write(data, nextFreeElementIndex);
                nextFreeElementIndex = 0;
            }
        }
        if (nextFreeElementIndex > sizeof(data)-1)
            nextFreeElementIndex = 0;
    }
}

void handleUdpIPAutoconf() {
    int packetSize = udpIPAutoconf.parsePacket();
    if (packetSize) {
        // receive incoming UDP packets
        // ///////////////////////////////// parse packet here and extract tabletIP 
        //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udpIPAutoconf.remoteIP().toString().c_str(), udpIPAutoconf.remotePort());
        int len = udpIPAutoconf.read(incomingPacket, 255);
        if (len > 0) {
            incomingPacket[len] = 0;
        }
        //Serial.printf("UDP packet contents: %s\n", incomingPacket);
    }

    if (process1Sec()) {
        udpIPAutoconf.beginPacket(broadcastIP, 4351);
        udpIPAutoconf.write(udpReplyPacekt);
        udpIPAutoconf.endPacket();
        //Serial.printf("UDP packet sended\n");
        //Serial.println("Try send...");       
    }
}


void handleSerialPort() {
    static byte data[2048];
    byte recv;
    static uint16_t nextFreeElementIndex = 0;
    while (Serial.available() > 0) {
        recv = Serial.read();
        if (recv == '$') 
            nextFreeElementIndex = 0;
        else {
            data[nextFreeElementIndex++] = recv;
            if (recv == '\n') {
                if (sendToKeysHandler(data, nextFreeElementIndex) != 0)
                    nextFreeElementIndex = 0;
            }
        }
        if (nextFreeElementIndex > sizeof(data)-1)
            nextFreeElementIndex = 0;
    }
}




  

//main setup
void setup() {  
    #warning 'Remove debug Serial.print, add parsing of incoming UDP packages'
    WiFi.mode(WIFI_STA);
    Serial.begin(9600);
    //Serial.println("Starting...");
    WiFi.begin(ssid, password);
   
    while (WiFi.status() != WL_CONNECTED) {  // Wait for connection
        delay(500);
        //Serial.print(".");
    }
    Serial.println("");
   // Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    udpIPAutoconf.begin(udpIPAutoconfPort);
    //Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), udpIPAutoconfPort);


    while (Serial.available() > 0) {
        Serial.write(Serial.read());
    }
    delay(2000);
    while (Serial.available() > 0) {
        Serial.write(Serial.read());
    }


    //sendToInterpreter(&client1, (byte*)cmd1, 0)
  
}




void loop() {

    tryReceiveAndHandle();

    handleUdpIPAutoconf();

    handleSerialPort();

}