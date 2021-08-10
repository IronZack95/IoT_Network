const int interruptPin = 15; //Button
const int LED=2; //On board blue LED 

//This program get executed when interrupt is occures i.e.change of input state
void ICACHE_RAM_ATTR ISR() { 
    Serial.println("Interrupt Detected"); 
}

void setup() { 
  Serial.begin(115200); 
  pinMode(LED,OUTPUT); 
  pinMode(interruptPin, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(interruptPin), ISR, CHANGE); 
} 

void loop() 
{ 
    digitalWrite(LED,HIGH); //LED off 
    delay(1000); 
    digitalWrite(LED,LOW); //LED on 
    delay(1000); 
} 
