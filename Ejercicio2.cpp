#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define I2C_SDA 21
#define I2C_SCL 22

#define SEALEVELPRESSURE_HPA (1013.25)

TwoWire I2CBME = TwoWire(0);
Adafruit_BME280 bme;

unsigned long delayTime;

void setup() {
    Serial.begin(115200);
    Serial.println(F("BME280 test"));
    I2CBME.begin(I2C_SDA, I2C_SCL, 100000);

    bool status;
    status = bme.begin(0x76, &I2CBME); 

    if (!status) {
        Serial.println("No se ha encontrado un sensor BME280 válido.");
        while (1);
  }

    Serial.println("-- Default Test --");
    delayTime = 1000;

    Serial.println();
}

void printValues() {
    // Temperatura
    Serial.print("Temperatura = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");
    
    // Presión
    Serial.print("Presión = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    // Altitud aproximada
    Serial.print("Altitud aprox. = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    // Humedad
    Serial.print("Humedad = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}

void loop() { 
  printValues();
  delay(delayTime);
}