#include "include.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char *ssid = SECRET_SSID;
const char *pass = SECRET_PASS;

const char broker[] = "192.168.1.112";
int        port     = 1883;

const char topicUmd[]  = "/Umidade";
const char topicTmp[]  = "/Temperatura";
const char topicVlc[]  = "/Velocidade";

const long intervalo = 5000;

int count = 0;

#define TX_PIN 48
#define RX_PIN 47
#define BR_485 9600
#define CFG485 SWSERIAL_8N1

SoftwareSerial RS485(RX_PIN,TX_PIN);

THConf thConfi({.id = 2, .AddTemp = 0x01, .AddUmid = 0x00, .intervalo = 5000});
AnConf anConfi({.id = 3, .Add = 0x00, .intervalo = 5000});

Termohigrometro TH(&thConfi,&RS485);
Anemometro AN(&anConfi,&RS485);


void setup() {
  
  Serial.begin(115200);
  RS485.begin(BR_485,CFG485);

  // while(!Serial);
  vTaskDelay(1000);
  
  Serial.printf("Tentando conectar ao WPA SSID: %s\n",SECRET_SSID);
  int stt = WiFi.begin(SECRET_SSID, SECRET_PASS);
  while(stt != WL_CONNECTED){
    Serial.printf("Status: %d\n",stt);
    stt = WiFi.status();
    delay(1000);
  }

  Serial.println("Conectado na rede Wifi!");
  Serial.print("Conectando no Broker...");
  
  if(!mqttClient.connect(broker, port)) {
    Serial.printf("Falha ao conectar no Broker MQTT! Erro: %d",mqttClient.connectError());
    while (1);
  }

  TH.start();
  AN.start();

  xTaskCreatePinnedToCore([](void*){

    mqttClient.poll();
    TickType_t ini;
    for(;;){
      ini = xTaskGetTickCount();

      char msg[50];

      snprintf(msg,50,"{\"IdSensor\":2,\"Umidade\":%2f}",TH.getUmidade());
      //Serial.println(msg);
      mqttClient.beginMessage(topicUmd);
      mqttClient.print(msg);
      mqttClient.endMessage();

      snprintf(msg,50,"{\"IdSensor\":1,\"Temperatura\":%2f}",TH.getTemperatura());
      //Serial.println(msg);
      mqttClient.beginMessage(topicTmp);
      mqttClient.print(msg);
      mqttClient.endMessage();

      snprintf(msg,50,"{\"IdSensor\":3,\"Velocidade\":%2f}",AN.getVelocidade());
      //Serial.println(msg);
      mqttClient.beginMessage(topicVlc);
      mqttClient.print(msg);
      mqttClient.endMessage();

      vTaskDelayUntil(&ini,intervalo);
    }

  },"Task_Broker",10000,NULL,1,NULL,1);

}

void loop(){}





