//This program get executed when interrupt is occures i.e.change of input state

void ICACHE_RAM_ATTR ISR() { 
    Serial.println("Interrupt Detected"); 
    return;
}
