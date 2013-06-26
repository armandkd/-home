#define DEBUG 1                                                                         //DEBUG ?
#include <SPI.h>                                                                        //SPI Library
#include <Ethernet.h>                                                                   //Ethernet Library
#include <SD.h>                                                                         //SD Library
#include <stdlib.h>                                                                     //String Library

int i;                                                                                  //Iteration variable
char c;                                                                                 //Request reader
char f[2];                                                                              //Header function reader
int func;                                                                               //Function number
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };                                    //MAC Adress
IPAddress ip( 192, 168, 1, 50 );                                                        //IP Adress
byte gateway[] = { 192 ,168 ,1 ,1 };                                                    //Gateway Adress
byte subnet[] = { 255 ,255 ,255 ,0 };                                                   //Subnet Mask
EthernetServer server(80);                                                              //Server variable
EthernetClient client;                                                                  //Client variable
String readstring = "";                                                                 //Header reader string
String rfid = "";                                                                       //RF id
String filename = "";                                                                   //File name

void setup(){
  Ethernet.begin(mac, ip, gateway, subnet);                                             //Begin Ethernet
  if(!SD.begin(4) && DEBUG) {                                                           //Begin SD
    Serial.println("Can't start SD");
  }
  if(DEBUG) {
    Serial.begin(9600);
    Serial.println("Setup finished");
    Serial.println(Ethernet.localIP());
  }
}

void loop()
{
  client = server.available();                                                          //Set a new client
  if (client) {                                                                         //If a client is detected
    if(DEBUG){Serial.println("Client detected");}
    while (client.connected()) {                                                        //While the client is connected
      if(DEBUG){Serial.println("Client connected");}
      if(client.available()) {                                                          //If the client is available
        if(DEBUG){Serial.println("Client available");}
        while(c != '\n') {                                                              //If the client haven't already sent a complete line
          c = client.read();                                                            //Read the new character
          readstring = readstring + c;                                                  //Add the character to the current request
        }
        if(DEBUG){Serial.println(readstring);}
        if(readstring.startsWith("GET /")) {                                            //Only accept GET requests
          if(DEBUG){Serial.println("GET request recognized");}
          f[0] = readstring.charAt(5);                                                  //Retrieve the first character in the request
          if(f[0] == '?') {                                                             //If a function is asked
            if(DEBUG){Serial.println("Function asked");}
            f[1] = readstring.charAt(8);                                                //Retrieve function id 0:OFF 1:ON
            for(i = 13;i<25;i++) {                                                      //Retrieve RFid
              rfid = rfid + readstring.charAt(i);
            }
            if(DEBUG){Serial.print(f[1]);Serial.print(":");Serial.println(rfid);}
            //////////////////////////////////////////////////////////////////////////////CHECK FUNCTION STATE ???
            sendRF(rfid);                                                               //Send RF signal
          } else if(f[0] == ' ') {                                                      //If just / asked
            if(DEBUG){Serial.println("Root asked");}
            sendFile("index.html");
          } else {                                                                      //For everything else asked
            if(DEBUG){Serial.print("Other asked, assuming it's a file");}
            i = 5;
            while(readstring.charAt(i) != ' ') {                                        //Retrieve filename
              filename = filename + readstring.charAt(i);
              i++;
            }
            if(DEBUG){Serial.print("Filename :");Serial.println(filename);}
            char charfilename[filename.length()];                                       //Convert filename's string to char (Any better method ???)
            filename.toCharArray(charfilename, filename.length());
            sendFile(charfilename);                                                     //Send file over network
          }
        }
        c = ' ';                                                                        //Resetting variables
        readstring = "";
        rfid = "";
        filename = "";
      }
    }
  }
}

boolean sendRF(String rfid) {
  ////////////////////////////////////////////////////////////////////////////////////////Send RFid
  if(DEBUG){Serial.print("Sending RFid : ");Serial.println(rfid);}
}

boolean sendFile(char* filename) {
  if(DEBUG){Serial.print("Sending file : ");Serial.println(rfid);
  if(SD.open(filename)) {                                                               //Check if file exists on SD
    if(DEBUG){Serial.println("File exists");}
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    //////////////////////////////////////////////////////////////////////////////////////SEND FILE
  } else {
    if(DEBUG){Serial.println("File doesn't exist");}
    client.println("HTTP/1.1 404 Not found");        
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("0");
  }
}
