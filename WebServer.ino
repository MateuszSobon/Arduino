#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "PageIndex.h" //--> Include the contents of the User Interface Web page, stored in the same folder as the .ino file
    
const char* ssid = "Orange_Swiatlowod_7510_2"; // SSID and Password of your WiFi router
const char* password = "HDwDTPYos4xvJ2eevs";

ESP8266WebServer server(80);  //--> Server on port 80
char data[200] = "";


void handleRoot() // This routine is executed when you open NodeMCU ESP8266 IP Address in browser
{
 String s = MAIN_page; //--> Read HTML contents
 server.send(200, "text/html", s); //--> Send web page
}

void handledata() // Procedure for reading the temperature value of a DHT11 sensor
{
  if(Serial.available()>0)
  {
    memset(data, '\0', sizeof(data)); // clear array
    Serial.readBytesUntil(';', data, 200);
    Serial.println(data);
  }
 
  server.send(200, "text/plane", data); //--> Send Temperature value only to client ajax request
}

void setup(void)
{
  Serial.begin(9600);

  WiFi.begin(ssid, password); //--> Connect to your WiFi router
  
  Serial.print("Connecting");   // Wait for connection
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");                              //If connection successful show IP address in serial monitor
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/", handleRoot); //--> Routine to handle at root location. This is to display web page.
  server.on("/readData", handledata);  //--> Routine to handle the call procedure handleDHT11Temperature
  
  server.begin(); //--> Start server
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();  //Handle client requests
}
