#include <DeviceWeb.h>

DeviceWebConfig config;


void setup() {
  Serial.begin(115200);

  config.setDebug(true);

  // Conexão WiFi
  config.setWifi("SSID", "PASSWORD");


  // TOKEN SMS
  config.enableSms("TOKEN_SMS");
}

void loop() {
  config.loop();  // mantém conexão e processa mensagens


  // Envia uma mensagem via SMS para um número específico.
  // Parâmetros:
  // - número: "5515999999999" (formato internacional com DDI) 
  // - mensagem: alerta  informando alta temperatura
  config.sendSms("5515999999999", "Alerta: Temperatura acima do normal detectada!");


  delay(10000);
}
