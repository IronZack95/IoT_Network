
void ReadCO2(){

  if(sensor.checkDataReady() == true){
        CO2 = sensor.getCO2PPM();
        TVOC = sensor.getTVOCPPB();
        Serial.print("CO2: ");
        Serial.print(CO2);
        Serial.print("ppm, TVOC: ");
        Serial.print(TVOC);
        Serial.println("ppb");
        
    } else {
        Serial.println("Data is not ready!");
    }
    sensor.writeBaseLine(0x847B);
  return;

}
