/*The purpose of this sketch is to receive and decode 433mhz data sent by Acurite 3-in-1 Weather sensor. 
 * A 433mhz reciever and an Arduino compatible microprocessor are required. 
*/
#include <AcuriteDecoder.h>;
AcuriteDecoder AD;

void setup() {
  Serial.begin(9600);
  delay(1);  
  while (!Serial);
  delay(1);
  Serial.println();
  Serial.println(__DATE__);  
  Serial.println(__TIME__);
  Serial.println(__FILE__);
  Serial.println();
  Serial.flush();
}

void loop() {
  AD.getWeather(10,3619);  //dataInputPin, Arduino Uno, pin 10
  Serial.println(AD.weather.temperature);
  Serial.println(AD.weather.wind);
  Serial.println(AD.weather.humidity);      
  delay(1);
}
