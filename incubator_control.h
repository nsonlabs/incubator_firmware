/**
 * @file incubator_control.h
 * @brief DS18B20 digital Temp, SHT31 Humid, Relays, RTC Calendars & Non-Volatile Flash preference records
 */

#ifndef INCUBATOR_CONTROL_H
#define INCUBATOR_CONTROL_H

#include <Preferences.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <ESP32Servo.h>

extern DallasTemperature dsSensor;
extern Adafruit_SHT31 sht31;
extern RtcDS1302<ThreeWire> RtcModule;
extern Servo chokeServo;

Preferences preferences;

// Instantiate sub-libs manually or reference
bool initSensorsAndRTC() {
  Wire.begin(PIN_SDA, PIN_SCL);

  bool sht31Ok = sht31.begin(0x44); // Standard SHT31 Address

  dsSensor.begin();
  dsSensor.setWaitForConversion(false); // Disable asynchronous blocking waiting

  RtcModule.Begin();
  
  if (!RtcModule.IsDateTimeValid()) {
    // Set factory compilation start reference if battery cell was fully discharged
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    RtcModule.SetDateTime(compiled);
  }
  
  return sht31Ok;
}

void initControlSystem() {
  pinMode(RELAY_HEATER, OUTPUT);
  pinMode(RELAY_HUMID, OUTPUT);
  pinMode(RELAY_MOTOR_UP, OUTPUT);
  pinMode(RELAY_MOTOR_DOWN, OUTPUT);
  
  pinMode(PIN_BACKUP_START, OUTPUT);
  pinMode(PIN_SOURCE_SELECT, OUTPUT);
  pinMode(PIN_MAINS_SENSE, INPUT_PULLUP);
  
  // Active High relays - Keep off during system booting
  digitalWrite(RELAY_HEATER, LOW); 
  digitalWrite(RELAY_HUMID, LOW);
  digitalWrite(RELAY_MOTOR_UP, LOW);
  digitalWrite(RELAY_MOTOR_DOWN, LOW);

  digitalWrite(PIN_BACKUP_START, LOW);
  digitalWrite(PIN_SOURCE_SELECT, LOW); // Source default to mains (LOW)

  #if defined(ESP32)
  chokeServo.attach(PIN_CHOKE_SERVO);
  chokeServo.write(0); // Return choke to normal idle run position
  #endif
}

void loadSettingsFromPreferences() {
  preferences.begin("incubator", true); // Open flash namespace
  currentSettings.targetTemp = preferences.getFloat("target_temp", DEFAULT_TARGET_TEMP);
  currentSettings.targetHumidity = preferences.getFloat("target_humid", DEFAULT_TARGET_HUMID);
  currentSettings.minTemp = preferences.getFloat("min_temp", DEFAULT_MIN_TEMP);
  currentSettings.maxTemp = preferences.getFloat("max_temp", DEFAULT_MAX_TEMP);
  currentSettings.minHumidity = preferences.getFloat("min_humid", DEFAULT_MIN_HUMID);
  currentSettings.maxHumidity = preferences.getFloat("max_humid", DEFAULT_MAX_HUMID);
  int loadedVal = preferences.getInt("turn_int", DEFAULT_TURN_INTERVAL);
  if (loadedVal <= 10) {
    currentSettings.turnIntervalHours = loadedVal * 60;
  } else {
    currentSettings.turnIntervalHours = loadedVal;
  }
  currentSettings.turnEnabled = preferences.getBool("turn_en", true);
  currentSettings.incubationDayStarted = preferences.getInt("day_start", 23);
  currentSettings.incubationMonthStarted = preferences.getInt("mon_start", 5);
  currentSettings.backupType = preferences.getInt("bkup_type", DEFAULT_BACKUP_TYPE);
  currentSettings.backupPressDurationSec = preferences.getFloat("bkup_dur", DEFAULT_BACKUP_DURATION);
  currentSettings.chokeServoEnabled = preferences.getBool("choke_en", true);
  currentSettings.maxGenStartRetries = preferences.getInt("gen_retry", DEFAULT_MAX_GEN_RETRIES);
  currentSettings.totalIncubationDays = preferences.getInt("inc_days", DEFAULT_INCUBATION_DAYS);
  currentSettings.stopTurningDay = preferences.getInt("stop_day", 18);
  currentSettings.use24HourFormat = preferences.getBool("use_24h", true);
  currentSettings.chokeServoDegrees = preferences.getInt("choke_deg", 50);
  currentSettings.backupSchedEnabled = preferences.getBool("sched_en", false);
  currentSettings.backupSchedStartHour = preferences.getInt("sched_start", 8);
  currentSettings.backupSchedEndHour = preferences.getInt("sched_end", 16);
  currentSettings.isWifiConfigured = preferences.getBool("wifi_cfg", false);
  currentSettings.simAndWifiMode = preferences.getInt("sim_wifi_mode", 2); // default to 2 (GSM + WIFI hybrid fallback)
  isDeviceLinked = preferences.getBool("dev_linked", false);
  String ssid = preferences.getString("wifi_ssid", "");
  strncpy(currentSettings.wifiSsid, ssid.c_str(), 32);
  currentSettings.wifiSsid[32] = 0;
  
  if (currentSettings.isWifiConfigured) {
    strncpy(selectedSsid, currentSettings.wifiSsid, 32);
    selectedSsid[32] = 0;
    String pass = preferences.getString("wifi_pass", "");
    strncpy(typedPassword, pass.c_str(), 63);
    typedPassword[63] = 0;
  } else {
    strcpy(selectedSsid, "None Selected");
    strcpy(typedPassword, "");
  }
  preferences.end();
}

void saveSettingsToPreferences(SystemSettings newSet) {
  preferences.begin("incubator", false);
  preferences.putFloat("target_temp", newSet.targetTemp);
  preferences.putFloat("target_humid", newSet.targetHumidity);
  preferences.putFloat("min_temp", newSet.minTemp);
  preferences.putFloat("max_temp", newSet.maxTemp);
  preferences.putFloat("min_humid", newSet.minHumidity);
  preferences.putFloat("max_humid", newSet.maxHumidity);
  preferences.putInt("turn_int", newSet.turnIntervalHours);
  preferences.putBool("turn_en", newSet.turnEnabled);
  preferences.putInt("day_start", newSet.incubationDayStarted);
  preferences.putInt("mon_start", newSet.incubationMonthStarted);
  preferences.putInt("bkup_type", newSet.backupType);
  preferences.putFloat("bkup_dur", newSet.backupPressDurationSec);
  preferences.putBool("choke_en", newSet.chokeServoEnabled);
  preferences.putInt("gen_retry", newSet.maxGenStartRetries);
  preferences.putInt("inc_days", newSet.totalIncubationDays);
  preferences.putInt("stop_day", newSet.stopTurningDay);
  preferences.putBool("use_24h", newSet.use24HourFormat);
  preferences.putInt("choke_deg", newSet.chokeServoDegrees);
  preferences.putBool("sched_en", newSet.backupSchedEnabled);
  preferences.putInt("sched_start", newSet.backupSchedStartHour);
  preferences.putInt("sched_end", newSet.backupSchedEndHour);
  preferences.putBool("wifi_cfg", newSet.isWifiConfigured);
  preferences.putInt("sim_wifi_mode", newSet.simAndWifiMode);
  preferences.putString("wifi_ssid", String(newSet.wifiSsid));
  preferences.putString("wifi_pass", String(typedPassword));
  preferences.putBool("dev_linked", isDeviceLinked);
  preferences.end();
}

int daysInMonth(int m, int y) {
  if (m == 2) {
    if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) return 29;
    return 28;
  }
  if (m == 4 || m == 6 || m == 9 || m == 11) return 30;
  return 31;
}

long absoluteDays(int y, int m, int d) {
  long days = d;
  for (int yr = 2000; yr < y; yr++) {
    days += (yr % 4 == 0) ? 366 : 365;
  }
  for (int mn = 1; mn < m; mn++) {
    days += daysInMonth(mn, y);
  }
  return days;
}

SensorReadings pollAllSensors(uint32_t currentEpochDay) {
  SensorReadings data;
  
  float dallasTemp = dsSensor.getTempCByIndex(0);
  dsSensor.requestTemperatures(); // Async trigger next cycle Dallas conversion (takes 0ms!)
  
  float sht31Humid = NAN;
  float sht31Temp = NAN;
  
  // 1. Guard SHT31 reading with an ultra-safe I2C check with active retry logic for transient EMI/noise resilience
  for (int retry = 0; retry < 3; retry++) {
    Wire.beginTransmission(0x44);
    if (Wire.endTransmission() == 0) {
      sht31Humid = sht31.readHumidity();
      sht31Temp = sht31.readTemperature();
      if (!isnan(sht31Humid) && !isnan(sht31Temp)) {
        break; // Successful read
      }
    }
    delay(15); // Short pause to let I2C bus recover from potential EMI switching transients
  }

  // Guard against NAN readings from disconnected devices to prevent CPU exceptions or watchdog reboots!
  if (isnan(sht31Humid)) sht31Humid = -1.0f;
  if (isnan(sht31Temp)) sht31Temp = -1.0f;
  if (isnan(dallasTemp)) dallasTemp = -127.0f;

  // 2. Continuous software filters for transient physical disconnects and high-temperature transients
  // (Prevents momentary noise spikes from heaters/motors turning on from triggering spurious "Err" screen states)
  static float lastGoodDallasTemp = 37.5f;
  static float lastGoodShtTemp = 37.5f;
  static float lastGoodShtHumid = DEFAULT_TARGET_HUMID;
  static int consecutiveDallasFailures = 0;
  static int consecutiveShtFailures = 0;

  bool currentDallasValid = (dallasTemp > -50.0f && dallasTemp < 85.0f && dallasTemp != -127.0f);
  if (currentDallasValid) {
    lastGoodDallasTemp = dallasTemp;
    consecutiveDallasFailures = 0;
  } else {
    consecutiveDallasFailures++;
  }

  bool currentShtValid = (sht31Humid > 0.0f && sht31Humid <= 100.0f && sht31Temp > -50.0f && sht31Temp < 85.0f);
  if (currentShtValid) {
    lastGoodShtTemp = sht31Temp;
    lastGoodShtHumid = sht31Humid;
    consecutiveShtFailures = 0;
  } else {
    consecutiveShtFailures++;
  }

  // Sensors are only marked offline/unhealthy after 3 consecutive failed read iterations (approx 3 seconds)
  bool tHealthy = (consecutiveDallasFailures < 3);
  bool hHealthy = (consecutiveShtFailures < 3);

  data.tempSensorHealthy = tHealthy;
  data.humidSensorHealthy = hHealthy;

  // Primary reading is DS18B20 digital thermal probe, backup fallback is SHT31 on-board Celsius, last-resort is safe default
  data.temperature = tHealthy ? lastGoodDallasTemp : (hHealthy ? lastGoodShtTemp : 37.5f);
  data.coreTemperature = lastGoodDallasTemp;
  data.humidity = hHealthy ? lastGoodShtHumid : DEFAULT_TARGET_HUMID;
  
  // Calculate incubation progress via battery-backed RTC
  RtcDateTime now = RtcModule.GetDateTime();
  bool isFuture = false;
  if (currentSettings.incubationMonthStarted > 0) {
    if (now.Month() < currentSettings.incubationMonthStarted) {
      isFuture = true;
    } else if (now.Month() == currentSettings.incubationMonthStarted && now.Day() < currentSettings.incubationDayStarted) {
      isFuture = true;
    }
  }

  if (isFuture) {
    data.dayOfCycle = 0; // 0 represents future start date
  } else {
    long daysNow = absoluteDays(now.Year(), now.Month(), now.Day());
    long daysStart = absoluteDays(now.Year(), currentSettings.incubationMonthStarted, currentSettings.incubationDayStarted);
    data.dayOfCycle = (daysNow - daysStart) + 1;
    if (data.dayOfCycle < 1) data.dayOfCycle = 1;
  }
  
  data.minutesPassed = now.Minute();
  data.isSensorsHealthy = tHealthy && hHealthy;
  
  return data;
}

void updateClimateRegulation(float currentTemp, float currentHumidity, bool &heaterRelay, bool &humidRelay) {
  // Hysteresis climate control between min and max range limits
  if (currentTemp < currentSettings.minTemp) {
    digitalWrite(RELAY_HEATER, HIGH); // heater relay ON (Active HIGH)
    heaterRelay = true;
  } else if (currentTemp >= currentSettings.maxTemp) {
    digitalWrite(RELAY_HEATER, LOW);  // heater relay OFF
    heaterRelay = false;
  }

  // Hysteresis humidification control between min and max range limits
  if (currentHumidity < currentSettings.minHumidity) {
    digitalWrite(RELAY_HUMID, HIGH); // humidifier relay ON (Active HIGH)
    humidRelay = true;
  } else if (currentHumidity >= currentSettings.maxHumidity) {
    digitalWrite(RELAY_HUMID, LOW);  // humidifier relay OFF
    humidRelay = false;
  }
}

extern bool manualTurnActive;
extern unsigned long manualTurnEndMillis;
extern bool manualTurnUpward;

bool processEggTurning(uint32_t elapsedSeconds, bool &turnRelayActive) {
  // If stop turning day is configured, check if we have reached/passed it:
  if (currentSettings.stopTurningDay > 0 && latestReadings.dayOfCycle >= currentSettings.stopTurningDay) {
    if (currentSettings.turnEnabled) {
      currentSettings.turnEnabled = false;
      saveSettingsToPreferences(currentSettings); // persist automatic disable!
    }
  }

  if (!currentSettings.turnEnabled) {
    // In manual mode, auto-turning is fully disabled.
    // If a manual override trigger is active (e.g., from web API):
    if (manualTurnActive) {
      if (millis() < manualTurnEndMillis) {
        if (manualTurnUpward) {
          digitalWrite(RELAY_MOTOR_DOWN, LOW);
          delay(15);
          digitalWrite(RELAY_MOTOR_UP, HIGH);
        } else {
          digitalWrite(RELAY_MOTOR_UP, LOW);
          delay(15);
          digitalWrite(RELAY_MOTOR_DOWN, HIGH);
        }
        turnRelayActive = true;
        return true;
      } else {
        digitalWrite(RELAY_MOTOR_UP, LOW);
        digitalWrite(RELAY_MOTOR_DOWN, LOW);
        manualTurnActive = false;
        turnRelayActive = false;
      }
    } else {
      // Ensure motors are off if manual turning is not active
      digitalWrite(RELAY_MOTOR_UP, LOW);
      digitalWrite(RELAY_MOTOR_DOWN, LOW);
      turnRelayActive = false;
    }
    return false;
  }

  // Auto Mode Egg Rotation State Machine
  // Calculate interval in seconds. turnIntervalHours represents minutes (30 to 300)
  uint32_t intervalSeconds = (uint32_t)currentSettings.turnIntervalHours * 60;
  if (intervalSeconds == 0) {
    intervalSeconds = 120 * 60; // Safeguard fallback to 2 hours
  }
  
  uint32_t cycleIndex = elapsedSeconds / intervalSeconds;

  // Swap direction relays every interval
  if (cycleIndex % 2 == 0) {
    // Motor UP is active, Motor DOWN is off
    digitalWrite(RELAY_MOTOR_DOWN, LOW);
    delay(15);
    digitalWrite(RELAY_MOTOR_UP, HIGH);
  } else {
    // Motor DOWN is active, Motor UP is off
    digitalWrite(RELAY_MOTOR_UP, LOW);
    delay(15);
    digitalWrite(RELAY_MOTOR_DOWN, HIGH);
  }

  turnRelayActive = true;
  return true;
}

void triggerManualEggTurn(bool turnUpward) {
  manualTurnUpward = turnUpward;
  if (turnUpward) {
    digitalWrite(RELAY_MOTOR_DOWN, LOW);
    delay(15);
    digitalWrite(RELAY_MOTOR_UP, HIGH);
  } else {
    digitalWrite(RELAY_MOTOR_UP, LOW);
    delay(15);
    digitalWrite(RELAY_MOTOR_DOWN, HIGH);
  }
  
  manualTurnActive = true;
  manualTurnEndMillis = millis() + 5000; // Track 5s manual rotation
}

#endif // INCUBATOR_CONTROL_H