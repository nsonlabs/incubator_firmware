/**
 * @file power_backup.h
 * @brief Handles smart Backup State Machine transitions, Generator Choke action & Starter Retries.
 * 
 * 💡 DESIGN PRINCIPLES:
 * 1. Dual-sensing system for zero-cross detection of Mains AC stability.
 * 2. Automated cold pull choke control using high-torque SG90 servo motor.
 * 3. Intelligent starter pulse limit algorithm to safeguard starter battery health.
 */

#ifndef POWER_BACKUP_H
#define POWER_BACKUP_H

#include <Arduino.h>
#include <ESP32Servo.h>

// Forward declarations of globals from main sketch or other systems
extern SystemSettings currentSettings;
extern Servo chokeServo;
extern bool backupActive;
extern int genStartAttempts;
extern bool genFailState;
extern bool upsFailState;
extern bool invFailState;
extern int clockHour;

// Backup system state parameters
enum BackupState {
  STATE_MAINS,                  // Running on Main Grid
  STATE_STARTING_BACKUP,        // Cranking starter relay pulse
  STATE_COOL_DOWN_RETRY,        // Resting starter motor before retrying
  STATE_RUNNING_BACKUP,         // Backup running stably, load connected
  STATE_STOPPING_BACKUP,        // Mains restored, cooling down backup and stopping
  STATE_LAST_RETRY_WAIT         // Wait for 3 seconds after final retry before declaring failure
};

// Global variables defined in power_backup module
BackupState backupState = STATE_MAINS;
unsigned long backupStateMillis = 0;
int genStopPhase = 0; // 0=None, 1=20s Cool-down, 2=Kill active (wait low), 3=Post-kill (10s extension)
unsigned long genStopPhaseMillis = 0;

bool isTimeInSchedWindow() {
  if (!currentSettings.backupSchedEnabled) return false;
  int h = clockHour;
  int start = currentSettings.backupSchedStartHour;
  int end = currentSettings.backupSchedEndHour;
  if (start < end) {
    return (h >= start && h < end);
  } else if (start > end) {
    return (h >= start || h < end);
  } else {
    return true; // if they are equal, treat as 24h
  }
}

void initPowerBackupSystem() {
  pinMode(PIN_BACKUP_START, OUTPUT);
  pinMode(PIN_SOURCE_SELECT, OUTPUT);
  pinMode(PIN_GEN_KILL, OUTPUT);
  pinMode(PIN_BACKUP_SENSE, INPUT_PULLUP);
  pinMode(PIN_MAINS_SENSE, INPUT_PULLUP);
  
  digitalWrite(PIN_BACKUP_START, LOW);
  digitalWrite(PIN_SOURCE_SELECT, LOW);
  digitalWrite(PIN_GEN_KILL, LOW);
  
  if (currentSettings.backupType == 2 && currentSettings.chokeServoEnabled) {
    chokeServo.write(0); // De-assert choke pull
  }
}

void updateBackupSystem() {
  unsigned long currentMillis = millis();
  static bool lastBackupDemanded = false;
  static unsigned long backupSensedContinuousMillis = 0;
  static bool genPreCrankWaiting = false;
  
  // Real-time Power Sensing (using pullup sensor pin PIN_MAINS_SENSE)
  // High = Mains OK, Low = Power Outage / Out of service
  bool mainsIsHealthy = (digitalRead(PIN_MAINS_SENSE) == HIGH);
  bool backupIsSensed = (digitalRead(PIN_BACKUP_SENSE) == HIGH);
  bool inSched = isTimeInSchedWindow();
  
  // Backup is demanded if mains is failed OR scheduled window is active
  bool backupIsDemanded = (!mainsIsHealthy || inSched);

  // Proactive non-blocking check: if backup is demanded (mains failed or in schedule)
  // and we sense backup power, always bypass/abort starting or errors, clear failures, and run directly on backup!
  if (backupIsDemanded && backupIsSensed && backupState == STATE_MAINS) {
    if (currentSettings.backupType == 0 || currentSettings.backupType == 2) {
      digitalWrite(PIN_BACKUP_START, LOW);
      if (currentSettings.backupType == 2) {
        digitalWrite(PIN_GEN_KILL, LOW); // Explicitly ensure kill is off when running
      }
    }
    genFailState = false;
    upsFailState = false;
    invFailState = false;
    genStartAttempts = 0;
    backupActive = true;
    
    // We do NOT set PIN_SOURCE_SELECT high immediately; let the 10s stabilization logic inside running do it!
    digitalWrite(PIN_SOURCE_SELECT, LOW);
    backupStateMillis = currentMillis;
    backupSensedContinuousMillis = currentMillis; // Initialize continuous sense
    backupState = STATE_RUNNING_BACKUP;
  }

  switch (backupState) {
    case STATE_MAINS:
      if (backupIsDemanded) {
        // Recovery check: if the backup source has power sensed while mains are failed/unavailable, recover instantly!
        if (backupIsSensed) {
          genFailState = false;
          upsFailState = false;
          invFailState = false;
          genStartAttempts = 0;
          backupActive = true;
          
          // Let 10s stabilization do the SELECT switch to HIGH safely
          digitalWrite(PIN_SOURCE_SELECT, LOW);
          backupStateMillis = currentMillis;
          backupSensedContinuousMillis = currentMillis;
          backupState = STATE_RUNNING_BACKUP;
          break;
        }

        // Track fresh demand state transition, reset fails
        if (!lastBackupDemanded) {
          genStartAttempts = 0;
          genFailState = false;
          upsFailState = false;
          invFailState = false;
        }
        lastBackupDemanded = true;

        // Skip starting if persistent failure is already present for the current type
        bool currentTypeHasFailed = (currentSettings.backupType == 2 && genFailState) ||
                                     (currentSettings.backupType == 0 && upsFailState) ||
                                     (currentSettings.backupType == 1 && invFailState);
                                     
        if (!currentTypeHasFailed) {
          // Keep PIN_SOURCE_SELECT low initially to stay on Mains power side during cranking/starting
          digitalWrite(PIN_SOURCE_SELECT, LOW);
          backupStateMillis = currentMillis;
          backupSensedContinuousMillis = 0;
          
          if (currentSettings.backupType == 2) {
            digitalWrite(PIN_GEN_KILL, LOW); // Explicitly deactivate kill switch when cranking starts
            if (currentSettings.chokeServoEnabled) {
              chokeServo.write(currentSettings.chokeServoDegrees); // cold pull choke to user defined degrees
              genPreCrankWaiting = true;
              digitalWrite(PIN_BACKUP_START, LOW); // Stay low for 5 seconds waiting
            } else {
              genPreCrankWaiting = false;
              digitalWrite(PIN_BACKUP_START, HIGH);
            }
          } else if (currentSettings.backupType == 0) {
            // UPS: Start crank pulse immediately
            digitalWrite(PIN_BACKUP_START, HIGH);
          } else {
            // Inverter: Keep PIN_BACKUP_START high all through
            digitalWrite(PIN_BACKUP_START, HIGH);
          }
          
          backupState = STATE_STARTING_BACKUP;
        }
      } else {
        lastBackupDemanded = false;
        genStartAttempts = 0;
        genFailState = false;
        upsFailState = false;
        invFailState = false;
      }
      break;

    case STATE_STARTING_BACKUP: {
      unsigned long elapsed = currentMillis - backupStateMillis;
      
      // If demand ends (e.g. mains restored), immediately abort starting!
      if (!backupIsDemanded) {
        digitalWrite(PIN_BACKUP_START, LOW);
        if (currentSettings.backupType == 2) {
          digitalWrite(PIN_GEN_KILL, LOW);
          if (currentSettings.chokeServoEnabled) {
            chokeServo.write(0); // return choke back to running position
          }
        }
        digitalWrite(PIN_SOURCE_SELECT, LOW);
        backupActive = false;
        genStartAttempts = 0;
        backupState = STATE_MAINS;
        break;
      }
      
      // For Inverter, if backup is sensed, transition immediately
      if (currentSettings.backupType == 1 && backupIsSensed) {
        genFailState = false;
        upsFailState = false;
        invFailState = false;
        genStartAttempts = 0;
        backupActive = true;
        digitalWrite(PIN_SOURCE_SELECT, LOW); // Kept low, let running state turn high after 10s
        backupStateMillis = currentMillis;
        backupSensedContinuousMillis = currentMillis; // Start counting continuous sense
        backupState = STATE_RUNNING_BACKUP;
        break;
      }
      
      if (currentSettings.backupType == 0) {
        // UPS starting logic:
        // Keep PIN_BACKUP_START high for full user direct set duration
        if (elapsed < (currentSettings.backupPressDurationSec * 1000)) {
          digitalWrite(PIN_BACKUP_START, HIGH);
        } else {
          digitalWrite(PIN_BACKUP_START, LOW);
          
          // Now we wait for up to 10 seconds (from the beginning) for PIN_BACKUP_SENSE to go high
          if (backupIsSensed) {
            upsFailState = false;
            backupActive = true;
            digitalWrite(PIN_SOURCE_SELECT, LOW); // Kept low, raised in RUNNING after 10s continuous
            backupStateMillis = currentMillis;
            backupSensedContinuousMillis = currentMillis;
            backupState = STATE_RUNNING_BACKUP;
            break;
          }
        }
        
        // If 10 seconds of waiting without sense has elapsed, transition to failure
        if (elapsed >= 10000) {
          upsFailState = true;
          digitalWrite(PIN_BACKUP_START, LOW);
          digitalWrite(PIN_SOURCE_SELECT, LOW);
          backupState = STATE_MAINS;
        }
      } else if (currentSettings.backupType == 2) {
        // Generator starting logic:
        if (currentSettings.chokeServoEnabled && genPreCrankWaiting) {
          // Waiting 5 seconds with choke active BEFORE cranking
          digitalWrite(PIN_BACKUP_START, LOW);
          if (elapsed >= 5000) {
            genPreCrankWaiting = false;
            backupStateMillis = currentMillis; // Reset timer so elapsed is cranking duration
            digitalWrite(PIN_BACKUP_START, HIGH);
          }
        } else {
          // Generator cranking duration: stay high for the full amount of seconds set by the user
          if (elapsed < (currentSettings.backupPressDurationSec * 1000)) {
            digitalWrite(PIN_BACKUP_START, HIGH);
          } else {
            digitalWrite(PIN_BACKUP_START, LOW);
            // Do NOT move the servo back to 0 here! Kept at choke position until 10s of stable run
            
            // Check if generator started after crank finished
            if (backupIsSensed) {
              backupActive = true;
              genFailState = false;
              genStartAttempts = 0;
              digitalWrite(PIN_SOURCE_SELECT, LOW); // Kept low, raised in RUNNING after 10s continuous
              backupStateMillis = currentMillis;
              backupSensedContinuousMillis = currentMillis;
              backupState = STATE_RUNNING_BACKUP;
            } else {
              // Starting failed
              genStartAttempts++;
              if (genStartAttempts >= currentSettings.maxGenStartRetries) {
                // Exhausted retries
                backupStateMillis = currentMillis;
                backupState = STATE_LAST_RETRY_WAIT;
              } else {
                // Cool down/pause for 6 seconds without blocking
                backupStateMillis = currentMillis;
                backupState = STATE_COOL_DOWN_RETRY;
              }
            }
          }
        }
      } else {
        // Inverter: Keep PIN_BACKUP_START high all through
        digitalWrite(PIN_BACKUP_START, HIGH);
        
        // Monitor for some 10 seconds before indicating INV FAIL!
        if (elapsed >= 10000) {
          invFailState = true;
          // Note: we DO NOT put PIN_BACKUP_START low and DO NOT change state to STATE_MAINS.
          // This keeps PIN_BACKUP_START HIGH continuously as far as PIN_MAINS_SENSE is low,
          // displaying the blinking INV FAIL error. If backupIsSensed goes high later,
          // the check at the top of the case handles transitioning to RUNNING.
        }
      }
      break;
    }

    case STATE_COOL_DOWN_RETRY:
      // If demand ends (e.g. mains restored), immediately abort!
      if (!backupIsDemanded) {
        digitalWrite(PIN_BACKUP_START, LOW);
        if (currentSettings.backupType == 2) {
          digitalWrite(PIN_GEN_KILL, LOW);
          if (currentSettings.chokeServoEnabled) {
            chokeServo.write(0);
          }
        }
        digitalWrite(PIN_SOURCE_SELECT, LOW);
        backupActive = false;
        genStartAttempts = 0;
        backupState = STATE_MAINS;
        break;
      }

      // Constant Check: during pauses check if PIN_BACKUP_SENSE went high
      if (backupIsSensed) {
        genFailState = false;
        upsFailState = false;
        invFailState = false;
        genStartAttempts = 0;
        backupActive = true;
        digitalWrite(PIN_SOURCE_SELECT, LOW); // Kept low, let running state turn high after 10s
        backupStateMillis = currentMillis;
        backupSensedContinuousMillis = currentMillis;
        backupState = STATE_RUNNING_BACKUP;
        break;
      }
      
      if (currentMillis - backupStateMillis >= 6000) { // Pause around 6 seconds
        backupStateMillis = currentMillis;
        if (currentSettings.backupType == 2) {
          digitalWrite(PIN_GEN_KILL, LOW); // Explicitly ensure kill is off
          if (currentSettings.chokeServoEnabled) {
            chokeServo.write(currentSettings.chokeServoDegrees); // cold pull choke
            genPreCrankWaiting = true;
            digitalWrite(PIN_BACKUP_START, LOW); // Wait 5 seconds
          } else {
            genPreCrankWaiting = false;
            digitalWrite(PIN_BACKUP_START, HIGH);
          }
        } else {
          genPreCrankWaiting = false;
          digitalWrite(PIN_BACKUP_START, HIGH);
        }
        backupState = STATE_STARTING_BACKUP;
      }
      break;

    case STATE_LAST_RETRY_WAIT: {
      unsigned long elapsed = currentMillis - backupStateMillis;
      
      // If demand ends (e.g. mains restored), immediately abort!
      if (!backupIsDemanded) {
        digitalWrite(PIN_BACKUP_START, LOW);
        if (currentSettings.backupType == 2) {
          digitalWrite(PIN_GEN_KILL, LOW);
          if (currentSettings.chokeServoEnabled) {
            chokeServo.write(0);
          }
        }
        digitalWrite(PIN_SOURCE_SELECT, LOW);
        backupActive = false;
        genStartAttempts = 0;
        backupState = STATE_MAINS;
        break;
      }
      
      if (backupIsSensed) {
        genFailState = false;
        upsFailState = false;
        invFailState = false;
        genStartAttempts = 0;
        backupActive = true;
        digitalWrite(PIN_SOURCE_SELECT, LOW); // Kept low, let running state turn high after 10s
        backupStateMillis = currentMillis;
        backupSensedContinuousMillis = currentMillis;
        backupState = STATE_RUNNING_BACKUP;
      } else if (elapsed >= 3000) { // Wait around 3 seconds
        // exhausted retries and 3s stabilization elapsed. Declare Generator Failure!
        genFailState = true;
        digitalWrite(PIN_SOURCE_SELECT, LOW); // Safe transition: route load back to mains
        backupState = STATE_MAINS;
      }
      break;
    }

    case STATE_RUNNING_BACKUP: {
      // Set starting pin level based on backup system type
      if (currentSettings.backupType == 1) {
        digitalWrite(PIN_BACKUP_START, HIGH); // Inverter keeps PIN_BACKUP_START high during operation
      } else {
        digitalWrite(PIN_BACKUP_START, LOW);
      }

      // Load Selection & Choke Stabilization Tracking
      if (backupIsSensed) {
        if (backupSensedContinuousMillis == 0) {
          backupSensedContinuousMillis = currentMillis;
        }
        
        if (currentSettings.backupType == 2) { // Generator
          // 1. Release choke servo back to 0 position safely after 10 seconds
          if (currentMillis - backupSensedContinuousMillis >= 10000) {
            if (currentSettings.chokeServoEnabled) {
              chokeServo.write(0);
            }
          } else {
            // Keep choke pulled to user settings so generator doesn't die premature
            if (currentSettings.chokeServoEnabled) {
              chokeServo.write(currentSettings.chokeServoDegrees);
            }
          }
          
          // 2. Wait an additional 3 seconds (13s total of continuous sense) before putting PIN_SOURCE_SELECT high
          if (currentMillis - backupSensedContinuousMillis >= 13000) {
            digitalWrite(PIN_SOURCE_SELECT, HIGH);
          } else {
            digitalWrite(PIN_SOURCE_SELECT, LOW);
          }
        } else { // UPS or Inverter
          // Put PIN_SOURCE_SELECT high after at least 10 seconds of PIN_BACKUP_SENSE being high
          if (currentMillis - backupSensedContinuousMillis >= 10000) {
            digitalWrite(PIN_SOURCE_SELECT, HIGH);
          } else {
            digitalWrite(PIN_SOURCE_SELECT, LOW);
          }
        }
      } else {
        // Monitor if backup is unexpectedly lost while running (e.g. out of fuel / died)
        backupSensedContinuousMillis = 0;
        digitalWrite(PIN_SOURCE_SELECT, LOW); // Drop load immediately
        backupActive = false;
        
        if (currentSettings.backupType == 2) {
          genFailState = true;
        } else if (currentSettings.backupType == 0) {
          upsFailState = true;
        } else {
          invFailState = true;
        }

        // Connect back to mains if available to protect incubator!
        if (mainsIsHealthy) {
          backupStateMillis = currentMillis;
          
          if (currentSettings.backupType == 2) {
            // Already lost backup sense so no need to cool-down, go straight to asserting kill (Phase 2)
            digitalWrite(PIN_GEN_KILL, HIGH);
            genStopPhase = 2; // Kill active
            genStopPhaseMillis = currentMillis;
          } else if (currentSettings.backupType == 0) {
            digitalWrite(PIN_BACKUP_START, HIGH); // Pulse to switch off UPS
          } else {
            digitalWrite(PIN_BACKUP_START, LOW);  // Inverter off
          }
          backupState = STATE_STOPPING_BACKUP;
        } else {
          if (currentSettings.backupType == 1) {
            // If Inverter, keep PIN_BACKUP_START high but transition back to STATE_STARTING_BACKUP
            // so we keep trying/monitoring while displaying INV FAIL! error
            digitalWrite(PIN_BACKUP_START, HIGH);
            digitalWrite(PIN_SOURCE_SELECT, HIGH);
            backupStateMillis = currentMillis;
            backupState = STATE_STARTING_BACKUP;
          } else {
            // Mains is also failed, return to MAINS to allow retrying
            digitalWrite(PIN_SOURCE_SELECT, LOW);
            backupState = STATE_MAINS;
          }
        }
        break;
      }

      // Transition off backup when demand ends (Mains is healthy AND scheduled window has elapsed)
      if (!backupIsDemanded) {
        // Return selector relay back to Mains power instantly
        digitalWrite(PIN_SOURCE_SELECT, LOW);
        backupStateMillis = currentMillis;
        
        if (currentSettings.backupType == 2) {
          digitalWrite(PIN_GEN_KILL, LOW); // Keep low for cool-down
          genStopPhase = 1; // Start Phase 1: Cool-down (at least 20 seconds)
          genStopPhaseMillis = currentMillis;
        } else if (currentSettings.backupType == 0) {
          digitalWrite(PIN_BACKUP_START, HIGH); // Pulse to switch off UPS
        } else {
          digitalWrite(PIN_BACKUP_START, LOW); // Keep Inverter PIN_BACKUP_START low when mains return
        }
        
        backupState = STATE_STOPPING_BACKUP;
      }
      break;
    }

    case STATE_STOPPING_BACKUP: {
      unsigned long elapsed = currentMillis - backupStateMillis;
      
      if (currentSettings.backupType == 2) { // Generator Stopping (Non-blocking cool down + kill)
        if (genStopPhase == 1) {
          // Cool down phase: wait for at least 20 seconds
          if (currentMillis - genStopPhaseMillis >= 20000) {
            genStopPhase = 2;
            genStopPhaseMillis = currentMillis;
            digitalWrite(PIN_GEN_KILL, HIGH); // Assert kill switch
          }
        } else if (genStopPhase == 2) {
          // Keep PIN_GEN_KILL high continuously on every loop tick to avoid overrides
          digitalWrite(PIN_GEN_KILL, HIGH); 
          
          // Release the kill relay only when:
          // 1. Generator output has ceased (!backupIsSensed) AND at least 15 seconds have passed (cooldown/standstill + bench testing)
          // 2. OR a maximum safety timeout of 30 seconds has passed to prevent relay overheat
          if ((!backupIsSensed && (currentMillis - genStopPhaseMillis >= 15000)) || (currentMillis - genStopPhaseMillis >= 30000)) {
            digitalWrite(PIN_GEN_KILL, LOW); // Deactivate kill relay safely to allow future starts
            backupActive = false;
            genStartAttempts = 0;
            genFailState = false;
            genStopPhase = 0;
            backupState = STATE_MAINS;
          }
        } else {
          // Fallback if phase status wasn't initialized
          digitalWrite(PIN_GEN_KILL, LOW);
          backupActive = false;
          genStartAttempts = 0;
          genFailState = false;
          genStopPhase = 0;
          backupState = STATE_MAINS;
        }
      } else if (currentSettings.backupType == 0) { // UPS Stopping
        // Shut off UPS by pulsing starting button ONCE
        if (elapsed >= (currentSettings.backupPressDurationSec * 1000)) {
          digitalWrite(PIN_BACKUP_START, LOW);
        }
        
        if (elapsed >= (currentSettings.backupPressDurationSec * 1000 + 1500)) {
          backupActive = false;
          upsFailState = false;
          backupState = STATE_MAINS;
        }
      } else { // Inverter stopping
        digitalWrite(PIN_BACKUP_START, LOW);
        backupActive = false;
        invFailState = false;
        backupState = STATE_MAINS;
      }
      break;
    }
  }
}

#endif // POWER_BACKUP_H