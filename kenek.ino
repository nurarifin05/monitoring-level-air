#include <ESP8266WiFi.h>
#include <Ultrasonic.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <DHT_U.h>
#include <DHT.h>
#include "DHT.h"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4); 

#define ssid "SUDO" 
#define password "saibudin" 
#define mqtt_server "202.154.63.168"
#define mqtt_username "skripsi"
#define mqtt_password "iot"
#define clientID "espClient"
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht (DHTPIN, DHTTYPE);

WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient);
long lastMsg = 100;
long duration;
int jarakAirdanSensor;
long tinggiBak;
float ketinggianAir;
long Volume;
#define pompa 14
#define echoPin D7
#define trigPin D6


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient_distance_sensor")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
void setup()
{
  Serial.begin(115200);
  setup_wifi(); 
  client.setServer(mqtt_server, 1883);  
  Serial.println(F("DHTxx test!"));
  dht.begin();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pompa, OUTPUT);
  lcd.init();
  lcd.backlight();
  tinggiBak=20; //menentukan tinggi dari tandon dalam satuan cm
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);


jarakAirdanSensor = pulseIn(echoPin, HIGH);
jarakAirdanSensor= jarakAirdanSensor*0.034/2;

Serial.print("jarak air ke sensor: ");
Serial.println(jarakAirdanSensor);
delay(100);

ketinggianAir = tinggiBak-jarakAirdanSensor;
Serial.print("Tinggi air: ");
Serial.println(ketinggianAir);
delay(100);

Volume = 3.14*49*ketinggianAir;
Serial.print("Volume Air: ");
Serial.println(Volume);
delay(100);

if(ketinggianAir < 7){
  digitalWrite(pompa, LOW);
  delay(100);
}else{
  digitalWrite(pompa, HIGH);
  delay(100);
}

long t = dht.readTemperature();
long h = dht.readHumidity();


client.publish("fathur/jarakair", String(jarakAirdanSensor).c_str(), true); //menampilkan jarak air dan sensor pada dashboard
client.publish("fathur/tinggiair", String(ketinggianAir).c_str(), true);  //menampilkan tinggi air pada dashboard
client.publish("fathur/volume", String(Volume).c_str(), true);  //menampilkan volume air pada dashboard
client.publish("fathur/temperature", String(t).c_str(), true);  //menampilkan suhu pada dashboard
client.publish("fathur/humidity", String(h).c_str(), true);     //menampilkan kelembapan air pada dashboard

  

  lcd.setCursor(0,0);
   lcd.print("TinggiAir="); //menampilkan tinggi air pada lcd
  lcd.print(ketinggianAir);
  lcd.setCursor(14,0);
  lcd.print("cm");
  delay(1000);
  
  lcd.setCursor(0,1);
  lcd.print("Suhu= ");    //menampilkan suhu pada lcd
  lcd.print(t);
   lcd.setCursor(9,1);
   lcd.print("Celcius");

  delay(1000);

  long now = millis();
  if (now - lastMsg > 100) {
    lastMsg = now;
  
    
  }
}
