#include <DeviceWeb.h>

DeviceWebConfig config;


void setup() {
  Serial.begin(115200);

  // Configuração de depuração
  config.setDebug(true);

  // Conexão WiFi
  config.setWifi("SSID", "PASSWORD");


  // TOKEN 
  config.enableWhatsapp("TOKEN_DO_WHATSAPP");
}

void loop() {
  config.loop();  // mantém conexão e processa mensagens


  // Envia uma mensagem via WhatsApp para um número específico.
  // Parâmetros:
  // - numero: "5515999999999" (número de telefone com DDI e DDD)
  // - mensagem: mensagem a ser enviada
  // - grupo: false (opcional, se for um grupo, defina como true)
  config.sendWhatsapp("5515999999999", "Alerta: Temperatura acima do normal detectada!", false);



  delay(30000);
}
