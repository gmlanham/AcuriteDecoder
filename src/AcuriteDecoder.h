#ifndef AcuriteDecoder_h
#define AcuriteDecoder_h
#include <Arduino.h>

typedef	struct weatherStruct {
		  String temperature;
		  String wind;
		  String humidity;
		} WEATHER;
		
class AcuriteDecoder {
	public:
			
		//Constructor
		AcuriteDecoder() {};
		WEATHER weather;
		
		String roundNumber(float x);
		
		// Weather function decodes radio signal from Acurite weather senso
		//returns data structure containing humididty, temperature and wind speed.
		weatherStruct getWeather(); 

	private:
	
};

#endif
