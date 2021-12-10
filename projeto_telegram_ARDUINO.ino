#include <ArduinoJson.h>
#include <SoftwareSerial.h>
SoftwareSerial esp8266(4,3);  /* pino 4 TX , pino 3 RX */

// Pinos
int pinoSirene = 2;
int pinoLuminosidade = A1;
int pinoGas = A0;

// Variáveis
float luminosidade;
float gas;
String dadosLido;

void setup(){
  Serial.begin(115200);
  esp8266.begin(115200);
  pinMode(pinoSirene, OUTPUT);
  pinMode(pinoLuminosidade, INPUT);
  pinMode(pinoGas, INPUT);
}
void loop(){
  lendoSensores();
  acoes();
  recebe();
}

void lendoSensores(){
  luminosidade = analogRead(pinoLuminosidade);
  gas = analogRead(pinoGas);  
}

void acoes(){
  if(luminosidade < 900) {
    digitalWrite(pinoSirene, LOW);
    delay(1*1000);
    
  }else{
    envia("sensor", "luminosidade", "alarme");
    digitalWrite(pinoSirene, HIGH);
  }
  
  if(gas > 300) {
    envia("sensor", "gas", "alarme");
    digitalWrite(pinoSirene, LOW);
    delay(0.5 * 1000);
    digitalWrite(pinoSirene, HIGH);
    delay(0.5 * 1000);
    
  }else{
    envia("sensor", "gas", "alarme");
    digitalWrite(pinoSirene, HIGH);
  }
}

void envia(String tipo, String dados1, String dados2){
  DynamicJsonDocument dado(1024);
  dado["tipo"] = tipo;
  dado["dados1"] = dados1;
  dado["dados2"] = dados2;
  serializeJson(dado, esp8266);
}

void recebe(){
  if(esp8266.available())
  {
    String comando="";
    while(esp8266.available()) {
      dadosLido = esp8266.readString();
    }

    DynamicJsonDocument dado(1024);
    DeserializationError error = deserializeJson(dado, dadosLido);
    if (error)
      return;

    String dados = dado["tipo"];
    if(comando.indexOf(dados)){
      envia("status", String(luminosidade), String(gas));
    }
  }
}
