#include <ArduinoJson.h>

#include <SoftwareSerial.h>
#include <SimpleDHT.h>

////LED START-----------------------------------------
#define BLUE 9
#define GREEN 8
///GPS START-----------------------------------------
#define NMEA_LATITUDE_INDEX (2)
#define NMEA_LATITUDE_HEMISPHERE_INDEX (3)
#define NMEA_LONGITUDE_INDEX (4)
#define NMEA_LONGITUDE_HEMISPHERE_INDEX (5)
#define NMEA_TIME_INDEX (0)
#define NMEA_DATE_INDEX (8)
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



void setup() {
  Serial.begin(9600);
////LED START-----------------------------------------
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  digitalWrite(GREEN, LOW);
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

 String deviceid = "device2";
 int datano = 1;

 
static const unsigned long REFRESH_INTERVAL = 1000; // ms
  static unsigned long lastRefreshTime = 0;
 

///GPS END-----------------------------------------

void loop() {
digitalWrite(GREEN, HIGH);
ADXLLoop();
PeriodicUpdate();
if (zfinal < 281 && isSent == 'n'){
  isSent = 'y';
  OrientationUpdate();
    
  }else if (zfinal > 380 && isSent == 'y'){
    isSent = 'n';
  }
  if (yfinal < 130 || yfinal > 520 || xfinal < 110 || xfinal > 420){
   ImpactUpdate();
  }
delay(50);
}




void jsonifyPeriodicUpdates(){
   StaticJsonBuffer<200> jsonBuffer;
   JsonObject& root = jsonBuffer.createObject();

   root["deviceid"] = deviceid;
   root["datano"] = datano;
   root["humidity"] = humidity;
   root["latitude"] = latitude;
   root["longitude"] = longitude;
   root["temperature"] = temperature;

char jsonChar[100];
   root.printTo(jsonChar);
   Serial.println(jsonChar);
}


void ImpactUpdate(){
   digitalWrite(BLUE, HIGH);
    Serial.print(" Impact ALERT: ");
    GPSLoop();
     DHT11Loop();
     
    Serial.println(temperature);
    Serial.println(humidity);
  Serial.print(latitude,5);
Serial.print(", ");
Serial.println(longitude,5);
    Serial.println(yfinal);
    digitalWrite(BLUE, LOW);
    datano = datano + 1;
    Serial.println("____________________________");
}

void OrientationUpdate(){
   
  digitalWrite(BLUE, HIGH);
    Serial.println("Orientation ALERT: SENT!");
    GPSLoop();
    DHT11Loop();
    Serial.println(temperature);
    Serial.println(humidity);
  Serial.print(latitude,5);
  Serial.print(", ");
Serial.println(longitude,5);
    Serial.println(zfinal);
    digitalWrite(BLUE, LOW);
    //datano = datano + 1;
      Serial.println("____________________________");
}

void PeriodicUpdate(){
 if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
  {
  lastRefreshTime += REFRESH_INTERVAL;
  digitalWrite(BLUE, HIGH);
  GPSLoop();
  DHT11Loop();

  //Serial.print("HELLO");
  jsonifyPeriodicUpdates();

 
datano = datano + 1;
digitalWrite(BLUE, LOW);

  }
  
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
    Serial.print("Read DHT11 failed");
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




