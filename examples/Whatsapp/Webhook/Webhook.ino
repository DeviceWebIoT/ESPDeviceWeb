#include <DeviceWeb.h>

DeviceWebConfig config;

void setup()
{
  Serial.begin(115200);

  // Configuração de depuração
  config.setDebug(true);

  // Conexão WiFi
  config.setWifi("SSID", "PASSWORD");

  // Parâmetros:
  // - token: Token disponivel na plataforma
  // - webhook: true ativa o webhook
  config.enableWhatsapp("TOKEN_WHATSAPP", true);
}

void loop()
{
  config.loop(); // mantém conexão e processa mensagens

  String resposta;
  while (config.dequeueWhatsappResponse(resposta))
  {
    Serial.println("[LOOP] Resposta WhatsApp recebida da fila:");
    Serial.println(resposta);

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, resposta);

    if (error)
    {
      if (config.debug)
        Serial.println("[Webhook JSON] Erro ao analisar JSON.");
      return;
    }

    Serial.print("Nome -> ");
    Serial.println(doc["nome"].as<String>());
    Serial.print("Numero -> ");
    Serial.println(doc["numero"].as<String>());
    Serial.print("Mensagem -> ");
    Serial.println(doc["mensagem"].as<String>());
  }

  delay(1000);
}
