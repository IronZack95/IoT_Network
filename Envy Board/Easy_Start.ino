//AZ-Envy
//Easy-Start
//© Niklas Heinzel
//2020
//Ver.2.1

//With this code you will be able to test the basic functions of the AZ-Envy and its programming.//

//Libaries//
#include <WEMOS_SHT3X.h> //Libary for your SHT30 humidity and temperature sensor

//Global declaration
SHT3X sht30(0x44); //adress of SHT30
const int analogInPin = A0;  //ADC-pin of AZ-Envy for the gas sensor
const int integrated_led = 2; //integrated led is connected to D2

void setup() {

  Serial.begin(115200); //starting the serial communication with a baud rate of 115200 bps
  Serial.println("------------------------------"); //print to serial monitor
  Serial.println("-----------AZ-Envy------------"); //print to serial monitor
  Serial.println("------by Niklas Heinzel-------"); //print to serial monitor
  Serial.println("------------------------------"); //print to serial monitor

  pinMode(analogInPin,INPUT); //set ADC-pin as a input
  pinMode(integrated_led,OUTPUT); //set the integrated led as a output

}

void loop() {

  //variables to work with
  float temperature = sht30.cTemp; //read the temperature from SHT30
  float humid = sht30.humidity; //read the humidity from SHT30
  int sensorValue = analogRead(analogInPin); //read the ADC-pin → connected to MQ-2

  //calibrate your temperature values - due to heat reasons from the MQ-2 (normally up to 4°C)
  float temperature_deviation = 0.0; //enter the deviation from the mq2 due to the resulting heat in order to calibrate the temperature value 
  float temperature_calibrated = temperature - temperature_deviation; //final value
  
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
  Serial.println("----------------------------------------------"); //print to serial monitor
  Serial.print("MQ2-value:"); //print to serial monitor
  Serial.println(sensorValue); //print data to serial monitor
  Serial.println("----------------------------------------------"); //print to serial monitor

 
  digitalWrite(integrated_led,HIGH);//turn the integrated led on
  
  delay(5000);

  digitalWrite(integrated_led,LOW);//turn the integrated led off
}
