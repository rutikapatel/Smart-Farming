
#include "ESP8266WiFi.h"
#include "DHT.h"
#define DHTPIN 2    // what digital pin we're connected to
                    //pin2 to D4 on esp board
#define DHTTYPE DHT11  // DHT 11
#define w1 16 // full
#define w2 14 //half
#define w3 12 //25%
#define soilSensor A0

DHT dht(DHTPIN,DHTTYPE);
int v0,v1,v2,v3,v4;

//const char WEBSITE[] = "api.pushingbox.com"; //pushingbox API server
//const String devid = "vBD94A164D9C6424"; //device ID from Pushingbox 
char auth[] = "8f27a470e550435db24977f47078d4bf";
const char* MY_SSID = "ITI";
const char* MY_PWD =  "1234567000";
// Blynk cloud server
const char* host = "blynk-cloud.com";
unsigned int port = 8080;
WiFiClient client;
bool httpRequest(const String& method,
                 const String& request,
                 String&       response)
{
  Serial.print(F("Connecting to "));
  Serial.print(host);
  Serial.print(":");
  Serial.print(port);
  Serial.print("... ");
  if (client.connect(host, port)) {
    Serial.println("OK");
  } else {
    Serial.println("failed");
    return false;
  }

  client.print(method); client.println(F(" HTTP/1.1"));
  client.print(F("Host: ")); client.println(host);
  client.println(F("Connection: close"));
  if (request.length()) {
    client.println(F("Content-Type: application/json"));
    client.print(F("Content-Length: ")); client.println(request.length());
    client.println();
    client.print(request);
  } else {
    client.println();
  }

  //Serial.println("Waiting response");
  int timeout = millis() + 5000;
  while (client.available() == 0) {
    if (timeout - millis() < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return false;
    }
  }

  //Serial.println("Reading response");
  int contentLength = -1;
  while (client.available()) {
    String line = client.readStringUntil('\n');
    line.trim();
    line.toLowerCase();
    if (line.startsWith("content-length:")) {
      contentLength = line.substring(line.lastIndexOf(':') + 1).toInt();
    } else if (line.length() == 0) {
      break;
    }
  }

  //Serial.println("Reading response body");
  response = "";
  response.reserve(contentLength + 1);
  while (response.length() < contentLength && client.connected()) {
    while (client.available()) {
      char c = client.read();
      response += c;
    }
  }
  client.stop();
  return true;
}


void setup()
{
  Serial.begin(9600);
  dht.begin();
  Serial.print("Connecting to "+*MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  Serial.println("going into wl connect");
  pinMode(w1,INPUT);
  pinMode(w2,INPUT);
  pinMode(w3,INPUT);
  while (WiFi.status() != WL_CONNECTED) //not connected,..waiting to connect
    {
      delay(1000);
      Serial.print(".");
    }
  Serial.println("wl connected");
  Serial.println("");
  Serial.println("Credentials accepted! Connected to wifi\n ");
  Serial.println("");
  Serial.print("123456");
}


void loop()
{ String response;
  //Serial.println("Reading value");

  if (httpRequest(String("GET /") + auth + "/get/V2", "", response)) {
   // Serial.print("Value from server: ");
  // if(response == "[""1111""]"){
  if(response.indexOf("1111") > 0){
        v2 = 1;
   }
   //if(response == "[""1212""]"){
   if(response.indexOf("1212") > 0){
        v2 = 0;
   }
    //Serial.println(response);
  }

  
  //Wait between measurements longer then normal to slow donwn
  //google sheet populate, as We dont want to exceed free service quota
  delay(5000); //10 seconds, (sampling rate vs. service call quota)

  float humidityData = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float celData = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float fehrData = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidityData) || isnan(celData) || isnan(fehrData))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hicData = dht.computeHeatIndex(celData, humidityData, false);
  // Compute heat index in Fahrenheit (the default)
  float hifData = dht.computeHeatIndex(fehrData, humidityData);
  v0=humidityData;
  v1=celData;
  int s = analogRead(soilSensor);
  v3 = map(s,1023,350,0,100);
  if(digitalRead(w1)==LOW){
    v4=100;
  }
  else if(digitalRead(w2)==LOW){
    v4=50;
  }
  else if(digitalRead(w3)==LOW){
    v4=25;
  }
  else{
    v4=0;
  }
  //v4=hifData;
  Serial.print('Z');delay(10);
  Serial.print((char)v0); 
  Serial.print((char)v1); 
  Serial.print((char)v2);
  Serial.print((char)v3);
  Serial.print((char)v4);
  String putData = String("[\"") + v1 + "\"]";
  if (httpRequest(String("PUT /") + auth + "/update/V1", putData, response)) {
    if (response.length() != 0) {
      //Serial.print("WARNING: ");
      //Serial.println(response);
    }
  }
  putData = String("[\"") + v0 + "\"]";
  if (httpRequest(String("PUT /") + auth + "/update/V0", putData, response)) {
    if (response.length() != 0) {
    }
  }
  putData = String("[\"") + v3 + "\"]";
  if (httpRequest(String("PUT /") + auth + "/update/V3", putData, response)) {
    if (response.length() != 0) {
    }
  }
  putData = String("[\"") + v4 + "\"]";
  if (httpRequest(String("PUT /") + auth + "/update/V4", putData, response)) {
    if (response.length() != 0) {
    }
  }
  //Print to Serial monitor or Terminal of your chocice at 115200 Baud
  /*Serial.print("Humidity: ");
  Serial.print(humidityData);
  Serial.print(" %\t");
  Serial.print("Temperature in Cel: ");
  Serial.print(celData);
  Serial.print(" *C ");
  Serial.print("Temperature in Fehr: ");
  Serial.print(fehrData);
  Serial.print(" *F\t");
  Serial.print("Heat index in Cel: ");
  Serial.print(hicData);
  Serial.print(" *C ");
  Serial.print("Heat index in Fehr: ");
  Serial.print(hifData);
  Serial.print(" *F\n");
  */  
 /* WiFiClient client;  //Instantiate WiFi object

    //Start or API service using our WiFi Client through PushingBox
    if (client.connect(WEBSITE, 80))
      { 
         client.print("GET /pushingbox?devid=" + devid
       + "&humidityData=" + (String) humidityData
       + "&celData="      + (String) celData
       + "&fehrData="     + (String) fehrData
       + "&hicData="      + (String) hicData
       + "&hifData="      + (String) hifData
         );

      client.println(" HTTP/1.1"); 
      client.print("Host: ");
      client.println(WEBSITE);
      client.println("User-Agent: ESP8266/1.0");
      client.println("Connection: close");
      client.println();
      }*/
}
