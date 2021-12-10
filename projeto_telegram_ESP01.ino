#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <UniversalTelegramBot.h>

// Configuração da REDE WIFI
#define WIFI_SSID "nome"
#define WIFI_PASSWORD "senha"

// Telegram BOT Token (Botfather)
#define BOT_TOKEN "xxxxxxxxxx"

const unsigned long BOT_MTBS = 1000;
  
// Você Telegram
#define CHAT_ID "1404485001"

// Variáveis
String dadosLido;

SoftwareSerial linkSerial(3, 1); // RX, TX

WiFiClientSecure secured_client;
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;

void handleNewMessages(int numNewMessages)
{
  for (int i = 0; i < numNewMessages; i++)
  {
    if (bot.messages[i].text == "STATUS")
    {
      lerStatus();
    }
  }
}

void setup() {
  Serial.begin(115200);
  linkSerial.begin(115200);
  secured_client.setTrustAnchors(&cert);
  Serial.print("Conectando com o WIFI ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi conectado. IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  lerDados();
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}

void enviarTelegram(String texto){
  bot.sendMessage(CHAT_ID, texto, "");
}

void lerDados() {
  bool Sr =false;
  if(linkSerial.available()){
    dadosLido = linkSerial.readString();
    Sr=true;
  }

  DynamicJsonDocument dado(512);
  DeserializationError error = deserializeJson(dado, dadosLido);
  if (error)
    return;

  String tipo = dado["tipo"];
  String dados1 = dado["dados1"];
  String dados2 = dado["dados2"];
  if(Sr==true){
    if(tipo == "sensor"){
      enviarTelegram(dados1+" -> "+dados2);
      
    }if(tipo == "status"){
      if(dados2.toInt() > 300) {
        enviarTelegram("Gas Ativado");
      }else{
        enviarTelegram("Gas Desativado");
      }
      if(dados1.toInt() < 900) {
        enviarTelegram("Luminosidade Ativada");
      }else{
        enviarTelegram("Luminosidade Desativada");
      }
    }
  }
}

// Função verificar status
void lerStatus(){
  DynamicJsonDocument dadoLeitura(256);
  dadoLeitura["tipo"] = "status";
  serializeJson(dadoLeitura, linkSerial);
}
