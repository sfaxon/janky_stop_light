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
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(10,0,1,187); // IP address of server
int server_port = 1718;
int server_retry_delay = 2000; // ms to wait between requests to server

int red = 22;
int yellow = 24;
int green = 26;
int blue = 28;

String currentLine = "";            // string to hold the text from server

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
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("IP address aquired.");
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, server_port)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET / HTTP/1.0");
    client.println();
  } 
  else {
    // this could retry, but for now we just show error codes
    // probably a wrong server IP address
    Serial.println("server connection failed, check server and sever_port.");
    error_server_connection();
  }
}

void loop()
{
  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
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
    }
    if ( currentLine.equals("GOOD") ) {
      Serial.println("found some GOOD code");
      clear_lights();
      digitalWrite(green, HIGH);
    }
    if ( currentLine.equals("BUILDING") ) {
      Serial.println("found some GOOD code");
      clear_lights();
      digitalWrite(yellow, HIGH);
    }
    
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    // do nothing forevermore:
    while(true);
  }
  
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
  while(true);
}

void error_server_connection()
{
  clear_lights();
  digitalWrite(blue, HIGH);
  digitalWrite(yellow, HIGH);
  while(true);
}
