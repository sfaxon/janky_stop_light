/*
  based on the Web Client by David A. Mellis

 This sketch connects to a host running (https://github.com/sfaxon/janky_wacher/)
 and outputs to some lights the appropirate status of builds.
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * LED's attached to 22, 24, 26, 28
 
*/

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x1D };
// only if DHCP fails do we assign a static IP address
IPAddress staticIp(10, 11, 18, 100);
IPAddress staticDns(10, 11, 18, 10);
IPAddress staticGateway(10, 11, 18, 1);
IPAddress staticSubnet(255, 255, 255, 0);

IPAddress server(66, 150, 174, 219); // IP address of server
int server_port = 80;

int red = 22;
int yellow = 24;
int green = 26;
int blue = 28;

String currentLine = "";            // string to hold the text from server
const unsigned long requestInterval = 10000;  // delay between requests
unsigned long lastAttemptTime = 0;            // last time you connected to the server, in milliseconds

// Initialize the Ethernet client library
// with the IP address and port of the server
EthernetClient client;

void setup() {
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  currentLine.reserve(256);
  boot_blink();
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("DHCP init...");
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, show error status on lights
    error_dhcp();
    Ethernet.begin(mac, staticIp, staticDns, staticGateway, staticSubnet);
    Serial.println("Set static IP address");
  }
  
  Serial.println(Ethernet.localIP());

  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("IP address aquired.");
  Serial.println("connecting...");

  makeRemoteRequest();
}

void loop()
{
  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.connected()) {
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
      currentLine += c;
      // clear currentLine when new line reached
      if (c == '\n') {
        currentLine = "";
      }
      
      //really inefficient, but functional
      if ( currentLine.equals("JANKY") ) {
        Serial.println("found some JANKY code");
        clear_lights();
        digitalWrite(red, HIGH);
        client.stop();
      }
      if ( currentLine.equals("GOOD") ) {
        Serial.println("found some GOOD code");
        clear_lights();
        digitalWrite(green, HIGH);
        client.stop();
      }
      if ( currentLine.equals("BUILDING") ) {
        Serial.println("found BUILDING code");
        clear_lights();
        digitalWrite(yellow, HIGH);
        client.stop();
      }
    }
  }
  else if (millis() - lastAttemptTime > requestInterval) {
    // if you're not connected, and two minutes have passed since
    // your last connection, then attempt to connect again:
    makeRemoteRequest();
  }
  
}

void makeRemoteRequest() {
  // attempt to connect, and wait a millisecond:
  Serial.println("connecting to server...");
  if (client.connect(server, server_port)) {
    Serial.println("making HTTP request...");
    // make HTTP GET request to twitter:
    client.println("GET / HTTP/1.1");
    client.println("HOST: buildlight.marshill.info");
    client.println();
  }
  else {
    // this could retry, but for now we just show error codes
    // probably a wrong server IP address
    Serial.println("server connection failed, check server and sever_port.");
    error_server_connection();
  }
  // note the time of this connect attempt:
  lastAttemptTime = millis();
}

// blink all the lights, just to be sure they work
void boot_blink()
{
  digitalWrite(green, HIGH);
  digitalWrite(yellow, HIGH);
  digitalWrite(red, HIGH);
  digitalWrite(blue, HIGH);
  delay(200); // ms
  clear_lights();
  delay(200);
  digitalWrite(green, HIGH);
  digitalWrite(yellow, HIGH);
  digitalWrite(red, HIGH);
  digitalWrite(blue, HIGH);
  delay(400);
  clear_lights();
}

void clear_lights()
{
  digitalWrite(green, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(red, LOW);
  digitalWrite(blue, LOW);
}

void address_aquired()
{
  clear_lights();
  digitalWrite(green, HIGH);
  delay(200);
  digitalWrite(green, LOW);
  digitalWrite(yellow, HIGH);
  delay(200);
  digitalWrite(yellow, LOW);
  digitalWrite(red, HIGH);
  delay(200);
  digitalWrite(red, LOW);
  digitalWrite(blue, HIGH);
  delay(200);
  digitalWrite(blue, LOW);
}

// never return, output blue, red for error code
void error_dhcp()
{
  clear_lights();
  digitalWrite(blue, HIGH);
  digitalWrite(red, HIGH);
  delay(400);
  clear_lights();
}

void error_server_connection()
{
  clear_lights();
  digitalWrite(blue, HIGH);
  digitalWrite(yellow, HIGH);
}
