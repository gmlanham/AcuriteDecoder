//The purpose of this sketch is to display wind speed and water temperature on an LED Matrix Display.
#include <Adafruit_GFX.h> //low footprint display library
#include <Max72xxPanel.h> //LED matrix display
#include <OneWire.h> //used with temperature sensor
#include <DS18B20.h> //waterproof digital temperature sensor
#include <LowPower.h> //used to powerdown to save battery
#include <AcuriteDecoder.h> //decodes the 433mhz signal

AcuriteDecoder AD;
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DS18B20 sensor(&oneWire); //waterproof temperature probe

//Low footprint display library
int pinCS = 10; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 1;
char* hi = (char*) malloc(8);
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
String tape = "Good";
int wait = 1000; // In milliseconds
int spacer = 1;
int width = 5 + spacer; // The font width is 5 pixels

//battery  protection
const int pResistor = A2; // Photoresistor at Arduino analog pin
const int ledPin =  LED_BUILTIN;// the number of the LED pin
int value=0;          // Store value from photoresistor (0-1023)
const int tooDark = 750;  //Solar panel battery charger, photoresistor (0-1023) cutoff, when too dark solar charging

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  delay(1);  
  while (!Serial);
  delay(10);
  Serial.println();
  Serial.println(__DATE__);  
  Serial.println(__TIME__);
  Serial.println(__FILE__);
 
  value=0;
  
  matrix.setIntensity(1); // Use a value between 0 and 15 for brightness
  matrix.setRotation(0, 1);    // The first display is position upside down
  matrix.setRotation(1, 1);    // The second display is position upside down
  matrix.setRotation(2, 1);    // The third hold for the last display
  matrix.setRotation(3, 1);    // The same hold for the last display
  matrix.print(tape);
  matrix.write(); // Send bitmap to display
  delay(2000);
    
  sensor.begin();   //waterproof temp sensor DS18B20
  delay(1500);   
  pinMode(ledPin, OUTPUT);   
  pinMode(pResistor, INPUT); 
  
  //433mhz setup
  //pinMode(pin, INPUT);
  Serial.flush(); //wait until all serial print is done
}

void displayData(String dData) {
  int str_len = dData.length() + 1;
  char char_array[str_len];
  dData.toCharArray(char_array, str_len);
  hi=char_array;
  tape=hi;
  wait = 100; // In milliseconds
  
  for ( int i = 0 ; i < width * tape.length() + matrix.width() - 1 - spacer; i++ ) {
    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2; // center the text vertically

    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < tape.length() ) {
        matrix.drawChar(x, y, tape[letter], HIGH, LOW, 1);
      }
      letter--;
      x -= width;
    }
    matrix.write(); // Send bitmap to display
    delay(wait);
    matrix.fillScreen(LOW); //clear display
  }
}

void photoresistor() {
  value = analogRead(pResistor);
  while (value > tooDark){ //sleep when too dark
   // Serial.println("sleeping..."); //may cause hang in loop
    delay(100); //give time to print sleeping...             
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); //8 seconds is longest possoble sleep call      
    value = analogRead(pResistor);     //read sensor again to determine is we stay in loop or continue
  }
}

void loop() {
  delay(1); 
  photoresistor();  //battery protection with sleep at night
  Serial.print("Illumination: "); Serial.println(value);    
  
  sensor.requestTemperatures();
  float wt=sensor.getTempC();
  wt = wt*1.8+32; 
  String wtr=AD.roundNumber(wt); 
  String watertemp = "Water: " + wtr + "F    ";
  Serial.println(watertemp);
  
  AD.getWeather(10,3619);  //dataInputPin, Arduino Uno, pin 10, channelCode for my unit is 3619
   
  delay(1);
  String dData;
  dData = watertemp + AD.weather.wind; //build string for display
  displayData(dData); //call display function with string parameter
  delay(1);
  Serial.println(dData);
  Serial.flush(); //wait until all serial print is done 
  
}
