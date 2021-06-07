/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>

SimpleDHT11 dht11;

// which analog pin to connect
#define THERMISTORPIN A0         
// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000      
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25   
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 10000    
// dht11 pin
#define DHT11PIN 16

int samples[NUMSAMPLES];

#define DELAY_MSG 2000

// Wifi definitions and MQTT topic
const char* ssid = "Vodafone-25757949";
const char* password = "2pwjsfvj4c3emek";
const char* mqtt_server = "192.168.1.100";
#define port 1883

// TOPIC
#define ledTopic "esp01/led"
#define systemTopic "esp01/sys"
#define tempATopic "esp01/tempA"
#define tempDTopic "esp01/tempD"
#define humDTopic "esp01/humD"


WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

// VARIABILI
float AnalogTemp = 0;

unsigned long now = 0;

int DigitalTemp = 0;
int DigitalHumidity = 0;
int old_DigitalTemp = 0;
int old_DigitalHumidity = 0;

// Prototipi delle funzioni
float Temperature();
void ReadDHT();
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

// SETUP
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
}

// MAIN LOOP
void loop() {
  
  //verifico di essere connesso
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
    // calcolo la temperatura Analogica
      AnalogTemp = Temperature();
      snprintf (msg, MSG_BUFFER_SIZE, "%1f °C", AnalogTemp);
      Serial.print("Publish Analog TMP message: ");
      Serial.println(msg);
      client.publish(tempATopic, msg);

    // Read DHT11
    ReadDHT();

      snprintf (msg, MSG_BUFFER_SIZE, "%1d", DigitalTemp);
      Serial.print("Publish DHT11 TMP message: ");
      Serial.println(msg);
      client.publish(tempDTopic, msg);
      
      snprintf (msg, MSG_BUFFER_SIZE, "%1d", DigitalHumidity);
      Serial.print("Publish DHT11 HUM message: ");
      Serial.println(msg);
      client.publish(humDTopic, msg);

      //now = millis();
      
    Serial.println("----------------------------------------------"); //print to serial monitor
    delay(5000);
}

void ReadDHT(){
    // read with raw sample data.
    byte temperature = 0;
    byte humidity = 0;
    byte data[40] = {0};

    // calcolo DHT11
    if (dht11.read(DHT11PIN, &temperature, &humidity, data)) {
      Serial.print("Read DHT11 failed");
      return;
    }

    DigitalTemp = (int)temperature;
    DigitalHumidity = (int)humidity;

    return;
  
  }

float Temperature(){
  uint8_t i;
  float average;
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
  
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;

  Serial.print("Average analog reading "); 
  Serial.println(average);
  
  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
  Serial.print("Thermistor resistance "); 
  Serial.println(average);
  
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert absolute temp to C
  
  Serial.print("Temperature "); 
  Serial.print(steinhart);
  Serial.println(" °C");

  return steinhart;
  }

void setup_wifi(){

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(systemTopic, "start");
      // ... and resubscribe
      client.subscribe(ledTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
