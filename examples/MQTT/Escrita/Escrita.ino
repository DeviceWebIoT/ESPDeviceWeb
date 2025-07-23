#include <DeviceWeb.h>

DeviceWebConfig config;

// Variáveis registradas
DeviceWebVar comandoLED("led1", "leitura", &config); // recebe do broker

void setup()
{
  Serial.begin(115200);

  config.setDebug(true);

  // Conexão WiFi
  config.setWifi("SSID", "PASSWORD");

  // Configuração MQTT
  config.setMqtt(
      "mqtt01.dadosrapido.com.br",           // servidor MQTT
      "contato@deviceweb.com.br/eT8A0C4eXo", // usuário MQTT
      "9RCssehdGS3n0tCGF3ev",                // senha MQTT
      "esp32cliente1",                       // client ID
      "eT8A0C4eXo/subscribe",                // tópico de leitura (entrada)
      "eT8A0C4eXo/publish",                  // tópico de escrita (saída)
      60000                                  // tempo para publicar no broker em ms
  );
}

void loop()
{
  config.loop(); // mantém conexão e processa mensagens

  // Exibe valor da variável recebida via MQTT
  if (comandoLED.getValue() != "")
  {
    Serial.println("[MQTT] Comando LED: " + comandoLED.getValue());
  }

  delay(10000);
}
