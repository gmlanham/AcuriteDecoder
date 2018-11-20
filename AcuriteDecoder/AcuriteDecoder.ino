/*No libraries used, all functionality coded in the sketch. The purpose of this sketch is to receive and decode 433mhz data sent by Acurite 3-in-1 Weather sensor. 
 * A 433mhz reciever and an Arduino compatible microprocessor are required. 
*/
//433mhz receiver variables
int dataInputPin = 10; //from 433 receiver, change pin as required for microprocessor.
int channelCode = 3619; //my Acurite 3-in-1 identity is 3619
byte ar[76]; //array to save the input RF pulse stream, pulse width of low; either 0 or 1 depending on width.
byte pos=0; //position index in ar[], data array

//Weather variables
double realtemp; double realtempF;                                  
unsigned char hum;
double windspeed=0;
String rtfTrim;
String rtTrim;
String wsTrim; //windspeed string
//String temperature;
//String wind;
//String humidity;

struct weatherStruct {
  String temperature;
  String wind;
  String humidity;
} weather;

//weatherStruct getWeather();//declare funtion as struct type

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
  // put your main code here, to run repeatedly:
  //String concatWeather=
  getWeather(dataInputPin,channelCode);  
  Serial.println("");
  
  Serial.println(weather.temperature);
  Serial.println(weather.wind);
  Serial.println(weather.humidity);      
  delay(1);
//  Serial.println("");
//  Serial.println(temperature);  
//  Serial.println(humidity);
//  Serial.println(wind);
}

String roundNumber(float x){
  String roundNum="";
  float y=(x*10);
  int z=(int)(y+0.5);
  float zf=(float)z;
  roundNum=String(zf/10).substring(0,String(zf/10).indexOf('.')+2);
  return roundNum;
}

//decode Acurite 433mhz radio signal, packet sent every 18 seconds.
//pulse width modulation- wide pulse = 0 or narrow pulse = 1.
weatherStruct getWeather(int dataInputPin, int channelCode){
  String concatHTW= "";
  b: pos=0;
  unsigned long dur; //pulse duration, width
  unsigned long synchL=800;
  unsigned long synchS=551;
  unsigned long zeroL=550;
  unsigned long zeroS=351;
  unsigned long oneL=350;
  unsigned long oneS=151; 

  //this first condition test for 4x, synch pulses in series,
    dur = pulseIn(dataInputPin, LOW); if ((dur>synchS)&&(dur<synchL)) {
      dur = pulseIn(dataInputPin, LOW); if ((dur>synchS)&&(dur<synchL)) { 
        dur = pulseIn(dataInputPin, LOW); if ((dur>synchS)&&(dur<synchL)) {
          dur = pulseIn(dataInputPin, LOW); if ((dur>synchS)&&(dur<synchL)) {
          }else goto b;
        }else goto b;
      }else goto b;
    }else goto b;
    
    //save the input RF pulse stream, pulse width of 0 or 1 depending on duration.
    for (int i=0; i <= 75; i++){
      dur = pulseIn(dataInputPin, LOW); //assign 1 or 0 depending on pulse width
      if ((dur>zeroS)&&(dur<zeroL)){ar[pos] = 0; Serial.print(0); pos++; } //assign each pulse, binary value to array, ar[]
      if ((dur>oneS)&&(dur<oneL)){ar[pos] = 1; Serial.print(1); pos++; }
    }
    int cc=0; for (int i=0; i <= 11; i++) {cc=cc << 1; cc=cc+ar[i]; //determine channel code, first 12 bits
    Serial.print(ar[i]); 
    }
    Serial.print(" ChannelCode: "); Serial.print(cc);Serial.println("");
    
  if (cc==channelCode) //print data for valid channel code, old value was 3619
    {
    int hum5=0; for (int i=0; i <= 6; i++) {hum5=hum5 << 1; hum5=hum5+ar[25+i]; /*Serial.print(ar[25+i]);*/ }
    //Serial.print(" ");Serial.print("Humidity25-6: "); Serial.print(hum5); Serial.println("%");
    hum=hum5;
    weather.humidity= "Hum: " + String(hum5) + "% "; //Serial.println (humidity);
    concatHTW=hum5 + ",";
    int temp=0; for (int i=0; i <= 11; i++) {if (i==7){i++;}temp=temp << 1; temp=temp+ar[33+i]; /*Serial.print(ar[33+i]);*/ } //2 bytes, (skip 1st nibble) skip parity bit of each byte
    realtemp = (float(0.457)*float(temp))-float(103.20); //calbration
    rtTrim=roundNumber(realtemp); 

    if(realtemp<43){   //test for reasonable temperature, <109.4F
      realtempF=32+realtemp*1.8; //convert to farenheit
      rtfTrim=roundNumber(realtempF);
      weather.temperature="Temp: " + rtfTrim + "F "; //string to transmit via LoRA
      concatHTW=concatHTW + rtfTrim + ",";
      //Serial.print(" Temperature33-11: "); Serial.print(temp); Serial.print(" raw, ");Serial.println(rtfTrim + "F, ");
    }
    int wind2=0; for (int i=0; i <= 6; i++) {wind2=wind2 << 1; wind2=wind2+ar[49+i]; /*Serial.print(" "+String(i)+" ");*/Serial.print(ar[49+i]);} 
    Serial.print(" " + String(wind2) + " raw ");
    float realwind2 = float(wind2) / float(1.2); //calibration, old calibration factor, float(50.1)
  
    //if(wind2/windspeed != 2)//wind doubling between readings probably not correct.
    {
      windspeed=wind2;
      wsTrim=roundNumber(realwind2);
      weather.wind= "Wind: " + wsTrim + "mph ";
      concatHTW=concatHTW + wsTrim;
      //wsTrim=String(windspeed).substring(0,String(windspeed).indexOf('.')+2);Serial.println(wsTrim);
      Serial.print(" ");Serial.print("wind49: "); Serial.print(realwind2);Serial.println("mph");//wind is averaged over an unknown period
    }
    //Serial.println(" ");
    wind2=0;
    }
  pos=0;
  yield();  
  //return concatHTW;
  return weather;
}
