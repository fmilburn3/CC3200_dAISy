/*
  Receives data from dAISy and forwards it to MarineTraffic using the TI CC3200 LaunchPad
  and Energia.
    
  Created by Frank Milburn, December 2015
  Tested with Energia v17 and dAISy BoosterPack on MSP-EXP430F5529 LaunchPad
  
  Notes:
  1) You must enable the auxiliary serial output from the dAISy debug menu for this sketch
     to receive data.  Enter "2" to connect at 38400 baud.  Then save the settings.
  2) Enter you own wifi and MarineTraffic account data in accounts.h
  
  Connections:
  
  dAISy                   CC3200
  --------------          --------------
  P4.4 (TX)               Pin 10 RX(1)               
  GND                     GND    
*/

#include <WiFi.h>
#include "accounts.h"

char ssid[]     = WIFI_SSID;                  
char password[] = WIFI_PASSWORD;     

char serverIP[] = SERVER_IP;              
int  serverPort = SERVER_PORT;                  

String inputData = "";                         // holds incoming data

WiFiClient client;

void setup()
{
  Serial1.begin(38400);                        // dAISy transmits to CC3200 at 38400 baud
                                               // on Serial1  
  Serial.begin(115200);                        // Output from CC3200 to serial monitor is
                                               // at 115200 baud on Serial
  pinMode(RED_LED, OUTPUT);                    // Use red LED to indicate connection status
  digitalWrite(RED_LED, LOW);
  
  Serial.print("Attempt network connect: ");
  Serial.println(ssid);                        // Print network name (SSID)
  WiFi.begin(ssid, password);                  // Connect to network
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print(".");                         // print dots while waiting to connect
    delay(300);
  }
  
  Serial.println("\nConnected");
  Serial.println(" Waiting for IP address");
  
  while (WiFi.localIP() == INADDR_NONE) {
    Serial.print(".");                          // print dots while waiting for ip addresss
    delay(300);
  }

  Serial.println("\nIP Address obtained");
  printWifiStatus();

  Serial.println("\nAttempt MarineTraffic server connection");     

  if (client.connect(serverIP, serverPort)) {
    Serial.print("Connected to IP: ");
    Serial.print(serverIP);
    Serial.print(", port ");
    Serial.println(serverPort);
    Serial.println("");
  }
}

void loop() {

  while (!client.connected()) {                // disconnected from MarineTraffic
    client.flush();                            
    client.stop();
    delay(5000);
    Serial.println("Not connected to MarineTraffic - attempt to connect");
    client.connect(serverIP, serverPort);
    digitalWrite(RED_LED, HIGH);
    delay(5000);
    digitalWrite(RED_LED, LOW);
  }
  
  while (Serial1.available()) {                 // look for data from dAISy

    char inChar = (char)Serial1.read();

    inputData += inChar;

    if (inChar == '\n') {
      Serial.println(String(inputData));
      client.print(String(inputData));         // send the string to MarineTraffic
      inputData = "";                          // clear the string:
    }
  }
}

void printWifiStatus() {
  Serial.print("SSID: ");                      // print the SSID of the network you're attached to:
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();               // print your WiFi shield's IP address:
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();                     // print the received signal strength:
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
