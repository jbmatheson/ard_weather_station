
// Libraries Declaration
#include <Elegoo_GFX.h>     // Core graphics library

#include <Elegoo_TFTLCD.h>    // Hardware-specific library
//#include <TouchScreen.h>      // Touch Screen Library

#include <DHT.h>              //DTH Library
#include <DHT_U.h>            //DTH Library

#include <Wire.h>
#include <DS1307RTC.h>

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// Color definitions
#define BLACK       0x0000      /*   0,   0,   0 */
#define NAVY        0x000F      /*   0,   0, 128 */
#define DARKGREEN   0x03E0      /*   0, 128,   0 */
#define DARKCYAN    0x03EF      /*   0, 128, 128 */
#define MAROON      0x7800      /* 128,   0,   0 */
#define PURPLE      0x780F      /* 128,   0, 128 */
#define OLIVE       0x7BE0      /* 128, 128,   0 */
#define LIGHTGREY   0xC618      /* 192, 192, 192 */
#define DARKGREY    0x7BEF      /* 128, 128, 128 */
#define BLUE        0x001F      /*   0,   0, 255 */
#define GREEN       0x07E0      /*   0, 255,   0 */
#define CYAN        0x07FF      /*   0, 255, 255 */
#define RED         0xF800      /* 255,   0,   0 */
#define MAGENTA     0xF81F      /* 255,   0, 255 */
#define YELLOW      0xFFE0      /* 255, 255,   0 */
#define WHITE       0xFFFF      /* 255, 255, 255 */
#define ORANGE      0xFD20      /* 255, 165,   0 */
#define GREENYELLOW 0xAFE5      /* 173, 255,  47 */

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A1  // must be an analog pin, use "An" notation!
#define YM 1   // can be a digital pin
#define XP 0   // can be a digital pin

//Touch For New ILI9341 TP
/*#define TS_MINX 204
#define TS_MINY 195
#define TS_MAXX 948
#define TS_MAXY 910*/

//Screen Declaration
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

//TouchScreen Area Declaration
//TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//Button object declaration
Elegoo_GFX_Button buttons;

//RTC Declaration
DS1307RTC clock;

//Custiom Variables
uint16_t identifier;                 //Store Screen Identifier

#define DHTPIN 52                    //what pin we're connected to DTH Sensor
#define DHTTYPE DHT11                //DHT 11

DHT dht(DHTPIN, DHTTYPE);            //DTH Object Declaration

//Variables
unsigned long startMillis;           //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 5000;   //the value is a number of milliseconds

void setup(void) {

  dht.begin();

  Serial.begin(9600);
  Serial.println(F("Weather Station"));
  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  pinMode( YP, OUTPUT );     //.kbv these pins are shared with TFT
  pinMode( XM, OUTPUT );     //.kbv these pins are shared with TFT

  drawScreen();
  drawTemperature();     // Draw Temperature Box
  readTempSensor();      // Read DHT11 Sensor

  startMillis = millis();  //initial start time
}

void loop() {

  //Start Timing to next temperature read
  currentMillis = millis();
  if (currentMillis - startMillis >= period)
  {
    readTempSensor();
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current .
  }

  //TSPoint p = ts.getPoint();

  /*if (p.z > ts.pressureThreshhold) {
    pinMode( YP, OUTPUT );     //.kbv these pins are shared with TFT
    pinMode( XM, OUTPUT );     //.kbv these pins are shared with TFT
    
    Serial.print("PRESSURE: "); Serial.println(p.z);
    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);
    p.y = map(p.y, TS_MAXY, TS_MINY, 0, 240);

    //Print TouchScreen area seleted
    Serial.print("P X"); Serial.print(p.x);
    Serial.print("P Y"); Serial.print(p.y);
  }*/
  printTime();
}

void drawScreen() {

  tft.reset();
  getIdentifierScreen();
  tft.begin(identifier);
  tft.setRotation(3);
  tft.fillScreen(BLACK);

  tft.setCursor(30, 10);
  tft.setTextColor(RED);  tft.setTextSize(3);
  tft.println("Weather Station");
  tft.drawLine(10, 40, 310, 40, CYAN);

  // TEMP & HUM Box
  tft.drawRect(10, 45, 125, 185, BLUE);
  tft.setCursor(15, 50);
  tft.setTextColor(CYAN);  tft.setTextSize(2);
  tft.println("TEMP:");
  tft.setCursor(15, 140);
  tft.println("HUMIDITY:");

  // DIVIDERS LINES
  tft.drawLine(140, 45, 140, 230, CYAN);
  tft.drawLine(145, 130, 310, 130, BLUE);
  tft.drawLine(145, 135, 310, 135, CYAN);

}

void getIdentifierScreen() {

  identifier = tft.readID();
  if (identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if (identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if (identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  } else if (identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if (identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if (identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if (identifier == 0x0101)
  {
    identifier = 0x9341;
    Serial.println(F("Found 0x9341 LCD driver"));
  } else {
    identifier = 0x9341;
  }
}

void drawTemperature() {

  tft.fillRect(145, 45, 166, 85, BLUE);
  tft.setCursor(210, 50);
  tft.setTextColor(RED);  
  tft.setTextSize(7);

  tft.println("F");
  tft.setCursor(195, 120);
  tft.setTextSize(1);
  tft.println("Fahrenheit");

  tft.setCursor(190, 50);
  tft.setTextSize(2);
  tft.println("o");
}

void readTempSensor() {

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

    //Draw blue window for Temp
    tft.drawRect(10, 45, 125, 185, BLUE);
    tft.setTextColor(CYAN);  
    tft.setTextSize(3);

    //Shows Temperature in Farenheit
    tft.setCursor(15, 70);
    tft.fillRect(15, 70, 110, 70, BLACK);
    tft.println(f);
      
    tft.setCursor(15, 160);
    tft.fillRect(15, 160, 110, 50, BLACK);
    tft.println(h);
    tft.setCursor(110, 160);
    tft.println("%");
}

void printTime() {
  tft.setCursor(145, 145);
  tft.println(__TIME__);
  tft.setCursor(145, 180);
  tft.println(__DATE__);
  delay(3000);
  /*Serial.print(clock.hour, DEC);
    Serial.print(":");
    Serial.print(clock.minute, DEC);
    Serial.print(":");
    Serial.print(clock.second, DEC);
    Serial.print("  ");
    Serial.print(clock.month, DEC);
    Serial.print("/");
    Serial.print(clock.dayOfMonth, DEC);
    Serial.print("/");
    Serial.print(clock.year+2000, DEC);
    Serial.print(" ");
    Serial.print(clock.dayOfMonth);
    Serial.print("*");
    switch (clock.dayOfWeek)// Friendly printout the weekday
    {
      case MON:
      Serial.print("MON");
      break;
      case TUE:
      Serial.print("TUE");
      break;
      case WED:
      Serial.print("WED");
      break;
      case THU:
      Serial.print("THU");
      break;
      case FRI:
      Serial.print("FRI");
      break;
      case SAT:
      Serial.print("SAT");
      break;
      case SUN:
      Serial.print("SUN");
      break;
    }
    Serial.println(" ");*/
}
