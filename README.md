
# 📡 DeviceWeb

**DeviceWeb** é uma biblioteca para ESP32 que facilita a comunicação com servidores MQTT, envio de dados via HTTPS, e envio de notificações via **WhatsApp**, **Telegram**, **SMS** e **Email**. Ideal para projetos de automação, IoT e monitoramento remoto.

---

## 📦 Instalação

### 🔧 Manual

1. Clone ou baixe este repositório:
   ```bash
   git clone https://github.com/DeviceWebIoT/ESPDeviceWeb.git
   ```

2. Copie a pasta `DeviceWeb` para o diretório de bibliotecas do Arduino:
   ```
   Documentos/Arduino/libraries/
   ```

3. Reinicie a IDE do Arduino.

---

## 📚 Dependências

Certifique-se de instalar as bibliotecas abaixo pela Library Manager da IDE Arduino:

- [WiFi](https://www.arduino.cc/en/Reference/WiFi)
- [HTTPClient](https://www.arduino.cc/en/Reference/HTTPClient)
- [PubSubClient](https://github.com/knolleary/pubsubclient)
- [ArduinoJson](https://arduinojson.org/)

---

## 🚀 Exemplo Rápido

```cpp
#include <DeviceWeb.h>

DeviceWebConfig config;

DeviceWebVar temperatura("temp1", "escrita", &config);
DeviceWebVar comandoLed("led1", "leitura", &config);

void setup() {
  Serial.begin(115200);

  config.setDebug(true);
  config.setWifi("SUA_REDE_WIFI", "SENHA_WIFI");
  config.setMqtt("mqtt01.deviceweb.com.br", "", "", "esp32-01", "meutopico/leitura", "meutopico/escrita", 5000);
  config.enableWhatsapp("SEU_TOKEN_WHATSAPP");
}

void loop() {
  temperatura.setValue(analogRead(34));

  if (comandoLed.getValue() == "1") {
    digitalWrite(2, HIGH);
  } else {
    digitalWrite(2, LOW);
  }

  config.loop();
}
```

---

## 🧩 Classes Principais

### 🔹 DeviceWebConfig

Classe de configuração do dispositivo.

| Método | Descrição |
|--------|-----------|
| `setDebug(bool ativo)` | Ativa logs no Serial |
| `setWifi(ssid, senha)` | Configura Wi-Fi |
| `setMqtt(server, user, pass, clientId, topicRead, topicWrite, intervalo)` | Configura MQTT |
| `setHttps(token, metodo)` | Configura envio por HTTPS (`POST` ou `GET`) |
| `enableWhatsapp(token)` | Ativa envio de mensagens via WhatsApp |
| `enableTelegram(token)` | Ativa envio de mensagens via Telegram |
| `enableSms(token)` | Ativa envio de mensagens via SMS |
| `enableEmail(token)` | Ativa envio de mensagens via Email |
| `sendWhatsapp(numero, mensagem, grupo=false)` | Envia mensagem por WhatsApp |
| `sendTelegram(chatId, mensagem)` | Envia mensagem por Telegram |
| `sendSms(numero, mensagem)` | Envia SMS |
| `sendEmail(destinatario, assunto, mensagem)` | Envia Email |
| `loop()` | Mantém conexões ativas (WiFi, MQTT ou HTTPS) |

### 🔹 DeviceWebVar

Representa uma variável monitorada ou controlada via nuvem.

| Método | Descrição |
|--------|-----------|
| `DeviceWebVar(nome, tipo, config)` | Cria nova variável (`tipo`: `"escrita"` ou `"leitura"`) |
| `setValue(valor)` | Define valor da variável |
| `getValue()` | Retorna valor atual |
| `getName()` | Retorna nome da variável |
| `getType()` | Retorna tipo da variável |

---

## 📡 Protocolos Suportados

- **MQTT**: comunicação assíncrona entre dispositivos e servidor
- **HTTPS**: envio periódico com autenticação por token
- **Notificações**: WhatsApp, Telegram, SMS e Email via APIs HTTP

---

## 📝 Licença

MIT © 2025 - DeviceWeb


---

## ✅ Tipos de Variáveis

| Tipo      | Descrição                                               |
|-----------|----------------------------------------------------------|
| `escrita` | A variável será enviada para o servidor (MQTT/HTTPS)     |
| `leitura` | A variável poderá ser atualizada pelo servidor remoto    |

---

## 📌 Observações Importantes

- O método `config.loop()` deve ser chamado dentro da função `loop()` do Arduino para manter a conectividade com o servidor.
- Para uso de HTTPS, a variável `token` deve estar cadastrada no servidor DeviceWeb.
- A biblioteca pode ser estendida para suportar sensores e atuadores com integração nativa.

---

## 🧪 Testes Recomendados

- Verifique se o ESP32 conecta corretamente à rede Wi-Fi.
- Certifique-se de que o broker MQTT está acessível pela rede.
- Teste a comunicação bidirecional (leitura e escrita).
- Utilize a plataforma [deviceweb.com.br](https://deviceweb.com.br) como backend.

---

## 🧑‍💻 Autor

**DeviceWeb**  
[DeviceWeb.com.br](https://deviceweb.com.br)  


---

## 📝 Licença

Este projeto está licenciado sob a Licença MIT - consulte o arquivo [LICENSE](LICENSE) para detalhes.

