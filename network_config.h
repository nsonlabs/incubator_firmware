/**
 * @file network_config.h
 * @brief Asynchronous GSM Cellular (LTE Cat-1 SIM7680C) & Wi-Fi Client with HiveMQ SSL Sync Fallback
 */

#ifndef NETWORK_CONFIG_H
#define NETWORK_CONFIG_H

#define TINY_GSM_MODEM_SIM7600
#define TINY_GSM_RX_BUFFER 1024

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <TinyGsmClient.h>
#include <SSLClient.h>
#include <PubSubClient.h>

// Link types
enum LinkType {
  NET_NONE,
  NET_WIFI,
  NET_GSM
};

// Cellular Connection States for the GSM non-blocking state machine
enum GsmConnState {
  GSM_CONN_IDLE,
  GSM_CONN_INIT,
  GSM_CONN_CHECK_SIM,
  GSM_CONN_WAIT_NETWORK,
  GSM_CONN_CONNECT_GPRS,
  GSM_CONN_READY,
  GSM_CONN_WAIT_RETRY
};

// SIM7680C Non-blocking customized client has been replaced with native TinyGSM
extern LinkType activeLink;
extern TinyGsm modem;
extern TinyGsmClient gsmClient;
extern SSLClient sslGsmClient;
extern WiFiClient wifiClient;
extern WiFiClientSecure wifiClientSecure;
extern PubSubClient mqttClient;

// Scanned and typed custom credentials
extern char selectedSsid[33];
extern char typedPassword[64];

LinkType activeLink = NET_NONE;
GsmConnState gsmState = GSM_CONN_IDLE;
unsigned long gsmTimer = 0;
unsigned long gsmRetryDelay = 5000; // starts at 5s, back off to 60s max
bool gprsConnected = false;

HardwareSerial SerialAT(1); // ESP32-S3 Hardware Serial1 (UART1) for SIM7680C/A7680C (UART2 does not exist on S3!)
TinyGsm modem(SerialAT);
TinyGsmClient gsmClient(modem, 0); // PDP context channel index 0
SSLClient sslGsmClient(&gsmClient); // Wrap TinyGSM socket client in secure TLS
WiFiClient wifiClient;
WiFiClientSecure wifiClientSecure;
PubSubClient mqttClient;

// Global cellular network quality & technology parameters
int gsmSignalBars = 0;
char gsmNetworkType[8] = "";

// Decoupled dual-core messaging state variables (Locks MQTT entirely to Core 0)
volatile bool pendingTelemetryPublish = false;
volatile bool pendingSettingsPublish = false;
volatile bool cachedMqttConnected = false;
volatile int mqttStateCode = -999;

volatile float telemetryTemp = 0.0f;
volatile float telemetryHum = 0.0f;
volatile int telemetryDay = 0;
volatile bool telemetryHeater = false;
volatile bool telemetryHumidifier = false;
volatile bool telemetryMotor = false;

bool getMqttConnectedState() {
  return cachedMqttConnected;
}

void publishSettings() {
  if (!mqttClient.connected()) return;
  char payload[768] = {0};
  snprintf(payload, sizeof(payload),
    "{\"cmd\":\"settings_update\","
    "\"targetTemp\":%.1f,"
    "\"targetHumidity\":%.1f,"
    "\"minTemp\":%.1f,"
    "\"maxTemp\":%.1f,"
    "\"minHumidity\":%d,"
    "\"maxHumidity\":%d,"
    "\"turnIntervalHours\":%.1f,"
    "\"turnEnabled\":%s,"
    "\"daysTotal\":%d,"
    "\"stopTurningDay\":%d,"
    "\"backupType\":%d,"
    "\"backupPressDurationSec\":%.1f,"
    "\"chokeServoEnabled\":%s,"
    "\"maxGenStartRetries\":%d,"
    "\"chokeServoDegrees\":%d,"
    "\"backupSchedEnabled\":%s,"
    "\"backupSchedStartHour\":%d,"
    "\"backupSchedEndHour\":%d,"
    "\"simAndWifiMode\":%d}",
    currentSettings.targetTemp,
    currentSettings.targetHumidity,
    currentSettings.minTemp,
    currentSettings.maxTemp,
    (int)currentSettings.minHumidity,
    (int)currentSettings.maxHumidity,
    (float)currentSettings.turnIntervalHours / 60.0f,
    currentSettings.turnEnabled ? "true" : "false",
    currentSettings.totalIncubationDays,
    currentSettings.stopTurningDay,
    currentSettings.backupType,
    currentSettings.backupPressDurationSec,
    currentSettings.chokeServoEnabled ? "true" : "false",
    currentSettings.maxGenStartRetries,
    currentSettings.chokeServoDegrees,
    currentSettings.backupSchedEnabled ? "true" : "false",
    currentSettings.backupSchedStartHour,
    currentSettings.backupSchedEndHour,
    currentSettings.simAndWifiMode
  );
  mqttClient.publish("egg/incubator/" DEV_SERIAL_NUMBER "/settings", payload);
}

float parseNumericValue(const char* key, const char* message) {
  if (!message) return 0.0f;
  const char* ptr = strstr(message, key);
  if (!ptr) return 0.0f;
  const char* colon = strchr(ptr, ':');
  if (!colon) return 0.0f;
  const char* valPtr = colon + 1;
  while (*valPtr && (*valPtr == ' ' || *valPtr == '"' || *valPtr == '\\')) {
    valPtr++;
  }
  return atof(valPtr);
}

int parseIntegerValue(const char* key, const char* message) {
  if (!message) return 0;
  const char* ptr = strstr(message, key);
  if (!ptr) return 0;
  const char* colon = strchr(ptr, ':');
  if (!colon) return 0;
  const char* valPtr = colon + 1;
  while (*valPtr && (*valPtr == ' ' || *valPtr == '"' || *valPtr == '\\')) {
    valPtr++;
  }
  return atoi(valPtr);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  char message[1024] = {0};
  for (unsigned int i = 0; i < length && i < 1023; i++) {
    message[i] = (char)payload[i];
  }
  
  if (strstr(topic, "control")) {
    // 1. Check for manual turn command
    if (strstr(message, "TURN_NOW")) {
      triggerManualEggTurn(true);
      return;
    }

    // 2. Query configurations (Bidirectional sync demand)
    if (strstr(message, "query_settings")) {
      pendingPublishSettings = true;
      return;
    }
    
    // 3. Link request
    if (strstr(message, "link") && !strstr(message, "unlink")) {
      isDeviceLinked = true;
      preferences.begin("incubator", false);
      preferences.putBool("dev_linked", true);
      preferences.end();
      
      // Publish confirmation response on status topic
      char respPayload[128];
      snprintf(respPayload, sizeof(respPayload), "{\"status\":\"linked\",\"serial\":\"%s\"}", DEV_SERIAL_NUMBER);
      mqttClient.publish("egg/incubator/" DEV_SERIAL_NUMBER "/status", respPayload);
      
      // Push full state coordinates immediately to the app
      pendingPublishSettings = true;
      forceDisplayRedraw = true;
      return;
    }

    // 4. Unlink request
    if (strstr(message, "unlink")) {
      isDeviceLinked = false;
      preferences.begin("incubator", false);
      preferences.putBool("dev_linked", false);
      preferences.end();
      
      char respPayload[128];
      snprintf(respPayload, sizeof(respPayload), "{\"status\":\"unlinked\",\"serial\":\"%s\"}", DEV_SERIAL_NUMBER);
      mqttClient.publish("egg/incubator/" DEV_SERIAL_NUMBER "/status", respPayload);
      forceDisplayRedraw = true;
      return;
    }

    // 5. Remote parameter settings update
    if (strstr(message, "set_params") && isDeviceLinked) {
      if (strstr(message, "targetTemp") || strstr(message, "temp")) {
        float val = parseNumericValue("targetTemp", message);
        if (val == 0.0f) val = parseNumericValue("temp", message);
        if (val >= 35.0f && val <= 41.50f) {
          currentSettings.targetTemp = val;
        }
      }
      
      if (strstr(message, "targetHumidity") || strstr(message, "humidity")) {
        float val = parseNumericValue("targetHumidity", message);
        if (val == 0.0f) val = parseNumericValue("humidity", message);
        if (val >= 30.0f && val <= 95.0f) {
          currentSettings.targetHumidity = val;
        }
      }

      if (strstr(message, "minTemp")) {
        currentSettings.minTemp = parseNumericValue("minTemp", message);
      }

      if (strstr(message, "maxTemp")) {
        currentSettings.maxTemp = parseNumericValue("maxTemp", message);
      }

      if (strstr(message, "minHumidity")) {
        currentSettings.minHumidity = parseIntegerValue("minHumidity", message);
      }

      if (strstr(message, "maxHumidity")) {
        currentSettings.maxHumidity = parseIntegerValue("maxHumidity", message);
      }
      
      if (strstr(message, "turnIntervalHours") || strstr(message, "turn_interval")) {
        float val = parseNumericValue("turnIntervalHours", message);
        if (val == 0.0f) val = parseNumericValue("turn_interval", message);
        if (val >= 0.5f && val <= 24.0f) {
          currentSettings.turnIntervalHours = val * 60; // saved in minutes
        }
      }
      
      if (strstr(message, "turnEnabled") || strstr(message, "turn_enabled")) {
        const char* turnEnPtr = strstr(message, "turnEnabled");
        if (!turnEnPtr) turnEnPtr = strstr(message, "turn_enabled");
        if (turnEnPtr) {
          const char* colon = strchr(turnEnPtr, ':');
          if (colon) {
            if (strstr(colon, "true")) {
              currentSettings.turnEnabled = true;
            } else if (strstr(colon, "false")) {
              currentSettings.turnEnabled = false;
            }
          }
        }
      }

      if (strstr(message, "daysTotal")) {
        currentSettings.totalIncubationDays = parseIntegerValue("daysTotal", message);
      }

      if (strstr(message, "stopTurningDay")) {
        currentSettings.stopTurningDay = parseIntegerValue("stopTurningDay", message);
      }

      if (strstr(message, "backupType")) {
        currentSettings.backupType = parseIntegerValue("backupType", message);
      }

      if (strstr(message, "backupPressDurationSec")) {
        currentSettings.backupPressDurationSec = parseNumericValue("backupPressDurationSec", message);
      }

      if (strstr(message, "chokeServoEnabled")) {
        const char* chokeEnPtr = strstr(message, "chokeServoEnabled");
        if (chokeEnPtr) {
          const char* colon = strchr(chokeEnPtr, ':');
          if (colon) {
            if (strstr(colon, "true")) {
              currentSettings.chokeServoEnabled = true;
            } else if (strstr(colon, "false")) {
              currentSettings.chokeServoEnabled = false;
            }
          }
        }
      }

      if (strstr(message, "maxGenStartRetries")) {
        currentSettings.maxGenStartRetries = parseIntegerValue("maxGenStartRetries", message);
      }

      if (strstr(message, "chokeServoDegrees")) {
        currentSettings.chokeServoDegrees = parseIntegerValue("chokeServoDegrees", message);
      }

      if (strstr(message, "backupSchedEnabled")) {
        const char* bkSchedEnPtr = strstr(message, "backupSchedEnabled");
        if (bkSchedEnPtr) {
          const char* colon = strchr(bkSchedEnPtr, ':');
          if (colon) {
            if (strstr(colon, "true")) {
              currentSettings.backupSchedEnabled = true;
            } else if (strstr(colon, "false")) {
              currentSettings.backupSchedEnabled = false;
            }
          }
        }
      }

      if (strstr(message, "backupSchedStartHour")) {
        currentSettings.backupSchedStartHour = parseIntegerValue("backupSchedStartHour", message);
      }

      if (strstr(message, "backupSchedEndHour")) {
        currentSettings.backupSchedEndHour = parseIntegerValue("backupSchedEndHour", message);
      }

      if (strstr(message, "simAndWifiMode")) {
        currentSettings.simAndWifiMode = parseIntegerValue("simAndWifiMode", message);
      }
      
      // Save all updated parameters remotely directly to preferences/EEPROM
      saveSettingsToPreferences(currentSettings);
      
      // Send receipt response status payload
      char respPayload[128];
      snprintf(respPayload, sizeof(respPayload), "{\"status\":\"params_synced\",\"serial\":\"%s\"}", DEV_SERIAL_NUMBER);
      mqttClient.publish("egg/incubator/" DEV_SERIAL_NUMBER "/status", respPayload);
      
      // Instantly synchronize settings on-air back to the app as well
      pendingPublishSettings = true;
      forceDisplayRedraw = true;
    }
  }
}

void initCommunications() {
  SerialAT.begin(115200, SERIAL_8N1, SIM_RX2, SIM_TX2); // Connect to A7680C at 115200
  delay(500); // Give the module Serial interface a moment to initialize after pinmux selection

  WiFi.mode(WIFI_STA);
  mqttClient.setCallback(mqttCallback);
  mqttClient.setBufferSize(1024); // Support high payload parameters JSON transfers up to 1024 bytes (prevents silent library drops)
  mqttClient.setSocketTimeout(5); // Reduce connection timeouts to 5 seconds to prevent sluggishness
}

void queryGsmNetworkStatus() {
  int csq = modem.getSignalQuality();
  if (csq == 99 || csq == 0) {
    gsmSignalBars = 0;
  } else {
    // Proportional CSQ to 4-bar mapping to show smooth increases/decreases
    if (csq >= 22) gsmSignalBars = 4;      // Excellent
    else if (csq >= 15) gsmSignalBars = 3; // Good
    else if (csq >= 8) gsmSignalBars = 2;  // Okay
    else if (csq >= 2) gsmSignalBars = 1;  // Marginal
    else gsmSignalBars = 0;
  }

  // Query network system mode dynamically to identify 2G vs 4G
  modem.sendAT("+CNSMOD?");
  String nsmodRes = "";
  if (modem.waitResponse(500, "+CNSMOD:") == 1) {
    nsmodRes = SerialAT.readStringUntil('\n');
    nsmodRes.trim();
  }
  if (nsmodRes.length() > 0) {
    int lastComma = nsmodRes.lastIndexOf(',');
    if (lastComma != -1) {
      int sysmode = nsmodRes.substring(lastComma + 1).toInt();
      if (sysmode == 1 || sysmode == 2 || sysmode == 3) {
        strcpy(gsmNetworkType, "2G");
      } else if (sysmode >= 9) {
        strcpy(gsmNetworkType, "4G");
      } else {
        strcpy(gsmNetworkType, "4G");
      }
    }
  } else {
    // Fallback to query +CPSI?
    modem.sendAT("+CPSI?");
    String cpsiRes = "";
    if (modem.waitResponse(500, "+CPSI:") == 1) {
      cpsiRes = SerialAT.readStringUntil('\n');
    }
    if (cpsiRes.indexOf("LTE") >= 0) {
      strcpy(gsmNetworkType, "4G");
    } else if (cpsiRes.indexOf("GSM") >= 0) {
      strcpy(gsmNetworkType, "2G");
    } else {
      strcpy(gsmNetworkType, "4G"); // Default
    }
  }
  Serial.printf("  [GSM] Signal CSQ: %d (Bars: %d), Mode Type: %s\n", csq, gsmSignalBars, gsmNetworkType);
}

void checkNetworkAndReconnect() {
  if (isScanningWifi) {
    activeLink = NET_NONE;
    return; // Suspend reconnect checks and cellular locks while user is actively scanning
  }

  int mode = (int)currentSettings.simAndWifiMode;
  
  // Mode 3: Disconnected / Offline (No sim & WIFI)
  if (mode == 3) {
    if (WiFi.status() == WL_CONNECTED || WiFi.getMode() != WIFI_OFF) {
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
    }
    if (gprsConnected) {
      modem.gprsDisconnect();
      gprsConnected = false;
    }
    gsmState = GSM_CONN_IDLE;
    activeLink = NET_NONE;
    return;
  }

  // Mode 0: Use SIM only - never open WiFi radio
  if (mode == 0) {
    if (WiFi.status() == WL_CONNECTED || WiFi.getMode() != WIFI_OFF) {
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
    }
  }

  // 1. Maintain WiFi Connection (Active in Mode 1: WiFi Only, Mode 2: SIM & WiFi)
  bool wifiConnected = false;
  if (mode == 1 || mode == 2) {
    if (WiFi.getMode() == WIFI_OFF) {
      WiFi.mode(WIFI_STA);
    }
    if (WiFi.status() == WL_CONNECTED) {
      wifiConnected = true;
    } else {
      // Non-blocking WiFi reconnect trigger
      static unsigned long lastWifiConnectAttempt = 0;
      if (lastWifiConnectAttempt == 0 || (millis() - lastWifiConnectAttempt > 30000)) {
        lastWifiConnectAttempt = millis();
        Serial.println("  [WiFi] Reconnecting non-blockingly...");
        if (currentSettings.isWifiConfigured && strlen(selectedSsid) > 0 && strcmp(selectedSsid, "None Selected") != 0) {
          WiFi.begin(selectedSsid, typedPassword);
        } else if (!currentSettings.isWifiConfigured) {
          #ifdef WIFI_SSID
          if (strlen(WIFI_SSID) > 0) {
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
          }
          #endif
        }
      }
    }
  }

  // 2. Maintain GSM Cellular Connection via TinyGSM (Active in Mode 0: SIM Only, or Mode 2 as Fallback when WiFi is down)
  if (mode == 0 || (mode == 2 && !wifiConnected)) {
    switch (gsmState) {
      case GSM_CONN_IDLE:
        gsmState = GSM_CONN_INIT;
        gsmTimer = millis();
        break;

      case GSM_CONN_INIT:
        Serial.println("  [GSM] Testing A7680C modem presence...");
        if (modem.testAT(500)) { // Fast 500ms non-blocking check
          Serial.println("  [GSM] Modem found! Configuring verbose errors...");
          modem.sendAT("+CMEE=2");
          modem.waitResponse(500);
          gsmState = GSM_CONN_CHECK_SIM;
          gsmTimer = millis();
        } else {
          Serial.println("  [GSM] Modem not responding to AT. Waiting to retry...");
          gsmState = GSM_CONN_WAIT_RETRY;
          gsmTimer = millis();
          gsmRetryDelay = 5000;
        }
        break;

      case GSM_CONN_CHECK_SIM:
        {
          SimStatus simStatus = modem.getSimStatus();
          if (simStatus == SIM_READY) {
            Serial.println("  [GSM] SIM status is READY. Waiting for network registration...");
            gsmState = GSM_CONN_WAIT_NETWORK;
            gsmTimer = millis();
          } else {
            Serial.printf("  [GSM] SIM not ready (Status Code: %d). Retrying...\n", (int)simStatus);
            gsmState = GSM_CONN_WAIT_RETRY;
            gsmTimer = millis();
          }
        }
        break;

      case GSM_CONN_WAIT_NETWORK:
        // Query network registration status non-blockingly every 2 seconds
        if (millis() - gsmTimer > 2000) {
          gsmTimer = millis();
          Serial.println("  [GSM] Checking cell network registration...");
          if (modem.isNetworkConnected()) {
            String op = modem.getOperator();
            Serial.printf("  [GSM] Cell registration complete! Operator: %s\n", op.c_str());
            
            // Query fresh CSQ RSSI and 2G/4G network mode
            queryGsmNetworkStatus();

            gsmState = GSM_CONN_CONNECT_GPRS;
          } else {
            Serial.println("  [GSM] Still waiting for cell tower attachment...");
          }
        }
        break;

      case GSM_CONN_CONNECT_GPRS:
        Serial.printf("  [GSM] Connecting to GPRS carrier network APN: '%s'...\n", GPRS_APN);
        if (modem.gprsConnect(GPRS_APN, GPRS_USER, GPRS_PASS)) {
          Serial.println("  [GSM] GPRS network connection established and active!");
          gprsConnected = true;
          gsmState = GSM_CONN_READY;
          gsmTimer = millis();
        } else {
          // GPRS failure - attempt "safaricom" GPRS APN fallback if APN is default "internet"
          if (strcmp(GPRS_APN, "internet") == 0) {
            Serial.println("  [GSM] Standard APN failed. Trying implicit 'safaricom' APN fallback...");
            if (modem.gprsConnect("safaricom", "", "")) {
              Serial.println("  [GSM] GPRS Connection established under fallback APN!");
              gprsConnected = true;
              gsmState = GSM_CONN_READY;
              gsmTimer = millis();
              break;
            }
          }
          Serial.println("  [GSM] APN configuration failed. Directing to retry delay...");
          gprsConnected = false;
          gsmState = GSM_CONN_WAIT_RETRY;
          gsmTimer = millis();
          gsmRetryDelay = 5000;
        }
        break;

      case GSM_CONN_READY:
        // Periodically verify GPRS layer health in background (every 20 seconds)
        if (millis() - gsmTimer > 20000) {
          gsmTimer = millis();
          if (!modem.isGprsConnected()) {
            Serial.println("  [GSM] Carrier network link disconnected!");
            gprsConnected = false;
            gsmState = GSM_CONN_INIT;
          } else {
            // Keep RSSI signal indicators and network mode technology (2G/4G) fresh on TFT screen
            queryGsmNetworkStatus();
          }
        }
        break;

      case GSM_CONN_WAIT_RETRY:
        if (millis() - gsmTimer >= gsmRetryDelay) {
          // Double delay upon failure up to 60s max (exponential backoff)
          gsmRetryDelay = min(gsmRetryDelay * 2, (unsigned long)60000);
          gsmState = GSM_CONN_INIT;
        }
        break;
    }
  } else {
    // Shutdown and idle cellular radios to save packet cost and RF power when Wi-Fi holds priority
    if (gprsConnected) {
      modem.gprsDisconnect();
      gprsConnected = false;
    }
    gsmState = GSM_CONN_IDLE;
  }

  // 3. Connect/Bridge physical transport interfaces for MQTT
  if (wifiConnected) {
    activeLink = NET_WIFI;
    static bool timeSynced = false;
    if (!timeSynced) {
      Serial.println("  [WiFi] Syncing system NTP clock to verify secure server TLS certificates...");
      configTime(0, 0, "pool.ntp.org", "time.nist.gov");
      timeSynced = true;
    }

    #if (MQTT_PORT == 8883)
      wifiClientSecure.setInsecure();
      mqttClient.setClient(wifiClientSecure);
    #else
      mqttClient.setClient(wifiClient);
    #endif
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  }
  else if (gsmState == GSM_CONN_READY && gprsConnected) {
    activeLink = NET_GSM;

    #if (MQTT_PORT == 8883)
      sslGsmClient.setInsecure();
      mqttClient.setClient(sslGsmClient);
    #else
      mqttClient.setClient(gsmClient);
    #endif
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  }
  else {
    activeLink = NET_NONE;
  }

  // 4. Maintain Active Secure SSL MQTT Connection to HiveMQ Cloud
  if (activeLink != NET_NONE) {
    if (!mqttClient.connected()) {
      static unsigned long lastMqttConnectAttempt = 0;
      if (lastMqttConnectAttempt == 0 || (millis() - lastMqttConnectAttempt > 10000)) {
        lastMqttConnectAttempt = millis();
        Serial.printf("  [%s] Connecting secure SSL MQTT client to HiveMQ Cloud...\n", (activeLink == NET_WIFI) ? "WiFi" : "GSM");
        if (mqttClient.connect(DEV_SERIAL_NUMBER, MQTT_USER, MQTT_PASS)) {
          mqttStateCode = 0;
          Serial.printf("  [%s] MQTT Client Bridge Established!\n", (activeLink == NET_WIFI) ? "WiFi" : "GSM");
          mqttClient.subscribe("egg/incubator/" DEV_SERIAL_NUMBER "/control");
        } else {
          mqttStateCode = mqttClient.state();
          Serial.printf("  [%s] MQTT Connection failure. PubSubClient error code: %d\n", (activeLink == NET_WIFI) ? "WiFi" : "GSM", mqttStateCode);
        }
      }
    }
  } else {
    mqttStateCode = -999;
  }
}

void processMqttLoop() {
  if (mqttClient.connected()) {
    mqttClient.loop();
  }
}

void triggerTelemetryPublish(SensorReadings readings, bool heater, bool humidifier, bool motor) {
  if (!mqttClient.connected()) return;
  
  char payload[192];
  snprintf(payload, sizeof(payload), 
    "{\"temp\":%.1f,\"humidity\":%.1f,\"day\":%d,\"heater\":%s,\"humidifier\":%s,\"motor\":%s}",
    readings.temperature, readings.humidity, readings.dayOfCycle,
    heater ? "true" : "false", humidifier ? "true" : "false", motor ? "true" : "false"
  );
  
  mqttClient.publish("egg/incubator/" DEV_SERIAL_NUMBER "/telemetry", payload);
}

#endif // NETWORK_CONFIG_H