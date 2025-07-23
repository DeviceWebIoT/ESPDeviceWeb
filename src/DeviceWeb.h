#ifndef DEVICEWEB_H
#define DEVICEWEB_H

#include <ArduinoJson.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <vector>
#define MAX_WHATSAPP_QUEUE 10

class DeviceWebConfig;

class DeviceWebVar
{
private:
  String nome, valor, tipo;
  DeviceWebConfig *config;

public:
  DeviceWebVar(const String &nome, const String &tipo, DeviceWebConfig *config);

  template <typename T>
  void setValue(const T &val)
  {
    valor = String(val);
    logUpdate(valor);
  }
  void setValue(const String &val);
  void logUpdate(const String &val);
  String getValue() const;
  String getName() const;
  String getType() const;
};

class DeviceWebConfig
{
private:
  WiFiClient wifiClient;
  String whatsappQueue[MAX_WHATSAPP_QUEUE];
  int queueStart = 0;
  int queueEnd = 0;
  int queueSize = 0;

public:
  bool debug = false;

  PubSubClient mqttClient;

  String dominio = "deviceweb.com.br";

  // Wi-Fi
  bool wifiEnabled = false;
  String ssid;
  String password;

  // HTTPS
  String protocolo = "NULL";
  String metodoHttps = "POST";
  String tokenHttps;

  // MQTT
  String serverMqtt;
  String userMqtt;
  String passwordMqtt;
  String clientIdMqtt;
  String topicReadMqtt;
  String topicWriteMqtt;
  int tempoAtualizacaoMqtt = 5000;
  unsigned long lastMqttUpdate = 0;

  // Notificações
  bool whatsappEnabled = false;
  String tokenWhatsapp;
  bool whatsappWebhookEnabled = false;
  int tempoUpdateWebHooksWhatsapp = 5000;
  unsigned long lastWebHookUpdateWhatsapp = 0;

  bool telegramEnabled = false;
  String tokenTelegram;

  bool smsEnabled = false;
  String tokenSms;

  bool emailEnabled = false;
  String tokenEmail;

  // Métodos
  DeviceWebConfig();

  void setDebug(bool ativo);
  void setWifi(const String &ssid, const String &password);
  void setHttps(const String &token, const String &metodo);
  void setMqtt(const String &server, const String &user, const String &pass, const String &clientId, const String &topicRead, const String &topicWrite, const int &tempoAtualizacao);

  void enableWhatsapp(const String &token, const bool &webhookEnabled = false);
  void enableTelegram(const String &token);
  void enableSms(const String &token);
  void enableEmail(const String &token);

  void sendWhatsapp(const String &numero, const String &mensagem, const bool &grupo = false);
  void webhookWhatsapp();
  void enqueueWhatsappResponse(const String &resposta);
  bool dequeueWhatsappResponse(String &resposta);

  void sendTelegram(const String &chatId, const String &mensagem);
  void sendSms(const String &numero, const String &mensagem);
  void sendEmail(const String &destinatario, const String &assunto, const String &mensagem);

  void connectWifi();
  void connectMqtt();
  void callbackMqtt(char *topic, byte *payload, unsigned int length);
  void loopMqtt();
  void scanHttps();
  void loop();
};

class DeviceWeb
{
public:
  static std::vector<DeviceWebVar *> _instancias;
};

#endif
