#include "DeviceWeb.h"

std::vector<DeviceWebVar *> DeviceWeb::_instancias;

DeviceWebConfig::DeviceWebConfig() : mqttClient(wifiClient) {}

void DeviceWebConfig::setDebug(bool ativo)
{
  debug = ativo;
  if (debug)
    Serial.println("[DeviceWeb] Debug ativado");
}

void DeviceWebConfig::setWifi(const String &ssid, const String &password)
{
  this->ssid = ssid;
  this->password = password;
  wifiEnabled = true;
  if (debug)
    Serial.printf("[WiFi] SSID configurado: %s\n", ssid.c_str());
}

void DeviceWebConfig::setHttps(const String &token, const String &metodo)
{
  this->tokenHttps = token;
  this->metodoHttps = metodo;
  protocolo = "HTTPS";
  if (debug)
  {
    Serial.printf("[HTTPS] Token configurado: %s\n", token.c_str());
    Serial.printf("[HTTPS] Método: %s\n", metodo.c_str());
  }
}

void DeviceWebConfig::setMqtt(const String &server, const String &user, const String &pass, const String &clientId, const String &topicRead, const String &topicWrite, const int &tempoAtualizacao)
{
  serverMqtt = server;
  userMqtt = user;
  passwordMqtt = pass;
  clientIdMqtt = clientId;
  topicReadMqtt = topicRead;
  topicWriteMqtt = topicWrite;
  protocolo = "MQTT";
  tempoAtualizacaoMqtt = tempoAtualizacao;
  mqttClient.setServer(serverMqtt.c_str(), 1883);
  if (debug)
  {
    Serial.printf("[MQTT] Servidor: %s\n", server.c_str());
    Serial.printf("[MQTT] Cliente ID: %s\n", clientId.c_str());
    Serial.printf("[MQTT] Topic Read: %s\n", topicRead.c_str());
    Serial.printf("[MQTT] Topic Write: %s\n", topicWrite.c_str());
  }
}

void DeviceWebConfig::enableWhatsapp(const String &token, const bool &webhookEnabled)
{
  tokenWhatsapp = token;
  whatsappEnabled = true;
  whatsappWebhookEnabled = webhookEnabled;
  if (webhookEnabled)
  {
    tempoUpdateWebHooksWhatsapp = 5000; // Tempo padrão para atualizações de webhook
    lastWebHookUpdateWhatsapp = millis();
    if (debug)
      Serial.println("[WhatsApp] Webhook ativado.");
  }
 
  if (debug)
    Serial.println("[WhatsApp] Token configurado e serviço ativado.");
}

void DeviceWebConfig::enableTelegram(const String &token)
{
  tokenTelegram = token;
  telegramEnabled = true;
  if (debug)
    Serial.println("[Telegram] Token configurado e serviço ativado.");
}

void DeviceWebConfig::enableSms(const String &token)
{
  tokenSms = token;
  smsEnabled = true;
  if (debug)
    Serial.println("[SMS] Token configurado e serviço ativado.");
}

void DeviceWebConfig::enableEmail(const String &token)
{
  tokenEmail = token;
  emailEnabled = true;
  if (debug)
    Serial.println("[Email] Token configurado e serviço ativado.");
}

void DeviceWebConfig::sendWhatsapp(const String &numero, const String &mensagem, const bool &grupo)
{
  if (!whatsappEnabled || WiFi.status() != WL_CONNECTED)
    return;

  HTTPClient http;
  http.begin("https://" + dominio + "/api/whatsapp/send");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization", "Bearer " + tokenWhatsapp);
  String postData = "numero=" + numero +
                    "&mensagem=" + mensagem +
                    "&grupo=" + String(grupo ? "true" : "false");
  int httpCode = http.POST(postData);

  if (debug)
  {
    String resposta = http.getString();
    Serial.printf("[WhatsApp] Enviando para %s: %s\n", numero.c_str(), mensagem.c_str());
    Serial.printf("[WhatsApp] Código HTTP: %d\n", httpCode);
    Serial.printf("[WhatsApp] Resposta: %s\n", resposta.c_str());
  }

  http.end();
}
void DeviceWebConfig::webhookWhatsapp()
{
  if (!whatsappEnabled || WiFi.status() != WL_CONNECTED || !whatsappWebhookEnabled)
    return;

  HTTPClient http;
  http.begin("https://" + dominio + "/api/whatsapp/webhook");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization", "Bearer " + tokenWhatsapp);
  int httpCode = http.POST("");
  String resposta = http.getString();
  http.end();
  if (debug)
  {
    Serial.printf("[WhatsApp Webhook] Código HTTP: %d\n", httpCode);
    Serial.printf("[WhatsApp Webhook] Resposta: %s\n", resposta.c_str());
  }

  // Verifica se total > 0 usando ArduinoJson
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, resposta);

  if (error)
  {
    if (debug)
      Serial.println("[Webhook JSON] Erro ao analisar JSON.");
    return;
  }

  int total = doc["total"] | 0;
  if (total > 0)
  {
    JsonArray mensagens = doc["mensagens"].as<JsonArray>();
    for (JsonObject obj : mensagens) {
      // Serializa o objeto individual em String JSON
      String jsonMsg;
      serializeJson(obj, jsonMsg);

      // Adiciona à fila
      enqueueWhatsappResponse(jsonMsg);

      
    }
  }
  else
  {
    if (debug)
      Serial.println("[Webhook] Nenhuma nova mensagem recebida.");
  }
}
void DeviceWebConfig::enqueueWhatsappResponse(const String &resposta)
{
  if (queueSize < MAX_WHATSAPP_QUEUE)
  {
    whatsappQueue[queueEnd] = resposta;
    queueEnd = (queueEnd + 1) % MAX_WHATSAPP_QUEUE;
    queueSize++;
  }
  else
  {
    if (debug)
      Serial.println("[Fila WhatsApp] Fila cheia, descartando resposta.");
  }
}
bool DeviceWebConfig::dequeueWhatsappResponse(String &resposta)
{
  if (queueSize == 0)
    return false;
  resposta = whatsappQueue[queueStart];
  queueStart = (queueStart + 1) % MAX_WHATSAPP_QUEUE;
  queueSize--;
  return true;
}

void DeviceWebConfig::sendTelegram(const String &chatId, const String &mensagem)
{
  if (!telegramEnabled || WiFi.status() != WL_CONNECTED)
    return;

  HTTPClient http;
  http.begin("https://" + dominio + "/api/telegram/send");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization", "Bearer " + tokenTelegram);
  String postData = "chatid=" + chatId +
                    "&mensagem=" + mensagem;
  int httpCode = http.POST(postData);

  if (debug)
  {
    String resposta = http.getString();
    Serial.printf("[Telegram] Enviando para %s: %s\n", chatId.c_str(), mensagem.c_str());
    Serial.printf("[Telegram] Código HTTP: %d\n", httpCode);
    Serial.printf("[Telegram] Resposta: %s\n", resposta.c_str());
  }

  http.end();
}

void DeviceWebConfig::sendSms(const String &numero, const String &mensagem)
{
  if (!smsEnabled || WiFi.status() != WL_CONNECTED)
    return;

  HTTPClient http;
  http.begin("https://" + dominio + "/api/sms/send");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization", "Bearer " + tokenSms);
  String postData = "numero=" + numero +
                    "&mensagem=" + mensagem;
  int httpCode = http.POST(postData);

  if (debug)
  {
    String resposta = http.getString();
    Serial.printf("[SMS] Enviando para %s: %s\n", numero.c_str(), mensagem.c_str());
    Serial.printf("[SMS] Código HTTP: %d\n", httpCode);
    Serial.printf("[SMS] Resposta: %s\n", resposta.c_str());
  }

  http.end();
}

void DeviceWebConfig::sendEmail(const String &destinatario, const String &assunto, const String &mensagem)
{
  if (!emailEnabled || WiFi.status() != WL_CONNECTED)
    return;

  HTTPClient http;
  http.begin("https://" + dominio + "/api/email/send");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization", "Bearer " + tokenEmail);
  String postData = "destinatario=" + destinatario +
                    "&assunto=" + assunto +
                    "&mensagem=" + mensagem;
  int httpCode = http.POST(postData);

  if (debug)
  {
    String resposta = http.getString();
    Serial.printf("[Email] Enviando para %s: %s\n", destinatario.c_str(), mensagem.c_str());
    Serial.printf("[Email] Código HTTP: %d\n", httpCode);
    Serial.printf("[Email] Resposta: %s\n", resposta.c_str());
  }

  http.end();
}

void DeviceWebConfig::connectWifi()
{
  WiFi.begin(ssid.c_str(), password.c_str());
  if (debug)
    Serial.printf("[WiFi] Conectando na rede %s...\n", ssid.c_str());

  for (int i = 0; i < 15 && WiFi.status() != WL_CONNECTED; ++i)
  {
    delay(1000);
    if (debug)
      Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    if (debug)
      Serial.printf("\n[WiFi] Conectado com IP: %s\n", WiFi.localIP().toString().c_str());
  }
  else
  {
    if (debug)
      Serial.println("\n[WiFi] Falha ao conectar.");
  }
}

void DeviceWebConfig::connectMqtt()
{
  if (!mqttClient.connected())
  {
    if (debug)
      Serial.println("[MQTT] Tentando conectar...");
    mqttClient.setCallback([this](char *topic, byte *payload, unsigned int length)
                           { this->callbackMqtt(topic, payload, length); });
    if (mqttClient.connect(clientIdMqtt.c_str(), userMqtt.c_str(), passwordMqtt.c_str()))
    {
      mqttClient.subscribe(topicReadMqtt.c_str());
      if (debug)
        Serial.println("[MQTT] Conectado e inscrito no tópico.");
    }
    else
    {
      if (debug)
        Serial.printf("[MQTT] Falha na conexão. Código: %d\n", mqttClient.state());
    }
  }
}

void DeviceWebConfig::loopMqtt()
{
  if (mqttClient.connected())
  {
    if (debug)
      Serial.println("[MQTT] Conectado, processando mensagens...");
    mqttClient.loop();

    if (millis() - lastMqttUpdate >= tempoAtualizacaoMqtt)
    {
      lastMqttUpdate = millis();
      String payload = "{";
      for (DeviceWebVar *var : DeviceWeb::_instancias)
      {
        if (var->getType() == "escrita")
        {
          if (payload.length() > 1)
            payload += ",";
          payload += "\"" + var->getName() + "\":\"" + var->getValue() + "\"";
        }
      }
      payload += "}";

      if (debug)
        Serial.printf("[MQTT] Enviando payload: %s\n", payload.c_str());

      mqttClient.publish(topicWriteMqtt.c_str(), payload.c_str());
    }
  }
  else
  {
    if (debug)
      Serial.println("[MQTT] Desconectado, tentando reconectar...");
    connectMqtt();
  }
}

void DeviceWebConfig::callbackMqtt(char *topic, byte *payload, unsigned int length)
{
  String payloadString;

  for (unsigned int i = 0; i < length; i++)
  {
    payloadString += (char)payload[i];
  }

  if (debug)
  {
    Serial.println("[MQTT] Mensagem recebida:");
    Serial.print("[MQTT] Tópico: ");
    Serial.println(topic);
    Serial.print("[MQTT] Payload: ");
    Serial.println(payloadString);
  }

  // Parse do JSON
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, payloadString);

  if (error)
  {
    if (debug)
    {
      Serial.print("[MQTT] Erro ao parsear JSON: ");
      Serial.println(error.c_str());
    }
    return;
  }

  JsonObject obj = doc.as<JsonObject>();
  for (JsonPair kv : obj)
  {
    String chave = String(kv.key().c_str());
    String valor = kv.value().as<String>();

    if (debug)
    {
      Serial.printf("[MQTT] Chave: %s | Valor: %s\n", chave.c_str(), valor.c_str());
    }

    // Verifica e atualiza variáveis do tipo leitura
    for (DeviceWebVar *var : DeviceWeb::_instancias)
    {
      if (var->getName() == chave && var->getType() == "leitura")
      {
        var->setValue(valor);
        if (debug)
        {
          Serial.printf("[MQTT] Atualiza leitura: %s => %s\n", chave.c_str(), valor.c_str());
        }
        break;
      }
    }
  }
}

void DeviceWebConfig::scanHttps()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    if (debug)
      Serial.println("[HTTPS] Falha: Wi-Fi não conectado.");
    return;
  }

  // Monta o payload JSON com as variáveis registradas
  String payload = "{";
  for (size_t i = 0; i < DeviceWeb::_instancias.size(); ++i)
  {
    DeviceWebVar *var = DeviceWeb::_instancias[i];
    if (var->getType() == "escrita")
    {

      if (i > 0)
        payload += ",";
      payload += "\"" + var->getName() + "\":\"" + var->getValue() + "\"";
    }
  }
  payload += "}";

  String url = "https://" + dominio + "/services/https/index?token=" + tokenHttps;

  if (debug)
  {
    Serial.println("[HTTPS] Iniciando requisição...");
    Serial.println("[HTTPS] URL: " + url);
    Serial.println("[HTTPS] Payload: " + payload);
  }

  HTTPClient http;
  int httpCode = -1;
  String resposta;

  if (metodoHttps == "POST")
  {
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String postData = "token=" + tokenHttps + "&payload=" + payload;
    httpCode = http.POST(postData);
  }
  else
  {
    url += "&payload=" + payload;
    http.begin(url);
    httpCode = http.GET();
  }

  if (httpCode > 0)
  {
    resposta = http.getString();

    if (debug)
    {
      Serial.printf("[HTTPS] Código de Resposta: %d\n", httpCode);
      Serial.println("[HTTPS] Corpo da Resposta:");
      Serial.println(resposta);
    }

    // Tenta desserializar a resposta
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, resposta);

    if (!error)
    {
      if (doc.is<JsonArray>())
      {
        for (JsonObject obj : doc.as<JsonArray>())
        {
          const char *nome = obj["variavel"];
          const char *ultimoValor = obj["valor"];
          const char *tipo = obj["tipo"]; // opcional: se vier do servidor

          if (nome && ultimoValor)
          {
            for (DeviceWebVar *var : DeviceWeb::_instancias)
            {
              if (var->getName() == nome && var->getType() == "leitura")
              {
                var->setValue(ultimoValor);
                if (debug)
                {
                  Serial.printf("[HTTPS] Atualizado: %s => %s\n", nome, ultimoValor);
                }
                break;
              }
            }
          }
        }
      }
      else
      {
        if (debug)
          Serial.println("[HTTPS] A resposta não é um array JSON.");
      }
    }
    else
    {
      if (debug)
      {
        Serial.print("[HTTPS] Erro ao analisar JSON: ");
        Serial.println(error.c_str());
      }
    }
  }
  else
  {
    if (debug)
    {
      Serial.printf("[HTTPS] Falha na requisição. Código: %d\n", httpCode);
    }
  }

  http.end();
}

void DeviceWebConfig::loop()
{
  if (wifiEnabled && WiFi.status() != WL_CONNECTED)
  {
    connectWifi();
  }
  else if (WiFi.status() == WL_CONNECTED)
  {
    // SINCRONIZA MQTT ou HTTPS
    if (protocolo == "MQTT")
    {
      loopMqtt();
    }
    else if (protocolo == "HTTPS")
    {
      scanHttps();
    }

    // SINCRONIZA WHATSAPP
    if (whatsappEnabled && millis() - lastWebHookUpdateWhatsapp >= tempoUpdateWebHooksWhatsapp)
    {
      lastWebHookUpdateWhatsapp = millis();
      webhookWhatsapp();
    }
  }
}
// ==========================
//    DEVICEWEBVAR MÉTODOS
// ==========================

DeviceWebVar::DeviceWebVar(const String &nome, const String &tipo, DeviceWebConfig *config)
    : nome(nome), tipo(tipo), config(config)
{
  DeviceWeb::_instancias.push_back(this);
  if (config->debug)
  {
    Serial.printf("[Var] Variável registrada: %s (%s)\n", nome.c_str(), tipo.c_str());
  }
}

void DeviceWebVar::setValue(const String &val)
{
  valor = val;
  logUpdate(valor);
}

void DeviceWebVar::logUpdate(const String &val)
{
  if (config && config->debug)
  {
    Serial.printf("[Var] %s atualizado para: %s\n", nome.c_str(), val.c_str());
  }
}

String DeviceWebVar::getValue() const
{
  return valor;
}

String DeviceWebVar::getName() const
{
  return nome;
}

String DeviceWebVar::getType() const
{
  return tipo;
}
