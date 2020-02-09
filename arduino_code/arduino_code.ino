#include <ESP8266WiFi.h>
#include <SPI.h>
#include <bitBangedSPI.h>
#include <MAX7219_Dot_Matrix.h>

const byte chips = 4; //No of MAX7219 Dpt Matrix Display
unsigned long lastMoved = 0;
unsigned long MOVE_INTERVAL = 100;  // mS
int  messageOffset;

MAX7219_Dot_Matrix display (chips, 2);  // Chips / LOAD 
char message [30]; //String to be displayed

const char* ssid = "home_wg"; // SSID Of the Router
const char* password = "omisoksbwn";// Access point Password
const char* host = "192.168.0.7"; //Server IP or URL
int requestTime=90000;
int exitT=1;

void updateDisplay ()
{
  display.sendSmooth (message, messageOffset); //Display commands for the Matrix
  // next time show one pixel onwards
  if (messageOffset++ >= (int) (strlen (message) * 8))
    messageOffset = - chips * 8;
}  // end of updateDisplay

void setup() {
  display.begin (); //Initialize teh displays
  Serial.begin(115200); 
  delay(100);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); // Initiate connection to the Wi-Fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); //Printout the IP assigned to the module in DHCP
  display.setIntensity(15); // Intensity of the Dot Matrix module can be 0-15
}

void loop() {
  if(millis()-requestTime>120000){ // Request data from the Srever after every 2 Minutes
    requestTime=millis();
    Serial.print("connecting to ");
    Serial.println(host);
    
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) { //Connect to SERVER
      Serial.println("connection failed");
    }
    
    String url = "/API/subscriber.php"; //Path of the webpage in the server to request
    Serial.print("Requesting URL: ");
    Serial.println(url);
    client.print(String("GET ") + url + " HTTP/1.0\r\n" +
        "Host: " + host + "\r\n" + 
        "Connection: close\r\n\r\n");

    while (client.available() == 0) { // check Response from server
    if (millis() - requestTime >30000) { // 30 seconds to wait for response before timeout
      Serial.println(">>> Client Timeout !");
      client.stop();
      exitT=0;
      break;
    }
    }
    String line="";
    while(client.available()){ // Read response from Server
      line += client.readStringUntil('\r');
    }
    line=line.substring(line.indexOf('[')+1,line.indexOf(']')); // Process the response
    Serial.println(line);
    int i;
    for (i=0;i<line.length();i++) // Put response  to char array to display
      message[i]=line[i]; 
    if(exitT==1){ 
      exitT=1;
      Serial.println("closing connection");
      client.stop();
    }
  }
   //display.sendString (message);
  if (millis () - lastMoved >= MOVE_INTERVAL){
    updateDisplay ();
    lastMoved = millis ();
  }
}
