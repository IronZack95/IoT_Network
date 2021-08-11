void MonitorStart(void) {
  display.clearDisplay();

  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.println(F(" ESP - 01"));
  display.println(F(" by Zack"));
  display.display();
  delay(2000);
}

void MonitorSleep(unsigned long t) {
    display.clearDisplay();
    display.setTextSize(2);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);     // Start at top-left corner
    display.println(F("DEEP SLEEP")); 
    display.print(t/60);display.println(F("min"));
    display.println(ssid);
    display.display();
  
}

void MonitorSetup(int i) {
  if(i == 0){
    display.clearDisplay();
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);     // Start at top-left corner
    display.println(F("Connecting to ")); 
    display.println(ssid);
    display.display();
  }
  if(i == 1){
    display.print(F("."));
    display.display();
  }
  if(i == 2){
    display.clearDisplay();
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);     // Start at top-left corner
    display.println(F("WiFi connected!"));
    display.println(F("IP address: ")); 
    display.println(WiFi.localIP());
    display.display();
  }
}

void MonitorReconnect(int i) {
    display.clearDisplay();
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);     // Start at top-left corner
    display.println(F("MQTT connection.. ")); 
    display.println(mqtt_server);
  if(i == 1){
    display.println(F("SUCCESS !!")); 
  }
  if(i == 2){
    display.println(F("Failed.. wait")); 
  }
  display.display();
}

void MonitorSensors(void) {
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.print(F("Temperature: "));display.println(AnalogTemp);
  display.print(F("CO2  : "));display.print(CO2); display.println(F("ppm"));
  display.print(F("TVOC : "));display.print(TVOC);display.println(F("ppb"));
  display.print(F("TMP: "));display.print(DigitalTemp);display.print(F(" HUM: "));display.println(DigitalHumidity);
  /*
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(3.141592);

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x")); display.println(0xDEADBEEF, HEX);
  */
  display.display();
  //delay(2000);
}

void sleepDisplay(Adafruit_SSD1306* display) {
  display->ssd1306_command(SSD1306_DISPLAYOFF);
}

void wakeDisplay(Adafruit_SSD1306* display) {
  display->ssd1306_command(SSD1306_DISPLAYON);
}
