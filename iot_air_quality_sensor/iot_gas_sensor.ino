#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <SimpleTimer.h>
#define CHAT_ID "Your chat ID"
float  minimo = 2.6;
float  maximo = 3.5;
int verde = 25;
int amarillo = 26;
int rojo = 27;
int flag = 0;
int acu = 0;
int horas = 1; // horas cada cuanto reiniciamos por seguridad
unsigned long previousMillis = 0;
const long interval = 1500;
SimpleTimer timer;

// Initialize Wifi connection to the router
char ssid[] = "Your Wifi Name";     // your network SSID (name)
char password[] = "Ypour Wifi Pass"; // your network key

// Initialize Telegram BOT
#define BOTtoken "Your Bot Token"  // your Bot Token (Get from Botfather)
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

void setup() {
  Serial.begin(115200);
  timer.setInterval(horas * 3600000, reinicio);
  pinMode(verde, OUTPUT);
  pinMode(rojo, OUTPUT);
  pinMode(amarillo, OUTPUT);
  digitalWrite(rojo, LOW);
  digitalWrite(verde, LOW);
  digitalWrite(amarillo, LOW);
}

void loop() {
  timer.run();
  int adc_MQ = analogRead(A0); //Lemos la salida analógica del MQ
  float voltaje = adc_MQ * (5.0 / 1023.0); //Convertimos la lectura en un valor de voltaje

  Serial.print("adc:");
  Serial.print(adc_MQ);
  Serial.print("    voltaje:");
  Serial.println(voltaje);

  if (voltaje <= minimo) {
    flag = 0;
    Serial.println("Bueno");
    digitalWrite(rojo, LOW);
    digitalWrite(amarillo, LOW);
    digitalWrite(verde, HIGH);
    delay(200);
    digitalWrite(verde, LOW);
    delay(200);
  }
  else if (  voltaje > minimo && voltaje < maximo ) {
    Serial.println("Regular");
    digitalWrite(rojo, LOW);
    digitalWrite(verde, LOW);
    digitalWrite(amarillo, HIGH);
    delay(200);
    digitalWrite(amarillo, LOW);
    delay(200);
  }
  else if ( voltaje > maximo) {
    Serial.println("Malo");
    digitalWrite(rojo, HIGH);
    digitalWrite(amarillo, LOW);
    digitalWrite(verde, LOW);
    if (flag == 0) {
      alerta();
    }
  }
  delay(100);
}

void alerta() {
  flag = 1;
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    acu++;
    delay(500);
    if (acu == 30) {
      ESP.restart();
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  String message = "Gases detectados";
  if (bot.sendMessage(CHAT_ID, message, "Markdown")) {
    Serial.println("TELEGRAM Successfully sent");
    WiFi.mode(WIFI_OFF); //apagar wifi par ahorrar bateria
  }
}
void reinicio() {
  ESP.restart();
}
