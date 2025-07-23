#include <ArduinoJson.h>
#include <DeviceWeb.h>

DeviceWebConfig config;

// ESCRITA = ENVIAR DADOS PARA O SERVIDOR
DeviceWebVar temperatura("temp1", "escrita", &config);
DeviceWebVar umidade("umid1", "escrita", &config);


void setup() {
  Serial.begin(115200);

  // Configura o DeviceWeb
  config.setDebug(true);

  // Configura WiFi
  config.setWifi("SSID", "PASSWORD");
  config.connectWifi();

  // Define o protocolo HTTPS com token e método
  config.setHttps("TOKEN_AQUI", "POST");

  // Atribui valores iniciais
  temperatura.setValue("23.7");
  umidade.setValue("45.2");
}

void loop() {

  // ENVIAR DADOS VIA HTTPS
  temperatura.setValue(String(random(20, 30)));
  umidade.setValue(String(random(40, 60)));


  config.loop();
  // Aguarda 10 segundos antes de novo envio
  delay(10000);
}
