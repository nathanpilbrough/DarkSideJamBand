//Title: Robotics Lab 2: Jam Band
//Description: This code contains the functions for creating the tune of the 
//             music. It controls two servo motors and 3 buzzers. The buzzers are 
//             connected to one output channel
//Team: Tophat - Nathan Pilbrough, Pedro Sierra, Quixin Li
#include <ESP8266WiFi.h>
     WiFiServer server(80);
     
void setup() {
 //initial web server setup 
         WiFi.mode(WIFI_AP); //Our ESP8266-12E is an AccessPoint
         WiFi.softAP("DARTH_VADER", "12345678"); // Provide the (SSID, password); .
         server.begin(); // Start the HTTP Server:

         //Looking under the hood
         Serial.begin(9600);
         IPAddress HTTPS_ServerIP= WiFi.softAPIP(); // Obtain the IP of the Server
         Serial.print("Server IP is: "); // Print the IP to the monitor window
         Serial.println(HTTPS_ServerIP);
         pinMode(D0, OUTPUT); //GPIO16 is an OUTPUT pin;
         digitalWrite(D0, LOW); //Initial state is ON
         
}

void loop() {
// Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
   
  // Wait until the client sends some data
  while(!client.available()){
    delay(1);
  }
   
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  //Serial.println(request);
  client.flush();
   
  // Match the request

 //if the play button is pressed then send a P
  int value = LOW;
  if (request.indexOf("/ON") != -1) {
    Serial.write("P");
    digitalWrite(D0, LOW);
    value = LOW;
  } 
  //if the stop button is pressed then send an S
  if (request.indexOf("/OFF") != -1){
    digitalWrite(D0, HIGH);
    Serial.write("S");
    value = HIGH;
  }
 
// Set ledPin according to the request
//digitalWrite(ledPin, value);
   
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  //client.println("<br><input type=\"button\" name=\"b1\" value=\"LED On\"");
   
  
  //client.println("onclick=\"location.href=’/ON’\">");
  client.println("<br><br><br>");
  client.println("<a href=\"/ON\"><input type=\"button\" name=\"b1\" value=\"PLAY\"</a><br>");
  //client.println("<br><input type=\"button\" name=\"b1\" value=\"LED Off\"");
  client.println("<a href=\"/OFF\"><input type=\"button\" name=\"b1\" value=\"STOP\"</a><br>");
  client.println("</html>");
 
  delay(1);
  //Serial.println("Client disonnected");
  //Serial.println("");        
}
