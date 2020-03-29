/*
   All thanks to DIY-Machines for allowing me to fork his work.
   This project is implemented using DIY-Machines's DigitalClockSmartShelving
   Can find his code on https://github.com/DIY-Machines/DigitalClockSmartShelving
   
   This is developed by SAMEER TUTEJA and if required to modify it, please contact beforehand.
   https://github.com/sam-tj

   This is tested for NODEMCU 1.0
   For any suggestions please contact me using GITHUB.
*/

#include <Adafruit_NeoPixel.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char *ssid     = "<SSID>";            //SSID for WIFI
const char *password = "<PASSWORD>";        //Password for WIFI

unsigned int timezoneSign = +1;             //Sign before your time zone use with 1 ie "+1" / "-1"
int timezoneHR = 5;                         //hours more than GMT
int timezoneMIN = 30;                       //mins more than GMT

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

int TimeVal[3];

// Connection of Light Sensor to 
#define LightSensorPin  A0

//Connection of NeoPixel LED to pins
#define LEDCLOCK_PIN  D6
#define LEDDOWNLIGHT_PIN  D5

// Number of NeoPixels are attached
#define LEDCLOCK_COUNT 216
#define LEDDOWNLIGHT_COUNT 12

//(red * 65536) + (green * 256) + blue ->for 32-bit merged colour value so 16777215 equals white
int clockMinuteColour = 51200; //1677
int clockHourColour = 140000000; //7712

int clockFaceBrightness = 0;

// Declare our NeoPixel objects:
Adafruit_NeoPixel stripClock(LEDCLOCK_COUNT, LEDCLOCK_PIN, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel stripDownlighter(LEDDOWNLIGHT_COUNT, LEDDOWNLIGHT_PIN, NEO_GRB + NEO_KHZ800);

//  Argument 1 = Number of pixels in NeoPixel strip
//  Argument 2 = NodeMCU pin number 
//  Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


// Smoothing of the readings from the light sensor so it is not too twitchy
const int numReadings = 12;

int readings[numReadings];        // the readings from the analog input
int readIndex = 0;                // the index of the current reading
long total = 0;                   // the running total
long average = 0;                 // the average


void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  unsigned int timezone = (((timezoneHR * 60) + timezoneMIN) * 60) * timezoneSign;
  timeClient.begin();
  timeClient.setTimeOffset(timezone);

  stripClock.begin();             // INITIALIZE NeoPixel stripClock object (REQUIRED)
  stripClock.show();              // Turn OFF all pixels ASAP
  stripClock.setBrightness(100);  // Set inital BRIGHTNESS (max = 255)


  stripDownlighter.begin();           // INITIALIZE NeoPixel stripClock object (REQUIRED)
  stripDownlighter.show();            // Turn OFF all pixels ASAP
  stripDownlighter.setBrightness(50); // Set BRIGHTNESS (max = 255)

  //smoothing
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

}

void loop() {

  readTime();        //display time is directly called when seconds equals "00"

  //additional delay if required can be given but would cause display issues if seconds value is not received correctly from net
  //can check if loop in readTime() line  #157

  delay(1000);

  //Record a reading from the light sensor and add it to the array
  readings[readIndex] = analogRead(LightSensorPin); //get an average light level from previouse set of samples
  Serial.print("Light sensor value added to array = ");
  Serial.println(readings[readIndex]);
  readIndex = readIndex + 1; // advance to the next position in the array:

  // if we're at the end of the array move the index back around...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  //now work out the sum of all the values in the array
  int sumBrightness = 0;
  for (int i = 0; i < numReadings; i++)
  {
    sumBrightness += readings[i];
  }
  Serial.print("Sum of the brightness array = ");
  Serial.println(sumBrightness);

  // and calculate the average:
  int lightSensorValue = sumBrightness / numReadings;
  Serial.print("Average light sensor value = ");
  Serial.println(lightSensorValue);


  //set the brightness based on ambiant light levels
  clockFaceBrightness = map(lightSensorValue, 50, 1000, 200, 1);
  stripClock.setBrightness(clockFaceBrightness); // Set brightness value of the LEDs
  Serial.print("Mapped brightness value = ");
  Serial.println(clockFaceBrightness);

  stripClock.show();

  //(red * 65536) + (green * 256) + blue ->for 32-bit merged colour value so 16777215 equals white
  stripDownlighter.fill(16777215, 0, LEDDOWNLIGHT_COUNT);
  stripDownlighter.show();

}

/*
   This funciton read time from NTP server using the GMT time provided at top.
   This then prints time on Serial Monitor for every second as (HH :: MM :: SS)
*/

void readTime() {

  Serial.println("");
  timeClient.update();
  String TimSplit = timeClient.getFormattedTime();
  int siz = TimSplit.length() + 1;
  char TimArray [siz + 1];
  TimSplit.toCharArray(TimArray, siz);
  char *token = strtok(TimArray, ":");
  int i = 0;
  while (token != NULL)
  {
    TimeVal[i] = atoi (token);
    token = strtok(NULL, ":");
    i++;
  }
  for (i = 0; i < 3; i++)
  {
    Serial.print(TimeVal[i]);
    Serial.print(" :: ");
  }
  if (TimeVal[2] <= 10)        // for latency issues seconds displayTime will be called for any value less than 10
    displayTime();
}

void displayTime() {

  stripClock.clear(); //clear the clock face

  int firstMinuteDigit = TimeVal[1] % 10;           //FIRST digit to display on clock (ie 4 from 10:34 (HH:MM))
  displayNumber(firstMinuteDigit, 0, clockMinuteColour);

  int secondMinuteDigit = floor(TimeVal[1] / 10);   //SECOND digit to display on clock (ie 3 from 10:34 (HH:MM))
  displayNumber(secondMinuteDigit, 63, clockMinuteColour);

  int firstHourDigit = TimeVal[0];                  //THIRD digit to display on clock (ie 0 from 10:34 (HH:MM))
  if (firstHourDigit > 12) {
    firstHourDigit = firstHourDigit - 12;
  }
  firstHourDigit = firstHourDigit % 10;
  displayNumber(firstHourDigit, 126, clockHourColour);

  int secondHourDigit = TimeVal[0];                 //FOURTH digit to display on clock (ie 1 from 10:34 (HH:MM))
  if (secondHourDigit > 12) {
    secondHourDigit = secondHourDigit - 12;
  }
  if (secondHourDigit > 9) {
    stripClock.fill(clockHourColour, 189, 18);
  }

}

void displayNumber(int digitToDisplay, int offsetBy, int colourToUse) {

  switch (digitToDisplay) {
    case 0:
      digitZero(offsetBy, colourToUse);
      break;
    case 1:
      digitOne(offsetBy, colourToUse);
      break;
    case 2:
      digitTwo(offsetBy, colourToUse);
      break;
    case 3:
      digitThree(offsetBy, colourToUse);
      break;
    case 4:
      digitFour(offsetBy, colourToUse);
      break;
    case 5:
      digitFive(offsetBy, colourToUse);
      break;
    case 6:
      digitSix(offsetBy, colourToUse);
      break;
    case 7:
      digitSeven(offsetBy, colourToUse);
      break;
    case 8:
      digitEight(offsetBy, colourToUse);
      break;
    case 9:
      digitNine(offsetBy, colourToUse);
      break;
    default:
      break;
  }
}
