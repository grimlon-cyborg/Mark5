float onewire_temperatur = 0;

#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is conntec to the Arduino digital pin 4
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire( ONE_WIRE_BUS );

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors( &oneWire );

void onewire_setup()
{
   sensors.begin();
}

void onewire_loop()
{
   sensors.requestTemperatures();

   onewire_temperatur = sensors.getTempCByIndex( 0 );
}
