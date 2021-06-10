//Libaries//
#include <WEMOS_SHT3X.h> //Libary for your SHT30 humidity and temperature sensor
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//Global declaration
SHT3X sht30(0x44); //adress of SHT30
const int analogInPin = A0;  //ADC-pin of AZ-Envy for the gas sensor

// Wifi utilityes and MQTT topic
const char* ssid = "Vodafone-25757949";
const char* password = "2pwjsfvj4c3emek";
const char* mqtt_server = "192.168.1.100";
#define port 1883

// TOPIC
#define humTopic "envy/hum"
#define tempTopic "envy/temp"
#define airTopic "envy/air"
#define systemTopic "envy/sys"
#define ledTopic "envy/led"

// WIFI
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];

// Reconnections
#define MAX_WIFI_ATTEMPT 30
#define MAX_MQTT_ATTEMPT 5
#define WifiSleep 20
#define MqttSleep 60
unsigned int WIFIAttempt = 0;
unsigned int MQTTAttempt = 0;

// Prototipi delle funzioni
void ReadSensors();
void SerialComunication();
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

// VARIABILI
float temperature = 0;
float humid = 0;
int sensorValue = 0;
float old_temperature = 0;
float old_humid = 0;
int old_sensorValue = 0;
float temperature_deviation = 4.0; //enter the deviation from the mq2 due to the resulting heat in order to calibrate the temperature value 
float temperature_calibrated = 0; //final value

// SETUP
void setup(){

  Serial.begin(115200); //starting the serial communication with a baud rate of 115200 bps
  Serial.println("------------------------------"); //print to serial monitor
  Serial.println("-----------Iron Envy----------"); //print to serial monitor
  Serial.println("----------by Z A C K----------"); //print to serial monitor
  Serial.println("------------------------------"); //print to serial monitor

  // Analog pin
  pinMode(analogInPin,INPUT);       //set ADC-pin as a input
  
  //Inizializzo Led
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH); // spengo led
  setup_wifi();
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
  ReadSensors();
}

// MAIN LOOP
void loop() {

  //verifico di essere connesso
  if(WiFi.status() != WL_CONNECTED){
      setup_wifi();
    }else{
      // verifico la connessione all'MQTT Brocker
      if (!client.connected()) {
        reconnect();
      }
      client.loop();
    }
    
  // Leggo i sensori
  ReadSensors();

  // Scrivo nella porta seriale
  SerialComunication();

  // MQTT Pubblish
  if(temperature != old_temperature){
    old_temperature = temperature;
    snprintf (msg, MSG_BUFFER_SIZE, "%1f °C", temperature_calibrated);
    Serial.print("Publish TMP message: ");
    Serial.println(msg);
    client.publish(tempTopic, msg);
  }
  
  if(humid != old_humid){
    old_humid = humid;
    snprintf (msg, MSG_BUFFER_SIZE, "%1f", humid);
    Serial.print("Publish HUM message: ");
    Serial.println(msg);
    client.publish(humTopic, msg);
  }
  
  if(sensorValue != old_sensorValue){
    old_sensorValue = sensorValue;
    snprintf (msg, MSG_BUFFER_SIZE, "%1d", sensorValue);
    Serial.print("Publish AIR message: ");
    Serial.println(msg);
    client.publish(airTopic, msg);
  }

  Serial.println("----------------------------------------------"); //print to serial monitor
 
  //digitalWrite(BUILTIN_LED,HIGH);//turn the integrated led on
  
  delay(5000);

  //digitalWrite(BUILTIN_LED,LOW);//turn the integrated led off
}


void ReadSensors(){
  //variables to work with
  temperature = sht30.cTemp; //read the temperature from SHT30
  humid = sht30.humidity; //read the humidity from SHT30
  sensorValue = analogRead(analogInPin); //read the ADC-pin → connected to MQ-2
  //calibrate your temperature values - due to heat reasons from the MQ-2 (normally up to 4°C)
  temperature_calibrated = temperature - temperature_deviation; //final value
  return;
  }

void SerialComunication(){
  
  //-SHT30-//
  if(sht30.get()==0){
    Serial.print("Temperature in Celsius: ");
    Serial.println(temperature_calibrated);
    Serial.print("Relative Humidity: ");
    Serial.println(humid);
  }
  else //if useless values are measured
  {
    Serial.println("Error, please check hardware or code!");
  }
  //-MQ-2-//
  Serial.print("MQ2-value:"); //print to serial monitor
  Serial.println(sensorValue); //print data to serial monitor
  return;
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
    WIFIAttempt++; 
    if(WIFIAttempt >= MAX_WIFI_ATTEMPT){
          Sleep(WifiSleep);  // sleep 20 secondi
      }
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  WIFIAttempt = 0; 
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
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
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
      Serial.println("connected after "); Serial.print(MQTTAttempt); Serial.println(" attempts");
      MQTTAttempt = 0;
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
      MQTTAttempt++; 
      if(MQTTAttempt >= MAX_MQTT_ATTEMPT){
            Sleep(MqttSleep);  // sleep 1 minuti
        }
    }
  }
}

void Sleep(unsigned long t){
  Serial.print("DEEP SLEEP MODE for "); Serial.print(t); Serial.println("s");
  delay(5000); // 5 sec

  // SEGNALE VISIVO SLEEP
  digitalWrite(BUILTIN_LED, HIGH); // SPENGO led
  delay(50);
  digitalWrite(BUILTIN_LED, LOW); // ACCENDO led
  delay(50);
  digitalWrite(BUILTIN_LED, HIGH); // SPENGO led
  delay(50);
  digitalWrite(BUILTIN_LED, LOW); // ACCENDO led
  delay(50);
  digitalWrite(BUILTIN_LED, HIGH); // SPENGO led
  ESP.deepSleep(t*1e6);
  return;
  }
