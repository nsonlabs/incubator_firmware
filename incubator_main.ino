/**
 * @file incubator_main.ino
 * @brief High Hatch-Rate IoT Egg Incubator calibration, pins, credentials & core sketch.
 * 
 * ⚠️ CRITICAL ARDUINO IDE DIRECTORY REQUIREMENT:
 * 1. Your sketch folder MUST be named exactly "incubator_main" to match this file name!
 * 2. If your parent folder has a different name (e.g., "Egg_Incubator" or "egg-incubator-esp32"),
 *    you MUST rename this main file to match your folder name exactly (e.g., "Egg_Incubator.ino"
 *    or "egg-incubator-esp32.ino").
 *    Otherwise, the Arduino IDE compiler will build an empty skeleton instead of compiling this file,
 *    leading to: "undefined reference to setup()" and "undefined reference to loop()" linker errors.
 * 3. Save all other files ("incubator_control.h", "screen_ui.h", "network_config.h") in the same folder.
 */

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_SHT31.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <ESP32Servo.h>

// ==========================================================
// 1. PINOUT DECLARATIONS (ESP32-S3 Optimized)
// ==========================================================
#define PIN_DS18B20  4    // OneWire Dallas Temperature Probe
#define PIN_SDA      1    // Hardware I2C SDA for SHT31
#define PIN_SCL      2    // Hardware I2C SCL for SHT31

// ST7735/ST7789 SPI TFT (GND, VCC, SCK, SDA, RST, DC, CS - No MISO)
#define TFT_SDA      11   // SPI Serial Data (MOSI on ESP32-S3)
#define TFT_SCLK     12   // SPI Serial Clock (SCK on ESP32-S3)
#define TFT_CS       10   // Display Chip Select
#define TFT_DC       9    // Display Data/Command Control
#define TFT_RST      14   // Display Hardware Reset
#define TFT_LED      21   // Display Backlight Control (High = ON)

#define RTC_CE_PIN   5    // DS1302 RTC Reset Pin
#define RTC_IO_PIN   6    // DS1302 Data (SDA)
#define RTC_SCLK_PIN 7    // DS1302 Clock

#define BTN_OK       15   // OK/Enter Menu Select Button
#define BTN_UP       16   // UP/Turn Up Button
#define BTN_DOWN     8    // DOWN/Turn Down Button (Reassigned to pin 8)

#define RELAY_HEATER 38   // Climate heating control
#define RELAY_HUMID  39   // Ultrasonic Mist humidifier
#define RELAY_MOTOR_UP   40   // Egg Turner motor direction UP/CW
#define RELAY_MOTOR_DOWN 13   // Egg Turner motor direction DOWN/CCW

#define PIN_BACKUP_START  41  // RELAY to start Backup (UPS, Inverter, or Generator)
#define PIN_SOURCE_SELECT 42  // RELAY (2 operated together) to select SOURCE (High=Backup, Low=Mains)
#define PIN_GEN_KILL      19  // RELAY for Generator stop (Kill switch)
#define PIN_BACKUP_SENSE  36  // Input pin to read Backup power presence (High=Backup ON, Low=Backup OFF)
#define PIN_CHOKE_SERVO   3   // Servo motor pin for Generator choke pulling (moved to GPIO 3)
#define PIN_MAINS_SENSE   37  // Pin to detect if MAINS voltage is present (High=Mains OK, Low=Fail)

#define SIM_TX2      17   // ESP32 TX pin - connected to A7680C RX (Physical wiring update from 43 to 17)
#define SIM_RX2      18   // ESP32 RX pin - connected to A7680C TX (Physical wiring update from 44 to 18)

// ==========================================================
// 2. INCUBATION PRESETS & CELLULAR/MQTT CREDENTIALS
// ==========================================================
#define WIFI_SSID         "Your_WiFi_Name"
#define WIFI_PASSWORD     "Your_WiFi_Password"
#define GPRS_APN          "safaricom"
#define GPRS_USER         "saf"
#define GPRS_PASS         "data"
#define MQTT_BROKER       "509a9f70a8eb4d538602f1643ad1565f.s1.eu.hivemq.cloud"
#define MQTT_PORT         8883  // Secure Port (Strict TLS mandatory for HiveMQ Cloud)
#define MQTT_USER         "incubator"
#define MQTT_PASS         "Incubator1"
#define DEV_SERIAL_NUMBER "INC-S3-9481-N16"

#define DEFAULT_TARGET_TEMP     37.5f  // Celsius (Chicken preset)
#define DEFAULT_TARGET_HUMID    55.0f  // Relative humidity percent
#define DEFAULT_MIN_TEMP        36.5f  // Min Temperature Celsius
#define DEFAULT_MAX_TEMP        37.5f  // Max Temperature Celsius
#define DEFAULT_MIN_HUMID       45.0f  // Min Humidity %
#define DEFAULT_MAX_HUMID       55.0f  // Max Humidity %
#define DEFAULT_TURN_INTERVAL   120    // Turn interval in minutes (e.g. 120 = 2 hours)
#define DEFAULT_TURN_ENABLED    1      // 1 = True (Auto), 0 = False (Manual)
#define DEFAULT_INCUBATION_DAYS 21     // Chickens cycle time

#define DEFAULT_BACKUP_TYPE     0      // 0 = UPS, 1 = INVERTER, 2 = GENERATOR
#define DEFAULT_BACKUP_DURATION 3.0f   // Seconds of starting long-press
#define DEFAULT_CHOKE_SERVO     1      // 1 = Enabled, 0 = Disabled
#define DEFAULT_MAX_GEN_RETRIES 3      // Max generator start attempts before failing

// Forward declarations of headers structure
#ifndef SYSTEM_SETTINGS_H
#define SYSTEM_SETTINGS_H
struct SystemSettings {
  float targetTemp;
  float targetHumidity;
  float minTemp;
  float maxTemp;
  float minHumidity;
  float maxHumidity;
  int turnIntervalHours;       // Turn interval in minutes (e.g., 30 to 300)
  bool turnEnabled;
  int incubationDayStarted;
  int incubationMonthStarted;
  int backupType;              // 0 = UPS, 1 = INVERTER, 2 = GENERATOR
  float backupPressDurationSec; // Duration of starting long press in seconds
  bool chokeServoEnabled;      // True if choke servo is enabled
  int maxGenStartRetries;      // Max number of generator start attempts
  int totalIncubationDays;     // Total incubation days (default 21)
  int stopTurningDay;          // Day to stop turning eggs (default 18, 0 for None)
  bool use24HourFormat;        // True if 24-hour mode is selected
  int chokeServoDegrees;       // Configured choke servo degrees (e.g. 50 or 90)
  bool backupSchedEnabled;     // True if scheduled backup usage is enabled
  int backupSchedStartHour;    // Start hour of scheduled backup (0-23)
  int backupSchedEndHour;      // End hour of scheduled backup (0-23)
  int simAndWifiMode;          // 0 = Use SIM only, 1 = Use WIFI only, 2 = use sim & WIFI, 3 = No sim & WIFI
  bool isWifiConfigured;       // True if a WiFi network is configured/saved
  char wifiSsid[33];           // Name of the saved SSID
};
#endif

struct SensorReadings {
  float temperature;
  float coreTemperature;
  float humidity;
  int dayOfCycle;
  int minutesPassed;
  bool isSensorsHealthy;
  bool tempSensorHealthy;
  bool humidSensorHealthy;
};

// Global parameters
extern SystemSettings currentSettings;
extern SystemSettings tempSettings;
extern SensorReadings latestReadings;
extern unsigned long previousMillisControl;
extern uint32_t globalHatchElapsedSeconds;
extern bool manualTurnActive;
extern unsigned long manualTurnEndMillis;
extern bool manualTurnUpward;

// Wi-Fi scan list, keyboard inputs, and state variables
char wifiScanBuffer[10][33];
int wifiScanRssi[10];
int wifiScanCount = -1;
bool isScanningWifi = false;
int wifiNetSelection = 0;
char selectedSsid[33] = "None Selected";
char typedPassword[64] = "";
int kbCharIndex = 0;
bool isShiftActive = false;
unsigned long wifiConnectStartTime = 0;
bool needKeyboardFullRedraw = true;
bool needConnectingScreenFullRedraw = true;
bool needNetScanFullRedraw = true;
const char kbCharactersLower[] = "1234567890qwertyuiopasdfghjkl_zxcvbnm-@#";
const char kbCharactersUpper[] = "1234567890QWERTYUIOPASDFGHJKL_ZXCVBNM-@#";

int clockYear = 2026;
int clockMonth = 5;
int clockDay = 20;
int clockHour = 12;
int clockMinute = 0;
int clockSetupActiveIndex = 0;

int tempClockHour = 12;
int tempClockMinute = 0;
int tempClockDay = 23;
int tempClockMonth = 5;
int tempClockYear = 2026;
int clockTimeEditState = 0; // 0 = Hour, 1 = Minute
int clockDateEditState = 0; // 0 = Day, 1 = Month, 2 = Year
unsigned long previousMillisClockUpdate = 0;

int mainMenuIndex = 0;
bool forceDisplayRedraw = true;
bool isDeviceLinked = false;
volatile bool pendingPublishSettings = false;
int appUpdatesSubFocusIdx = 0;

int tempSubFocusIdx = 0;
int humidSubFocusIdx = 0;
int turnSubFocusIdx = 0;
int turnSetupSubFocusIdx = 0;
int backupSetupSubFocusIdx = 0;
int netDashboardFocusIdx = 0;
int simWifiSetupFocusIdx = 0;
int tempSimAndWifiMode = 2; // Default to Use SIM & WIFI after selection before saving
bool showRecountConfirmation = false;
unsigned long recountConfirmationEndMillis = 0;
bool isEditingValue = false;
bool backupActive = false;
int genStartAttempts = 0;
bool genFailState = false;
bool upsFailState = false;
bool invFailState = false;
int saveState = 0; // 0: IDLE, 1: SAVING, 2: SAVED, 3: FAILED
int saveProgress = 0;

bool manualTurnActive = false;
unsigned long manualTurnEndMillis = 0;
bool manualTurnUpward = true;

// Include submodules (headers in Arduino sketch folder)
#include "incubator_control.h"
#include "screen_ui.h"
#include "network_config.h"
#include "power_backup.h"

// Define global structures
SystemSettings currentSettings = { 
  DEFAULT_TARGET_TEMP, 
  DEFAULT_TARGET_HUMID, 
  DEFAULT_MIN_TEMP,
  DEFAULT_MAX_TEMP,
  DEFAULT_MIN_HUMID,
  DEFAULT_MAX_HUMID,
  DEFAULT_TURN_INTERVAL, 
  true, 
  23,
  5,
  DEFAULT_BACKUP_TYPE,
  DEFAULT_BACKUP_DURATION,
  DEFAULT_CHOKE_SERVO,
  DEFAULT_MAX_GEN_RETRIES,
  DEFAULT_INCUBATION_DAYS, // totalIncubationDays
  18,                     // stopTurningDay
  true,                    // use24HourFormat
  50,                     // chokeServoDegrees (50 degrees default)
  false,                  // backupSchedEnabled (Disabled by default)
  8,                      // backupSchedStartHour (8:00 default)
  16,                     // backupSchedEndHour (16:00 default)
  2,                      // simAndWifiMode (2: Use SIM & WIFI default)
  false,                  // isWifiConfigured (default false)
  ""                      // wifiSsid (default empty string)
};
SystemSettings tempSettings = currentSettings;
SensorReadings latestReadings = { 0.0f, 0.0f, 0.0f, 0, 0, false, true, true };
unsigned long previousMillisControl = 0;
unsigned long previousMillisTelemetry = 0;
uint32_t globalHatchElapsedSeconds = 0;

extern volatile int mqttStateCode;

// Hardware driver instantiations
Adafruit_SHT31 sht31 = Adafruit_SHT31();
OneWire oneWire(PIN_DS18B20);
DallasTemperature dsSensor(&oneWire);
ThreeWire rtcWiring(RTC_IO_PIN, RTC_SCLK_PIN, RTC_CE_PIN);
RtcDS1302<ThreeWire> RtcModule(rtcWiring);
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Servo chokeServo;

TaskHandle_t TaskCommHandle = NULL;
TaskHandle_t TaskIncubatorHandle = NULL;

void incubatorLoopStep(); // Forward declaration for core 0 worker loop

void TaskIncubatorCode(void * pvParameters) {
  for(;;) {
    incubatorLoopStep();
    vTaskDelay(pdMS_TO_TICKS(1)); // Relinquish slices dynamically to watchdogs
  }
}

void TaskCommCode(void * pvParameters) {
  for(;;) {
    checkNetworkAndReconnect();
    
    // Core 0/1 background thread safe telemetry publishing
    if (pendingTelemetryPublish) {
      pendingTelemetryPublish = false;
      if (mqttClient.connected()) {
        char payload[192];
        snprintf(payload, sizeof(payload), 
          "{\"temp\":%.1f,\"humidity\":%.1f,\"day\":%d,\"heater\":%s,\"humidifier\":%s,\"motor\":%s}",
          telemetryTemp, telemetryHum, telemetryDay,
          telemetryHeater ? "true" : "false", telemetryHumidifier ? "true" : "false", telemetryMotor ? "true" : "false"
        );
        mqttClient.publish("egg/incubator/" DEV_SERIAL_NUMBER "/telemetry", payload);
      }
    }

    // Core 0/1 background thread safe settings syncs
    if (pendingSettingsPublish) {
      pendingSettingsPublish = false;
      publishSettings();
    }

    processMqttLoop();
    cachedMqttConnected = mqttClient.connected();
    mqttStateCode = cachedMqttConnected ? 0 : mqttClient.state();
    vTaskDelay(pdMS_TO_TICKS(100)); // Relinquish control to watchdog
  }
}

void setup() {
  delay(1000); // Allow physical USB CDC connection to stabilize

  SPI.begin(TFT_SCLK, -1, TFT_SDA, TFT_CS);

  initTftDisplay();

  initControlSystem();

  initPowerBackupSystem(); // Configure pin modes for backup/generator controls

  loadSettingsFromPreferences();

  initButtonPins();
  
  bool sensorsOk = initSensorsAndRTC();

  initCommunications();

  // Create physical control thread pinned to Core 0 to keep climate regulation active and uninterruptible
  xTaskCreatePinnedToCore(
    TaskIncubatorCode, "TaskIncubator", 16384, NULL, 1, &TaskIncubatorHandle, 0
  );

  // Route GSM GPRS, WiFi, MQTT fallback connections over Core 1 (completely isolated from Core 0!)
  xTaskCreatePinnedToCore(
    TaskCommCode, "TaskComm", 16384, NULL, 1, &TaskCommHandle, 1
  );

  activeTftScreen = SCREEN_DASHBOARD;
  delay(1500); // Allow startup logo screen to display beautifully.
}

void incubatorLoopStep() {
  unsigned long currentMillis = millis();
  handleButtonTicks();
  updateBackupSystem();

  if (pendingPublishSettings) {
    pendingPublishSettings = false;
    pendingSettingsPublish = true; // Thread-safe proxy handoff to background task
  }

  // Momentary manual egg turning push-button control:
  // Operating with sub-millisecond physical response. 
  // Active only on dashboard when auto-turning is disabled.
  if (activeTftScreen == SCREEN_DASHBOARD && !currentSettings.turnEnabled) {
    bool upRaw = (digitalRead(BTN_UP) == LOW);
    bool downRaw = (digitalRead(BTN_DOWN) == LOW);
    
    if (upRaw || downRaw) {
      if (manualTurnActive) {
        manualTurnActive = false; // Cancel any background timed turn
      }
      
      if (upRaw && !downRaw) {
        // Rotate Up
        digitalWrite(RELAY_MOTOR_DOWN, LOW);
        delay(15);
        digitalWrite(RELAY_MOTOR_UP, HIGH);
      } else if (downRaw && !upRaw) {
        // Rotate Down
        digitalWrite(RELAY_MOTOR_UP, LOW);
        delay(15);
        digitalWrite(RELAY_MOTOR_DOWN, HIGH);
      } else {
        // Safety lock: both buttons pressed
        digitalWrite(RELAY_MOTOR_UP, LOW);
        digitalWrite(RELAY_MOTOR_DOWN, LOW);
      }
    } else {
      // Both buttons released
      if (!manualTurnActive) {
        digitalWrite(RELAY_MOTOR_UP, LOW);
        digitalWrite(RELAY_MOTOR_DOWN, LOW);
      }
    }
  }

  // Draw blinking Backup FAIL alert or Generator Cool-down / Stop Phase info instantly on dashboard
  bool anyFailActive = (currentSettings.backupType == 2 && genFailState) || 
                       (currentSettings.backupType == 0 && upsFailState) || 
                       (currentSettings.backupType == 1 && invFailState);
  bool showStoppingSeq = (currentSettings.backupType == 2 && backupState == STATE_STOPPING_BACKUP && genStopPhase > 0);
  
  if (activeTftScreen == SCREEN_DASHBOARD) {
    static bool lastAnyFailActive = false;
    static bool lastShowStoppingSeq = false;
    static int lastRemSec = -1;
    static int lastStopPhase = -1;
    
    if (showStoppingSeq) {
      if (!lastShowStoppingSeq) {
        lastRemSec = -1;
        lastStopPhase = -1;
        tft.fillRect(264, 7, 48, 16, ST77XX_BLACK);
      }
      
      tft.setFont(NULL);
      tft.setTextSize(1);
      
      if (genStopPhase == 1) {
        int remSec = 20 - (int)((currentMillis - genStopPhaseMillis) / 1000);
        if (remSec < 0) remSec = 0;
        if (remSec > 20) remSec = 20;
        
        if (remSec != lastRemSec) {
          tft.fillRect(264, 7, 48, 16, ST77XX_BLACK);
          tft.setCursor(264, 11);
          tft.setTextColor(ST77XX_BLUE, ST77XX_BLACK);
          tft.printf("%dsec", remSec);
          lastRemSec = remSec;
        }
      } else {
        // Phase 2 or 3: GEN KILL
        if (genStopPhase != lastStopPhase) {
          tft.fillRect(264, 7, 48, 16, ST77XX_BLACK);
          tft.setCursor(264, 11);
          tft.setTextColor(ST77XX_BLUE, ST77XX_BLACK);
          tft.print("GEN KILL");
          lastStopPhase = genStopPhase;
        }
      }
    } else if (anyFailActive) {
      static unsigned long lastBlinkTime = 0;
      if (currentMillis - lastBlinkTime >= 500) {
        lastBlinkTime = currentMillis;
        bool showBlink = (currentMillis / 500) % 2 == 0;
        tft.setFont(NULL);
        tft.setTextSize(1);
        tft.setCursor(264, 11);
        if (showBlink) {
          tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
          if (currentSettings.backupType == 2 && genFailState) {
            tft.print("GEN FAIL");
          } else if (currentSettings.backupType == 0 && upsFailState) {
            tft.print("UPS FAIL");
          } else if (currentSettings.backupType == 1 && invFailState) {
            tft.print("INV FAIL");
          }
        } else {
          tft.fillRect(264, 7, 48, 16, ST77XX_BLACK);
        }
      }
    } else if (lastAnyFailActive || lastShowStoppingSeq) {
      // Clean residual text from top bar immediately when any fail state stops or stopping sequence concludes
      tft.fillRect(264, 7, 48, 16, ST77XX_BLACK);
      lastRemSec = -1;
      lastStopPhase = -1;
    }
    
    lastAnyFailActive = anyFailActive;
    lastShowStoppingSeq = showStoppingSeq;
  }

  // Asynchronous manual motor turning cut-off safety guard
  if (manualTurnActive && currentMillis >= manualTurnEndMillis) {
    digitalWrite(RELAY_MOTOR_UP, LOW); 
    digitalWrite(RELAY_MOTOR_DOWN, LOW); 
    manualTurnActive = false;
  }

  if (currentMillis - previousMillisControl >= 1000) {
    previousMillisControl = currentMillis;
    
    // Poll all local climate sensors
    latestReadings = pollAllSensors(0);

    // Dynamic temperature and humidity microclimate regulators (bulb / ultrasonic mist relay)
    bool heaterState = false;
    bool humidifierState = false;
    updateClimateRegulation(latestReadings.temperature, latestReadings.humidity, heaterState, humidifierState);

    // Motor turning countdown ticks
    bool turningState = false;
    globalHatchElapsedSeconds += 1;
    processEggTurning(globalHatchElapsedSeconds, turningState);

    // Screen UI display frame update
    if (activeTftScreen == SCREEN_DASHBOARD) {
      int daysRemaining = DEFAULT_INCUBATION_DAYS - latestReadings.dayOfCycle;
      drawMainDashboard(
        latestReadings.temperature,
        latestReadings.humidity,
        daysRemaining >= 0 ? daysRemaining : 0,
        currentSettings.targetTemp,
        currentSettings.targetHumidity,
        (activeLink == NET_WIFI),
        (activeLink == NET_GSM),
        getMqttConnectedState(),
        turningState
      );
    } else if (activeTftScreen == SCREEN_MENU_TIME && saveState == 0) {
      drawLiveClockDisplay();
    } else if (activeTftScreen == SCREEN_MENU_NET_SCAN && wifiScanCount == -1) {
      int16_t nObj = WiFi.scanComplete();
      if (nObj == WIFI_SCAN_RUNNING) {
        static unsigned long lastScanAnimate = 0;
        if (currentMillis - lastScanAnimate > 30) {
          lastScanAnimate = currentMillis;
          drawWifiScannerScreen(); // Increments angleIdx and draws rotating spinner
        }
      } else if (nObj >= 0) {
        wifiScanCount = (nObj < 0) ? 0 : ((nObj > 10) ? 10 : nObj);
        for (int i = 0; i < wifiScanCount; i++) {
          strncpy(wifiScanBuffer[i], WiFi.SSID(i).c_str(), 32);
          wifiScanBuffer[i][32] = 0;
          wifiScanRssi[i] = WiFi.RSSI(i);
        }
        isScanningWifi = false; // complete
        extern bool needNetScanFullRedraw;
        needNetScanFullRedraw = true;
        drawWifiScannerScreen();
      } else {
        wifiScanCount = 0;
        isScanningWifi = false;
        extern bool needNetScanFullRedraw;
        needNetScanFullRedraw = true;
        drawWifiScannerScreen();
      }
    } else if (activeTftScreen == SCREEN_WIFI_CONNECTING) {
      static unsigned long lastAnimate = 0;
      if (currentMillis - lastAnimate > 30) {
        lastAnimate = currentMillis;
        drawWifiConnectingScreen(); // Update rotating circle animation
      }
      
      // Check connection status
      if (WiFi.status() == WL_CONNECTED) {
        currentSettings.isWifiConfigured = true;
        strncpy(currentSettings.wifiSsid, selectedSsid, 32);
        currentSettings.wifiSsid[32] = 0;
        
        saveSettingsToPreferences(currentSettings); // Keep SSID credentials in memory
        
        drawWifiConnectionStatusScreen(true);
        delay(2000);
        
        activeTftScreen = SCREEN_NET_DASHBOARD;
        netDashboardFocusIdx = 0;
        drawNetworkDashboardScreen();
      } else if (currentMillis - wifiConnectStartTime > 15000) {
        // Timeout after 15 seconds!
        drawWifiConnectionStatusScreen(false);
        delay(2500);
        
        activeTftScreen = SCREEN_NET_DASHBOARD;
        netDashboardFocusIdx = 0;
        drawNetworkDashboardScreen();
      }
    }

    // Telemetry Sync with HiveMQ Cloud (Thread-safe background queue handoff)
    if (currentMillis - previousMillisTelemetry >= 10000) {
      previousMillisTelemetry = currentMillis;
      telemetryTemp = latestReadings.temperature;
      telemetryHum = latestReadings.humidity;
      telemetryDay = latestReadings.dayOfCycle;
      telemetryHeater = heaterState;
      telemetryHumidifier = humidifierState;
      telemetryMotor = turningState;
      pendingTelemetryPublish = true; // Signals background thread to build payload and publish!
    }
  }
  yield();
}

void loop() {
  vTaskDelete(NULL); // Delete default Arduino loop task as logic resides on Core 0 & Core 1 tasks
}