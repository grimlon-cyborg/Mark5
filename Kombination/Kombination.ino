
#include "sunrise_i2c_single.h"
#include "onewire_sensor.h"
#include "arduino_secrets.h"
#include "thingProperties.h"

int relay_1 = 1;  // assigns relay_1 to pin 1. It is important that we assign it to pin 1, as the relay is internally wired to this pin.
int relay_2 = 2;  // assigns relay_2 to pin 2. Same here, the relay is wired to pin 2, so we can't use a pin of our choosing.

void setup() {
  sunrise_setup();
  onewire_setup();

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  pinMode(A1, INPUT);  // Pin 0 dient nun als Input
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);

  pinMode(relay_1, OUTPUT);
  pinMode(relay_2, OUTPUT);
}

void loop() {

  static bool relaisUsed = false;  // wenn Motor an, keine Verzoegerung und keine CO2 Messung

  bool Ausgabe = true;  // wenn false, Ausgabe nur bei gesendeten seriellen Daten (egal was, aber mit Return)
  if (Serial.available()) {
    Serial.read();  // nur auslesen, keine Verwendung
    Ausgabe = true;
  }

  delay(333);

  if (Ausgabe) Serial.println("----------------- loop() ----------------------- v9");

  webText = "v9";

  ArduinoCloud.update();

  static int Counter = 0;
  Counter += 1;

  if (!relaisUsed) {
    if (Counter > 5) {
      if (Ausgabe) Serial.println("CO2 Messung");
      Counter = 0;
      sunrise_loop();  // Messung dauert ca. 6 Sekunden
      co2 = co2Val;
    } else {
      if (Ausgabe) Serial.println("keine neue CO2 Messung");
      delay(500);
    }

    if (Ausgabe) {
      Serial.print("CO2 (filtered): ");
      Serial.print(co2Val);
      Serial.println(" ppm");
    }
  }

  onewire_loop();

  if (Ausgabe) {
    Serial.print("Celsius temperature: ");
    // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
    Serial.println(onewire_temperatur);
  }

  temperatur = onewire_temperatur;

  // ------

  luftqualitaet = analogRead(A1);  // Zuerst wird der Wert am Pin A0 ausgelesen...
  if (Ausgabe) {
    Serial.print("Der erfasste Luftqualitaet Wert lautet : ");
    Serial.println(luftqualitaet, DEC);  //... und anschließend im seriellen Monitor als Dezimalzahl ausgegeben.
    Serial.println();
  }

  bodenfeuchte = analogRead(A2);  // Zuerst wird der Wert am Pin A0 ausgelesen...
  if (Ausgabe) {
    Serial.print("Der erfasste Bodenfeuchte Wert lautet : ");
    Serial.println(bodenfeuchte, DEC);  //... und anschließend im seriellen Monitor als Dezimalzahl ausgegeben.
    Serial.println();
  }

  licht_1 = analogRead(A3);  // Zuerst wird der Wert am Pin A0 ausgelesen...
  if (Ausgabe) {
    Serial.print("Der erfasste Licht_1 Wert lautet : ");
    Serial.println(licht_1, DEC);  //... und anschließend im seriellen Monitor als Dezimalzahl ausgegeben.
    Serial.println();
  }

  licht_2 = analogRead(A4);  // Zuerst wird der Wert am Pin A0 ausgelesen...
  if (Ausgabe) {
    Serial.print("Der erfasste Licht_2 Wert lautet : ");
    Serial.println(licht_2, DEC);  //... und anschließend im seriellen Monitor als Dezimalzahl ausgegeben.
    Serial.println();
  }

  relaisUsed = false;

  if (licht_1 > 400) {
    digitalWrite(relay_1, HIGH);  // COM & NO verbunden
                                  // relaisUsed = true;
  } else {
    digitalWrite(relay_1, LOW);  // COM & NC verbunden // Licht an
  }

  if (licht_2 < 600) {
    digitalWrite(relay_2, HIGH);  // COM & NO verbunden
                                  // relaisUsed = true;
  } else {
    digitalWrite(relay_2, LOW);  // COM & NC verbunden // Licht an
  }

  xxx = licht_1 + licht_2;

  // ------------------------------------------------------------------------------

  static int bisherBesterCO2 = 999999;  // Startwert moeglichst gross
  static int bisherBesterCO2beiLichtWert = 4;

  static int licht_2_Zaehler = 0;
  static int licht_2_Werte[10];
  int licht_2_Durchschnitt;

  const int licht_2_erlaubte_Abweichung = 150;

  const int n = 10;

  if (licht_2_Zaehler < n) {
    licht_2_Werte[licht_2_Zaehler] = (licht_1 + licht_2) / 2;
    licht_2_Zaehler += 1;
  } else {

    Serial.println("Rechnung");

    licht_2_Durchschnitt = 0;
    for (int i = 0; i < n; ++i) {
      licht_2_Durchschnitt += licht_2_Werte[i];
      //Serial.println(i);
      //Serial.println(licht_2_Werte[i]);
      //Serial.println(licht_2_Durchschnitt);
    }

    licht_2_Durchschnitt /= n;
    //Serial.print("Durchschnitt ");
    //Serial.println(licht_2_Durchschnitt);

    int Abweichungen = 0;
    for (int i = 0; i < n; ++i) {
      int diff = abs(licht_2_Durchschnitt - licht_2_Werte[i]);
      if (diff > licht_2_erlaubte_Abweichung) {
        Abweichungen += 1;
      }
    }

    if (Abweichungen >= 3) {
      Serial.println("zu unregelmaessig");
    } else {
      Serial.println("akzeptabel");

      if (co2Val < bisherBesterCO2) {
        bisherBesterCO2 = co2Val;
        bisherBesterCO2beiLichtWert = licht_2_Durchschnitt;

        // in die Cloud Variablen kopieren
        besterCO2 = bisherBesterCO2;
        besterCO2Licht = bisherBesterCO2beiLichtWert;
      }
    }


    licht_2_Zaehler = 0;
  }
}

// =================================================================================

/*
  Since Temperatur is READ_WRITE variable, onTemperaturChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onTemperaturChange() {
  // Add your code here to act upon Temperatur change
}
/*
  Since Bodenfeuchte is READ_WRITE variable, onBodenfeuchteChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onBodenfeuchteChange() {
  // Add your code here to act upon Bodenfeuchte change
}
/*
  Since Licht1 is READ_WRITE variable, onLicht1Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onLicht1Change() {
  // Add your code here to act upon Licht1 change
}
/*
  Since Licht2 is READ_WRITE variable, onLicht2Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onLicht2Change() {
  // Add your code here to act upon Licht2 change
}
/*
  Since Xxx is READ_WRITE variable, onXxxChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onXxxChange() {
  // Add your code here to act upon Xxx change
}

/*
  Since WebText is READ_WRITE variable, onWebTextChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onWebTextChange() {
  // Add your code here to act upon WebText change
}
/*
  Since Co2 is READ_WRITE variable, onCo2Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onCo2Change() {
  // Add your code here to act upon Co2 change
}
/*
  Since BesterCO2 is READ_WRITE variable, onBesterCO2Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onBesterCO2Change() {
  // Add your code here to act upon BesterCO2 change
}
/*
  Since BesterCO2Licht is READ_WRITE variable, onBesterCO2LichtChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onBesterCO2LichtChange() {
  // Add your code here to act upon BesterCO2Licht change
}
