///Define the libraries needed for the application.
#include <ArduinoJson.h>
#include <pitches.h>
#include <SoftwareSerial.h>
#include <SimpleDHT.h>

///Define the locations of each light bulb on Arduino UNO.
#define BLUE 9
#define GREEN 8

///Define the attributes required for the GPS module.
#define NMEA_LATITUDE_INDEX (2)
#define NMEA_LATITUDE_HEMISPHERE_INDEX (3)
#define NMEA_LONGITUDE_INDEX (4)
#define NMEA_LONGITUDE_HEMISPHERE_INDEX (5)
#define NMEA_TIME_INDEX (0)
#define NMEA_DATE_INDEX (8)

///Define the locations of the GPS Module on Arduino.
SoftwareSerial GPSModule(3, 2); // RX, TX
int updates;
int failedUpdates;
int pos;
int stringplace = 0;
String timeUp;
String nmea[15];
String labels[12] {"Time: ", "Status: ", "Latitude: ", "Hemisphere: ", "Longitude: ", "Hemisphere: ", "Speed: ", "Track Angle: ", "Date: "};

///DHT11 Pin Specification
int pinDHT11 = 4;
SimpleDHT11 dht11;

///ADXL335 Pin Sepcifiation
char isSent = 'n';
int x, y, z, xfinal, yfinal, zfinal, sec, stop;
int a1 = A0;
int a2 = A1;
int a3 = A2;

///Define Led Pin Specification
int greenledPin = 11;
int redledPin = 12;

///Define Button Specification
int startbuttonpin = 10;
int stopbuttonpin = 13;
int changeidpin = 7;

///Checker for if the device is running
int run;

///Specifying melody for the buzzer component
int changeidMelody[] = {NOTE_B5, NOTE_B5};
int startMelody[] = {NOTE_C5, NOTE_C6};
int stopMelody[] = {NOTE_C5, NOTE_C5};
int dht11ErrorMelody[] = {NOTE_D5, NOTE_D5};
int gpsErrorMelody[] = {NOTE_G6, NOTE_G6,NOTE_G6};
int startMelodyDelay = 300;
int stopMelodyDelay = 400;

//This method is run only once when the device is power up.
void setup() {
  //Sepcifiy the baud for all serial prints.
  Serial.begin(9600);

  //Start the checker as device not running.
  run = 0; 

  //Specify all the pin types for all the leds and buttons.
  pinMode(greenledPin, OUTPUT);
  pinMode(redledPin, OUTPUT);
  pinMode(startbuttonpin, INPUT_PULLUP);
  pinMode(stopbuttonpin, INPUT_PULLUP); 
  pinMode(changeidpin, INPUT_PULLUP);   
  digitalWrite(redledPin, HIGH);
  pinMode(BLUE, OUTPUT);
  digitalWrite(BLUE, LOW);

  //Sepcifiy the baud for the GPS component.
  GPSModule.begin(9600);

  //Specify the ADXL335 pin types.
  pinMode(a1,INPUT);
  pinMode(a2,INPUT);
  pinMode(a3,INPUT);
}

//Convert String to Float (USED BY GPS Module)
float stringToFloat(String s){
  char buffer[10];
  s.toCharArray(buffer, 10);
  return atof(buffer);
}

//Specify pre-set values for all of the data that will be collected.
byte temperature = 0;
byte humidity = 0;
float latitude;
float longitude;
String impact = "-";
String orientation = "-";
int deviceid = 1;
int datano = 1;
String buttonstate = "on";

//Set the refresh value for the device to collect data.
static const unsigned long REFRESH_INTERVAL = 5000; // ms
static unsigned long lastRefreshTime = 0; //ms

//This method will run continuously until the device is powered off.
void loop()
{
  //Specify varibale placeholder for the ADXL335.
  x = analogRead(a1);     
  y = analogRead(a2);     
  z = analogRead(a3); 
  xfinal = x, DEC;
  yfinal = y, DEC;
  zfinal = z, DEC;

  //Run the conditional statement when the Change ID button is pressed.
  if(digitalRead(changeidpin) == LOW) {
    //Check if the device is already running. (Not Running = 0. Running > 0).
    if(run > 0){ 
      //*Device is running.*
      ///Do Nothing.
    }else{
      //*Device is not running.*

      //Change the device id by one increment.
      deviceid++;
      //Restart the data number to 1.
      datano = 1;
      //Play a medoly indicating that the device id has been changed.
      for (int thisNote = 0; thisNote < 2; thisNote++) {
        // pin8 output the voice, every scale is 300 milliseconds
        tone(8, changeidMelody[thisNote], 300);
        // Output the voice after specified delay
        delay(startMelodyDelay);
      }
    }
  }

  //Run the conditional statement when the Start button is pressed.
  if(digitalRead(startbuttonpin) == LOW) {
    //Set run checker to 255 meaning device is running.
    run = 255;
    //Play a medoly indicating that the start button executed.
    for (int thisNote = 0; thisNote < 2; thisNote++) {
      // pin8 output the voice, every scale is 300 milliseconds
      tone(8, startMelody[thisNote], 300);
      // Output the voice after specified delay
      delay(startMelodyDelay);
    }

  }

  //Run the conditional statement when the Stop button is pressed.
  if(digitalRead(stopbuttonpin) == LOW) {
    //Set run checker to 0 meaning device is not running.
    run = 0;
    //Play a medoly indicating that the stop button executed.
    for (int thisNote = 0; thisNote < 2; thisNote++) {
      // pin8 output the voice, every scale is 300 milliseconds
      tone(8, stopMelody[thisNote], 300);
      // Output the voice after specified delay
      delay(stopMelodyDelay);
    }
  }

  //Run the conditional statement to check the run checker of the device.
  if(run > 0)
  //Device is running.
  { 
    //Turn off red led and turn on green led.
    digitalWrite(redledPin, LOW);
    digitalWrite(greenledPin, HIGH);

    //Run the conditional statement to check if ADXL335 reading exceeds defined value range.
    if (zfinal < 340 && isSent == 'n'){
      //Trigger method to send a data updated.
      isSent = 'y';
      orientation = zfinal;
      orientationUpdate();
    }else if (zfinal > 380 && isSent == 'y'){
      isSent = 'n';
    }
    if (yfinal < 130 || yfinal > 450){
      impact = yfinal;
      impactUpdate();
    }
    if (xfinal < 110 || xfinal > 550){
      impact = xfinal;
      impactUpdate();
    }
    //Check every 100 milliseconds.
    delay(100); 
    //Method is to start checking ADXL335, DHT11 and GPS.
    startUpdates();
  }else{
    //Device is not running.
    //Turn off green led and turn on red led.
    digitalWrite(redledPin, HIGH);
    digitalWrite(greenledPin, LOW);
  }
}

//This method trigger further method to start checking component data.
void startUpdates(){
  //Check ADXL335 data.
  ADXLLoop();
  //Check DHT11 Data
  PeriodicUpdate();
}

//This method is trigger when the conditional statement in the loop method is triggered.
void impactUpdate(){
  digitalWrite(redledPin, HIGH);
  //Collect DHT11 data.
  DHT11Loop();
  //Collect GPS data.
  GPSLoop();
  digitalWrite(redledPin, LOW);
  digitalWrite(BLUE, HIGH);
  //Create JSON from collected data.
  createJson();
  //Set next data number.
  datano = datano + 1;
  //Set next impact value to -.
  impact = "-";
  digitalWrite(BLUE, LOW);
}

//This method is trigger when the conditional statement in the loop method is triggered.
void orientationUpdate(){
  digitalWrite(redledPin, HIGH);
  //Collect DHT11 data.
  DHT11Loop();
  //Collect GPS data.
  GPSLoop();
  digitalWrite(redledPin, LOW);
  digitalWrite(BLUE, HIGH);
  //Create JSON from collected data.
  createJson();
  //Set next data number.
  datano = datano + 1;
  //Set next orientation value to -.
  orientation = "-";
  digitalWrite(BLUE, LOW);
}

//This method is trigger when the conditional statement in the loop method is triggered.
void PeriodicUpdate(){
  //Run the conditional statement to check if last refresh time is equal or greater than the refresh interval. .
  if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
  {
    digitalWrite(redledPin, HIGH);
    //Assign lastRefreshTime with new value.
    lastRefreshTime += REFRESH_INTERVAL;
    //Collect DHT11 data.
    DHT11Loop();
    //Collect GPS data.
    GPSLoop();
    digitalWrite(redledPin, LOW);
    digitalWrite(BLUE, HIGH);
    //Create JSON from collected data.
    createJson();
    
    //---------------------------------------------
    //StaticJsonBuffer<200> jsonBuffer;
    //JsonObject& root = jsonBuffer.createObject();
    //root["deviceid"] = deviceid;
    //root["datano"] = datano;
    //root["humidity"] = humidity;
    //root["latitude"] = latitude;
    //root["longitude"] = longitude;
    //root["temperature"] = temperature;
    //root["impact"] = impact;
    //root["orientation"] = orientation;
    //String jsonChar;
    //root.printTo(jsonChar);
    //Serial.println(jsonChar);
    //---------------------------------------------
    
    //Set next data number.
    datano = datano + 1;
    digitalWrite(BLUE, LOW);
  }
}

//This method collects all of the values that have been set by the components and creates a JSON.
void createJson(){
  //Prints the JSON to the serial monitor.
  Serial.print('{');
  Serial.print('"');
  Serial.print("deviceid");
  Serial.print('"');
  Serial.print(':');
  Serial.print('"');
  Serial.print("device");
  Serial.print(deviceid);
  Serial.print('"');
  Serial.print(',');
  Serial.print('"');
  Serial.print("datano");
  Serial.print('"');
  Serial.print(':');
  Serial.print('"');
  Serial.print(datano);
  Serial.print('"');
  Serial.print(',');
  Serial.print('"');
  Serial.print("humidity");
  Serial.print('"');
  Serial.print(':');
  Serial.print('"');
  Serial.print(humidity);
  Serial.print('"');
  Serial.print(',');
  Serial.print('"');
  Serial.print("latitude");
  Serial.print('"');
  Serial.print(':');
  Serial.print('"');
  Serial.print(latitude);
  Serial.print('"');
  Serial.print(',');
  Serial.print('"');
  Serial.print("longitude");
  Serial.print('"');
  Serial.print(':');
  Serial.print('"');
  Serial.print(longitude);
  Serial.print('"');
  Serial.print(',');
  Serial.print('"');
  Serial.print("temperature");
  Serial.print('"');
  Serial.print(':');
  Serial.print('"');
  Serial.print(temperature);
  Serial.print('"');
  Serial.print(',');
  Serial.print('"');
  Serial.print("impact");
  Serial.print('"');
  Serial.print(':');
  Serial.print('"');
  Serial.print(impact);
  Serial.print('"');
  Serial.print(',');
  Serial.print('"');
  Serial.print("orientation");
  Serial.print('"');
  Serial.print(':');
  Serial.print('"');
  Serial.print(orientation);
  Serial.print('"');
  Serial.println('}');
}

//This method is trigger when the conditional statement in the loop method is triggered.
void ADXLLoop(){
  //Obtain all of the values from ADXL335 and assigned to variables.
  x = analogRead(a1);     
  y = analogRead(a2);     
  z = analogRead(a3); 
  xfinal = x, DEC;
  yfinal = y, DEC;
  zfinal = z, DEC;
}

//This method is trigger when the conditional statement in the loop method is triggered.
void DHT11Loop(){
  //Obtain all of the values from DHT11 and assigned to variables.
  delay(500);
  byte data[40] = {0};
  //Check if DHT11 is functional and readable.
  if (dht11.read(pinDHT11, &temperature, &humidity, data)) {
    //*DHT11 is not functioning correctly.*
    //Play error melody.
    for (int thisNote = 0; thisNote < 2; thisNote++) {
      // pin8 output the voice, every scale is 100 millisecond
      tone(8, dht11ErrorMelody[thisNote], 100);
      // Output the voice after 400 millisecond
      delay(400);
    }
    return;
  }
}

//This method is trigger when the conditional statement in the loop method is triggered.
void GPSLoop(){
  //Flush makes sure that all data has been sent, and the buffer is empty.
  Serial.flush();
  GPSModule.flush();
  //Check if GPS component functioning correctly.
  while (GPSModule.available() > 0)
  {
    GPSModule.read();
  }
  //Check if GPS component has a signal and able to collect data.
  if (GPSModule.find("$GPRMC,")) {
    //GPS signal found.
    String tempMsg = GPSModule.readStringUntil('\n');
    for (int i = 0; i < tempMsg.length(); i++) {
      if (tempMsg.substring(i, i + 1) == ",") {
        nmea[pos] = tempMsg.substring(stringplace, i);
        stringplace = i + 1;
        pos++;
      }
      if (i == tempMsg.length() - 1) {
        nmea[pos] = tempMsg.substring(stringplace, i);
      }
    }
    updates++;
    latitude = ConvertLat(nmea[NMEA_LATITUDE_INDEX], nmea[NMEA_LATITUDE_HEMISPHERE_INDEX]);
    longitude = ConvertLng(nmea[NMEA_LONGITUDE_INDEX], nmea[NMEA_LONGITUDE_HEMISPHERE_INDEX]);
  } else {
    //GPS signal not found.
    failedUpdates++;
  }
  stringplace = 0;
  pos = 0;
}

//Convert latitude string into a float.
float ConvertLat(String latitudeWrongFormat, String hemisphere) {
  String posneg = "";
  if (hemisphere == "S") {
    posneg = "-";
  }
  String latfirst;
  float latsecond;
  for (int i = 0; i < latitudeWrongFormat.length(); i++) {
    if (latitudeWrongFormat.substring(i, i + 1) == ".") {
      latfirst = latitudeWrongFormat.substring(0, i - 2);
      latsecond = stringToFloat(latitudeWrongFormat.substring(i - 2));
    }
  }
  latsecond = latsecond / 60;
  String CalcLat = "";
  char charVal[9];
  dtostrf(latsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLat += charVal[i];
  }
  if (latfirst.startsWith("0")) {
    latfirst = latfirst.substring(1);  
  }
  latfirst += CalcLat.substring(1);
  latfirst = posneg += latfirst;
  return stringToFloat(latfirst);
}

//Convert longitude string into a float.
float ConvertLng(String longitudeWrongFormat, String hemisphere) {
  String posneg = "";
  if (hemisphere == "W") {
    posneg = "-";
  }
  String lngfirst;
  float lngsecond;
  for (int i = 0; i < longitudeWrongFormat.length(); i++) {
    if (longitudeWrongFormat.substring(i, i + 1) == ".") {
      lngfirst = longitudeWrongFormat.substring(0, i - 2);
      lngsecond = stringToFloat(longitudeWrongFormat.substring(i - 2));
    }
  }
  lngsecond = lngsecond / 60;
  String CalcLng = "";
  char charVal[9];
  dtostrf(lngsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLng += charVal[i];
  }
  lngfirst += CalcLng.substring(1);
  while (lngfirst.startsWith("0")) {
    lngfirst = lngfirst.substring(1);  
  }
  if (lngfirst.startsWith(".")){
    lngfirst = "0" + lngfirst;
  }
  lngfirst = posneg += lngfirst;
  return stringToFloat(lngfirst);
}
