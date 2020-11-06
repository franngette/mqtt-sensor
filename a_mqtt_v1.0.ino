#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include "DHT.h"
#define DHTPIN 3
#define DHTTYPE DHT22
#include "EmonLib.h"

IPAddress ip(192, 168, 2, 80);
IPAddress gateway(192, 168, 2, 1); 
IPAddress subnet(255, 255, 255, 0);
IPAddress server(0, 0, 0, 0); //server IP
IPAddress myDns(8, 8, 8, 8);

EnergyMonitor emon1;                 
DHT dht(DHTPIN, DHTTYPE);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

void reconnect() {
  while (!mqttClient.connected()) {
      Serial.print("Connecting to MQTT broker ...");
      if (mqttClient.connect("//id", "//username", "//password")) {
        Serial.println("OK");
        } 
      else {
        Serial.print("Error : ");
        Serial.print(mqttClient.state());
        Serial.println(" Wait 5 secondes before to retry");
        delay(5000);
      }
     }
    }

void setup()
{
  emon1.current(0, 10);  // Current: input pin, calibration.
  dht.begin();
  Serial.begin(9600);
  Ethernet.begin(mac, ip);              
  mqttClient.setServer(server, 1883);   
      if (mqttClient.connect("//id", "//username", "//password")) {
  {
    Serial.println("Conected to BrokerMQTT");
  } 
  else 
  {
    Serial.println("Connection failed...");
    reconnect();
  }
}

void loop(){
  
  float Irms = emon1.calcIrms(1480);  // Calculate Irms only
  if (Irms <= 0.05){
    Irms = 0;
  }
  float power = Irms*220.0;
  Serial.println(Irms);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
   if (mqttClient.connect("//id", "//username", "//password")) {

  char buffer[10];
  dtostrf(t,0, 2, buffer);
  if(mqttClient.publish("//topic1", buffer)){
    Serial.println("Publish message T success");
  }
  else{
  Serial.println("Could not send message :(");
  reconnect();
  }
  delay(4000);
  
  char buffer2[10];
  dtostrf(h,0, 2, buffer2);
  if(mqttClient.publish("//topic2", buffer2))
  {
    Serial.println("Publish message H success");
  }
  else {
  Serial.println("Could not send message :(");
  reconnect();
  }
  
  delay(4000);
  char buffer3[10];
  dtostrf(power,0, 2, buffer3);
  if(mqttClient.publish("//topic3", buffer3)){
    Serial.println("Publish message P success");
  }
  else {
  Serial.println("Could not send message");
  reconnect();
  }
  delay(600000);
}
