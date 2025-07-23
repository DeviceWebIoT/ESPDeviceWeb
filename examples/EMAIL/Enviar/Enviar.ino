#include <DeviceWeb.h>

DeviceWebConfig config;


void setup() {
  Serial.begin(115200);

  // Configuração de depuração
  config.setDebug(true);

  // Conexão WiFi
  config.setWifi("SSID", "PASSWORD");


  // TOKEN EMAIL
  config.enableEmail("TOKEN_EMAIL");
}

void loop() {
  config.loop();  // mantém conexão e processa mensagens


  // Envia uma mensagem via WhatsApp para um número específico.
  // Parâmetros:
  // - destino: "email@email.com"
  // - assunto: assunto do email
  // - mensagem: mensagem a ser enviada
  config.sendEmail("email@email.com", "Atencao", "Alerta: Temperatura acima do normal detectada!");


  delay(30000);
}
