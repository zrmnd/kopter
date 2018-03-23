#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <stdint.h>
#include "string.h"
#include "ctype.h"

#define MAX_CLIENT 3

extern "C" {
  #include "user_interface.h"
}





static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static inline uint8_t is_base64(uint8_t c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}
unsigned int base64_encode_(const uint8_t* bytes_to_encode, unsigned int in_len, uint8_t* encoded_buffer, unsigned int out_len_)
{
    unsigned int out_len = out_len_;
    int i = 0;
    int j = 0;
    uint8_t char_array_3[3] = { 0, 0, 0 };
    uint8_t char_array_4[4] = { 0, 0, 0, 0 };

    out_len = 0;
    while (in_len--)
    {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3)
        {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4 ; i++)
            {
                encoded_buffer[out_len++] = base64_chars[char_array_4[i]];
            }
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 3; j++)
        {
            char_array_3[j] = '\0';
        }

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; j < (i + 1); j++)
        {
            encoded_buffer[out_len++] = base64_chars[char_array_4[j]];
        }

        while (i++ < 3)
        {
            encoded_buffer[out_len++] = '=';
        }
    }

    return out_len;
}


uint8_t findch(char* str, char ch) {
  uint8_t i = 0;
  while (str[i] != ch && i < 255) {
    i++;
  }
  return i;
}

unsigned int base64_decode_(const uint8_t* encoded_string, unsigned int in_len, uint8_t* decoded_buffer, unsigned int out_len_)
{
    unsigned int out_len = out_len_;
    size_t i = 0;
    size_t j = 0;
    int in_ = 0;
    uint8_t char_array_3[3] = { 0, 0, 0 };
    uint8_t char_array_4[4] = { 0, 0, 0, 0 };

    out_len = 0;
    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
    {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i == 4)
        {
            for (i = 0; i < 4; i++)
            {
                char_array_4[i] = findch((char*)base64_chars, char_array_4[i]);// (base64_chars.find(char_array_4[i]));
            }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; i < 3; i++)
            {
                decoded_buffer[out_len++] = char_array_3[i];
            }
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 4; j++)
        {
            char_array_4[j] = 0;
        }

        for (j = 0; j < 4; j++)
        {
            char_array_4[j] = findch((char*)base64_chars, char_array_4[j]);//(uint8_t)(base64_chars.find(char_array_4[j]));
        }

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++)
        {
            decoded_buffer[out_len++] = char_array_3[j];
        }
    }
    return out_len;
}










char *ssid = "PULT";
char *password = "pult7834";


WiFiUDP Udp;
unsigned int localUdpPort = 4351;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacekt[] = "$PUA00,PULT V1.0 in debug,\n";  // a reply string to send back

WiFiClient client1;
WiFiServer server(80);
WiFiClient client[MAX_CLIENT];

/* Soft AP network parameters */
IPAddress copterIP(192, 168, 8, 34);   ////////////////39 PC, 34 - odroid
IPAddress broadcastIP(192, 168, 8, 255);
IPAddress netMsk(255, 255, 255, 0);
unsigned int copterInterpreterPort = 4355;


// есть таблица с количеством принимаемых и отправляемых каждым из контроллеров ввода данных
uint8_t pultIOcontrollersDataLength[][3] = {{0x01, 2, 0}, {0x02, 5, 2}, {0x03, 1, 0}, {0x04, 2, 2}, {0x05, 1, 1}, {0x06, 1, 1}, {0x07, 1, 1}, {0x08, 1, 1}, {0x09, 2, 1}, {0x0A, 4, 2}};

char cmdPult[10][5] ={ {'M', 0x07, 0x01, 0x0F, 0x00},
                    {'M', 0x07, 0x02, 0x0F, 0x00},
                    {'M', 0x07, 0x03, 0x0F, 0x00},
                    {'M', 0x07, 0x04, 0x0F, 0x00},
                    {'M', 0x07, 0x05, 0x0F, 0x00},
                    {'M', 0x07, 0x06, 0x0F, 0x00},
                    {'M', 0x07, 0x07, 0x0F, 0x00},
                    {'M', 0x07, 0x08, 0x0F, 0x00},
                    {'M', 0x07, 0x09, 0x0F, 0x00},
                    {'M', 0x07, 0x0A, 0x0F, 0x00}};
//M 0x07 0x01 0x0F 



char cmdUartS[] = "$INE01,77,3,115200,\n";
char cmdUartSendBegin[] = "$INE02,77,3,"; 
char cmdUartSendEnd[] = ",\n       $INE08,77,3,\n";
char cmdUartSendRxEnd[] = "$INE09,77,3,\n";







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





byte sendToInterpreter(WiFiClient *cl, byte *data, uint16_t len) {
  if (!cl->connected()) {
    cl->stop();
    Serial.print("Try connect...");
    cl->connect(copterIP, copterInterpreterPort);
    //delay(100);   
  }
  uint16_t sended = 0;
  uint8_t res = 0;
  if (cl->connected()) {
    if (len == 0) 
      res = cl->write((char*)data);
    else
      res = cl->write((char*)data, len);
  }
  if (res == 0)
    cl->stop();
  else 
    cl->flush();

  return res;

}


void handleReceiveFromInterpreter() {
  

  
}


/*uint16_t receiveFromInterpreter(byte *data, uint16_t max_size, uint16_t timeout) { //timeout in ms
  if (!cl->connected()) {
    Serial.print("Try connect...");
    cl->.connect(host, port);
    delay(100);   
  }
  uint16_t received = 0;
  while (cl->connected() && received < len) {
    data[received] = client.read(); 
    received++;    
  }
  if (sended == len)
    return 1;
  else
    return 0;  
  
}*/




  

//main setup
void setup() {  

//analogWriteFreq(1000);
//analogWriteRange(255);
  WiFi.mode(WIFI_STA);
  
  //initialize Serial
  Serial.begin(115200);
  delay(500);
  Serial.println("Starting...");

  WiFi.begin(ssid, password);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);


  Serial.println("Starting TCP server");
  // Start the TCP server
  server.begin();

  while (Serial.available() > 0) {
      Serial.write(Serial.read());
    }
  delay(2000);
  while (Serial.available() > 0) {
      Serial.write(Serial.read());
    }

  Serial.write(cmdUartS);

  
  //uint16_t len;
  //len = base64_decode((const uint8_t*)msg.data.str, strlen((const char*)msg.data.str),  tmp_buff, sizeof(tmp_buff));
  uint8_t arr[10][15], len[10];
  for (uint8_t i = 0; i < 10; i++) {
    len[i] = base64_encode_((const uint8_t*)cmdPult[i], strlen((const char*)cmdPult[i]),  arr[i], 15);
    arr[i][len[i]] = 0;
    //sendToInterpreter(&client1, (byte*)cmd1, 0)
    Serial.write(cmdUartSendBegin);
    Serial.write(arr[i], len[i]);
    Serial.write(cmdUartSendEnd);
    while (Serial.available() > 0) {
      Serial.write(Serial.read());
    }
    delay(100);
    
    while (Serial.available() > 0) {
      Serial.write(Serial.read());
    }
    delay(100);
    Serial.write(cmdUartSendRxEnd);
    delay(100);
    while (Serial.available() > 0) {
      Serial.write(Serial.read());
    }
  }
  

  

  
  //check connection init
 // checkconnection_timer = millis();
  
}




char cmd1[] = "$PUI02,66,1,AAAAAA!\n";

//main loop
void loop() {
  //check connection
 // if (millis() > checkconnection_timer) {
   // checkconnection_timer = millis() + CHECKCONNECTION_INTERVALL;
    
 // }

 static int index;
 byte buff[500];
  
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);
  }



  while (Serial.available() > 0) {
     //Serial.write(Serial.read());
     byte t = Serial.read();
     if (t == 0x24 || index >= 500) 
      index = 0;
     buff[index++] = t;
     
     if (t == 0x0A) {
        buff[index] = 0;
        if (sendToInterpreter(&client1, buff, index))
      Serial.println("OK");
    else 
      Serial.println("fail");
        
        Serial.println((char*)&buff[0]);
        index = 0;
     }
  }

  

  /*uint8_t i;
  if(server.hasClient())
  {
   for(i=0;i<MAX_CLIENT;i++)
   {
     if (!client[i] || !client[i].connected()){
      if(client[i]) client[i].stop();
        client[i] = server.available();
        continue;
     }
    } 
  }
  for(i = 0; i < MAX_CLIENT; i++){
    if (client[i] && client[i].connected()){
      if(client[i].available()){
        while(client[i].available()){
          Serial.write(client[i].read()); 
        }
        Serial.write("\n");
        client[i].write("Data received\n");
      }
    }
  }*/

  if (process1Sec()) {
    //Udp.beginPacket(broadcastIP, 4351);
    //Udp.write(replyPacekt);
    //Udp.endPacket();
    //Serial.printf("UDP packet sended\n");
    //Serial.println("Try send...");
    
  }
  
}
