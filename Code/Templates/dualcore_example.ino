#include <WiFi.h>
#include <ThingSpeak.h>

#define pinIn 33

TaskHandle_t Task1, Task2;
//To give credentials to wificlient
const char* ssid = "############";
const char* password = "############";
//Initiation of global variables
int cont = 0;
//ThingSpeak Credentials
unsigned long channelID = 1356484;
const char* WriteAPIKey = "############";
//Initiation of wificlient
WiFiClient cliente;
void codeForTask1( void * parameter )
{
  unsigned long time1=0, time2=0, periodo = 0;
  long wait = 100;
  for (;;) {
    periodo = time1 - time2;
    if (periodo >=15000){
      ThingSpeak.setField(1,cont);
      delay(wait);
      ThingSpeak.writeFields(channelID,WriteAPIKey);
      Serial.println("PUBLISHED TO CLOUD!!!!!!!!!!! Time:");
      Serial.println(periodo);
      time2 = millis();
    }
    time1 = millis();
    delay(wait);
  }
}

void codeForTask2( void * parameter )
{
  int estado = 0,prev = 0;
  for (;;) {
    estado = digitalRead(pinIn);
    if(estado == LOW){
      cont++;
      while(estado == LOW){
        estado = digitalRead(pinIn);
      }
    }
    if (prev != cont){
      Serial.print("                                                 ");
      Serial.println(cont);
    }
    prev = cont;
    delay(10);
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  //For serial communication 
  Serial.begin(115200);
  //Set pin as INPUT
  pinMode(pinIn, INPUT);

  //Getting access to wifi communication
  Serial.print("Starting setup");
  WiFi.begin(ssid,password);
  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi conectado!");

  //TASK FOR CORE1
  xTaskCreatePinnedToCore(
    codeForTask1,
    "led1Task",
    10000,
    NULL,
    1,
    &Task1,
    0);
  delay(500);  // needed to start-up task1

  //TASK FOR CORE1
  xTaskCreatePinnedToCore(
    codeForTask2,
    "led2Task",
    1000,
    NULL,
    1,
    &Task2,
    1);

  //Initiating ThingSpeak
  ThingSpeak.begin(cliente); 
}

void loop() {
  delay(1000);
}
