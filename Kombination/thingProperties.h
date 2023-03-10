// Code generated by Arduino IoT Cloud, DO NOT EDIT.

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char SSID[] = SECRET_SSID;           // Network SSID (name)
const char PASS[] = SECRET_OPTIONAL_PASS;  // Network password (use for WPA, or use as key for WEP)

void onWebTextChange();
void onTemperaturChange();
void onBodenfeuchteChange();
void onCo2Change();
void onLicht1Change();
void onLicht2Change();
void onXxxChange();
void onBesterCO2Change();
void onBesterCO2LichtChange();

String webText;
float temperatur;
int bodenfeuchte;
int co2;
int licht_1;
int licht_2;
int luftqualitaet;
int xxx;
int besterCO2;
int besterCO2Licht;

void initProperties() {

  ArduinoCloud.addProperty(webText, READWRITE, ON_CHANGE, onWebTextChange);
  ArduinoCloud.addProperty(temperatur, READWRITE, 1 * SECONDS, onTemperaturChange);
  ArduinoCloud.addProperty(bodenfeuchte, READWRITE, 1 * SECONDS, onBodenfeuchteChange);
  ArduinoCloud.addProperty(co2, READWRITE, ON_CHANGE, onCo2Change);
  ArduinoCloud.addProperty(licht_1, READWRITE, 1 * SECONDS, onLicht1Change);
  ArduinoCloud.addProperty(licht_2, READWRITE, 1 * SECONDS, onLicht2Change);
  ArduinoCloud.addProperty(luftqualitaet, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(xxx, READWRITE, ON_CHANGE, onXxxChange);
  ArduinoCloud.addProperty(besterCO2, READWRITE, ON_CHANGE, onBesterCO2Change);
  ArduinoCloud.addProperty(besterCO2Licht, READWRITE, ON_CHANGE, onBesterCO2LichtChange);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
