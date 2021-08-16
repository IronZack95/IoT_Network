void setup_wifi(){
  
  delay(10);
  // We start by connecting to a WiFi network
  digitalWrite(LED1, LOW);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  // Display
  MonitorSetup(0);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    MonitorSetup(1);
    WIFIAttempt++; 
    if(WIFIAttempt >= MAX_WIFI_ATTEMPT){
          Sleep(WifiSleep);  // sleep 20 secondi
      }
  }

  
  randomSeed(micros());

  Serial.println("");
  Serial.print("WiFi connected after "); Serial.print(WIFIAttempt); Serial.println(" attempts");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  WIFIAttempt = 0; 
  // Display
  MonitorSetup(2);
  delay(2000);
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
    digitalWrite(LED2, HIGH);
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    digitalWrite(LED2, LOW);
  }

}

void reconnect() {
  // Loop until we're reconnected
  digitalWrite(LED1, LOW);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    MonitorReconnect(0);
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected after "); Serial.print(MQTTAttempt); Serial.println(" attempts");
      MQTTAttempt = 0;
      MonitorReconnect(1);
      // Once connected, publish an announcement...
      client.publish(systemTopic, "start");
      // ... and resubscribe
      client.subscribe(ledTopic);
    } else {
      Serial.print("failed, rc=");
      MonitorReconnect(2);
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
  MonitorSleep(t);
  delay(5000); // 5 sec
  sensor.setMeasCycle(sensor.eClosed);
  sleepDisplay(&display);
  ESP.deepSleep(t*1e6);
  return;
  }
