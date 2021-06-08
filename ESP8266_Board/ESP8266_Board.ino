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

// Wifi library
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Sensors library
#include <SimpleDHT.h>
#include <CCS811.h>

// OLED library
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Define
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define NUMFLAKES     10 // Number of snowflakes in the animation example
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

// I2C ADDRESS
#define OLEDAddress 0x3C
#define CO2Address  0x5A

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
#define CO2Topic "esp01/CO2"
#define TVOCTopic "esp01/TVOC"

// Objects
WiFiClient espClient;
PubSubClient client(espClient);
SimpleDHT11 dht11;
//CCS811 sensor(&Wire, /*IIC_ADDRESS=*/0x5A);
CCS811 sensor;

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
int CO2 = 0;
int TVOC = 0;

// Prototipi delle funzioni
float Temperature();
void ReadDHT();
void ReadCO2();
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

// ------------------ SETUP ------------------
void setup() {
  // Inizializzo Seriale
  Serial.begin(115200);
  Serial.println("------------------------------"); //print to serial monitor
  Serial.println("----------Iron ESP01----------"); //print to serial monitor
  Serial.println("----------by Z A C K----------"); //print to serial monitor
  Serial.println("------------------------------"); //print to serial monitor
  
  // Inizializzo Pin
  pinMode(BUILTIN_LED, OUTPUT);
  
  // Inizializzo Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();

  //Inizializzo CO2
  while(sensor.begin() != 0){
      Serial.println("failed to init chip, please check if the chip connection is fine");
      delay(1000);
  }
    /**
   * @brief Set measurement cycle
   * @param cycle:in typedef enum{
   *                  eClosed,      //Idle (Measurements are disabled in this mode)
   *                  eCycle_1s,    //Constant power mode, IAQ measurement every second
   *                  eCycle_10s,   //Pulse heating mode IAQ measurement every 10 seconds
   *                  eCycle_60s,   //Low power pulse heating mode IAQ measurement every 60 seconds
   *                  eCycle_250ms  //Constant power mode, sensor measurement every 250ms
   *                  }eCycle_t;
   */
  sensor.setMeasCycle(sensor.eCycle_250ms);
    
  // Inizializzo WiFi
  setup_wifi();
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
}




// ------------------ MAIN LOOP ------------------
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

    // Read CCS811
    ReadCO2();

    // Read DHT11
    ReadDHT();

    snprintf (msg, MSG_BUFFER_SIZE, "%1d", CO2);
    Serial.print("Publish CO2 message: ");
    Serial.println(msg);
    client.publish(CO2Topic, msg);
        
    snprintf (msg, MSG_BUFFER_SIZE, "%1d", TVOC);
    Serial.print("Publish CO2 message: ");
    Serial.println(msg);
    client.publish(TVOCTopic, msg);
    
    snprintf (msg, MSG_BUFFER_SIZE, "%1d", DigitalTemp);
    Serial.print("Publish DHT11 TMP message: ");
    Serial.println(msg);
    client.publish(tempDTopic, msg);
    
    snprintf (msg, MSG_BUFFER_SIZE, "%1d", DigitalHumidity);
    Serial.print("Publish DHT11 HUM message: ");
    Serial.println(msg);
    client.publish(humDTopic, msg);

    
    
    //now = millis();
    Monitor();

    Serial.println("----------------------------------------------"); //print to serial monitor
    delay(5000);

}
