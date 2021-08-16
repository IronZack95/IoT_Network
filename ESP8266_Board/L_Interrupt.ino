//This program get executed when interrupt is occures i.e.change of input state

void ICACHE_RAM_ATTR ISR() { 
  if(DisplayStatus == false){
    Serial.println("ACCENDO DISPLAY");
    previousMillis = millis();
    wakeDisplay(&display);
    DisplayStatus = true;
  }
    return;
}
