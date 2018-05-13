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
///GPS END-----------------------------------------
////DHT11 START-----------------------------------------
int pinDHT11 = 4;
SimpleDHT11 dht11;
////DHT11 END-----------------------------------------
////ADXL335 START-----------------------------------------
char isSent = 'n';
int x, y, z, xfinal, yfinal, zfinal, sec, stop;
int a1 = A0;
int a2 = A1;
int a3 = A2;
////ADXL335 END-----------------------------------------

int greenledPin = 11;
int redledPin = 12;
int startbuttonpin = 10;
int stopbuttonpin = 13;
int changeidpin = 7;
byte leds = 0;
int run;

int changeidMelody[] = {NOTE_B5, NOTE_B5};
int startMelody[] = {NOTE_C5, NOTE_C6};
int stopMelody[] = {NOTE_C5, NOTE_C5};
int dht11ErrorMelody[] = {NOTE_D5, NOTE_D5};
int gpsErrorMelody[] = {NOTE_G6, NOTE_G6,NOTE_G6};
int startMelodyDelay = 300;
int stopMelodyDelay = 400;



void setup() {
  Serial.begin(9600);
   run = 0; //starts stopped
////LED START-----------------------------------------
  pinMode(greenledPin, OUTPUT);
  pinMode(redledPin, OUTPUT);
  pinMode(startbuttonpin, INPUT_PULLUP);
  pinMode(stopbuttonpin, INPUT_PULLUP); 
  pinMode(changeidpin, INPUT_PULLUP);   
  digitalWrite(redledPin, HIGH);
  pinMode(BLUE, OUTPUT);
  digitalWrite(BLUE, LOW);
///GPS START-----------------------------------------
  GPSModule.begin(9600);
///GPS END-----------------------------------------
///ADXL335 START-----------------------------------------
 pinMode(a1,INPUT);
  pinMode(a2,INPUT);
  pinMode(a3,INPUT);
///ADXL335 END-----------------------------------------
}

///GPS START-----------------------------------------
float stringToFloat(String s){
  char buffer[10];
  s.toCharArray(buffer, 10);
  return atof(buffer);
}

 byte temperature = 0;
 byte humidity = 0;
 float latitude;
 float longitude;
 String impact = "-";
 String orientation = "-";

 int deviceid = 1;
 int datano = 1;
 String buttonstate = "on";

 
static const unsigned long REFRESH_INTERVAL = 5000; // ms
  static unsigned long lastRefreshTime = 0;
 

///GPS END-----------------------------------------


void loop()
{

 x = analogRead(a1);     
  y = analogRead(a2);     
  z = analogRead(a3); 
  xfinal = x, DEC;
  yfinal = y, DEC;
  zfinal = z, DEC;

  if(digitalRead(changeidpin) == LOW) {
    if(run > 0){ 
      
    
  }else{
    deviceid++;
      datano = 1;
    for (int thisNote = 0; thisNote < 2; thisNote++) {
    // pin8 output the voice, every scale is 0.5 sencond
    tone(8, changeidMelody[thisNote], 300);
    // Output the voice after several minutes
    delay(startMelodyDelay);
  }
  }

         
         
  
  }
  
  if(digitalRead(startbuttonpin) == LOW) {
         run = 255;
         for (int thisNote = 0; thisNote < 2; thisNote++) {
    // pin8 output the voice, every scale is 0.5 sencond
    tone(8, startMelody[thisNote], 300);
    // Output the voice after several minutes
    delay(startMelodyDelay);
  }
  
  }

  if(digitalRead(stopbuttonpin) == LOW) {
         run = 0;
          for (int thisNote = 0; thisNote < 2; thisNote++) {
    // pin8 output the voice, every scale is 0.5 sencond
    tone(8, stopMelody[thisNote], 300);
    // Output the voice after several minutes
    delay(stopMelodyDelay);
  }
  }

  if(run > 0)
  { 
    digitalWrite(redledPin, LOW);
    digitalWrite(greenledPin, HIGH);
    
    if (zfinal < 340 && isSent == 'n'){
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
  delay(100); 
  startUpdates();
  }else{
    digitalWrite(redledPin, HIGH);
    digitalWrite(greenledPin, LOW);
  }
}



void startUpdates(){
  ADXLLoop();
PeriodicUpdate();
}


void impactUpdate(){
  digitalWrite(redledPin, HIGH);
  DHT11Loop();
  GPSLoop();
  digitalWrite(redledPin, LOW);
  digitalWrite(BLUE, HIGH);
  createJson();
  datano = datano + 1;
  impact = "-";
  digitalWrite(BLUE, LOW);
}

void orientationUpdate(){
  digitalWrite(redledPin, HIGH);
  DHT11Loop();
  GPSLoop();
  digitalWrite(redledPin, LOW);
  digitalWrite(BLUE, HIGH);
  createJson();
  datano = datano + 1;
  orientation = "-";
  digitalWrite(BLUE, LOW);
}

void PeriodicUpdate(){
 if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
  {
  digitalWrite(redledPin, HIGH);
  lastRefreshTime += REFRESH_INTERVAL;
  DHT11Loop();
  GPSLoop();
  digitalWrite(redledPin, LOW);
  digitalWrite(BLUE, HIGH);
  createJson();
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
   datano = datano + 1;
   digitalWrite(BLUE, LOW);

  }
}


void createJson(){
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

void ADXLLoop(){
  x = analogRead(a1);     
  y = analogRead(a2);     
  z = analogRead(a3); 
  xfinal = x, DEC;
  yfinal = y, DEC;
  zfinal = z, DEC;
}


void DHT11Loop(){
  delay(500);
  byte data[40] = {0};
  if (dht11.read(pinDHT11, &temperature, &humidity, data)) {
     for (int thisNote = 0; thisNote < 2; thisNote++) {
    // pin8 output the voice, every scale is 0.5 sencond
    tone(8, dht11ErrorMelody[thisNote], 100);
    // Output the voice after several minutes
    delay(400);
     }
    return;
  }
}

void GPSLoop(){
  
  Serial.flush();
  GPSModule.flush();
  while (GPSModule.available() > 0)
  {
    GPSModule.read();

  }
  if (GPSModule.find("$GPRMC,")) {
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
    
  }
  else {
    failedUpdates++;
     
    
  }
  stringplace = 0;
  pos = 0;
  
  
}















///GPS START-----------------------------------------
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
///GPS END-----------------------------------------




