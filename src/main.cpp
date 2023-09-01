#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>
#include <ArduinoJson.h>
// #include <ESPAsyncTCP.h>
// #include <ESPAsyncWebServer.h>
// #include <AsyncElegantOTA.h>

const char* ssid = "TPLink";
const char* pass = "qwertyuio";
const char* mqtt_server = "192.168.0.150";
const uint16_t mqtt_server_port = 1883; 
const char* mqttTopicIn = "/line1/dataToMCU";
// const int displayWidth = 1;
// const int displayHeight = 1;
const uint8_t *font = System5x7;
long lastRefreshTime;
char* SKU = "A";
char* oldSKU;
int partTat = 1;
long target = 10;
long oldTarget;
long paceTarget = 0;
long oldPaceTarget;
long paceActual = 0;
long oldpaceActual;
long paceDiff = 0;
long oldPaceDiff;
unsigned long timeStart = 0;
char* data;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

SPIDMD dmd(1,5);
DMD_TextBox box1(dmd,0,0);
DMD_TextBox box2(dmd,0,20,32,8);
DMD_TextBox box3(dmd,0,36,32,8);
DMD_TextBox box4(dmd,0,52,32,8);
DMD_TextBox box5(dmd,0,68,32,8);
// AsyncWebServer server(80);

String message;

StaticJsonDocument<96> doc;


void setupWiFi()
{
  delay(100);
  Serial.println("Connecting to WiFi");
  // box.clear();
  // box.print("WiFi");
  Serial.println(ssid);
  WiFi.begin(ssid,pass);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
    // box.print(".");
  }
  Serial.print("Connected to ");
  Serial.println(ssid);
  
  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(200, "text/plain", "Hi! This is a sample response.");
  // });

  // AsyncElegantOTA.begin(&server);    // Start AsyncElegantOTA
  // server.begin();
  // Serial.println("HTTP server started");
}

void connect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    String mqttClientId = "";
    if (mqttClient.connect(mqttClientId.c_str())) {
      Serial.println("connected");
      mqttClient.subscribe(mqttTopicIn);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      // Serial.println(" will try again in 0.5 seconds");
      // delay(500);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: '");
  Serial.print(topic);
  Serial.print("' with payload: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();



  //data Parsing from json below...........................................................................................
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  JsonObject root_0 = doc[0];
  const char* root_0_time = root_0["time"]; // "2023-08-27T13:14:38.000Z"
  const char* root_0_SKU = root_0["SKU"]; // "ABCDE12345"
  const char* root_0_TAT = root_0["TAT"]; // "5"
  int root_0_paceActual = root_0["paceActual"]; // 593
  int root_0_target = root_0["target"]; // "1000"
  partTat = atoi(root_0_TAT);
  strcpy(SKU,root_0_SKU);
  paceActual = root_0_paceActual;
  target = root_0_target;
  // String myCurrentTime = timeClient.getFormattedTime();
  // mqttClient.publish(mqttTopicOut,("ESP8266: Cedalo Mosquitto is awesome. ESP8266-Time: " + myCurrentTime).c_str());
  timeStart=millis();





  box1.clear();
  box1.print(SKU);
}

void setupMQTT()
{
  mqttClient.setServer(mqtt_server, mqtt_server_port);
  mqttClient.setCallback(callback);
}

void initDisplay()
{
  dmd.setBrightness(190);
  dmd.selectFont(font);
  dmd.begin();
  // box1.print("Hello");
  Serial.println("Hello, firmware v0.2");
  // box1.clear();
  // box1.print("WiFi....");
  // box1.print(ssid);
}

String getRetainedMQTTMessage()
{
  // client.callback();
  return("");
}

String extractData(String messageToParse)
{
  return("");
}

void updateDisplay(String dataToDisplay)
{

}

String mqttSubscription()
{
  return("");
}

ICACHE_RAM_ATTR void countISR()
{
  detachInterrupt(D1);
  delay(500);
  paceActual++;
  attachInterrupt(D1,countISR,RISING);
}

ICACHE_RAM_ATTR void reverseCountISR()
{
  detachInterrupt(D2);
  delay(500);
  paceActual--;
  attachInterrupt(D2,reverseCountISR,RISING);
}

void updateData()
{
  paceTarget = ((millis()-timeStart)/1000)/partTat;
  paceDiff = paceActual-paceTarget;
}

void displayData()
{
  // if(millis()>lastRefreshTime+1000)
  // {
    // if(oldSKU!=SKU)
    // {
    //   strcpy(oldSKU,SKU);
    // }
    if(oldTarget!=target)
    {
      box2.clear();
      box2.print(target);
      oldTarget=target;
    }
    if(oldPaceTarget!=paceTarget)
    {
      box3.clear();
      box3.print(paceTarget);
      oldPaceTarget=paceTarget;
    }
    if(oldpaceActual!=paceActual)
    {
      box4.clear();
      box4.print(paceActual);
      oldpaceActual=paceActual;
    }
    if(oldPaceDiff!=paceDiff)
    {
      box5.clear();
      box5.print(paceDiff);
      oldPaceDiff=paceDiff;
    }

  //   lastRefreshTime = millis();
  // }
}

void parseData()
{
  
}

void setup()
{
  Serial.begin(9600);
  setupWiFi();
  initDisplay();
  setupMQTT();
  parseData();
  pinMode(D1,INPUT);
  pinMode(D2,INPUT);
  attachInterrupt(digitalPinToInterrupt(D2), reverseCountISR, RISING);
  attachInterrupt(digitalPinToInterrupt(D1), countISR, RISING);
  timeStart = millis();
}

void loop()
{
  if (!mqttClient.connected()) {
    connect();
  }
  mqttClient.loop();
  updateData();
  displayData();
}