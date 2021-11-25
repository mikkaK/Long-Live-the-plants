#include <WiFi.h>
#include <PubSubClient.h>

#define PIN_RED    23 // GIOP23
#define PIN_GREEN  22 // GIOP22
#define PIN_BLUE   21 // GIOP21

const char* ssid = "GuestWLANPortal";
const char* mqtt_server = "142.93.174.193";
const int sensor_pin = 36;  /* Connect Soil moisture analog sensor pin to A0 of NodeMCU */

const char* topic1 = "zh/team6/lltp/moisture/in";
const char* topic2 = "zh/team6/lltp/moisture/out";
const char* topic3 = "zh/team6/lltp/water/in";


WiFiClient espClient;
PubSubClient client(espClient);
void setup_wifi() {
Serial.print("Connecting to ");
Serial.print(ssid); WiFi.begin(ssid);
while(WiFi.status() != WL_CONNECTED) {
  delay(500); 
  Serial.print(".");
  }
  Serial.println("done!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); 
  }

void setup() {
  Serial.begin(9600); /* Define baud rate for serial communication */
  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(PIN_RED,   OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE,  OUTPUT);

}


  void callback(char* topic, byte* payload, unsigned int length) {
    if(strcmp(topic, topic2) == 0) {
      char message[length];
      for(int i = 0; i < length; i++) {
        message[i] = (char)payload[i]; 
        } Serial.print("Received Message: ");
        Serial.println(message); 
        }
          }
void reconnect() {
  Serial.print("Attempting MQTT connection...");
  while(!client.connected()) {
    if(client.connect("ESP32")) {
      Serial.println("done!");
      client.subscribe(topic1);
      client.subscribe(topic2);
      } 
      else
      { delay(500);
      Serial.print("."); 
      }
     }
    }
void loop() {
  if(!client.connected()) {
    reconnect(); 
    }
 double moisture_percentage;
 double initial;


  //moisture_percentage = ( 100.00 - ( (analogRead(sensor_pin)/1023.00) * 100.00 ) );
initial = ((analogRead(sensor_pin) * 100)/4095);
  moisture_percentage= (100-initial);
  Serial.print("Soil Moisture(in Percentage) = ");
  Serial.print(moisture_percentage);
  Serial.println("%");
  
double dry = 5;
double almostdry = 15;
double moist = 30;
double prettymoist = 45;
double wet = 55;
double reallywet= 100;
int water = 0;


  if (moisture_percentage < dry){
        analogWrite(PIN_RED, 255);
  analogWrite(PIN_GREEN, 0);
  analogWrite(PIN_BLUE, 0);
  delay(100);
      char WaterBuffer[] = "yes";
     client.publish(topic3, WaterBuffer);
  }
  
  else if (moisture_percentage > dry && moisture_percentage < almostdry){
         analogWrite(PIN_RED, 192);
  analogWrite(PIN_GREEN, 63);
  analogWrite(PIN_BLUE, 0);  
  delay(100);
    char WaterBuffer[] = "yes";
     client.publish(topic3, WaterBuffer);
  }
  else if (moisture_percentage > almostdry && moisture_percentage < moist){
         analogWrite(PIN_RED, 129);
  analogWrite(PIN_GREEN, 126);
  analogWrite(PIN_BLUE, 0);  
  delay(100); 
  water = 0;
  }
  else if (moisture_percentage > moist && moisture_percentage < prettymoist){
         analogWrite(PIN_RED, 66);
  analogWrite(PIN_GREEN, 159);
  analogWrite(PIN_BLUE, 0);  
  delay(100); 
  water = 0;
  }
  else if (moisture_percentage > prettymoist && moisture_percentage < wet){
         analogWrite(PIN_RED, 66);
  analogWrite(PIN_GREEN, 159);
  analogWrite(PIN_BLUE, 0);
  delay(100);
  }
  else if (moisture_percentage > wet){
    analogWrite(PIN_RED, 0);
  analogWrite(PIN_GREEN, 0);
  analogWrite(PIN_BLUE, 255);
    delay(100);
    water = 0;
  }
  char MoistBuffer[1000];
  sprintf(MoistBuffer, "%f", moisture_percentage);
  client.publish(topic1, MoistBuffer);
  
 

  delay(200);
  client.loop();
}
