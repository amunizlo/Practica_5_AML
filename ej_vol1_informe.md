# Práctica 5 Ejercicio 1 subir nota (pulsómetro)
###### Andrea Muñiz
<p></p>

## Programa + explicación

> Declaramos las bibliotecas necesarias

```
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <MAX30105.h>
#include <heartRate.h>
```

> Declaramos las variables que utilizaremos para determinar la media de pulsaciones 

```
MAX30105 particleSensor;
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;
```

> Definimos las medidas del display en pixeles

```
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
```

> Declaramos el nombre del display

```
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
```

> Creamos los _bitmaps_ del dibujo del corazón latiendo

```
static const unsigned char PROGMEM logo2_bmp[] =
{ 0x03, 0xC0, 0xF0, 0x06, 0x71, 0x8C, 0x0C, 0x1B, 0x06, 0x18, 0x0E, 0x02, 0x10, 0x0C, 0x03, 0x10,
0x04, 0x01, 0x10, 0x04, 0x01, 0x10, 0x40, 0x01, 0x10, 0x40, 0x01, 0x10, 0xC0, 0x03, 0x08, 0x88,
0x02, 0x08, 0xB8, 0x04, 0xFF, 0x37, 0x08, 0x01, 0x30, 0x18, 0x01, 0x90, 0x30, 0x00, 0xC0, 0x60,
0x00, 0x60, 0xC0, 0x00, 0x31, 0x80, 0x00, 0x1B, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x04, 0x00,  };

static const unsigned char PROGMEM logo3_bmp[] =
{ 0x01, 0xF0, 0x0F, 0x80, 0x06, 0x1C, 0x38, 0x60, 0x18, 0x06, 0x60, 0x18, 0x10, 0x01, 0x80, 0x08,
0x20, 0x01, 0x80, 0x04, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0xC0, 0x00, 0x08, 0x03,
0x80, 0x00, 0x08, 0x01, 0x80, 0x00, 0x18, 0x01, 0x80, 0x00, 0x1C, 0x01, 0x80, 0x00, 0x14, 0x00,
0x80, 0x00, 0x14, 0x00, 0x80, 0x00, 0x14, 0x00, 0x40, 0x10, 0x12, 0x00, 0x40, 0x10, 0x12, 0x00,
0x7E, 0x1F, 0x23, 0xFE, 0x03, 0x31, 0xA0, 0x04, 0x01, 0xA0, 0xA0, 0x0C, 0x00, 0xA0, 0xA0, 0x08,
0x00, 0x60, 0xE0, 0x10, 0x00, 0x20, 0x60, 0x20, 0x06, 0x00, 0x40, 0x60, 0x03, 0x00, 0x40, 0xC0,
0x01, 0x80, 0x01, 0x80, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x30, 0x0C, 0x00,
0x00, 0x08, 0x10, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x01, 0x80, 0x00  };
```

> Función stup()

En esta función inicializamos el display, generamos un retraso/espera de tres segundos e inicializamos el sensor.

```
void setup() {  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(3000);
  particleSensor.begin(Wire, I2C_SPEED_FAST);
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
}
```

> Función bucle

En esta función creamos varios bucles _if()_ 
- Primer _if_: Si se detecta un dedo, elimina el mensaje de la pantalla y dibuja la primera imagen del corazón y BMP.
- Segundo _if_: Si se detecta un latido se dibuja la segunda imagen del corazón y se escribe la media de latidos calculada.
- Tercer _if_: Calcula la media de latidos por minuto.
- Cuarto _if_: Si no detecta un dedo, escribe por pantalla _Please place your finger_.

```
void loop() {
  long irValue = particleSensor.getIR();
  if(irValue > 7000){
    display.clearDisplay();
    display.drawBitmap(5, 5, logo2_bmp, 24, 21, WHITE);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(50,0);
    display.println("BPM");
    display.setCursor(50,18);
    display.println(beatAvg);
    display.display();
    
    if (checkForBeat(irValue) == true){
      display.clearDisplay();
      display.drawBitmap(0, 0, logo3_bmp, 32, 32, WHITE);
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(50,0);
      display.println("BPM");
      display.setCursor(50,18);
      display.println(beatAvg);
      display.display();
      long delta = millis() - lastBeat;
      lastBeat = millis();

      beatsPerMinute = 60 / (delta / 1000.0);

      if (beatsPerMinute < 255 && beatsPerMinute > 20){
        rates[rateSpot++] = (byte)beatsPerMinute;
        rateSpot %= RATE_SIZE;
        beatAvg = 0;
        for (byte x = 0 ; x < RATE_SIZE ; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }
  }
    if (irValue < 7000){
      beatAvg=0;
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(30,5);
      display.println("Please Place ");
      display.setCursor(30,15);
      display.println("your finger ");
      display.display();
    }
}
```

***

## Resultados

Junto a los archivos de la práctica 5 se adjunta un video del funcioamiento.
Al cargarse el programa a la placa se puede leer en el _display_ el mensaje que nos pide que coloquemos el dedo en el sensor, al ponerlo la pantalla se actualiza y aparece la pantalla donde se muestran las pulsaciones.
