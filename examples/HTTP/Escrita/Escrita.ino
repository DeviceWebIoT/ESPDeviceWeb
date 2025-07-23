#include <ArduinoJson.h>
#include <DeviceWeb.h>

DeviceWebConfig config;

//LEITURA = RECEBER DADOS DO SERVIDOR
DeviceWebVar ledStatus("led1", "leitura", &config);

void setup() {
  Serial.begin(115200);

  config.setDebug(true);

  // Configura WiFi
  config.setWifi("SSID", "PASSWORD");
  config.connectWifi();

  // Define o protocolo HTTPS com token e método
  config.setHttps("TOKEN_AQUI", "POST");


}

void loop() {

  
  Serial.print(" LED STATUS -> ");
  Serial.println(ledStatus.getValue());

  config.loop();
  // Aguarda 10 segundos antes de novo envio
  delay(10000);
}
