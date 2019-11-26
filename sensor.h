#include <ESP8266WiFi.h>
#include <PubSubClient.h>

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "twguest";        // your network SSID (name)
char pass[] = "LULZ";    // your network password (use for WPA, or use as key for WEP)
const char* mqtt_server = "broker.mqttdashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);

int sensor = 5;  // Digital pin D1
int led = 14; // d5 esp 8266 ex
bool motionDetected = false;
int lastMotion = 0;

void setup() {

  pinMode(sensor, INPUT);
   pinMode(led, OUTPUT);
  Serial.begin(115200);

  connectToWifi();  
  randomSeed(micros());

  configureMqtt();
}

void connectToWifi() {
  // Connecting the WIFI
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to..");

  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }

  Serial.print("Connected. Ip Address ");
  Serial.println(WiFi.localIP());
}

void configureMqtt() {
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //client.publish("OccupyMeetingRoom", "hello");
  
  bool currentlyInMotion = sensorDetectsMotion();

  if (currentlyInMotion) {
    Serial.println("Motion detected!");
    digitalWrite(led, HIGH);
    lastMotion = 0;
  } else {
    Serial.println("No motion detected.");
    digitalWrite(led, LOW);

  }
  motionDetected = currentlyInMotion;
  delay(1000);
}

bool sensorDetectsMotion() {
  long state = digitalRead(sensor);
  if (state == HIGH) {
    Serial.println("Motion detected!");
    return true;
  }
  else {
    Serial.println("No motion detected.");
    return false;
  }
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("OccupyMeetingRoom", "Connected again");
//      // ... and resubscribe
//      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
