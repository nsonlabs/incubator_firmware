/**
 * @file screen_ui.h
 * @brief ST7735/ST7789 TFT User Interface Drawing & Navigation Buttons Routing
 */

#ifndef SCREEN_UI_H
#define SCREEN_UI_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <WiFi.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

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

extern SystemSettings currentSettings;
extern SystemSettings tempSettings;

extern Adafruit_ST7789 tft;
extern bool manualTurnActive;
extern unsigned long manualTurnEndMillis;

enum DisplayScreen {
  SCREEN_BOOT,
  SCREEN_LINK_QR,
  SCREEN_DASHBOARD,
  SCREEN_MAIN_MENU,
  SCREEN_MENU_TEMP,
  SCREEN_MENU_HUMID,
  SCREEN_MENU_TURN,
  SCREEN_MENU_TURNING_SETUP,
  SCREEN_MENU_BACKUP_SETUP,
  SCREEN_MENU_TIME,
  SCREEN_NET_DASHBOARD,
  SCREEN_SIM_WIFI_SETUP,
  SCREEN_MENU_NET_SCAN_ERROR,
  SCREEN_MENU_NET_SCAN,
  SCREEN_MENU_KEYBOARD,
  SCREEN_WIFI_CONNECTING,
  SCREEN_APP_UPDATES_MENU,
  SCREEN_SOFTWARE_OTA
};

DisplayScreen activeTftScreen = SCREEN_BOOT;

// Scanned Wi-Fi reference and inputs
extern char wifiScanBuffer[10][33];
extern int wifiScanRssi[10];
extern int wifiScanCount;
extern bool isScanningWifi;
extern int wifiNetSelection;
extern char selectedSsid[33];
extern char typedPassword[64];
extern int kbCharIndex;
extern const char kbCharacters[];

extern int clockYear;
extern int clockMonth;
extern int clockDay;
extern int clockHour;
extern int clockMinute;
extern int clockSetupActiveIndex;

extern int mainMenuIndex;
extern bool forceDisplayRedraw;
extern bool isDeviceLinked;
extern bool gprsConnected;
extern int appUpdatesSubFocusIdx;

extern int tempSubFocusIdx;
extern int humidSubFocusIdx;
extern int turnSubFocusIdx;
extern int turnSetupSubFocusIdx;
extern int backupSetupSubFocusIdx;
extern bool showRecountConfirmation;
extern unsigned long recountConfirmationEndMillis;
extern bool isEditingValue;
extern bool backupActive;
extern int saveState;
extern int saveProgress;

void drawThickChar(char c, int16_t x, int16_t y, uint16_t color) {
  int w = 12; // width
  int h = 16; // height
  int t = 3;  // thickness
  
  if (c == 'N') {
    tft.fillRect(x, y, t, h, color);
    tft.fillRect(x + w - t, y, t, h, color);
    for (int i = 0; i < h; i++) {
      int px = x + (i * (w - t)) / h;
      tft.fillRect(px, y + i, t, 1, color);
    }
  } else if (c == 'S') {
    tft.fillRect(x, y, w, t, color); // top
    tft.fillRect(x, y, t, h/2, color); // top-left
    tft.fillRect(x, y + h/2 - 1, w, t, color); // middle
    tft.fillRect(x + w - t, y + h/2 - 1, t, h/2 + 1, color); // bottom-right
    tft.fillRect(x, y + h - t, w, t, color); // bottom
  } else if (c == 'O') {
    tft.fillRect(x, y, w, t, color); // top
    tft.fillRect(x, y + h - t, w, t, color); // bottom
    tft.fillRect(x, y, t, h, color); // left
    tft.fillRect(x + w - t, y, t, h, color); // right
  } else if (c == 'T') {
    tft.fillRect(x, y, w, t, color); // top
    tft.fillRect(x + w/2 - 1, y, t, h, color); // stem
  } else if (c == 'E') {
    tft.fillRect(x, y, w, t, color); // top
    tft.fillRect(x, y + h/2 - 1, w - 2, t, color); // mid
    tft.fillRect(x, y + h - t, w, t, color); // bot
    tft.fillRect(x, y, t, h, color); // left stem
  } else if (c == 'C') {
    tft.fillRect(x, y, w, t, color); // top
    tft.fillRect(x, y + h - t, w, t, color); // bottom
    tft.fillRect(x, y, t, h, color); // left
  } else if (c == 'H') {
    tft.fillRect(x, y, t, h, color); // left
    tft.fillRect(x + w - t, y, t, h, color); // right
    tft.fillRect(x, y + h/2 - 1, w, t, color); // middle
  } else if (c == 'F') {
    tft.fillRect(x, y, w, t, color); // top
    tft.fillRect(x, y + h/2 - 1, w - 2, t, color); // mid
    tft.fillRect(x, y, t, h, color); // left
  } else if (c == 'A') {
    tft.fillRect(x, y + t, t, h - t, color); // left
    tft.fillRect(x + w - t, y + t, t, h - t, color); // right
    tft.fillRect(x, y, w, t, color); // top cap
    tft.fillRect(x, y + h/2 - 1, w, t, color); // mid belt
  } else if (c == 'R') {
    tft.fillRect(x, y, t, h, color); // left vertical
    tft.fillRect(x, y, w, t, color); // top
    tft.fillRect(x + w - t, y, t, h/2 + 1, color); // right loop
    tft.fillRect(x, y + h/2, w, t, color); // mid
    for (int i = 0; i < h/2; i++) {
      int px = x + h/2 - t + i;
      tft.fillRect(px, y + h/2 + i, t, 1, color);
    }
  } else if (c == 'M') {
    tft.fillRect(x, y, t, h, color); // left vertical
    tft.fillRect(x + w - t, y, t, h, color); // right vertical
    for (int i = 0; i <= h/2; i++) {
      int px1 = x + i;
      int px2 = x + w - t - i;
      tft.fillRect(px1, y + i, t, 1, color);
      tft.fillRect(px2, y + i, t, 1, color);
    }
  }
}

void drawPerfectEggShell(int16_t cx, int16_t cy, uint16_t color) {
  for (int yOffset = -45; yOffset <= 45; yOffset++) {
    int r = 52;
    if (yOffset < 0) {
      r = 52 + (yOffset * 4) / 10;
    } else {
      r = 52 - (yOffset * 2) / 10;
    }
    tft.fillCircle(cx, cy + yOffset, r, color);
  }
}

void drawTopShell(int16_t cx, int16_t cy, uint16_t color) {
  if (cx < -60 || cx > 380 || cy < -60 || cy > 300) return;
  // Top half of egg: yOffset from -45 to 0
  for (int yOffset = -45; yOffset <= 0; yOffset++) {
    int r = 52 + (yOffset * 4) / 10;
    tft.drawFastHLine(cx - r, cy + yOffset, 2 * r, color);
  }
  // Detailed jagged cracks sculpted with black cutaways on bottom boundary of top shell
  tft.fillTriangle(cx - 35, cy + 1, cx - 25, cy - 8, cx - 15, cy + 1, ST77XX_BLACK);
  tft.fillTriangle(cx - 15, cy + 1, cx, cy - 10, cx + 15, cy + 1, ST77XX_BLACK);
  tft.fillTriangle(cx + 15, cy + 1, cx + 25, cy - 8, cx + 35, cy + 1, ST77XX_BLACK);
}

void drawBottomShell(int16_t cx, int16_t cy, uint16_t color) {
  if (cx < -60 || cx > 380 || cy < -60 || cy > 300) return;
  // Bottom half of egg: yOffset from 1 to 45
  for (int yOffset = 1; yOffset <= 45; yOffset++) {
    int r = 52 - (yOffset * 2) / 10;
    tft.drawFastHLine(cx - r, cy + yOffset, 2 * r, color);
  }
  // Detailed jagged cracks sculpted with black cutaways on top boundary of bottom shell
  tft.fillTriangle(cx - 45, cy, cx - 35, cy + 8, cx - 25, cy, ST77XX_BLACK);
  tft.fillTriangle(cx - 25, cy, cx - 10, cy + 10, cx, cy, ST77XX_BLACK);
  tft.fillTriangle(cx, cy, cx + 15, cy + 8, cx + 30, cy, ST77XX_BLACK);
  tft.fillTriangle(cx + 30, cy, cx + 40, cy + 10, cx + 50, cy, ST77XX_BLACK);
}

void drawChickLegs(int16_t chickY, uint16_t color) {
  // Overlap slightly inside the body radius (body bottom is at +38)
  tft.drawFastVLine(153, chickY + 32, 13, color);
  tft.drawFastVLine(167, chickY + 32, 13, color);
  
  // Feet toes (three-prong fork structure)
  tft.drawLine(153, chickY + 45, 149, chickY + 48, color);
  tft.drawLine(153, chickY + 45, 153, chickY + 49, color);
  tft.drawLine(153, chickY + 45, 157, chickY + 48, color);
  
  tft.drawLine(167, chickY + 45, 163, chickY + 48, color);
  tft.drawLine(167, chickY + 45, 167, chickY + 49, color);
  tft.drawLine(167, chickY + 45, 171, chickY + 48, color);
}

void drawCircTraceThick(int x1, int y1, int x2, int y2, uint16_t col) {
  if (y1 == y2) {
    tft.drawLine(x1, y1 - 1, x2, y1 - 1, col);
    tft.drawLine(x1, y1,     x2, y1,     col);
    tft.drawLine(x1, y1 + 1, x2, y1 + 1, col);
  } else {
    tft.drawLine(x1,     y1,     x2,     y2,     col);
    tft.drawLine(x1 + 1, y1,     x2 + 1, y2,     col);
    tft.drawLine(x1 - 1, y1,     x2 - 1, y2,     col);
    tft.drawLine(x1,     y1 + 1, x2,     y2 + 1, col);
    tft.drawLine(x1,     y1 - 1, x2,     y2 - 1, col);
  }
}

void drawCircuitTrace(uint16_t botY, uint16_t col) {
  uint16_t goldPadColor = 0xFDA0; // Vibrant Gold (RGB565)
  
  // Trace 1: Top Trace (y offset starting at botY + 2)
  drawCircTraceThick(95, botY + 2, 167, botY + 2, col);
  drawCircTraceThick(167, botY + 2, 185, botY + 20, col);
  drawCircTraceThick(185, botY + 20, 235, botY + 20, col);
  tft.fillCircle(95, botY + 2, 4, goldPadColor);
  tft.fillCircle(95, botY + 2, 1, ST77XX_BLACK); // Inner drill hole
  tft.fillCircle(235, botY + 20, 4, goldPadColor);
  tft.fillCircle(235, botY + 20, 1, ST77XX_BLACK);
  
  // Trace 2: Middle Trace (y offset starting at botY + 12) - THE CHICK PECKS THIS ONE AT x=160
  drawCircTraceThick(75, botY + 12, 175, botY + 12, col);
  drawCircTraceThick(175, botY + 12, 195, botY + 32, col);
  drawCircTraceThick(195, botY + 32, 245, botY + 32, col);
  tft.fillCircle(75, botY + 12, 4, goldPadColor);
  tft.fillCircle(75, botY + 12, 1, ST77XX_BLACK);
  tft.fillCircle(245, botY + 32, 4, goldPadColor);
  tft.fillCircle(245, botY + 32, 1, ST77XX_BLACK);

  // Trace 3: Bottom Trace (y offset starting at botY + 22)
  drawCircTraceThick(55, botY + 22, 183, botY + 22, col);
  drawCircTraceThick(183, botY + 22, 203, botY + 42, col);
  drawCircTraceThick(203, botY + 42, 255, botY + 42, col);
  tft.fillCircle(55, botY + 22, 4, goldPadColor);
  tft.fillCircle(55, botY + 22, 1, ST77XX_BLACK);
  tft.fillCircle(255, botY + 42, 4, goldPadColor);
  tft.fillCircle(255, botY + 42, 1, ST77XX_BLACK);
}

struct PopLetter {
  char character;
  int16_t x;
  uint16_t color;
};

void initTftDisplay() {
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH); // Activate screen backlight
  tft.init(240, 320);          // Initialize ST7789 physical screen
  tft.setRotation(1);          // ROTATION 1: Widescreen Landscape (320x240)
  tft.invertDisplay(false);
  
  // Custom high contrast boot welcome active animation (NSON TECH FARMS)
  tft.fillScreen(ST77XX_BLACK);
  
  // Draw perfect smooth solid egg shell in center (wider layout)
  drawPerfectEggShell(160, 100, 0xC46D); // Beautiful Brown Egg Shell
  delay(1000);
  
  // Phase 1: Tiny hairline central stressed fracture (Star crack)
  tft.drawLine(158, 100, 162, 100, 0x5B2C);
  tft.drawLine(160, 98, 160, 102, 0x5B2C);
  delay(500);
  
  // Phase 2: Structural crack spreads with micro-fractures of stress
  tft.drawLine(158, 100, 154, 96, 0x5B2C);
  tft.drawLine(162, 100, 167, 104, 0x5B2C);
  tft.drawLine(160, 98, 158, 92, 0x5B2C);
  tft.drawLine(160, 102, 164, 108, 0x5B2C);
  delay(400);
  
  // Phase 3: Sudden violent propagation shooting zigzag jagged teeth out to both edges!
  // Crack propagating to the Left jaggedly
  tft.drawLine(154, 96, 142, 103, 0x5B2C);
  tft.drawLine(142, 103, 130, 95, 0x5B2C);
  tft.drawLine(130, 95, 118, 104, 0x5B2C);
  tft.drawLine(118, 104, 110, 100, 0x5B2C); // reached left outer egg boundary
  delay(300);

  // Crack propagating to the Right jaggedly
  tft.drawLine(167, 104, 178, 96, 0x5B2C);
  tft.drawLine(178, 96, 190, 105, 0x5B2C);
  tft.drawLine(190, 105, 202, 95, 0x5B2C);
  tft.drawLine(202, 95, 210, 100, 0x5B2C); // reached right outer egg boundary
  delay(800);
  
  int prevTopX = 160;
  int prevTopY = 100;
  int prevBotX = 160;
  int prevBotY = 100;
 
  // Slide Top/Bottom shells apart & emerge chick
  for (int t = 0; t <= 32; t++) {
    // 1. Erase previous positions cleanly to prevent ANY trails on screen borders
    tft.fillRect(120, 50, 80, 110, ST77XX_BLACK); // Chick region clear
    drawTopShell(prevTopX, prevTopY, ST77XX_BLACK);
    drawBottomShell(prevBotX, prevBotY, ST77XX_BLACK);
    
    // 2. Draw Cute Yellow Chick standing up
    int chickY = 104 - (t * 8) / 32;
    tft.fillCircle(160, chickY + 15, 23, 0xFFE0); // Chick body
    tft.fillCircle(160, chickY - 3, 16, 0xFFE0);  // Chick head
    
    // Eyes with high-contrast reflection dots
    tft.fillCircle(154, chickY - 5, 2, ST77XX_BLACK);
    tft.drawPixel(154, chickY - 5, ST77XX_WHITE);
    tft.fillCircle(166, chickY - 5, 2, ST77XX_BLACK);
    tft.drawPixel(166, chickY - 5, ST77XX_WHITE);
    
    // Orange beak
    tft.fillTriangle(158, chickY, 162, chickY, 160, chickY + 6, 0xFD20);
    
    // Little active orange chicken legs
    drawChickLegs(chickY, 0xFD20);
    
    // Wings flap actively
    if (t % 2 == 0) {
      tft.fillTriangle(137, chickY + 15, 141, chickY + 10, 143, chickY + 18, 0xFD20);
      tft.fillTriangle(183, chickY + 15, 179, chickY + 10, 177, chickY + 18, 0xFD20);
    } else {
      tft.fillTriangle(135, chickY + 12, 141, chickY + 10, 143, chickY + 18, 0xFD20);
      tft.fillTriangle(185, chickY + 12, 179, chickY + 10, 177, chickY + 18, 0xFD20);
    }
    
    // 3. Draw Top Shell at its descending gravity trajectory
    int topX = 160 - (t * 3);
    int topY = 100 - (t * 1) + (t * t) / 3;
    if (topY < 240 + 45 && topX > -52) {
      drawTopShell(topX, topY, 0xC46D);
      prevTopX = topX;
      prevTopY = topY;
    } else {
      prevTopX = -200;
      prevTopY = -200;
    }
    
    // 4. Draw Bottom Shell at its descending gravity trajectory
    int botX = 160 + (t * 3);
    int botY = 100 + (t * t) / 3;
    if (botY < 240 + 45 && botX < 320 + 52) {
      drawBottomShell(botX, botY, 0xC46D);
      prevBotX = botX;
      prevBotY = botY;
    } else {
      prevBotX = -200;
      prevBotY = -200;
    }
    
    delay(40);
  }
  
  // Wipe both bottom and side border zones completely to ensure absolute blank canvas for typography
  tft.fillRect(0, 160, 320, 80, ST77XX_BLACK);
  tft.fillRect(0, 0, 110, 240, ST77XX_BLACK);
  tft.fillRect(210, 0, 110, 240, ST77XX_BLACK);
 
  // Chick Pecking the PCB trace under its belly (umbilical link)
  int chickY = 96;
  int botY = 114;
  drawCircuitTrace(botY, ST77XX_GREEN);
  delay(300);
  
  for (int peck = 0; peck < 3; peck++) {
    // Standard beak overwrite with body color
    tft.fillTriangle(158, chickY, 162, chickY, 160, chickY + 6, 0xFFE0);
    
    // Peck down beak reaching the trace
    tft.fillTriangle(159, chickY, 161, chickY, 160, botY + 12, 0xFD20);
    
    // Sparks/Glow effect
    drawCircuitTrace(botY, ST77XX_YELLOW);
    tft.fillCircle(160, botY + 12, 7, ST77XX_WHITE);
    delay(150);
    
    // Clear flash
    tft.fillCircle(160, botY + 12, 7, ST77XX_BLACK);
    drawCircuitTrace(botY, ST77XX_GREEN);
    
    // Erase downward beak
    tft.drawLine(160, chickY, 160, botY + 12, ST77XX_BLACK);
    tft.drawTriangle(159, chickY, 161, chickY, 160, botY + 12, ST77XX_BLACK);
    
    // Re-render yellow body parts & normal look
    tft.fillCircle(160, chickY + 15, 23, 0xFFE0);
    tft.fillCircle(160, chickY - 3, 16, 0xFFE0);
    tft.fillTriangle(158, chickY, 162, chickY, 160, chickY + 6, 0xFD20);
    
    tft.fillCircle(154, chickY - 5, 2, ST77XX_BLACK);
    tft.drawPixel(154, chickY - 5, ST77XX_WHITE);
    tft.fillCircle(166, chickY - 5, 2, ST77XX_BLACK);
    tft.drawPixel(166, chickY - 5, ST77XX_WHITE);
    
    // Re-render legs
    drawChickLegs(chickY, 0xFD20);
    
    delay(180);
  }
  
  // Neck raises upwards revealing secret text
  for (int lift = 0; lift <= 8; lift += 2) {
    tft.fillCircle(160, chickY - 3 - (lift - 2), 16, ST77XX_BLACK);
    tft.fillCircle(160, chickY + 15, 23, 0xFFE0);
    tft.fillCircle(160, chickY - 3 - lift, 16, 0xFFE0);
    tft.fillRect(152, chickY - 3, 16, lift + 2, 0xFFE0);
    
    tft.fillTriangle(157, chickY - lift, 163, chickY - lift, 160, chickY - lift - 5, 0xFD20);
    tft.fillCircle(154, chickY - 5 - lift, 2, ST77XX_BLACK);
    tft.drawPixel(154, chickY - 5 - lift, ST77XX_WHITE);
    tft.fillCircle(166, chickY - 5 - lift, 2, ST77XX_BLACK);
    tft.drawPixel(166, chickY - 5 - lift, ST77XX_WHITE);
    
    // Re-render legs during liftoff
    drawChickLegs(chickY, 0xFD20);
    
    delay(50);
  }
  
  // Chirp wave arcs
  tft.drawCircle(130, chickY - 20, 4, ST77XX_YELLOW);
  tft.drawCircle(190, chickY - 20, 4, ST77XX_YELLOW);
  delay(100);
  tft.drawCircle(130, chickY - 20, 8, ST77XX_YELLOW);
  tft.drawCircle(190, chickY - 20, 8, ST77XX_YELLOW);
  delay(200);
  
  // Draw Letters pop systematically
  PopLetter popName[] = {
    {'N', 27, 0x07FF}, {'S', 43, 0x07FF}, {'O', 59, 0x07FF}, {'N', 75, 0x07FF},
    {'T', 111, 0xFDA0}, {'E', 127, 0xFDA0}, {'C', 143, 0xFDA0}, {'H', 159, 0xFDA0},
    {'F', 195, 0xF81F}, {'A', 211, 0xF81F}, {'R', 227, 0xF81F}, {'M', 243, 0xF81F}, {'S', 259, 0xF81F}
  };
  int totalLetters = 13;
  
  for (int i = 0; i < totalLetters; i++) {
    drawThickChar(popName[i].character, popName[i].x, 195, ST77XX_WHITE);
    delay(80);
    drawThickChar(popName[i].character, popName[i].x, 195, popName[i].color);
    delay(100);
  }
  
  delay(2800); // Let the startup brand glow majestically on screen
}

void drawAppUpdatesMenuScreen() {
  tft.fillScreen(ST77XX_BLACK);
  tft.drawRoundRect(6, 6, 308, 228, 8, ST77XX_BLUE);
  
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);
  tft.setCursor(65, 18);
  tft.println("APP & UPDATES");
  tft.drawFastHLine(20, 38, 280, 0xAD55); // Custom Gray horizontal line

  const char* options[] = {
    "1. Link to App",
    "2. Software & OTA"
  };

  tft.setTextSize(2);
  for (int i = 0; i < 2; i++) {
    int y = 60 + (i * 36);
    if (i == appUpdatesSubFocusIdx) {
      tft.fillRect(20, y - 6, 280, 28, ST77XX_BLUE);
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(30, y);
      tft.printf("> %s", options[i]);
    } else {
      tft.setTextColor(ST77XX_CYAN);
      tft.setCursor(30, y);
      tft.printf("  %s", options[i]);
    }
  }

  // Back Option
  int btnY = 160;
  if (appUpdatesSubFocusIdx == 2) {
    tft.fillRect(20, btnY, 280, 34, ST77XX_RED);
    tft.setTextColor(ST77XX_WHITE);
    tft.drawRoundRect(20, btnY, 280, 34, 4, ST77XX_WHITE);
  } else {
    tft.drawRoundRect(20, btnY, 280, 34, 4, ST77XX_RED);
    tft.setTextColor(ST77XX_RED);
  }
  tft.setTextSize(2);
  tft.setCursor(120, btnY + 9);
  tft.print("GO BACK");
}

void drawSoftwareOtaScreen() {
  tft.fillScreen(ST77XX_BLACK);
  tft.drawRoundRect(6, 6, 308, 228, 8, ST77XX_MAGENTA);
  
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);
  tft.setCursor(70, 18);
  tft.println("SOFTWARE & OTA");
  tft.drawFastHLine(20, 38, 280, 0xAD55); // Custom Gray horizontal line

  // Display active internet status & info
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(30, 55);
  
  if (WiFi.status() == WL_CONNECTED) {
    tft.print("Active Connection: ");
    tft.setTextColor(ST77XX_GREEN);
    tft.print("Wi-Fi Connected");
  } else if (gprsConnected) {
    tft.print("Active Connection: ");
    tft.setTextColor(ST77XX_GREEN);
    tft.print("SIM Card (A7680C)");
  } else {
    tft.print("Active Connection: ");
    tft.setTextColor(ST77XX_RED);
    tft.print("Disconnected");
  }

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(30, 75);
  tft.print("Current Firmware ver: ");
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("v2.1.4");

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(30, 95);
  tft.print("Server: ");
  tft.setTextColor(ST77XX_CYAN);
  tft.print("ota.nsonlabs.com/incubator");

  // A progress bar or simulation box
  tft.drawRoundRect(25, 115, 270, 38, 4, 0x52AA);
  
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(35, 128);
  tft.print("Update State: Up-to-date");

  // OK Button trigger to "Check for Updates"
  int btnY = 170;
  tft.fillRect(20, btnY, 280, 34, ST77XX_BLUE);
  tft.setTextColor(ST77XX_WHITE);
  tft.drawRoundRect(20, btnY, 280, 34, 4, ST77XX_WHITE);
  tft.setCursor(55, btnY + 9);
  tft.setTextSize(2);
  tft.print("PRESS OK TO RE-CHECK");

  tft.setTextSize(1);
  tft.setTextColor(0xAD55);
  tft.setCursor(25, 212);
  tft.print("Press UP/DOWN keys to return to menu");
}

void drawQrLinkingScreen() {
  tft.fillScreen(ST77XX_BLACK);
  tft.drawRoundRect(6, 6, 308, 228, 8, ST77XX_BLUE);
  
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);
  tft.setCursor(75, 18);
  tft.println("APP & UPDATES");
  tft.drawFastHLine(20, 38, 280, 0xAD55); // Custom Gray horizontal line

  if (isDeviceLinked) {
    // 1. Double bordered Status Panel
    tft.drawRoundRect(20, 50, 280, 58, 6, ST77XX_GREEN);
    tft.drawRoundRect(21, 51, 278, 56, 6, ST77XX_GREEN);
    
    tft.setTextColor(ST77XX_GREEN);
    tft.setTextSize(2);
    tft.setCursor(35, 60);
    tft.print("STATUS: PAIRED & SYNCED");
    
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.setCursor(30, 88);
    tft.print("Linked to phone app cloud dashboard.");

    // 2. System updates Panel
    tft.drawRoundRect(20, 118, 280, 54, 6, 0x52AA); // Cyan/Dim border
    
    tft.setTextColor(ST77XX_CYAN);
    tft.setTextSize(1);
    tft.setCursor(30, 126);
    tft.print("Firmware: v2.1.4 (Latest)");
    
    tft.setTextColor(ST77XX_GREEN);
    tft.setCursor(30, 142);
    tft.print("Update state: System fully up-to-date");
    
    tft.setTextColor(0xAD55);
    tft.setCursor(30, 158);
    tft.print("Broker: HiveMQ Public Cluster");

    // 3. Action button cues
    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(1);
    tft.setCursor(20, 192);
    tft.print("OK: Press select button to UNLINK peer app");
    
    tft.setTextColor(0xAD55);
    tft.setCursor(20, 208);
    tft.print("UP/DOWN: Return to Incubator Main Menu");
  } else {
    // 1. Double bordered Serial Box for legibility
    tft.drawRoundRect(20, 50, 280, 58, 6, ST77XX_YELLOW);
    tft.drawRoundRect(21, 51, 278, 56, 6, ST77XX_YELLOW);
    
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(1);
    tft.setCursor(35, 58);
    tft.print("APP PAIRING SERIAL NUMBER:");
    
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(35, 76);
    tft.print(DEV_SERIAL_NUMBER);

    // 2. System updates Panel
    tft.drawRoundRect(20, 118, 280, 54, 6, 0x52AA); // Dim Gray border
    
    tft.setTextColor(ST77XX_CYAN);
    tft.setTextSize(1);
    tft.setCursor(30, 126);
    tft.print("Firmware: v2.1.4 (Ready)");
    
    tft.setTextColor(ST77XX_GREEN);
    tft.setCursor(30, 142);
    tft.print("Update state: System fully up-to-date");
    
    tft.setTextColor(0xAD55);
    tft.setCursor(30, 158);
    tft.print("Enter serial on phone app to bind device.");

    // 3. Action button cues
    tft.setTextColor(ST77XX_GREEN);
    tft.setTextSize(1);
    tft.setCursor(20, 192);
    tft.print("OK: Press to SIMULATE successful application binding");
    
    tft.setTextColor(0xAD55);
    tft.setCursor(20, 208);
    tft.print("UP/DOWN: Return to Incubator Main Menu");
  }
}

void drawWifiSymbol(int16_t x, int16_t y, bool connected) {
  tft.fillRect(x, y, 22, 16, ST77XX_BLACK);
  int mode = currentSettings.simAndWifiMode;
  
  if (mode == 0 || mode == 3) {
    // Disabled state - Draw dimmed symbol with a red D over it
    uint16_t col = 0x52AA; // dim gray
    tft.fillCircle(x + 11, y + 14, 2, col);
    tft.drawCircle(x + 11, y + 15, 6, col);
    tft.drawCircle(x + 11, y + 15, 7, col);
    tft.drawCircle(x + 11, y + 15, 11, col);
    tft.drawCircle(x + 11, y + 15, 12, col);
    tft.fillRect(x, y + 16, 22, 10, ST77XX_BLACK);
    
    tft.setFont(NULL);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_RED);
    tft.setCursor(x + 6, y + 1);
    tft.print("D");
  } else {
    // Enabled state (WiFi only or SIM & WiFi)
    uint16_t col = connected ? ST77XX_GREEN : 0x52AA;
    tft.fillCircle(x + 11, y + 14, 2, col);
    tft.drawCircle(x + 11, y + 15, 6, col);
    tft.drawCircle(x + 11, y + 15, 7, col);
    tft.drawCircle(x + 11, y + 15, 11, col);
    tft.drawCircle(x + 11, y + 15, 12, col);
    tft.fillRect(x, y + 16, 22, 10, ST77XX_BLACK);
    
    if (!connected) {
      // Elegant high-visibility red X strike over the disconnected icon
      tft.drawLine(x + 2, y + 2, x + 20, y + 14, ST77XX_RED);
      tft.drawLine(x + 3, y + 2, x + 21, y + 14, ST77XX_RED);
      tft.drawLine(x + 20, y + 2, x + 2, y + 14, ST77XX_RED);
      tft.drawLine(x + 21, y + 2, x + 3, y + 14, ST77XX_RED);
    }
  }
}

extern int gsmSignalBars;
extern char gsmNetworkType[8];
extern volatile bool cachedMqttConnected;
extern volatile int mqttStateCode;

void drawCellularSymbol(int16_t x, int16_t y, bool connected) {
  tft.fillRect(x - 17, y, 17, 16, ST77XX_BLACK); // clear text area in front (17 pixels wide)
  tft.fillRect(x, y, 22, 16, ST77XX_BLACK);       // clear symbol area
  int mode = currentSettings.simAndWifiMode;
  
  if (mode == 1 || mode == 3) {
    // Cellular is Disabled in Mode 1 (WIFI only) and Mode 3 (No sim & WIFI)
    // Draw dimmed bars
    for (int i = 0; i < 4; i++) {
      int barH = 3 + (i * 4);
      tft.fillRect(x + (i * 5) + 2, y + 15 - barH, 3, barH, 0x52AA);
    }
    // Draw red high-visibility 'D' over the bars
    tft.setFont(NULL);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_RED);
    tft.setCursor(x + 4, y + 1);
    tft.print("D");
  } else {
    // Cellular is Enabled (Mode 0: SIM only, or Mode 2: SIM & WiFi)
    if (connected) {
      // Draw network type text "4G" or "2G"
      tft.setFont(NULL);
      tft.setTextSize(1);
      tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
      tft.setCursor(x - 15, y + 4);
      tft.print(gsmNetworkType); // "4G" or "2G"
      
      // Draw green/dim bars according to strength
      for (int i = 0; i < 4; i++) {
        int barH = 3 + (i * 4);
        uint16_t col = (i < gsmSignalBars) ? ST77XX_GREEN : 0x52AA;
        tft.fillRect(x + (i * 5) + 2, y + 15 - barH, 3, barH, col);
      }
    } else {
      // Disconnected / Failed connection
      for (int i = 0; i < 4; i++) {
        int barH = 3 + (i * 4);
        tft.fillRect(x + (i * 5) + 2, y + 15 - barH, 3, barH, 0x52AA);
      }
      // Draw red high-visibility 'X' over the bars
      tft.drawLine(x + 1, y + 1, x + 21, y + 13, ST77XX_RED);
      tft.drawLine(x + 21, y + 1, x + 1, y + 13, ST77XX_RED);
    }
  }
}

void drawServerSymbol(int16_t x, int16_t y, bool connected) {
  tft.fillRect(x - 2, y - 1, 20, 18, ST77XX_BLACK);
  uint16_t color = connected ? ST77XX_GREEN : 0x52AA; // dim gray
  
  // 3-tier server rack representation
  tft.drawRoundRect(x, y + 1, 16, 4, 1, color);
  tft.drawPixel(x + 3, y + 3, color);
  tft.drawPixel(x + 5, y + 3, color);
  
  tft.drawRoundRect(x, y + 6, 16, 4, 1, color);
  tft.drawPixel(x + 3, y + 8, color);
  tft.drawPixel(x + 5, y + 8, color);
  
  tft.drawRoundRect(x, y + 11, 16, 4, 1, color);
  tft.drawPixel(x + 3, y + 13, color);
  tft.drawPixel(x + 5, y + 13, color);
  
  if (!connected) {
    tft.drawLine(x + 1, y + 1, x + 14, y + 14, ST77XX_RED);
    tft.drawLine(x + 14, y + 1, x + 1, y + 14, ST77XX_RED);
  }
}

void drawMainsLightningSymbol(int16_t x, int16_t y, bool mainsHealthy) {
  tft.fillRect(x, y, 40, 16, ST77XX_BLACK);
  uint16_t color = mainsHealthy ? ST77XX_YELLOW : ST77XX_RED;
  
  tft.setTextColor(color);
  tft.setTextSize(1);
  tft.setCursor(x + 2, y + 4);
  tft.print("M");
  
  int offset = 10;
  tft.drawLine(x + offset + 10, y + 1, x + offset + 4, y + 8, color);
  tft.drawLine(x + offset + 4, y + 8, x + offset + 9, y + 8, color);
  tft.drawLine(x + offset + 9, y + 8, x + offset + 5, y + 15, color);
  tft.drawLine(x + offset + 5, y + 15, x + offset + 12, y + 7, color);
  tft.drawLine(x + offset + 12, y + 7, x + offset + 7, y + 7, color);
  tft.drawLine(x + offset + 7, y + 7, x + offset + 10, y + 1, color);
  
  tft.fillTriangle(x + offset + 10, y + 1, x + offset + 4, y + 8, x + offset + 8, y + 8, color);
  tft.fillTriangle(x + offset + 9, y + 8, x + offset + 5, y + 15, x + offset + 11, y + 8, color);
}

void drawBackupLightningSymbol(int16_t x, int16_t y, bool backupWorking, bool backupFailed) {
  tft.fillRect(x, y, 40, 16, ST77XX_BLACK);
  uint16_t color;
  if (backupWorking) {
    color = ST77XX_GREEN;
  } else if (backupFailed) {
    color = ST77XX_RED;
  } else {
    color = 0x52AA; // Gray standby
  }
  
  tft.setTextColor(color);
  tft.setTextSize(1);
  tft.setCursor(x + 2, y + 4);
  tft.print("B");
  
  int offset = 10;
  tft.drawLine(x + offset + 10, y + 1, x + offset + 4, y + 8, color);
  tft.drawLine(x + offset + 4, y + 8, x + offset + 9, y + 8, color);
  tft.drawLine(x + offset + 9, y + 8, x + offset + 5, y + 15, color);
  tft.drawLine(x + offset + 5, y + 15, x + offset + 12, y + 7, color);
  tft.drawLine(x + offset + 12, y + 7, x + offset + 7, y + 7, color);
  tft.drawLine(x + offset + 7, y + 7, x + offset + 10, y + 1, color);
  
  tft.fillTriangle(x + offset + 10, y + 1, x + offset + 4, y + 8, x + offset + 8, y + 8, color);
  tft.fillTriangle(x + offset + 9, y + 8, x + offset + 5, y + 15, x + offset + 11, y + 8, color);
}

void drawMqttErrorCode(int16_t x, int16_t y, int stateVal, bool connected) {
  tft.fillRect(x, y, 48, 16, ST77XX_BLACK);
  tft.setFont(NULL);
  tft.setTextSize(1);
  if (connected) {
    tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  } else {
    tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
  }
  tft.setCursor(x, y + 4);
  if (connected) {
    tft.print("MQ:OK");
  } else {
    tft.printf("MQ:%d", stateVal);
  }
}

void drawLightningSymbol(int16_t x, int16_t y, bool backupWorking) {
  drawBackupLightningSymbol(x, y, backupWorking, false);
}

void drawGearSymbol(int16_t cx, int16_t cy, uint16_t color) {
  tft.drawCircle(cx, cy, 6, color);
  tft.drawCircle(cx, cy, 3, color);
  tft.fillRect(cx - 1, cy - 9, 2, 3, color);
  tft.fillRect(cx - 1, cy + 6, 2, 3, color);
  tft.fillRect(cx - 9, cy - 1, 3, 2, color);
  tft.fillRect(cx + 6, cy - 1, 3, 2, color);
  tft.drawLine(cx - 5, cy - 5, cx - 7, cy - 7, color);
  tft.drawLine(cx + 5, cy + 5, cx + 7, cy + 7, color);
  tft.drawLine(cx - 5, cy + 5, cx - 7, cy + 7, color);
  tft.drawLine(cx + 5, cy - 5, cx + 7, cy - 7, color);
}

void drawGaugeArc(int16_t cx, int16_t cy, int16_t r, float value, float minVal, float maxVal, uint16_t color) {
  float pct = (value - minVal) / (maxVal - minVal);
  if (pct < 0.0f) pct = 0.0f;
  if (pct > 1.0f) pct = 1.0f;

  float startAngle = 3.14159265f; // left side (180 deg)
  float endAngle = 6.28318530f;   // right side (360 deg)
  float totalAngle = endAngle - startAngle;

  int segments = 45; // Smooth high-resolution arc
  
  // 1. Draw solid grey background arc track (thickness of 5 pixels using concentric arcs)
  int16_t lastX_m = cx + (r * cos(startAngle));
  int16_t lastY_m = cy + (r * sin(startAngle));
  int16_t lastX_i1 = cx + ((r-1) * cos(startAngle));
  int16_t lastY_i1 = cy + ((r-1) * sin(startAngle));
  int16_t lastX_i2 = cx + ((r-2) * cos(startAngle));
  int16_t lastY_i2 = cy + ((r-2) * sin(startAngle));
  int16_t lastX_o1 = cx + ((r+1) * cos(startAngle));
  int16_t lastY_o1 = cy + ((r+1) * sin(startAngle));
  int16_t lastX_o2 = cx + ((r+2) * cos(startAngle));
  int16_t lastY_o2 = cy + ((r+2) * sin(startAngle));

  for (int i = 1; i <= segments; i++) {
    float angle = startAngle + (totalAngle * ((float)i / segments));
    int16_t x_m = cx + (r * cos(angle));
    int16_t y_m = cy + (r * sin(angle));
    int16_t x_i1 = cx + ((r-1) * cos(angle));
    int16_t y_i1 = cy + ((r-1) * sin(angle));
    int16_t x_i2 = cx + ((r-2) * cos(angle));
    int16_t y_i2 = cy + ((r-2) * sin(angle));
    int16_t x_o1 = cx + ((r+1) * cos(angle));
    int16_t y_o1 = cy + ((r+1) * sin(angle));
    int16_t x_o2 = cx + ((r+2) * cos(angle));
    int16_t y_o2 = cy + ((r+2) * sin(angle));

    tft.drawLine(lastX_m, lastY_m, x_m, y_m, 0x18C3);
    tft.drawLine(lastX_i1, lastY_i1, x_i1, y_i1, 0x18C3);
    tft.drawLine(lastX_i2, lastY_i2, x_i2, y_i2, 0x18C3);
    tft.drawLine(lastX_o1, lastY_o1, x_o1, y_o1, 0x18C3);
    tft.drawLine(lastX_o2, lastY_o2, x_o2, y_o2, 0x18C3);

    lastX_m = x_m; lastY_m = y_m;
    lastX_i1 = x_i1; lastY_i1 = y_i1;
    lastX_i2 = x_i2; lastY_i2 = y_i2;
    lastX_o1 = x_o1; lastY_o1 = y_o1;
    lastX_o2 = x_o2; lastY_o2 = y_o2;
  }

  // 2. Draw solid colored/white active progress arc track
  int activeSegments = (int)(pct * segments);
  if (activeSegments > 0) {
    lastX_m = cx + (r * cos(startAngle));
    lastY_m = cy + (r * sin(startAngle));
    lastX_i1 = cx + ((r-1) * cos(startAngle));
    lastY_i1 = cy + ((r-1) * sin(startAngle));
    lastX_i2 = cx + ((r-2) * cos(startAngle));
    lastY_i2 = cy + ((r-2) * sin(startAngle));
    lastX_o1 = cx + ((r+1) * cos(startAngle));
    lastY_o1 = cy + ((r+1) * sin(startAngle));
    lastX_o2 = cx + ((r+2) * cos(startAngle));
    lastY_o2 = cy + ((r+2) * sin(startAngle));

    for (int i = 1; i <= activeSegments; i++) {
      float angle = startAngle + (totalAngle * ((float)i / segments));
      int16_t x_m = cx + (r * cos(angle));
      int16_t y_m = cy + (r * sin(angle));
      int16_t x_i1 = cx + ((r-1) * cos(angle));
      int16_t y_i1 = cy + ((r-1) * sin(angle));
      int16_t x_i2 = cx + ((r-2) * cos(angle));
      int16_t y_i2 = cy + ((r-2) * sin(angle));
      int16_t x_o1 = cx + ((r+1) * cos(angle));
      int16_t y_o1 = cy + ((r+1) * sin(angle));
      int16_t x_o2 = cx + ((r+2) * cos(angle));
      int16_t y_o2 = cy + ((r+2) * sin(angle));

      tft.drawLine(lastX_m, lastY_m, x_m, y_m, color);
      tft.drawLine(lastX_i1, lastY_i1, x_i1, y_i1, color);
      tft.drawLine(lastX_i2, lastY_i2, x_i2, y_i2, color);
      tft.drawLine(lastX_o1, lastY_o1, x_o1, y_o1, color);
      tft.drawLine(lastX_o2, lastY_o2, x_o2, y_o2, color);

      lastX_m = x_m; lastY_m = y_m;
      lastX_i1 = x_i1; lastY_i1 = y_i1;
      lastX_i2 = x_i2; lastY_i2 = y_i2;
      lastX_o1 = x_o1; lastY_o1 = y_o1;
      lastX_o2 = x_o2; lastY_o2 = y_o2;
    }
  }
}

void draw7SegmentDigit(int16_t x, int16_t y, char digit, int16_t w, int16_t h, int16_t thickness, uint16_t color) {
  bool a = false, b = false, c = false, d = false, e = false, f = false, g = false;
  switch (digit) {
    case '0': a = b = c = d = e = f = true; break;
    case '1': b = c = true; break;
    case '2': a = b = g = e = d = true; break;
    case '3': a = b = g = c = d = true; break;
    case '4': f = g = b = c = true; break;
    case '5': a = f = g = c = d = true; break;
    case '6': a = f = g = e = c = d = true; break;
    case '7': a = b = c = true; break;
    case '8': a = b = c = d = e = f = g = true; break;
    case '9': a = b = c = d = f = g = true; break;
    case '-': g = true; break;
    case 'E': case 'e': a = f = g = e = d = true; break;
    case 'R': case 'r': e = g = true; break;
    default: break;
  }
  
  int16_t halfH = h / 2;
  
  // Segment A (top)
  if (a) tft.fillRect(x + thickness, y, w - 2 * thickness, thickness, color);
  // Segment B (top-right)
  if (b) tft.fillRect(x + w - thickness, y + thickness, thickness, halfH - thickness, color);
  // Segment C (bottom-right)
  if (c) tft.fillRect(x + w - thickness, y + halfH, thickness, halfH - thickness, color);
  // Segment D (bottom)
  if (d) tft.fillRect(x + thickness, y + h - thickness, w - 2 * thickness, thickness, color);
  // Segment E (bottom-left)
  if (e) tft.fillRect(x, y + halfH, thickness, halfH - thickness, color);
  // Segment F (top-left)
  if (f) tft.fillRect(x, y + thickness, thickness, halfH - thickness, color);
  // Segment G (middle)
  if (g) tft.fillRect(x + thickness, y + halfH - thickness / 2, w - 2 * thickness, thickness, color);
}

int16_t get7SegmentStringWidth(const char* str, int16_t charW, int16_t thickness, int16_t gap) {
  int16_t width = 0;
  while (*str) {
    if (*str == '.') {
      width += thickness * 2 + gap;
    } else {
      width += charW + gap;
    }
    str++;
  }
  if (width > 0) width -= gap;
  return width;
}

void draw7SegmentString(int16_t x, int16_t y, const char* str, int16_t charW, int16_t h, int16_t thickness, int16_t gap, uint16_t color) {
  int16_t curX = x;
  while (*str) {
    char c = *str;
    if (c == '.') {
      tft.fillRect(curX, y + h - thickness * 2, thickness * 2, thickness * 2, color);
      curX += thickness * 2 + gap;
    } else {
      draw7SegmentDigit(curX, y, c, charW, h, thickness, color);
      curX += charW + gap;
    }
    str++;
  }
}

void drawMainDashboard(float temp, float hum, int daysRem, float targetT, float targetH, bool wifiVal, bool gsmVal, bool mqttVal, bool motorState) {
  static float lastTemp = -999.0f;
  static float lastHum = -999.0f;
  static float lastTargetT = -999.0f;
  static float lastTargetH = -999.0f;
  static bool lastWifi = false;
  static bool lastGsm = false;
  static bool lastMqttConnectedState = false;
  static int lastGsmSignalBars = -1;
  static char lastGsmNetworkType[8] = "";
  static bool lastMainsHealthy = false;
  static bool lastBackupActive = false;
  static bool lastBackupFailed = false;
  static bool lastTurnEnabled = false;
  static int lastDayOfCycle = -1;
  static bool firstRun = true;
  static bool lastTempHealthy = true;
  static bool lastHumHealthy = true;

  if (!firstRun && (latestReadings.tempSensorHealthy != lastTempHealthy || latestReadings.humidSensorHealthy != lastHumHealthy)) {
    forceDisplayRedraw = true;
  }

  bool mainsHealthy = (digitalRead(PIN_MAINS_SENSE) == HIGH);
  bool backupFailed = (currentSettings.backupType == 2 && genFailState) || 
                      (currentSettings.backupType == 0 && upsFailState) || 
                      (currentSettings.backupType == 1 && invFailState);

  bool forceRedraw = forceDisplayRedraw || firstRun;

  if (forceRedraw) {
    forceDisplayRedraw = false;
    firstRun = false;
    lastTempHealthy = latestReadings.tempSensorHealthy;
    lastHumHealthy = latestReadings.humidSensorHealthy;
    tft.fillScreen(ST77XX_BLACK);
    
    // Draw visual containers - perfectly aligned to 320x240 screen
    tft.drawRoundRect(6, 4, 308, 22, 4, ST77XX_WHITE);         // Top status bar banner
    tft.drawRoundRect(6, 30, 150, 160, 6, ST77XX_WHITE);       // Temperature Gauge block (160px height)
    tft.drawRoundRect(164, 30, 150, 160, 6, ST77XX_WHITE);     // Humidity Gauge block (160px height)
    tft.drawRoundRect(6, 194, 308, 42, 6, ST77XX_WHITE);       // Footer controller action block

    // Draw static separators inside blocks
    tft.drawFastHLine(12, 151, 138, 0x18C3); // Soft gray line under temperature reading
    tft.drawFastHLine(170, 151, 138, 0x18C3); // Soft gray line under humidity reading

    // Draw static unit labels inside blocks
    tft.setFont(NULL);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setCursor(75, 131);
    tft.print("C");

    tft.setCursor(215, 131);
    tft.print("% RH");

    // Clear caches to force redraw of dynamic elements
    lastTemp = -999.0f;
    lastHum = -999.0f;
    lastTargetT = -999.0f;
    lastTargetH = -999.0f;
    lastWifi = !wifiVal;
    lastGsm = !gsmVal;
    lastMqttConnectedState = !mqttVal;
    lastMainsHealthy = !mainsHealthy;
    lastBackupActive = !backupActive;
    lastBackupFailed = !backupFailed;
    lastTurnEnabled = !currentSettings.turnEnabled;
    lastDayOfCycle = -1;
  }
  
  // 1. Draw status bar icons only when state changes to avoid flickering
  if (wifiVal != lastWifi || forceRedraw) {
    drawWifiSymbol(14, 7, wifiVal);
    lastWifi = wifiVal;
  }
  
  if (mqttVal != lastMqttConnectedState || forceRedraw) {
    drawServerSymbol(54, 7, mqttVal);
    lastMqttConnectedState = mqttVal;
  }

  bool gsmStateChanged = (gsmVal != lastGsm || 
                          gsmSignalBars != lastGsmSignalBars || 
                          strcmp(gsmNetworkType, lastGsmNetworkType) != 0);
  if (gsmStateChanged || forceRedraw) {
    drawCellularSymbol(105, 7, gsmVal);
    lastGsm = gsmVal;
    lastGsmSignalBars = gsmSignalBars;
    strcpy(lastGsmNetworkType, gsmNetworkType);
  }
  if (mainsHealthy != lastMainsHealthy || forceRedraw) {
    drawMainsLightningSymbol(155, 7, mainsHealthy);
    lastMainsHealthy = mainsHealthy;
  }
  if (backupActive != lastBackupActive || backupFailed != lastBackupFailed || forceRedraw) {
    drawBackupLightningSymbol(220, 7, backupActive, backupFailed);
    lastBackupActive = backupActive;
    lastBackupFailed = backupFailed;
  }

  // 2. Draw Temperature Gauge Arc and giant typography (Only when value changes)
  bool tempChanged = ((int)(temp * 10.0f + 0.5f) != (int)(lastTemp * 10.0f + 0.5f));
  if (tempChanged || forceRedraw) {
    if (latestReadings.tempSensorHealthy) {
      // Clear only the bounding box where the 7-segment numbers are printed to zero-out old segments quickly
      tft.fillRect(25, 71, 112, 46, ST77XX_BLACK);
      
      // Redraw Gauge Arc (it handles its own over-writing, so no background area clearing is needed!)
      drawGaugeArc(81, 110, 68, temp, 20.0f, 42.0f, ST77XX_WHITE);
      
      char tempStr[16];
      sprintf(tempStr, "%.1f", temp);
      int16_t tempW = get7SegmentStringWidth(tempStr, 28, 4, 4);
      int16_t tempX = 81 - (tempW / 2);
      draw7SegmentString(tempX, 71, tempStr, 28, 46, 4, 4, ST77XX_WHITE);
    } else {
      // Temp sensor disconnected: Clear the entire top inner container area (the arc zone) and display red 'Err'
      tft.fillRect(7, 31, 148, 120, ST77XX_BLACK);
      
      const char* tempStr = "Err";
      int16_t tempW = get7SegmentStringWidth(tempStr, 28, 4, 4);
      int16_t tempX = 81 - (tempW / 2);
      draw7SegmentString(tempX, 71, tempStr, 28, 46, 4, 4, ST77XX_RED);
    }
    
    lastTemp = temp;
  }

  // Draw Set temperature only when target temp is updated
  if (currentSettings.minTemp != lastTargetT || forceRedraw) {
    tft.fillRect(10, 168, 142, 20, ST77XX_BLACK); // Clear SET area at the absolute bottom
    tft.setFont(NULL);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setCursor(10, 172);
    tft.printf("%.1f-%.1f C", currentSettings.minTemp, currentSettings.maxTemp);
    lastTargetT = currentSettings.minTemp;
  }

  // 3. Draw Humidity Gauge Arc and giant typography (Only when value changes)
  bool humChanged = ((int)(hum + 0.5f) != (int)(lastHum + 0.5f));
  if (humChanged || forceRedraw) {
    if (latestReadings.humidSensorHealthy) {
      // Clear only the bounding box where the 7-segment numbers are printed
      tft.fillRect(190, 71, 98, 46, ST77XX_BLACK);

      // Redraw Gauge Arc (it handles its own over-writing, so no background area clearing is needed!)
      drawGaugeArc(239, 110, 68, hum, 30.0f, 85.0f, ST77XX_WHITE);

      char humStr[16];
      sprintf(humStr, "%.0f", hum);
      int16_t humW = get7SegmentStringWidth(humStr, 28, 4, 4);
      int16_t humX = 239 - (humW / 2);
      draw7SegmentString(humX, 71, humStr, 28, 46, 4, 4, ST77XX_WHITE);
    } else {
      // Humidity sensor disconnected: Clear the entire top inner container area (the arc zone) and display red 'Err'
      tft.fillRect(165, 31, 148, 120, ST77XX_BLACK);

      const char* humStr = "Err";
      int16_t humW = get7SegmentStringWidth(humStr, 28, 4, 4);
      int16_t humX = 239 - (humW / 2);
      draw7SegmentString(humX, 71, humStr, 28, 46, 4, 4, ST77XX_RED);
    }
    
    lastHum = hum;
  }

  // Draw Set humidity only when target hum is updated
  if (currentSettings.minHumidity != lastTargetH || forceRedraw) {
    tft.fillRect(168, 168, 142, 20, ST77XX_BLACK); // Clear SET area at the absolute bottom
    tft.setFont(NULL);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setCursor(173, 172);
    tft.printf("%.0f-%.0f%% RH", currentSettings.minHumidity, currentSettings.maxHumidity);
    lastTargetH = currentSettings.minHumidity;
  }

  // 4. Draw Footer layout with DAY, OK circle at center, and AUTO/MANUAL status (Only on changes)
  bool footerChanged = (latestReadings.dayOfCycle != lastDayOfCycle || currentSettings.turnEnabled != lastTurnEnabled);
  if (footerChanged || forceRedraw) {
    tft.fillRect(12, 198, 296, 34, ST77XX_BLACK);
    
    // Left: Day Counter matching size 2
    tft.setFont(NULL);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setCursor(14, 208);
    if (latestReadings.dayOfCycle == 0) {
      tft.printf("%02d/%02d", currentSettings.incubationDayStarted, currentSettings.incubationMonthStarted);
    } else {
      tft.printf("DAY %d", latestReadings.dayOfCycle);
    }

    // Center: Filled Circle for "OK" key cue at absolute x=160 center
    tft.fillCircle(160, 215, 12, ST77XX_WHITE);
    tft.setTextColor(ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setCursor(154, 211);
    tft.print("OK");
    
    // Right: AUTO, MANUAL, or STOPPED status matching size 2
    tft.setTextSize(2);
    if (currentSettings.stopTurningDay > 0 && latestReadings.dayOfCycle >= currentSettings.stopTurningDay) {
      tft.setCursor(226, 208);
      tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
      tft.print("STOPPED");
    } else if (currentSettings.turnEnabled) {
      tft.setCursor(244, 208);
      tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
      tft.print("AUTO");
    } else {
      tft.setCursor(226, 208);
      tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
      tft.print("MANUAL");
    }
    
    lastDayOfCycle = latestReadings.dayOfCycle;
    lastTurnEnabled = currentSettings.turnEnabled;
  }
}

void drawMainOptionMenu() {
  tft.fillScreen(ST77XX_BLACK);
  tft.drawRoundRect(6, 6, 308, 228, 8, ST77XX_YELLOW);
  
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);
  tft.setCursor(20, 18);
  tft.println("INCUBATOR MAIN MENU");
  tft.drawFastHLine(20, 38, 280, 0xAD55); // Custom Gray horizontal line

  const char* options[] = {
    "1. Set temperature",
    "2. Set Humidity",
    "3. Turning & Backup",
    "4. Time & clock set",
    "5. Network & internet",
    "6. App & updates"
  };

  tft.setTextSize(2);
  for (int i = 0; i < 6; i++) {
    int y = 46 + (i * 22);
    if (i == mainMenuIndex) {
      tft.fillRect(14, y - 3, 292, 19, ST77XX_BLUE);
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(20, y);
      tft.printf("> %s", options[i]);
    } else {
      tft.setTextColor(ST77XX_CYAN);
      tft.setCursor(20, y);
      tft.printf("  %s", options[i]);
    }
  }

  // Draw Back red button at the bottom (y=182 to y=216, height 34, width 280)
  int btnY = 182;
  int btnH = 34;
  if (mainMenuIndex == 6) {
    tft.fillRoundRect(20, btnY, 280, btnH, 6, ST77XX_RED);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(136, btnY + 10);
    tft.print("BACK");
  } else {
    tft.drawRoundRect(20, btnY, 280, btnH, 6, ST77XX_RED);
    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(2);
    tft.setCursor(136, btnY + 10);
    tft.print("BACK");
  }
}

void drawManualSettingsMenu(const char* label, float currentSetVal, const char* unit, int menuIdx) {
  if (saveState == 1 && saveProgress > 0) {
    // Highly optimized flicker-free redraw: only update the bar width and current percentage
    int barW = (200 - 6) * saveProgress / 100;
    tft.fillRect(57, 93, barW, 18, ST77XX_GREEN);
    
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setCursor(110, 140);
    tft.printf("%d%% written... ", saveProgress);
    return;
  }

  tft.fillScreen(ST77XX_BLACK);
  tft.drawRoundRect(6, 6, 308, 228, 8, ST77XX_YELLOW);
  
  if (menuIdx == 1 || menuIdx == 2) {
    // ----------------- TEMP & HUMIDITY SETTINGS -----------------
    if (saveState == 1) {
      // Draw progress view (at 0%)
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(2);
      tft.setCursor(45, 45);
      tft.println("SAVING TO MEMORY");
      tft.drawRoundRect(54, 90, 200, 24, 6, 0x5AEB);
      
      // Draw progress bar inside - Colored GREEN
      int barW = (200 - 6) * saveProgress / 100;
      tft.fillRect(57, 93, barW, 18, ST77XX_GREEN);
      
      tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
      tft.setTextSize(1);
      tft.setCursor(110, 140);
      tft.printf("%d%% written... ", saveProgress);
      return;
    }
    
    if (saveState == 2) {
      // Draw success tick
      tft.drawCircle(160, 90, 30, ST77XX_GREEN);
      tft.drawLine(150, 90, 157, 98, ST77XX_GREEN);
      tft.drawLine(157, 98, 175, 80, ST77XX_GREEN);
      
      tft.setTextColor(ST77XX_GREEN);
      tft.setTextSize(2);
      tft.setCursor(125, 145);
      tft.println("SAVED!");
      
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(1);
      tft.setCursor(115, 180);
      tft.println("EEPROM updated");
      return;
    }
    
    if (saveState == 3) {
      // Draw fail cross
      tft.drawCircle(160, 90, 30, ST77XX_RED);
      tft.drawLine(150, 80, 170, 100, ST77XX_RED);
      tft.drawLine(170, 80, 150, 100, ST77XX_RED);
      
      tft.setTextColor(ST77XX_RED);
      tft.setTextSize(2);
      tft.setCursor(120, 145);
      tft.println("FAILED!");
      return;
    }

    tft.setTextSize(2);
    if (menuIdx == 1) {
      tft.setTextColor(ST77XX_YELLOW);
      tft.setCursor(70, 20);
      tft.println("SET TEMPERATURE");
      tft.drawFastHLine(70, 38, 180, ST77XX_YELLOW);
    } else {
      tft.setTextColor(0x5DFF); // Sky blue
      tft.setCursor(88, 20);
      tft.println("SET HUMIDITY");
      tft.drawFastHLine(88, 38, 144, 0x5DFF);
    }

    // Min threshold card
    int minSelected = (menuIdx == 1) ? (tempSubFocusIdx == 0) : (humidSubFocusIdx == 0);
    if (minSelected) {
      if (isEditingValue) {
        tft.fillRoundRect(20, 72, 278, 42, 6, 0x2124); // Selected and editing (warm/dark blue)
        tft.drawRoundRect(20, 72, 278, 42, 6, ST77XX_YELLOW);
        tft.setTextColor(ST77XX_YELLOW);
      } else {
        tft.fillRoundRect(20, 72, 278, 42, 6, 0x18EA); // Selected (blue-gray)
        tft.drawRoundRect(20, 72, 278, 42, 6, ST77XX_CYAN);
        tft.setTextColor(ST77XX_WHITE);
      }
    } else {
      tft.drawRoundRect(20, 72, 278, 42, 6, 0x4208); // Normal border
      tft.setTextColor(0xAD55); // Gray text
    }
    tft.setTextSize(2);
    tft.setCursor(35, 84);
    if (menuIdx == 1) {
      tft.printf("MIN TEMP:   %.1f C", tempSettings.minTemp);
    } else {
      tft.printf("MIN HUMID:  %.1f %%", tempSettings.minHumidity);
    }

    // Max threshold card
    int maxSelected = (menuIdx == 1) ? (tempSubFocusIdx == 1) : (humidSubFocusIdx == 1);
    if (maxSelected) {
      if (isEditingValue) {
        tft.fillRoundRect(20, 126, 278, 42, 6, 0x2124);
        tft.drawRoundRect(20, 126, 278, 42, 6, ST77XX_YELLOW);
        tft.setTextColor(ST77XX_YELLOW);
      } else {
        tft.fillRoundRect(20, 126, 278, 42, 6, 0x18EA);
        tft.drawRoundRect(20, 126, 278, 42, 6, ST77XX_CYAN);
        tft.setTextColor(ST77XX_WHITE);
      }
    } else {
      tft.drawRoundRect(20, 126, 278, 42, 6, 0x4208);
      tft.setTextColor(0xAD55);
    }
    tft.setTextSize(2);
    tft.setCursor(35, 138);
    if (menuIdx == 1) {
      tft.printf("MAX TEMP:   %.1f C", tempSettings.maxTemp);
    } else {
      tft.printf("MAX HUMID:  %.1f %%", tempSettings.maxHumidity);
    }

    // Back and Save Buttons side-by-side
    int backSelected = (menuIdx == 1) ? (tempSubFocusIdx == 2) : (humidSubFocusIdx == 2);
    int saveSelected = (menuIdx == 1) ? (tempSubFocusIdx == 3) : (humidSubFocusIdx == 3);
    
    // Back button
    if (backSelected) {
      tft.fillRoundRect(20, 184, 130, 36, 6, ST77XX_RED);
      tft.setTextColor(ST77XX_WHITE);
    } else {
      tft.drawRoundRect(20, 184, 130, 36, 6, ST77XX_RED);
      tft.setTextColor(ST77XX_RED);
    }
    tft.setTextSize(2);
    tft.setCursor(62, 194);
    tft.print("Back");

    // Save button
    if (saveSelected) {
      tft.fillRoundRect(168, 184, 130, 36, 6, ST77XX_BLUE);
      tft.setTextColor(ST77XX_WHITE);
    } else {
      tft.drawRoundRect(168, 184, 130, 36, 6, ST77XX_BLUE);
      tft.setTextColor(ST77XX_BLUE);
    }
    tft.setTextSize(2);
    tft.setCursor(210, 194);
    tft.print("Save");

  } else if (menuIdx == 3) {
    // ----------------- TURNING & BACKUP OPTION MENU -----------------
    tft.setTextSize(2);
    tft.setTextColor(0x5AEB); // Blue-ish sky
    tft.setCursor(65, 12);
    tft.println("TURNING & BACKUP");
    tft.drawFastHLine(65, 30, 190, 0x5AEB);

    // Dynamic bigger items for matching font size with settings menu
    // Option 1: Turning Setup
    int opt0Selected = (turnSubFocusIdx == 0);
    if (opt0Selected) {
      tft.fillRoundRect(20, 60, 278, 28, 6, 0x18EA); // Cyan-blue fill
      tft.drawRoundRect(20, 60, 278, 28, 6, ST77XX_CYAN);
      tft.setTextColor(ST77XX_WHITE);
    } else {
      tft.drawRoundRect(20, 60, 278, 28, 6, 0x4208); // Dark gray outline
      tft.setTextColor(0xAD55);
    }
    tft.setCursor(35, 67);
    tft.print(opt0Selected ? "> 1. Turning Setup" : "  1. Turning Setup");

    // Option 2: Backup Setup
    int opt1Selected = (turnSubFocusIdx == 1);
    if (opt1Selected) {
      tft.fillRoundRect(20, 105, 278, 28, 6, 0x18EA);
      tft.drawRoundRect(20, 105, 278, 28, 6, ST77XX_CYAN);
      tft.setTextColor(ST77XX_WHITE);
    } else {
      tft.drawRoundRect(20, 105, 278, 28, 6, 0x4208);
      tft.setTextColor(0xAD55);
    }
    tft.setCursor(35, 112);
    tft.print(opt1Selected ? "> 2. Backup Setup" : "  2. Backup Setup");

    // BACK option at the bottom
    int opt2Selected = (turnSubFocusIdx == 2);
    if (opt2Selected) {
      tft.fillRoundRect(20, 185, 280, 34, 6, ST77XX_RED);
      tft.setTextColor(ST77XX_WHITE);
    } else {
      tft.drawRoundRect(20, 185, 280, 34, 6, ST77XX_RED);
      tft.setTextColor(ST77XX_RED);
    }
    tft.setCursor(136, 195);
    tft.print("BACK");

  } else if (menuIdx == 8) {
    // ----------------- TURNING SETUP SUBMENU -----------------
    if (saveState == 1) {
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(2);
      tft.setCursor(45, 45);
      tft.println("SAVING TO MEMORY");
      tft.drawRoundRect(54, 90, 200, 24, 6, 0x5AEB);
      int barW = (200 - 6) * saveProgress / 100;
      tft.fillRect(57, 93, barW, 18, ST77XX_GREEN);
      tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
      tft.setTextSize(1);
      tft.setCursor(110, 140);
      tft.printf("%d%% written... ", saveProgress);
      return;
    }
    if (saveState == 2) {
      tft.drawCircle(160, 90, 30, ST77XX_GREEN);
      tft.drawLine(150, 90, 157, 98, ST77XX_GREEN);
      tft.drawLine(157, 98, 175, 80, ST77XX_GREEN);
      tft.setTextColor(ST77XX_GREEN);
      tft.setTextSize(2);
      tft.setCursor(125, 145);
      tft.println("SAVED!");
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(1);
      tft.setCursor(115, 180);
      tft.println("EEPROM updated");
      return;
    }
    if (showRecountConfirmation) {
      tft.drawCircle(160, 90, 30, ST77XX_GREEN);
      tft.drawLine(150, 90, 157, 98, ST77XX_GREEN);
      tft.drawLine(157, 98, 175, 80, ST77XX_GREEN);
      tft.setTextColor(ST77XX_GREEN);
      tft.setTextSize(2);
      tft.setCursor(85, 145);
      tft.println("COUNTING BEGAN!");
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(1);
      tft.setCursor(55, 180);
      tft.println("Incubation days reset successfully");
      return;
    }

    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(85, 8);
    tft.println("TURNING SETUP");
    tft.drawFastHLine(85, 24, 150, ST77XX_WHITE);

    tft.setTextSize(2);

    // Row 0: Interval
    int r0Sel = (turnSetupSubFocusIdx == 0);
    bool isIntervalN_A = !tempSettings.turnEnabled;
    if (isIntervalN_A) {
      tft.drawRoundRect(20, 30, 278, 20, 4, 0x2211);
      tft.setTextColor(0x52AA);
    } else if (r0Sel) {
      if (isEditingValue) {
        tft.fillRoundRect(20, 30, 278, 20, 4, 0x2124);
        tft.drawRoundRect(20, 30, 278, 20, 4, ST77XX_YELLOW);
        tft.setTextColor(ST77XX_YELLOW);
      } else {
        tft.fillRoundRect(20, 30, 278, 20, 4, 0x18EA);
        tft.drawRoundRect(20, 30, 278, 20, 4, ST77XX_CYAN);
        tft.setTextColor(ST77XX_WHITE);
      }
    } else {
      tft.drawRoundRect(20, 30, 278, 20, 4, 0x4208);
      tft.setTextColor(0xAD55);
    }
    tft.setCursor(30, 34);
    if (isIntervalN_A) {
      tft.print("Interval:   N/A");
    } else if (tempSettings.turnIntervalHours < 60) {
      tft.printf("Interval: %d Min", tempSettings.turnIntervalHours);
    } else {
      tft.printf("Interval: %.1f Hrs", (float)tempSettings.turnIntervalHours / 60.0f);
    }

    // Row 1: Turn mode
    int r1Sel = (turnSetupSubFocusIdx == 1);
    if (r1Sel) {
      if (isEditingValue) {
        tft.fillRoundRect(20, 53, 278, 20, 4, 0x2124);
        tft.drawRoundRect(20, 53, 278, 20, 4, ST77XX_YELLOW);
        tft.setTextColor(ST77XX_YELLOW);
      } else {
        tft.fillRoundRect(20, 53, 278, 20, 4, 0x18EA);
        tft.drawRoundRect(20, 53, 278, 20, 4, ST77XX_CYAN);
        tft.setTextColor(ST77XX_WHITE);
      }
    } else {
      tft.drawRoundRect(20, 53, 278, 20, 4, 0x4208);
      tft.setTextColor(0xAD55);
    }
    tft.setCursor(30, 57);
    tft.printf("Turn Mode: %s", tempSettings.turnEnabled ? "AUTO" : "MANUAL");

    // Row 2: Total Days
    int r2Sel = (turnSetupSubFocusIdx == 2);
    bool isTotalN_A = !tempSettings.turnEnabled;
    if (isTotalN_A) {
      tft.drawRoundRect(20, 76, 278, 20, 4, 0x2211);
      tft.setTextColor(0x52AA);
    } else if (r2Sel) {
      if (isEditingValue) {
        tft.fillRoundRect(20, 76, 278, 20, 4, 0x2124);
        tft.drawRoundRect(20, 76, 278, 20, 4, ST77XX_YELLOW);
        tft.setTextColor(ST77XX_YELLOW);
      } else {
        tft.fillRoundRect(20, 76, 278, 20, 4, 0x18EA);
        tft.drawRoundRect(20, 76, 278, 20, 4, ST77XX_CYAN);
        tft.setTextColor(ST77XX_WHITE);
      }
    } else {
      tft.drawRoundRect(20, 76, 278, 20, 4, 0x4208);
      tft.setTextColor(0xAD55);
    }
    tft.setCursor(30, 80);
    if (isTotalN_A) {
      tft.print("Total Days: N/A");
    } else {
      tft.printf("Total Days: %d Days", tempSettings.totalIncubationDays);
    }

    // Row 3: Stop Day
    int r3Sel = (turnSetupSubFocusIdx == 3);
    bool isStopN_A = !tempSettings.turnEnabled;
    if (isStopN_A) {
      tft.drawRoundRect(20, 99, 278, 20, 4, 0x2211);
      tft.setTextColor(0x52AA);
    } else if (r3Sel) {
      if (isEditingValue) {
        tft.fillRoundRect(20, 99, 278, 20, 4, 0x2124);
        tft.drawRoundRect(20, 99, 278, 20, 4, ST77XX_YELLOW);
        tft.setTextColor(ST77XX_YELLOW);
      } else {
        tft.fillRoundRect(20, 99, 278, 20, 4, 0x18EA);
        tft.drawRoundRect(20, 99, 278, 20, 4, ST77XX_CYAN);
        tft.setTextColor(ST77XX_WHITE);
      }
    } else {
      tft.drawRoundRect(20, 99, 278, 20, 4, 0x4208);
      tft.setTextColor(0xAD55);
    }
    tft.setCursor(30, 103);
    if (isStopN_A) {
      tft.print("Stop Day:   N/A");
    } else if (tempSettings.stopTurningDay == 0) {
      tft.print("Stop Day:  None");
    } else {
      tft.printf("Stop Day:   Day %d", tempSettings.stopTurningDay);
    }

    // Row 4: Start Month
    int r4Sel = (turnSetupSubFocusIdx == 4);
    bool isMonN_A = !tempSettings.turnEnabled;
    if (isMonN_A) {
      tft.drawRoundRect(20, 122, 278, 20, 4, 0x2211);
      tft.setTextColor(0x52AA);
    } else if (r4Sel) {
      if (isEditingValue) {
        tft.fillRoundRect(20, 122, 278, 20, 4, 0x2124);
        tft.drawRoundRect(20, 122, 278, 20, 4, ST77XX_YELLOW);
        tft.setTextColor(ST77XX_YELLOW);
      } else {
        tft.fillRoundRect(20, 122, 278, 20, 4, 0x18EA);
        tft.drawRoundRect(20, 122, 278, 20, 4, ST77XX_CYAN);
        tft.setTextColor(ST77XX_WHITE);
      }
    } else {
      tft.drawRoundRect(20, 122, 278, 20, 4, 0x4208);
      tft.setTextColor(0xAD55);
    }
    tft.setCursor(30, 126);
    if (isMonN_A) {
      tft.print("Start Month: N/A");
    } else {
      tft.printf("Start Month: %02d", tempSettings.incubationMonthStarted);
    }

    // Row 5: Start Day
    int r5Sel = (turnSetupSubFocusIdx == 5);
    bool isDayN_A = !tempSettings.turnEnabled;
    if (isDayN_A) {
      tft.drawRoundRect(20, 145, 278, 20, 4, 0x2211);
      tft.setTextColor(0x52AA);
    } else if (r5Sel) {
      if (isEditingValue) {
        tft.fillRoundRect(20, 145, 278, 20, 4, 0x2124);
        tft.drawRoundRect(20, 145, 278, 20, 4, ST77XX_YELLOW);
        tft.setTextColor(ST77XX_YELLOW);
      } else {
        tft.fillRoundRect(20, 145, 278, 20, 4, 0x18EA);
        tft.drawRoundRect(20, 145, 278, 20, 4, ST77XX_CYAN);
        tft.setTextColor(ST77XX_WHITE);
      }
    } else {
      tft.drawRoundRect(20, 145, 278, 20, 4, 0x4208);
      tft.setTextColor(0xAD55);
    }
    tft.setCursor(30, 149);
    if (isDayN_A) {
      tft.print("Start Day:   N/A");
    } else {
      tft.printf("Start Day:   %02d", tempSettings.incubationDayStarted);
    }

    // Row 6: Begin count now button
    int r6Sel = (turnSetupSubFocusIdx == 6);
    bool isBtnN_A = !tempSettings.turnEnabled;
    if (isBtnN_A) {
      tft.drawRoundRect(20, 168, 278, 22, 6, 0x2211);
      tft.setTextColor(0x52AA);
    } else if (r6Sel) {
      tft.fillRoundRect(20, 168, 278, 22, 6, 0x51E9);
      tft.drawRoundRect(20, 168, 278, 22, 6, ST77XX_MAGENTA);
      tft.setTextColor(ST77XX_WHITE);
    } else {
      tft.drawRoundRect(20, 168, 278, 22, 6, 0x318A);
      tft.setTextColor(0x8410);
    }
    tft.setCursor(38, 172);
    if (isBtnN_A) {
      tft.print("Begin count: N/A");
    } else {
      RtcDateTime nowTime = RtcModule.GetDateTime();
      if (tempSettings.incubationDayStarted == nowTime.Day() && tempSettings.incubationMonthStarted == nowTime.Month()) {
        tft.print("Set to Start TODAY");
      } else {
        tft.print("Begin count now");
      }
    }

    // Row 7 (Back) and Row 8 (Save) Buttons at bottom
    int backSel = (turnSetupSubFocusIdx == 7);
    int saveSel = (turnSetupSubFocusIdx == 8);

    if (backSel) {
      tft.fillRoundRect(20, 196, 130, 34, 6, ST77XX_RED);
      tft.setTextColor(ST77XX_WHITE);
    } else {
      tft.drawRoundRect(20, 196, 130, 34, 6, ST77XX_RED);
      tft.setTextColor(ST77XX_RED);
    }
    tft.setCursor(62, 206);
    tft.print("Back");

    if (saveSel) {
      tft.fillRoundRect(168, 196, 130, 34, 6, ST77XX_BLUE);
      tft.setTextColor(ST77XX_WHITE);
    } else {
      tft.drawRoundRect(168, 196, 130, 34, 6, ST77XX_BLUE);
      tft.setTextColor(ST77XX_BLUE);
    }
    tft.setCursor(210, 206);
    tft.print("Save");

  } else if (menuIdx == 9) {
    // ----------------- BACKUP SETUP SUBMENU -----------------
    if (saveState == 1) {
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(2);
      tft.setCursor(45, 45);
      tft.println("SAVING TO MEMORY");
      tft.drawRoundRect(54, 90, 200, 24, 6, 0x5AEB);
      int barW = (200 - 6) * saveProgress / 100;
      tft.fillRect(57, 93, barW, 18, ST77XX_GREEN);
      tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
      tft.setTextSize(1);
      tft.setCursor(110, 140);
      tft.printf("%d%% written... ", saveProgress);
      return;
    }
    if (saveState == 2) {
      tft.drawCircle(160, 90, 30, ST77XX_GREEN);
      tft.drawLine(150, 90, 157, 98, ST77XX_GREEN);
      tft.drawLine(157, 98, 175, 80, ST77XX_GREEN);
      tft.setTextColor(ST77XX_GREEN);
      tft.setTextSize(2);
      tft.setCursor(125, 145);
      tft.println("SAVED!");
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(1);
      tft.setCursor(115, 180);
      tft.println("EEPROM updated");
      return;
    }

    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(90, 8);
    tft.println("BACKUP SETUP");
    tft.drawFastHLine(90, 24, 140, ST77XX_WHITE);

    tft.setTextSize(2);

    int startItem = 0;
    if (backupSetupSubFocusIdx >= 0 && backupSetupSubFocusIdx <= 7) {
      if (backupSetupSubFocusIdx >= 4) {
        startItem = backupSetupSubFocusIdx - 3;
        if (startItem > 3) startItem = 3;
      }
    } else {
      startItem = 3;
    }

    for (int visibleIdx = 0; visibleIdx < 5; visibleIdx++) {
      int i = startItem + visibleIdx;
      int itemY = 32 + (visibleIdx * 29);
      bool isSelected = (backupSetupSubFocusIdx == i);
      bool isN_A = false;
      String itemText = "";

      if (i == 0) {
        const char* typeStr = "UPS";
        if (tempSettings.backupType == 1) typeStr = "INVERTER";
        else if (tempSettings.backupType == 2) typeStr = "GENERATOR";
        itemText = String("Source:  ") + typeStr;
      } else if (i == 1) {
        if (tempSettings.backupType == 1) isN_A = true;
        itemText = isN_A ? "Duration:N/A" : String("Durat.:  ") + String(tempSettings.backupPressDurationSec, 1) + "s";
      } else if (i == 2) {
        if (tempSettings.backupType != 2) isN_A = true;
        itemText = isN_A ? "Choke:   N/A" : String("Choke:   ") + (tempSettings.chokeServoEnabled ? "ON" : "OFF");
      } else if (i == 3) {
        if (tempSettings.backupType != 2) isN_A = true;
        itemText = isN_A ? "Retries: N/A" : String("Retries: ") + String(tempSettings.maxGenStartRetries) + "x";
      } else if (i == 4) {
        if (tempSettings.backupType != 2 || !tempSettings.chokeServoEnabled) isN_A = true;
        itemText = isN_A ? "ChokeAng:N/A" : String("ChokeAng:") + String(tempSettings.chokeServoDegrees) + "d";
      } else if (i == 5) {
        itemText = String("Sched.B: ") + (tempSettings.backupSchedEnabled ? "ON" : "OFF");
      } else if (i == 6) {
        if (!tempSettings.backupSchedEnabled) isN_A = true;
        itemText = isN_A ? "StartHr: N/A" : String("StartHr: ") + (tempSettings.backupSchedStartHour < 10 ? "0" : "") + String(tempSettings.backupSchedStartHour) + ":00";
      } else if (i == 7) {
        if (!tempSettings.backupSchedEnabled) isN_A = true;
        itemText = isN_A ? "EndHr:   N/A" : String("EndHr:   ") + (tempSettings.backupSchedEndHour < 10 ? "0" : "") + String(tempSettings.backupSchedEndHour) + ":00";
      }

      if (isN_A) {
        tft.drawRoundRect(20, itemY, 268, 25, 4, 0x2104);
        tft.setTextColor(0x52AA);
      } else if (isSelected) {
        if (isEditingValue) {
          tft.fillRoundRect(20, itemY, 268, 25, 4, 0x2124);
          tft.drawRoundRect(20, itemY, 268, 25, 4, ST77XX_YELLOW);
          tft.setTextColor(ST77XX_YELLOW);
        } else {
          tft.fillRoundRect(20, itemY, 268, 25, 4, 0x18EA);
          tft.drawRoundRect(20, itemY, 268, 25, 4, ST77XX_CYAN);
          tft.setTextColor(ST77XX_WHITE);
        }
      } else {
        tft.drawRoundRect(20, itemY, 268, 25, 4, 0x4208);
        tft.setTextColor(0xAD55);
      }

      tft.setCursor(28, itemY + 5);
      tft.print(itemText);
    }

    // Draw subtle scrollbar on the right
    tft.drawFastVLine(298, 32, 140, 0x2104); // subtle grey track
    int thumbHeight = 45; 
    int thumbY = 32 + (startItem * (140 - thumbHeight) / 3);
    tft.fillRect(296, thumbY, 5, thumbHeight, ST77XX_CYAN); // cyan scrollbar thumb

    // Row 8 (Back) and Row 9 (Save) Buttons at bottom
    int backSel = (backupSetupSubFocusIdx == 8);
    int saveSel = (backupSetupSubFocusIdx == 9);

    tft.setTextSize(2);
    if (backSel) {
      tft.fillRoundRect(20, 186, 130, 34, 6, ST77XX_RED);
      tft.setTextColor(ST77XX_WHITE);
    } else {
      tft.drawRoundRect(20, 186, 130, 34, 6, ST77XX_RED);
      tft.setTextColor(ST77XX_RED);
    }
    tft.setCursor(62, 196);
    tft.print("Back");

    if (saveSel) {
      tft.fillRoundRect(168, 186, 130, 34, 6, ST77XX_BLUE);
      tft.setTextColor(ST77XX_WHITE);
    } else {
      tft.drawRoundRect(168, 186, 130, 34, 6, ST77XX_BLUE);
      tft.setTextColor(ST77XX_BLUE);
    }
    tft.setCursor(210, 196);
    tft.print("Save");
  } else {
    // ----------------- GENERIC SINGLE SETTING (Turn Interval, etc.) -----------------
    tft.setTextColor(ST77XX_MAGENTA);
    tft.setTextSize(2);
    tft.setCursor(25, 20);
    tft.println("THRESHOLD SETTINGS");
    tft.drawFastHLine(25, 42, 270, 0x52AA);

    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.setCursor(25, 65);
    tft.printf("ADJUST: %s", label);

    tft.setTextSize(4);
    tft.setTextColor(ST77XX_GREEN);
    tft.setCursor(45, 100);
    tft.printf("%.1f %s", currentSetVal, unit);

    tft.setTextSize(1);
    tft.setTextColor(ST77XX_CYAN);
    tft.setCursor(20, 175);
    tft.println("UP [Increase] | DOWN [Decrease] button ticks");
    tft.setCursor(20, 198);
    tft.println("Press OK key button to SAVE and exit back");
  }
}

void drawLiveClockDisplay() {
  tft.fillRect(21, 30, 276, 18, ST77XX_BLACK); // Clear inside info box
  RtcDateTime now = RtcModule.GetDateTime();
  tft.setTextSize(1);
  tft.setFont(NULL);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(28, 35);
  
  char timeStr[48];
  if (tempSettings.use24HourFormat) {
    sprintf(timeStr, "LIVE: %02d/%02d/%04d %02d:%02d:%02d", now.Day(), now.Month(), now.Year(), now.Hour(), now.Minute(), now.Second());
  } else {
    int hr12 = now.Hour() % 12;
    if (hr12 == 0) hr12 = 12;
    const char* pmAmText = (now.Hour() >= 12) ? "PM" : "AM";
    sprintf(timeStr, "LIVE: %02d/%02d/%04d %02d:%02d:%02d %s", now.Day(), now.Month(), now.Year(), hr12, now.Minute(), now.Second(), pmAmText);
  }
  tft.print(timeStr);
}

void drawTimeAdjustmentMenu() {
  if (saveState == 1 && saveProgress > 0) {
    int barW = (200 - 6) * saveProgress / 100;
    tft.fillRect(57, 93, barW, 18, ST77XX_GREEN);
    
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setCursor(110, 140);
    tft.printf("%d%% written... ", saveProgress);
    return;
  }

  if (saveState == 1) {
    tft.fillScreen(ST77XX_BLACK);
    tft.drawRoundRect(6, 6, 308, 228, 8, ST77XX_YELLOW);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(45, 45);
    tft.println("SAVING TO MEMORY");
    tft.drawRoundRect(54, 90, 200, 24, 6, 0x5AEB);
    int barW = (200 - 6) * saveProgress / 100;
    tft.fillRect(57, 93, barW, 18, ST77XX_GREEN);
    
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setCursor(110, 140);
    tft.printf("%d%% written... ", saveProgress);
    return;
  }

  if (saveState == 2) {
    tft.fillScreen(ST77XX_BLACK);
    tft.drawRoundRect(6, 6, 308, 228, 8, ST77XX_YELLOW);
    tft.drawCircle(160, 90, 30, ST77XX_GREEN);
    tft.drawLine(150, 90, 157, 98, ST77XX_GREEN);
    tft.drawLine(157, 98, 175, 80, ST77XX_GREEN);
    tft.setTextColor(ST77XX_GREEN);
    tft.setTextSize(2);
    tft.setCursor(125, 145);
    tft.println("SAVED!");
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.setCursor(115, 180);
    tft.println("EEPROM updated");
    return;
  }

  tft.fillScreen(ST77XX_BLACK);
  tft.drawRoundRect(6, 6, 308, 228, 8, ST77XX_YELLOW);
  
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(85, 8);
  tft.println("TIME & CLOCK SET");
  tft.drawFastHLine(85, 24, 150, ST77XX_WHITE);

  // Live Current Clock Panel (Info Only, can't focus/edit directly here)
  tft.drawRoundRect(20, 29, 278, 20, 4, 0x4208);
  drawLiveClockDisplay();

  tft.setTextSize(2);

  // Row 0: Clock Format
  int r0Sel = (clockSetupActiveIndex == 0);
  if (r0Sel) {
    if (isEditingValue) {
      tft.fillRoundRect(20, 53, 278, 20, 4, 0x2124);
      tft.drawRoundRect(20, 53, 278, 20, 4, ST77XX_YELLOW);
      tft.setTextColor(ST77XX_YELLOW);
    } else {
      tft.fillRoundRect(20, 53, 278, 20, 4, 0x18EA);
      tft.drawRoundRect(20, 53, 278, 20, 4, ST77XX_CYAN);
      tft.setTextColor(ST77XX_WHITE);
    }
  } else {
    tft.drawRoundRect(20, 53, 278, 20, 4, 0x4208);
    tft.setTextColor(0xAD55);
  }
  tft.setCursor(30, 57);
  tft.printf("Format:    %s", tempSettings.use24HourFormat ? "24-Hour" : "12-Hour");

  // Row 1: Adjust Time
  char timeDisplay[32];
  if (tempSettings.use24HourFormat) {
    if (isEditingValue && clockSetupActiveIndex == 1) {
      if (clockTimeEditState == 0) {
        sprintf(timeDisplay, "[%02d] : %02d", tempClockHour, tempClockMinute);
      } else {
        sprintf(timeDisplay, "%02d : [%02d]", tempClockHour, tempClockMinute);
      }
    } else {
      sprintf(timeDisplay, "%02d : %02d", tempClockHour, tempClockMinute);
    }
  } else {
    int hr12 = tempClockHour % 12;
    if (hr12 == 0) hr12 = 12;
    const char* ampm = (tempClockHour >= 12) ? "PM" : "AM";
    if (isEditingValue && clockSetupActiveIndex == 1) {
      if (clockTimeEditState == 0) {
        sprintf(timeDisplay, "[%02d] : %02d %s", hr12, tempClockMinute, ampm);
      } else {
        sprintf(timeDisplay, "%02d : [%02d] %s", hr12, tempClockMinute, ampm);
      }
    } else {
      sprintf(timeDisplay, "%02d : %02d %s", hr12, tempClockMinute, ampm);
    }
  }

  int r1Sel = (clockSetupActiveIndex == 1);
  if (r1Sel) {
    if (isEditingValue) {
      tft.fillRoundRect(20, 76, 278, 20, 4, 0x2124);
      tft.drawRoundRect(20, 76, 278, 20, 4, ST77XX_YELLOW);
      tft.setTextColor(ST77XX_YELLOW);
    } else {
      tft.fillRoundRect(20, 76, 278, 20, 4, 0x18EA);
      tft.drawRoundRect(20, 76, 278, 20, 4, ST77XX_CYAN);
      tft.setTextColor(ST77XX_WHITE);
    }
  } else {
    tft.drawRoundRect(20, 76, 278, 20, 4, 0x4208);
    tft.setTextColor(0xAD55);
  }
  tft.setCursor(30, 80);
  tft.printf("Set time: %s", timeDisplay);

  // Row 2: Set Date
  char dateDisplay[32];
  if (isEditingValue && clockSetupActiveIndex == 2) {
    if (clockDateEditState == 0) {
      sprintf(dateDisplay, "[%02d]/%02d/%04d", tempClockDay, tempClockMonth, tempClockYear);
    } else if (clockDateEditState == 1) {
      sprintf(dateDisplay, "%02d/[%02d]/%04d", tempClockDay, tempClockMonth, tempClockYear);
    } else {
      sprintf(dateDisplay, "%02d/%02d/[%04d]", tempClockDay, tempClockMonth, tempClockYear);
    }
  } else {
    sprintf(dateDisplay, "%02d/%02d/%04d", tempClockDay, tempClockMonth, tempClockYear);
  }

  int r2Sel = (clockSetupActiveIndex == 2);
  if (r2Sel) {
    if (isEditingValue) {
      tft.fillRoundRect(20, 99, 278, 20, 4, 0x2124);
      tft.drawRoundRect(20, 99, 278, 20, 4, ST77XX_YELLOW);
      tft.setTextColor(ST77XX_YELLOW);
    } else {
      tft.fillRoundRect(20, 99, 278, 20, 4, 0x18EA);
      tft.drawRoundRect(20, 99, 278, 20, 4, ST77XX_CYAN);
      tft.setTextColor(ST77XX_WHITE);
    }
  } else {
    tft.drawRoundRect(20, 99, 278, 20, 4, 0x4208);
    tft.setTextColor(0xAD55);
  }
  tft.setCursor(30, 103);
  tft.printf("Set date: %s", dateDisplay);

  // Row 3 (Back) and Row 4 (Save) Buttons at bottom
  int backSel = (clockSetupActiveIndex == 3);
  int saveSel = (clockSetupActiveIndex == 4);

  if (backSel) {
    tft.fillRoundRect(20, 196, 130, 34, 6, ST77XX_RED);
    tft.setTextColor(ST77XX_WHITE);
  } else {
    tft.drawRoundRect(20, 196, 130, 34, 6, ST77XX_RED);
    tft.setTextColor(ST77XX_RED);
  }
  tft.setCursor(62, 206);
  tft.print("Back");

  if (saveSel) {
    tft.fillRoundRect(168, 196, 130, 34, 6, ST77XX_BLUE);
    tft.setTextColor(ST77XX_WHITE);
  } else {
    tft.drawRoundRect(168, 196, 130, 34, 6, ST77XX_BLUE);
    tft.setTextColor(ST77XX_BLUE);
  }
  tft.setCursor(210, 206);
  tft.print("Save");
}

void drawNetworkDashboardScreen() {
  tft.fillScreen(ST77XX_BLACK);
  tft.drawRoundRect(6, 6, 308, 228, 8, ST77XX_YELLOW);
  
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);
  tft.setCursor(20, 18);
  tft.println("NETWORK & INTERNET");
  tft.drawFastHLine(20, 38, 280, 0xAD55); // Custom Gray line

  int numOptions = currentSettings.isWifiConfigured ? 3 : 2;
  const char* options[3];
  options[0] = "1. Scan wifi";
  options[1] = "2. sim & wifi";
  options[2] = "3. Disconnect";

  tft.setTextSize(2);
  for (int i = 0; i < numOptions; i++) {
    int y = 56 + (i * 26);
    if (i == netDashboardFocusIdx) {
      tft.fillRect(14, y - 3, 292, 23, ST77XX_BLUE);
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(20, y);
      tft.printf("> %s", options[i]);
    } else {
      tft.setTextColor(ST77XX_CYAN);
      tft.setCursor(20, y);
      tft.printf("  %s", options[i]);
    }
  }

  // Draw Back red button at the bottom (y=182 to y=216, height 34, width 280)
  int btnY = 182;
  int btnH = 34;
  if (netDashboardFocusIdx == numOptions) {
    tft.fillRoundRect(20, btnY, 280, btnH, 6, ST77XX_RED);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(136, btnY + 10);
    tft.print("BACK");
  } else {
    tft.drawRoundRect(20, btnY, 280, btnH, 6, ST77XX_RED);
    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(2);
    tft.setCursor(136, btnY + 10);
    tft.print("BACK");
  }
}

void drawSimWifiSetupScreen() {
  if (saveState == 1 && saveProgress > 0) {
    // Highly optimized flicker-free redraw: only update the bar width and current percentage
    int barW = (200 - 6) * saveProgress / 100;
    tft.fillRect(57, 93, barW, 18, ST77XX_GREEN);
    
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setCursor(110, 140);
    tft.printf("%d%% written... ", saveProgress);
    return;
  }

  tft.fillScreen(ST77XX_BLACK);
  tft.drawRoundRect(6, 6, 308, 228, 8, ST77XX_YELLOW);

  if (saveState == 1) {
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(45, 45);
    tft.println("SAVING TO MEMORY");
    tft.drawRoundRect(54, 90, 200, 24, 6, 0x5AEB);
    
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setCursor(110, 140);
    tft.println("Writing EEPROM... ");
    return;
  }

  if (saveState == 2) {
    tft.fillScreen(ST77XX_BLACK);
    tft.drawRoundRect(6, 6, 308, 228, 8, ST77XX_YELLOW);
    tft.drawCircle(160, 90, 30, ST77XX_GREEN);
    tft.drawLine(150, 90, 157, 98, ST77XX_GREEN);
    tft.drawLine(157, 98, 175, 80, ST77XX_GREEN);
    tft.setTextColor(ST77XX_GREEN);
    tft.setTextSize(2);
    tft.setCursor(125, 145);
    tft.println("SAVED!");
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.setCursor(115, 180);
    tft.println("EEPROM updated");
    return;
  }

  tft.fillScreen(ST77XX_BLACK);
  tft.drawRoundRect(6, 6, 308, 228, 8, ST77XX_YELLOW);
  
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);
  tft.setCursor(20, 15);
  tft.println("SIM & WIFI MODE SETUP");
  tft.drawFastHLine(20, 32, 280, 0xAD55); // Custom Gray line

  const char* options[] = {
    "1. Use sim only",
    "2. Use WIFI only",
    "3. use sim & WIFI",
    "4. No sim & WIFI"
  };

  tft.setTextSize(2);
  for (int i = 0; i < 4; i++) {
    int y = 38 + (i * 24);
    
    // Determine background / text color if highlighted by scroll
    bool isFocus = (simWifiSetupFocusIdx == i);
    bool isSelectedVal = (tempSimAndWifiMode == i);
    
    if (isFocus) {
      tft.fillRect(14, y - 2, 292, 21, ST77XX_BLUE);
      tft.setTextColor(ST77XX_WHITE);
    } else {
      tft.setTextColor(ST77XX_CYAN);
    }
    
    tft.setCursor(20, y);
    if (isSelectedVal) {
      tft.printf("%s [OK]", options[i]);
    } else {
      tft.printf("%s", options[i]);
    }
  }

  // Draw Back and Save buttons at the bottom
  int btnY = 182;
  int btnH = 34;
  
  // Back button (left bottom: x=20, width=130)
  if (simWifiSetupFocusIdx == 4) {
    tft.fillRoundRect(20, btnY, 130, btnH, 6, ST77XX_RED);
    tft.setTextColor(ST77XX_WHITE);
  } else {
    tft.drawRoundRect(20, btnY, 130, btnH, 6, ST77XX_RED);
    tft.setTextColor(ST77XX_RED);
  }
  tft.setTextSize(2);
  tft.setCursor(62, btnY + 10);
  tft.print("BACK");

  // Save button (right bottom: x=170, width=130)
  if (simWifiSetupFocusIdx == 5) {
    tft.fillRoundRect(170, btnY, 130, btnH, 6, ST77XX_BLUE);
    tft.setTextColor(ST77XX_WHITE);
  } else {
    tft.drawRoundRect(170, btnY, 130, btnH, 6, ST77XX_BLUE);
    tft.setTextColor(ST77XX_BLUE);
  }
  tft.setTextSize(2);
  tft.setCursor(212, btnY + 10);
  tft.print("SAVE");
}

void drawNetScanErrorScreen() {
  tft.fillScreen(ST77XX_BLACK);
  tft.drawRoundRect(6, 6, 308, 228, 8, ST77XX_YELLOW);
  
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.setCursor(30, 25);
  tft.println("WIFI SCAN BLOCKED!");
  tft.drawFastHLine(20, 48, 280, 0xAD55);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(20, 70);
  tft.println("WiFi scanning is not allowed in current");
  tft.setCursor(20, 85);
  tft.println("network configuration.");
  
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(20, 115);
  tft.println("Current mode:");
  tft.setTextColor(ST77XX_MAGENTA);
  tft.setCursor(20, 130);
  if (currentSettings.simAndWifiMode == 0) {
    tft.println("Use sim only");
  } else if (currentSettings.simAndWifiMode == 3) {
    tft.println("No sim & WIFI");
  } else {
    tft.println("Unknown mode");
  }

  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(20, 160);
  tft.println("Pls set Sim & Wifi mode to either:");
  tft.setCursor(20, 175);
  tft.println("- Use WIFI only OR - use sim & WIFI");

  // Bottom central return button
  tft.fillRoundRect(60, 196, 200, 26, 4, ST77XX_RED);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(85, 204);
  tft.print("PRESS OK TO RETURN");
}

void drawWifiConnectingScreen() {
  extern bool needConnectingScreenFullRedraw;
  if (needConnectingScreenFullRedraw) {
    tft.fillScreen(ST77XX_BLACK);
    tft.drawRoundRect(6, 6, 308, 228, 8, ST77XX_CYAN);
    
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(55, 30);
    tft.println("CONNECTING WIFI");
    
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_CYAN);
    tft.setCursor(35, 60);
    tft.printf("SSID: %s", selectedSsid);
    
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setCursor(65, 185);
    tft.print("Negotiating wireless handshakes...");
    
    needConnectingScreenFullRedraw = false;
  }
  
  // Revolving circle ui
  int centerX = 160;
  int centerY = 130;
  int radius = 22;
  static int angleIdx = 0;
  
  // Clear the spinner area only!
  tft.fillRect(centerX - 32, centerY - 32, 64, 64, ST77XX_BLACK);
  
  for (int i = 0; i < 8; i++) {
    double angle = (angleIdx + i) * 2.0 * PI / 8.0;
    int x = centerX + radius * cos(angle);
    int y = centerY + radius * sin(angle);
    uint16_t color = (i == 7) ? ST77XX_WHITE : ((i >= 5) ? ST77XX_GREEN : 0x0180);
    tft.fillCircle(x, y, 6, color);
  }
  angleIdx = (angleIdx + 1) % 8;
}

void drawWifiConnectionStatusScreen(bool success) {
  tft.fillScreen(ST77XX_BLACK);
  tft.drawRoundRect(6, 6, 308, 228, 8, success ? ST77XX_GREEN : ST77XX_RED);
  
  tft.setTextSize(2);
  if (success) {
    tft.setTextColor(ST77XX_GREEN);
    tft.setCursor(55, 45);
    tft.println("CONNECTION OK!");
    
    // Draw graphical checkmark icon
    tft.fillRect(130, 95, 60, 60, ST77XX_BLACK);
    tft.drawLine(135, 125, 155, 145, ST77XX_GREEN);
    tft.drawLine(155, 145, 185, 105, ST77XX_GREEN);
    tft.drawLine(135, 126, 155, 146, ST77XX_GREEN);
    tft.drawLine(155, 146, 185, 106, ST77XX_GREEN);
    
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(55, 180);
    tft.println("Wi-Fi credentials saved to storage.");
  } else {
    tft.setTextColor(ST77XX_RED);
    tft.setCursor(45, 45);
    tft.println("CONNECTION FAILED");
    
    // Draw graphical error X cross-out icon
    tft.drawLine(135, 105, 185, 145, ST77XX_RED);
    tft.drawLine(185, 105, 135, 145, ST77XX_RED);
    tft.drawLine(135, 106, 185, 146, ST77XX_RED);
    tft.drawLine(185, 106, 135, 146, ST77XX_RED);
    
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(50, 180);
    tft.println("SSID target offline or password mismatch.");
  }
}

void drawWifiScannerScreen() {
  static bool wasScanning = false;
  static int prevWifiNetSelection = -1;
  extern bool needNetScanFullRedraw;

  if (wifiScanCount == -1) {
    if (!wasScanning) {
      tft.fillScreen(ST77XX_BLACK);
      tft.drawRoundRect(6, 6, 308, 228, 8, ST77XX_BLUE);
      
      tft.setTextColor(ST77XX_CYAN);
      tft.setTextSize(2);
      tft.setCursor(20, 18);
      tft.println("WIFI WIRELESS SCANNER");
      tft.drawFastHLine(20, 38, 280, 0xAD55);

      tft.setTextSize(2);
      tft.setTextColor(ST77XX_GREEN);
      tft.setCursor(110, 165);
      tft.print("Scanning...");
      
      tft.setTextSize(1);
      tft.setTextColor(ST77XX_YELLOW);
      tft.setCursor(35, 195);
      tft.println("(Searching airwaves for local SSIDs)");
      wasScanning = true;
    }

    int centerX = 160;
    int centerY = 110;
    int radius = 25;
    static int angleIdx = 0;
    
    // Clear only the scanner dots area around (160, 110)
    tft.fillRect(centerX - 35, centerY - 35, 70, 70, ST77XX_BLACK);
    
    // Draw 8 circles revolving like a spinner
    for (int i = 0; i < 8; i++) {
      double angle = (angleIdx + i) * 2.0 * PI / 8.0;
      int x = centerX + radius * cos(angle);
      int y = centerY + radius * sin(angle);
      uint16_t color = (i == 7) ? ST77XX_GREEN : ((i >= 5) ? 0x05E0 : 0x0180); // Fading green trail
      tft.fillCircle(x, y, 6, color);
    }
    angleIdx = (angleIdx + 1) % 8;
    return;
  }

  // Scanning complete/idle, reset flag for next scan sessions
  wasScanning = false;

  // Let's draw the list if full redraw is requested
  if (needNetScanFullRedraw) {
    tft.fillScreen(ST77XX_BLACK);
    tft.drawRoundRect(6, 6, 308, 228, 8, ST77XX_BLUE);
    
    tft.setTextColor(ST77XX_CYAN);
    tft.setTextSize(2);
    tft.setCursor(20, 18);
    tft.println("WIFI WIRELESS SCANNER");
    tft.drawFastHLine(20, 38, 280, 0xAD55);

    if (wifiScanCount <= 0) {
      tft.setTextSize(1);
      tft.setTextColor(ST77XX_RED);
      tft.setCursor(20, 90);
      tft.println("Error: Zero Wi-Fi networks found!");
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(20, 120);
      tft.println("OK: Scan networks again");
      tft.setCursor(20, 142);
      tft.println("UP/DOWN: Close menu scanner");
      needNetScanFullRedraw = false;
      return;
    }

    // Since we did full redraw, draw all visible list items
    int startIdx = 0;
    if (wifiNetSelection < wifiScanCount) {
      if (wifiNetSelection >= 5) {
        startIdx = wifiNetSelection - 4;
      }
    } else {
      if (wifiScanCount > 5) {
        startIdx = wifiScanCount - 5;
      }
    }

    tft.setTextSize(1);
    for (int drawCount = 0; drawCount < 5 && (startIdx + drawCount) < wifiScanCount; drawCount++) {
      int i = startIdx + drawCount;
      int y = 55 + (drawCount * 26);
      if (i == wifiNetSelection) {
        tft.fillRect(14, y - 3, 292, 22, ST77XX_BLUE);
        tft.setTextColor(ST77XX_WHITE);
      } else {
        tft.setTextColor(ST77XX_CYAN);
      }
      tft.setCursor(20, y + 3);
      tft.printf("%d. %s  (%d dBm)", i + 1, wifiScanBuffer[i], wifiScanRssi[i]);
    }

    // Draw BACK button
    int btnY = 182;
    int btnH = 34;
    if (wifiNetSelection == wifiScanCount) {
      tft.fillRoundRect(20, btnY, 135, btnH, 6, ST77XX_RED);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(2);
      tft.setCursor(63, btnY + 10);
      tft.print("BACK");
    } else {
      tft.drawRoundRect(20, btnY, 135, btnH, 6, ST77XX_RED);
      tft.setTextColor(ST77XX_RED);
      tft.setTextSize(2);
      tft.setCursor(63, btnY + 10);
      tft.print("BACK");
    }

    // Draw REFRESH button
    if (wifiNetSelection == wifiScanCount + 1) {
      tft.fillRoundRect(165, btnY, 135, btnH, 6, ST77XX_GREEN);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(2);
      tft.setCursor(190, btnY + 10);
      tft.print("REFRESH");
    } else {
      tft.drawRoundRect(165, btnY, 135, btnH, 6, ST77XX_GREEN);
      tft.setTextColor(ST77XX_GREEN);
      tft.setTextSize(2);
      tft.setCursor(190, btnY + 10);
      tft.print("REFRESH");
    }

    prevWifiNetSelection = wifiNetSelection;
    needNetScanFullRedraw = false;
    return;
  }

  // Smart partial update!
  if (prevWifiNetSelection != wifiNetSelection) {
    int startIdx = 0;
    if (wifiNetSelection < wifiScanCount) {
      if (wifiNetSelection >= 5) {
        startIdx = wifiNetSelection - 4;
      }
    } else {
      if (wifiScanCount > 5) {
        startIdx = wifiScanCount - 5;
      }
    }

    int prevStartIdx = 0;
    if (prevWifiNetSelection < wifiScanCount) {
      if (prevWifiNetSelection >= 5) {
        prevStartIdx = prevWifiNetSelection - 4;
      }
    } else {
      if (wifiScanCount > 5) {
        prevStartIdx = wifiScanCount - 5;
      }
    }

    if (startIdx == prevStartIdx) {
      // Very fast update! Just redraw the changed items!
      
      // 1. Redraw previous selected row
      if (prevWifiNetSelection >= 0 && prevWifiNetSelection < wifiScanCount) {
        int drawCount = prevWifiNetSelection - startIdx;
        int y = 55 + (drawCount * 26);
        tft.fillRect(14, y - 3, 292, 22, ST77XX_BLACK);
        tft.setTextColor(ST77XX_CYAN);
        tft.setTextSize(1);
        tft.setCursor(20, y + 3);
        tft.printf("%d. %s  (%d dBm)", prevWifiNetSelection + 1, wifiScanBuffer[prevWifiNetSelection], wifiScanRssi[prevWifiNetSelection]);
      } else if (prevWifiNetSelection == wifiScanCount) {
        // Redraw BACK button as deselected
        int btnY = 182;
        int btnH = 34;
        tft.fillRect(20, btnY, 135, btnH, ST77XX_BLACK);
        tft.drawRoundRect(20, btnY, 135, btnH, 6, ST77XX_RED);
        tft.setTextColor(ST77XX_RED);
        tft.setTextSize(2);
        tft.setCursor(63, btnY + 10);
        tft.print("BACK");
      } else if (prevWifiNetSelection == wifiScanCount + 1) {
        // Redraw REFRESH button as deselected
        int btnY = 182;
        int btnH = 34;
        tft.fillRect(165, btnY, 135, btnH, ST77XX_BLACK);
        tft.drawRoundRect(165, btnY, 135, btnH, 6, ST77XX_GREEN);
        tft.setTextColor(ST77XX_GREEN);
        tft.setTextSize(2);
        tft.setCursor(190, btnY + 10);
        tft.print("REFRESH");
      }

      // 2. Redraw new selected row
      if (wifiNetSelection >= 0 && wifiNetSelection < wifiScanCount) {
        int drawCount = wifiNetSelection - startIdx;
        int y = 55 + (drawCount * 26);
        tft.fillRect(14, y - 3, 292, 22, ST77XX_BLUE);
        tft.setTextColor(ST77XX_WHITE);
        tft.setTextSize(1);
        tft.setCursor(20, y + 3);
        tft.printf("%d. %s  (%d dBm)", wifiNetSelection + 1, wifiScanBuffer[wifiNetSelection], wifiScanRssi[wifiNetSelection]);
      } else if (wifiNetSelection == wifiScanCount) {
        // Redraw BACK button as selected
        int btnY = 182;
        int btnH = 34;
        tft.fillRect(20, btnY, 135, btnH, ST77XX_BLACK);
        tft.fillRoundRect(20, btnY, 135, btnH, 6, ST77XX_RED);
        tft.setTextColor(ST77XX_WHITE);
        tft.setTextSize(2);
        tft.setCursor(63, btnY + 10);
        tft.print("BACK");
      } else if (wifiNetSelection == wifiScanCount + 1) {
        // Redraw REFRESH button as selected
        int btnY = 182;
        int btnH = 34;
        tft.fillRect(165, btnY, 135, btnH, ST77XX_BLACK);
        tft.fillRoundRect(165, btnY, 135, btnH, 6, ST77XX_GREEN);
        tft.setTextColor(ST77XX_WHITE);
        tft.setTextSize(2);
        tft.setCursor(190, btnY + 10);
        tft.print("REFRESH");
      }
    } else {
      // Scroll window has shifted! Let's clear the list area and redraw all items
      tft.fillRect(14, 52, 292, 128, ST77XX_BLACK);
      
      tft.setTextSize(1);
      for (int drawCount = 0; drawCount < 5 && (startIdx + drawCount) < wifiScanCount; drawCount++) {
        int i = startIdx + drawCount;
        int y = 55 + (drawCount * 26);
        if (i == wifiNetSelection) {
          tft.fillRect(14, y - 3, 292, 22, ST77XX_BLUE);
          tft.setTextColor(ST77XX_WHITE);
        } else {
          tft.setTextColor(ST77XX_CYAN);
        }
        tft.setCursor(20, y + 3);
        tft.printf("%d. %s  (%d dBm)", i + 1, wifiScanBuffer[i], wifiScanRssi[i]);
      }

      // Redraw BACK button state
      int btnY = 182;
      int btnH = 34;
      tft.fillRect(20, btnY, 135, btnH, ST77XX_BLACK);
      if (wifiNetSelection == wifiScanCount) {
        tft.fillRoundRect(20, btnY, 135, btnH, 6, ST77XX_RED);
        tft.setTextColor(ST77XX_WHITE);
      } else {
        tft.drawRoundRect(20, btnY, 135, btnH, 6, ST77XX_RED);
        tft.setTextColor(ST77XX_RED);
      }
      tft.setTextSize(2);
      tft.setCursor(63, btnY + 10);
      tft.print("BACK");

      // Redraw REFRESH button state
      tft.fillRect(165, btnY, 135, btnH, ST77XX_BLACK);
      if (wifiNetSelection == wifiScanCount + 1) {
        tft.fillRoundRect(165, btnY, 135, btnH, 6, ST77XX_GREEN);
        tft.setTextColor(ST77XX_WHITE);
      } else {
        tft.drawRoundRect(165, btnY, 135, btnH, 6, ST77XX_GREEN);
        tft.setTextColor(ST77XX_GREEN);
      }
      tft.setTextSize(2);
      tft.setCursor(190, btnY + 10);
      tft.print("REFRESH");
    }

    prevWifiNetSelection = wifiNetSelection;
  }
}

void getKeyboardKeyRect(int idx, int& x, int& y, int& w, int& h) {
  int startX = 20;
  int startY = 82;
  int KW = 24;
  int KH = 17;
  int gapX = 4;
  int gapY = 4;

  if (idx < 40) {
    int r = idx / 10;
    int c = idx % 10;
    x = startX + c * (KW + gapX);
    y = startY + r * (KH + gapY);
    w = KW;
    h = KH;
  } else if (idx == 44) { // BACK key span at bottom
    x = startX;
    y = 191;
    w = 280;
    h = 32;
  } else { // 40, 41, 42, 43 on Row 4
    y = startY + 4 * (KH + gapY);
    h = KH + 4; // 21
    if (idx == 40) { // SHIFT
      x = startX;
      w = 60;
    } else if (idx == 41) { // SPACE
      x = startX + 60 + gapX; // 84
      w = 70;
    } else if (idx == 42) { // BS (Backspace)
      x = startX + 60 + gapX + 70 + gapX; // 158
      w = 60;
    } else { // CONNECT (43)
      x = startX + 60 + gapX + 70 + gapX + 60 + gapX; // 222
      w = 78;
    }
  }
}

void drawKeyboardPasswordArea() {
  // Clear the inner area of the password rectangle only!
  tft.fillRect(16, 42, 288, 26, ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_GREEN);
  tft.setCursor(20, 48);
  tft.printf("%s_", typedPassword); // Cursor caret
}

void drawSingleKeyboardKey(int i, bool isSel) {
  int kx, ky, kw, kh;
  getKeyboardKeyRect(i, kx, ky, kw, kh);
  
  // Clear the key bound first to erase potential previous backgrounds
  tft.fillRect(kx, ky, kw, kh, ST77XX_BLACK);
  
  if (i == 44) { // Spanning BACK button at bottom
    if (isSel) {
      tft.fillRoundRect(kx, ky, kw, kh, 6, ST77XX_RED);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(2);
      tft.setCursor(kx + (kw - 48) / 2, ky + (kh - 16) / 2);
      tft.print("BACK");
    } else {
      tft.drawRoundRect(kx, ky, kw, kh, 6, ST77XX_RED);
      tft.setTextColor(ST77XX_RED);
      tft.setTextSize(2);
      tft.setCursor(kx + (kw - 48) / 2, ky + (kh - 16) / 2);
      tft.print("BACK");
    }
    return;
  }

  if (isSel) {
    tft.fillRoundRect(kx, ky, kw, kh, 3, ST77XX_BLUE);
    tft.drawRoundRect(kx, ky, kw, kh, 3, ST77XX_WHITE);
    tft.setTextColor(ST77XX_WHITE);
  } else {
    tft.drawRoundRect(kx, ky, kw, kh, 3, 0x187F); // Dark blue/cyan border
    tft.setTextColor(ST77XX_CYAN);
  }
  
  tft.setTextSize(1);
  if (i < 40) {
    tft.setCursor(kx + (kw - 6) / 2, ky + (kh - 8) / 2);
    char val = isShiftActive ? kbCharactersUpper[i] : kbCharactersLower[i];
    tft.print(val);
  } else if (i == 40) { // SHIFT
    tft.setCursor(kx + (kw - 30) / 2, ky + (kh - 8) / 2);
    if (isShiftActive) {
      if (isSel) {
        tft.fillRoundRect(kx, ky, kw, kh, 3, ST77XX_YELLOW);
        tft.setTextColor(ST77XX_BLACK);
      } else {
        tft.fillRoundRect(kx, ky, kw, kh, 3, 0x52AA); // dim yellow/green grey
        tft.setTextColor(ST77XX_YELLOW);
      }
    }
    tft.print("SHIFT");
  } else if (i == 41) { // SPACE
    tft.setCursor(kx + (kw - 30) / 2, ky + (kh - 8) / 2);
    tft.print("SPACE");
  } else if (i == 42) { // BS
    tft.setCursor(kx + (kw - 12) / 2, ky + (kh - 8) / 2);
    tft.print("BS");
  } else if (i == 43) { // CONNECT
    tft.setCursor(kx + (kw - 42) / 2, ky + (kh - 8) / 2);
    tft.print("CONNECT");
  }
}

void drawKeyboardScreen() {
  static int prevKbSelectedIdx = -1;
  static char prevTypedPassword[64] = "";
  
  extern bool needKeyboardFullRedraw;
  if (needKeyboardFullRedraw) {
    tft.fillScreen(ST77XX_BLACK);
    tft.drawRoundRect(6, 6, 308, 228, 8, ST77XX_CYAN);
    
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(1);
    tft.setCursor(18, 14);
    tft.print("TYPE WIFI SECURITY PASSWORD (QWERTY)");
    
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(18, 28);
    tft.printf("TARGET SSID: %s", selectedSsid);
    
    // Draw initial typed password box frame
    tft.drawRoundRect(14, 40, 292, 30, 4, ST77XX_GREEN);
    
    // Draw all 45 keys fully once
    for (int i = 0; i < 45; i++) {
      drawSingleKeyboardKey(i, (i == kbCharIndex));
    }
    
    // Draw current password content
    drawKeyboardPasswordArea();
    
    prevKbSelectedIdx = kbCharIndex;
    strcpy(prevTypedPassword, typedPassword);
    needKeyboardFullRedraw = false;
    return;
  }
  
  // If not full redraw, perform smooth partial update!
  // 1. If password string has changed, update password entry field
  if (strcmp(prevTypedPassword, typedPassword) != 0) {
    drawKeyboardPasswordArea();
    strcpy(prevTypedPassword, typedPassword);
  }
  
  // 2. If selection index has changed, redraw previous selected and newly selected key only
  if (prevKbSelectedIdx != kbCharIndex) {
    // Redraw old key as deselected
    if (prevKbSelectedIdx >= 0 && prevKbSelectedIdx < 45) {
      drawSingleKeyboardKey(prevKbSelectedIdx, false);
    }
    // Redraw new key as selected
    drawSingleKeyboardKey(kbCharIndex, true);
    prevKbSelectedIdx = kbCharIndex;
  }
}

void initButtonPins() {
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
}

void handleButtonTicks() {
  static bool prevOkState = HIGH;
  static bool prevUpState = HIGH;
  static bool prevDnState = HIGH;
  static unsigned long lastOkTime = 0;
  static unsigned long lastUpTime = 0;
  static unsigned long lastDnTime = 0;
  const unsigned long DEBOUNCE_DELAY = 60; // Perfect for physical keys state changes
  
  bool currentOk = digitalRead(BTN_OK);
  bool currentUp = digitalRead(BTN_UP);
  bool currentDown = digitalRead(BTN_DOWN);
  
  bool okClicked = false;
  bool upClicked = false;
  bool downClicked = false;
  
  unsigned long now = millis();
  
  if (currentOk != prevOkState) {
    if (now - lastOkTime > DEBOUNCE_DELAY) {
      if (currentOk == LOW) {
        okClicked = true;
      }
      prevOkState = currentOk;
      lastOkTime = now;
    }
  }
  
  if (currentUp != prevUpState) {
    if (now - lastUpTime > DEBOUNCE_DELAY) {
      if (currentUp == LOW) {
        upClicked = true;
      }
      prevUpState = currentUp;
      lastUpTime = now;
    }
  }
  
  if (currentDown != prevDnState) {
    if (now - lastDnTime > DEBOUNCE_DELAY) {
      if (currentDown == LOW) {
        downClicked = true;
      }
      prevDnState = currentDown;
      lastDnTime = now;
    }
  }

  if (!okClicked && !upClicked && !downClicked) return;

  // Link to App screen controls (Unlinking and simulated linking)
  if (activeTftScreen == SCREEN_LINK_QR) {
    if (okClicked) {
      if (isDeviceLinked) {
        isDeviceLinked = false;
        preferences.begin("incubator", false);
        preferences.putBool("dev_linked", false);
        preferences.end();
        drawQrLinkingScreen();
      } else {
        isDeviceLinked = true;
        preferences.begin("incubator", false);
        preferences.putBool("dev_linked", true);
        preferences.end();
        drawQrLinkingScreen();
      }
    } else if (upClicked || downClicked) {
      activeTftScreen = SCREEN_APP_UPDATES_MENU;
      appUpdatesSubFocusIdx = 0;
      drawAppUpdatesMenuScreen();
    }
    return;
  }

  // App & Updates Sub-menu Selection List Controls
  if (activeTftScreen == SCREEN_APP_UPDATES_MENU) {
    if (upClicked) {
      appUpdatesSubFocusIdx = (appUpdatesSubFocusIdx - 1 + 3) % 3;
      drawAppUpdatesMenuScreen();
    } else if (downClicked) {
      appUpdatesSubFocusIdx = (appUpdatesSubFocusIdx + 1) % 3;
      drawAppUpdatesMenuScreen();
    } else if (okClicked) {
      if (appUpdatesSubFocusIdx == 0) {
        activeTftScreen = SCREEN_LINK_QR;
        drawQrLinkingScreen();
      } else if (appUpdatesSubFocusIdx == 1) {
        activeTftScreen = SCREEN_SOFTWARE_OTA;
        drawSoftwareOtaScreen();
      } else if (appUpdatesSubFocusIdx == 2) {
        activeTftScreen = SCREEN_MAIN_MENU;
        drawMainOptionMenu();
      }
    }
    return;
  }

  // Software & OTA update check simulator screen controls
  if (activeTftScreen == SCREEN_SOFTWARE_OTA) {
    if (okClicked) {
      // Simulate looking for OTA file over internet
      tft.fillRect(25 + 2, 115 + 2, 270 - 4, 38 - 4, ST77XX_BLACK);
      tft.setTextColor(ST77XX_YELLOW);
      tft.setTextSize(1);
      tft.setCursor(35, 128);
      tft.print("Checking server for updates...");
      delay(1200);
      drawSoftwareOtaScreen();
    } else if (upClicked || downClicked) {
      activeTftScreen = SCREEN_APP_UPDATES_MENU;
      appUpdatesSubFocusIdx = 1;
      drawAppUpdatesMenuScreen();
    }
    return;
  }

  // Dashboard OK click navigates to main option menu
  if (activeTftScreen == SCREEN_DASHBOARD) {
    if (okClicked) {
      activeTftScreen = SCREEN_MAIN_MENU;
      mainMenuIndex = 0;
      drawMainOptionMenu();
    } else {
      // Direct dashboard button clicks are ignored. Manual momentary control handles the motors when turnEnabled is false.
    }
    return;
  }

  // Main parameter menu action list routing
  if (activeTftScreen == SCREEN_MAIN_MENU) {
    if (upClicked) {
      mainMenuIndex = (mainMenuIndex - 1 + 7) % 7;
      drawMainOptionMenu();
    } else if (downClicked) {
      mainMenuIndex = (mainMenuIndex + 1) % 7;
      drawMainOptionMenu();
    } else if (okClicked) {
      if (mainMenuIndex == 0) {
        activeTftScreen = SCREEN_MENU_TEMP;
        tempSubFocusIdx = 0;
        isEditingValue = false;
        saveState = 0;
        tempSettings = currentSettings;
        drawManualSettingsMenu("Set temperature", tempSettings.targetTemp, "C", 1);
      } else if (mainMenuIndex == 1) {
        activeTftScreen = SCREEN_MENU_HUMID;
        humidSubFocusIdx = 0;
        isEditingValue = false;
        saveState = 0;
        tempSettings = currentSettings;
        drawManualSettingsMenu("Set Humidity", tempSettings.targetHumidity, "%", 2);
      } else if (mainMenuIndex == 2) {
        activeTftScreen = SCREEN_MENU_TURN;
        turnSubFocusIdx = 0;
        isEditingValue = false;
        saveState = 0;
        tempSettings = currentSettings;
        drawManualSettingsMenu("Turning & Backup", 0.0f, "", 3);
      } else if (mainMenuIndex == 3) {
        activeTftScreen = SCREEN_MENU_TIME;
        clockSetupActiveIndex = 0;
        isEditingValue = false;
        saveState = 0;
        tempSettings = currentSettings;
        
        // Grab current RTC values to prepopulate settings before adjusting
        RtcDateTime nowTime = RtcModule.GetDateTime();
        tempClockHour = nowTime.Hour();
        tempClockMinute = nowTime.Minute();
        tempClockDay = nowTime.Day();
        tempClockMonth = nowTime.Month();
        tempClockYear = nowTime.Year();
        
        drawTimeAdjustmentMenu();
      } else if (mainMenuIndex == 4) {
        activeTftScreen = SCREEN_NET_DASHBOARD;
        netDashboardFocusIdx = 0;
        drawNetworkDashboardScreen();
      } else if (mainMenuIndex == 5) {
        activeTftScreen = SCREEN_APP_UPDATES_MENU;
        appUpdatesSubFocusIdx = 0;
        drawAppUpdatesMenuScreen();
      } else if (mainMenuIndex == 6) {
        activeTftScreen = SCREEN_DASHBOARD;
        forceDisplayRedraw = true;
      }
    }
    return;
  }

  // Adjust parameters: Temperature Setup
  if (activeTftScreen == SCREEN_MENU_TEMP) {
    if (saveState != 0) return;
    
    if (okClicked) {
      if (tempSubFocusIdx == 0 || tempSubFocusIdx == 1) {
        isEditingValue = !isEditingValue;
        drawManualSettingsMenu("Set temperature", 0.0f, "C", 1);
      } else if (tempSubFocusIdx == 2) {
        activeTftScreen = SCREEN_MAIN_MENU;
        drawMainOptionMenu();
      } else if (tempSubFocusIdx == 3) {
        saveState = 1; // SAVING
        saveProgress = 0;
        drawManualSettingsMenu("Set temperature", 0.0f, "C", 1);
        
        for (int p = 0; p <= 100; p += 10) {
          saveProgress = p;
          drawManualSettingsMenu("Set temperature", 0.0f, "C", 1);
          delay(120);
        }
        
        tempSettings.targetTemp = tempSettings.maxTemp;
        currentSettings = tempSettings; // Copy saved settings to running system
        
        saveSettingsToPreferences(currentSettings);
        saveState = 2; // SAVED
        drawManualSettingsMenu("Set temperature", 0.0f, "C", 1);
        delay(1200);
        
        saveState = 0; // IDLE
        isEditingValue = false;
        activeTftScreen = SCREEN_MAIN_MENU;
        drawMainOptionMenu();
      }
    } else if (upClicked) {
      if (isEditingValue) {
        if (tempSubFocusIdx == 0) {
          float next = tempSettings.minTemp + 0.1f;
          if (next < tempSettings.maxTemp) {
            tempSettings.minTemp = next;
          }
        } else if (tempSubFocusIdx == 1) {
          float next = tempSettings.maxTemp + 0.1f;
          if (next <= 45.0f) {
            tempSettings.maxTemp = next;
          }
        }
      } else {
        tempSubFocusIdx = (tempSubFocusIdx - 1 + 4) % 4;
      }
      drawManualSettingsMenu("Set temperature", 0.0f, "C", 1);
    } else if (downClicked) {
      if (isEditingValue) {
        if (tempSubFocusIdx == 0) {
          float next = tempSettings.minTemp - 0.1f;
          if (next >= 30.0f) {
            tempSettings.minTemp = next;
          }
        } else if (tempSubFocusIdx == 1) {
          float next = tempSettings.maxTemp - 0.1f;
          if (next > tempSettings.minTemp) {
            tempSettings.maxTemp = next;
          }
        }
      } else {
        tempSubFocusIdx = (tempSubFocusIdx + 1) % 4;
      }
      drawManualSettingsMenu("Set temperature", 0.0f, "C", 1);
    }
    return;
  }

  // Adjust parameters: Humidity Setup
  if (activeTftScreen == SCREEN_MENU_HUMID) {
    if (saveState != 0) return;
    
    if (okClicked) {
      if (humidSubFocusIdx == 0 || humidSubFocusIdx == 1) {
        isEditingValue = !isEditingValue;
        drawManualSettingsMenu("Set Humidity", 0.0f, "%", 2);
      } else if (humidSubFocusIdx == 2) {
        activeTftScreen = SCREEN_MAIN_MENU;
        drawMainOptionMenu();
      } else if (humidSubFocusIdx == 3) {
        saveState = 1; // SAVING
        saveProgress = 0;
        drawManualSettingsMenu("Set Humidity", 0.0f, "%", 2);
        
        for (int p = 0; p <= 100; p += 10) {
          saveProgress = p;
          drawManualSettingsMenu("Set Humidity", 0.0f, "%", 2);
          delay(120);
        }
        
        tempSettings.targetHumidity = tempSettings.maxHumidity;
        currentSettings = tempSettings; // Copy saved settings to running system
        
        saveSettingsToPreferences(currentSettings);
        saveState = 2; // SAVED
        drawManualSettingsMenu("Set Humidity", 0.0f, "%", 2);
        delay(1200);
        
        saveState = 0; // IDLE
        isEditingValue = false;
        activeTftScreen = SCREEN_MAIN_MENU;
        drawMainOptionMenu();
      }
    } else if (upClicked) {
      if (isEditingValue) {
        if (humidSubFocusIdx == 0) {
          float next = tempSettings.minHumidity + 1.0f;
          if (next < tempSettings.maxHumidity) {
            tempSettings.minHumidity = next;
          }
        } else if (humidSubFocusIdx == 1) {
          float next = tempSettings.maxHumidity + 1.0f;
          if (next <= 95.0f) {
            tempSettings.maxHumidity = next;
          }
        }
      } else {
        humidSubFocusIdx = (humidSubFocusIdx - 1 + 4) % 4;
      }
      drawManualSettingsMenu("Set Humidity", 0.0f, "%", 2);
    } else if (downClicked) {
      if (isEditingValue) {
        if (humidSubFocusIdx == 0) {
          float next = tempSettings.minHumidity - 1.0f;
          if (next >= 20.0f) {
            tempSettings.minHumidity = next;
          }
        } else if (humidSubFocusIdx == 1) {
          float next = tempSettings.maxHumidity - 1.0f;
          if (next > tempSettings.minHumidity) {
            tempSettings.maxHumidity = next;
          }
        }
      } else {
        humidSubFocusIdx = (humidSubFocusIdx + 1) % 4;
      }
      drawManualSettingsMenu("Set Humidity", 0.0f, "%", 2);
    }
    return;
  }

  // Adjust parameters: Turning & Backup Master Option Menu
  if (activeTftScreen == SCREEN_MENU_TURN) {
    if (okClicked) {
      if (turnSubFocusIdx == 0) {
        // Go to Turning Setup Submenu
        activeTftScreen = SCREEN_MENU_TURNING_SETUP;
        turnSetupSubFocusIdx = 0;
        isEditingValue = false;
        tempSettings = currentSettings;
        drawManualSettingsMenu("Turning Setup", 0.0f, "", 8);
      } else if (turnSubFocusIdx == 1) {
        // Go to Backup Setup Submenu
        activeTftScreen = SCREEN_MENU_BACKUP_SETUP;
        backupSetupSubFocusIdx = 0;
        isEditingValue = false;
        tempSettings = currentSettings;
        // Auto-skip N/A option on entry if needed
        if (tempSettings.backupType == 1 && backupSetupSubFocusIdx == 1) {
          backupSetupSubFocusIdx = 2;
        }
        drawManualSettingsMenu("Backup Setup", 0.0f, "", 9);
      } else if (turnSubFocusIdx == 2) {
        // Return to Main Menu
        activeTftScreen = SCREEN_MAIN_MENU;
        drawMainOptionMenu();
      }
    } else if (upClicked) {
      turnSubFocusIdx = (turnSubFocusIdx - 1 + 3) % 3;
      drawManualSettingsMenu("Turning & Backup", 0.0f, "", 3);
    } else if (downClicked) {
      turnSubFocusIdx = (turnSubFocusIdx + 1) % 3;
      drawManualSettingsMenu("Turning & Backup", 0.0f, "", 3);
    }
    return;
  }

  // Adjust parameters: Turning Setup (Submenu 8)
  if (activeTftScreen == SCREEN_MENU_TURNING_SETUP) {
    if (saveState != 0 || showRecountConfirmation) return;

    if (okClicked) {
      if (turnSetupSubFocusIdx == 7) {
        // Back -> Return to master menu without saving
        activeTftScreen = SCREEN_MENU_TURN;
        drawManualSettingsMenu("Turning & Backup", 0.0f, "", 3);
      } else if (turnSetupSubFocusIdx == 8) {
        // Save -> Trigger Saving sequence
        saveState = 1;
        saveProgress = 0;
        drawManualSettingsMenu("Turning Setup", 0.0f, "", 8);
        
        for (int p = 0; p <= 100; p += 10) {
          saveProgress = p;
          drawManualSettingsMenu("Turning Setup", 0.0f, "", 8);
          delay(120);
        }
        
        currentSettings = tempSettings;
        saveSettingsToPreferences(currentSettings);
        saveState = 2; // SAVED
        drawManualSettingsMenu("Turning Setup", 0.0f, "", 8);
        delay(1200);
        
        saveState = 0; // IDLE
        isEditingValue = false;
        activeTftScreen = SCREEN_MENU_TURN;
        drawManualSettingsMenu("Turning & Backup", 0.0f, "", 3);
      } else if (turnSetupSubFocusIdx == 6) {
        // "Begin count now": Only sets tempSettings variables but DOES NOT save immediately.
        RtcDateTime nowTime = RtcModule.GetDateTime();
        tempSettings.incubationDayStarted = nowTime.Day();
        tempSettings.incubationMonthStarted = nowTime.Month();
        tempSettings.turnEnabled = true; // Auto-turn ON when reset begins
        drawManualSettingsMenu("Turning Setup", 0.0f, "", 8);
      } else {
        isEditingValue = !isEditingValue;
        drawManualSettingsMenu("Turning Setup", 0.0f, "", 8);
      }
    } else if (upClicked) {
      if (isEditingValue) {
        if (turnSetupSubFocusIdx == 0) {
          int next = tempSettings.turnIntervalHours + 30;
          if (next <= 300) tempSettings.turnIntervalHours = next;
        } else if (turnSetupSubFocusIdx == 1) {
          tempSettings.turnEnabled = !tempSettings.turnEnabled;
        } else if (turnSetupSubFocusIdx == 2) {
          int next = tempSettings.totalIncubationDays + 1;
          if (next <= 50) tempSettings.totalIncubationDays = next;
        } else if (turnSetupSubFocusIdx == 3) {
          int next = tempSettings.stopTurningDay + 1;
          if (next <= tempSettings.totalIncubationDays) tempSettings.stopTurningDay = next;
        } else if (turnSetupSubFocusIdx == 4) { // Start Month
          tempSettings.incubationMonthStarted = (tempSettings.incubationMonthStarted % 12) + 1;
        } else if (turnSetupSubFocusIdx == 5) { // Start Day
          int mDays = daysInMonth(tempSettings.incubationMonthStarted, 2026);
          tempSettings.incubationDayStarted = (tempSettings.incubationDayStarted % mDays) + 1;
        }
      } else {
        turnSetupSubFocusIdx--;
        if (turnSetupSubFocusIdx < 0) turnSetupSubFocusIdx = 8;
        
        // Skip non-applicable options if Turn Mode is MANUAL
        int attempts = 0;
        while (attempts < 9) {
          if (!tempSettings.turnEnabled && (turnSetupSubFocusIdx == 0 || turnSetupSubFocusIdx == 2 || turnSetupSubFocusIdx == 3 || turnSetupSubFocusIdx == 4 || turnSetupSubFocusIdx == 5 || turnSetupSubFocusIdx == 6)) {
            turnSetupSubFocusIdx = (turnSetupSubFocusIdx - 1 + 9) % 9;
          } else {
            break;
          }
          attempts++;
        }
      }
      drawManualSettingsMenu("Turning Setup", 0.0f, "", 8);
    } else if (downClicked) {
      if (isEditingValue) {
        if (turnSetupSubFocusIdx == 0) {
          int next = tempSettings.turnIntervalHours - 30;
          if (next >= 30) tempSettings.turnIntervalHours = next;
        } else if (turnSetupSubFocusIdx == 1) {
          tempSettings.turnEnabled = !tempSettings.turnEnabled;
        } else if (turnSetupSubFocusIdx == 2) {
          int next = tempSettings.totalIncubationDays - 1;
          if (next >= 1) {
            tempSettings.totalIncubationDays = next;
            // Cap stop day at total days
            if (tempSettings.stopTurningDay > next) tempSettings.stopTurningDay = next;
          }
        } else if (turnSetupSubFocusIdx == 3) {
          int next = tempSettings.stopTurningDay - 1;
          if (next >= 0) tempSettings.stopTurningDay = next; // 0 represents "None"
        } else if (turnSetupSubFocusIdx == 4) { // Start Month
          tempSettings.incubationMonthStarted = tempSettings.incubationMonthStarted - 1;
          if (tempSettings.incubationMonthStarted < 1) tempSettings.incubationMonthStarted = 12;
        } else if (turnSetupSubFocusIdx == 5) { // Start Day
          int mDays = daysInMonth(tempSettings.incubationMonthStarted, 2026);
          tempSettings.incubationDayStarted = tempSettings.incubationDayStarted - 1;
          if (tempSettings.incubationDayStarted < 1) tempSettings.incubationDayStarted = mDays;
        }
      } else {
        turnSetupSubFocusIdx = (turnSetupSubFocusIdx + 1) % 9;
        
        // Skip non-applicable options if Turn Mode is MANUAL
        int attempts = 0;
        while (attempts < 9) {
          if (!tempSettings.turnEnabled && (turnSetupSubFocusIdx == 0 || turnSetupSubFocusIdx == 2 || turnSetupSubFocusIdx == 3 || turnSetupSubFocusIdx == 4 || turnSetupSubFocusIdx == 5 || turnSetupSubFocusIdx == 6)) {
            turnSetupSubFocusIdx = (turnSetupSubFocusIdx + 1) % 9;
          } else {
            break;
          }
          attempts++;
        }
      }
      drawManualSettingsMenu("Turning Setup", 0.0f, "", 8);
    }
    return;
  }

  // Adjust parameters: Backup Setup (Submenu 9)
  if (activeTftScreen == SCREEN_MENU_BACKUP_SETUP) {
    if (saveState != 0) return;

    if (okClicked) {
      if (backupSetupSubFocusIdx == 8) {
        // Back
        activeTftScreen = SCREEN_MENU_TURN;
        drawManualSettingsMenu("Turning & Backup", 0.0f, "", 3);
      } else if (backupSetupSubFocusIdx == 9) {
        // Save
        saveState = 1;
        saveProgress = 0;
        drawManualSettingsMenu("Backup Setup", 0.0f, "", 9);
        
        for (int p = 0; p <= 100; p += 10) {
          saveProgress = p;
          drawManualSettingsMenu("Backup Setup", 0.0f, "", 9);
          delay(120);
        }
        
        currentSettings = tempSettings;
        saveSettingsToPreferences(currentSettings);
        saveState = 2; // SAVED
        drawManualSettingsMenu("Backup Setup", 0.0f, "", 9);
        delay(1200);
        
        saveState = 0; // IDLE
        isEditingValue = false;
        activeTftScreen = SCREEN_MENU_TURN;
        drawManualSettingsMenu("Turning & Backup", 0.0f, "", 3);
      } else {
        isEditingValue = !isEditingValue;
        drawManualSettingsMenu("Backup Setup", 0.0f, "", 9);
      }
    } else if (upClicked) {
      if (isEditingValue) {
        if (backupSetupSubFocusIdx == 0) {
          tempSettings.backupType = (tempSettings.backupType + 1) % 3;
        } else if (backupSetupSubFocusIdx == 1) {
          float next = tempSettings.backupPressDurationSec + 0.5f;
          if (next <= 12.0f) tempSettings.backupPressDurationSec = next;
        } else if (backupSetupSubFocusIdx == 2) {
          tempSettings.chokeServoEnabled = !tempSettings.chokeServoEnabled;
        } else if (backupSetupSubFocusIdx == 3) {
          int next = tempSettings.maxGenStartRetries + 1;
          if (next <= 10) tempSettings.maxGenStartRetries = next;
        } else if (backupSetupSubFocusIdx == 4) {
          int next = tempSettings.chokeServoDegrees + 5;
          if (next <= 180) tempSettings.chokeServoDegrees = next;
        } else if (backupSetupSubFocusIdx == 5) {
          tempSettings.backupSchedEnabled = !tempSettings.backupSchedEnabled;
        } else if (backupSetupSubFocusIdx == 6) {
          tempSettings.backupSchedStartHour = (tempSettings.backupSchedStartHour + 1) % 24;
        } else if (backupSetupSubFocusIdx == 7) {
          tempSettings.backupSchedEndHour = (tempSettings.backupSchedEndHour + 1) % 24;
        }
      } else {
        backupSetupSubFocusIdx--;
        if (backupSetupSubFocusIdx < 0) backupSetupSubFocusIdx = 9;

        // Skip non-applicable options based on backup source and state
        int attempts = 0;
        while (attempts < 10) {
          if (backupSetupSubFocusIdx == 1 && tempSettings.backupType == 1) { // Inverter: No Duration
            backupSetupSubFocusIdx = (backupSetupSubFocusIdx - 1 + 10) % 10;
          } else if (backupSetupSubFocusIdx == 2 && tempSettings.backupType != 2) { // Non-generator: No Choke
            backupSetupSubFocusIdx = (backupSetupSubFocusIdx - 1 + 10) % 10;
          } else if (backupSetupSubFocusIdx == 3 && tempSettings.backupType != 2) { // Non-generator: No Retries
            backupSetupSubFocusIdx = (backupSetupSubFocusIdx - 1 + 10) % 10;
          } else if (backupSetupSubFocusIdx == 4 && (tempSettings.backupType != 2 || !tempSettings.chokeServoEnabled)) { // Choke angle: Gen + Choke enabled only
            backupSetupSubFocusIdx = (backupSetupSubFocusIdx - 1 + 10) % 10;
          } else if ((backupSetupSubFocusIdx == 6 || backupSetupSubFocusIdx == 7) && !tempSettings.backupSchedEnabled) { // Hours: Sched enabled only
            backupSetupSubFocusIdx = (backupSetupSubFocusIdx - 1 + 10) % 10;
          } else {
            break;
          }
          attempts++;
        }
      }
      drawManualSettingsMenu("Backup Setup", 0.0f, "", 9);
    } else if (downClicked) {
      if (isEditingValue) {
        if (backupSetupSubFocusIdx == 0) {
          tempSettings.backupType = (tempSettings.backupType - 1 + 3) % 3;
        } else if (backupSetupSubFocusIdx == 1) {
          float next = tempSettings.backupPressDurationSec - 0.5f;
          if (next >= 0.5f) tempSettings.backupPressDurationSec = next;
        } else if (backupSetupSubFocusIdx == 2) {
          tempSettings.chokeServoEnabled = !tempSettings.chokeServoEnabled;
        } else if (backupSetupSubFocusIdx == 3) {
          int next = tempSettings.maxGenStartRetries - 1;
          if (next >= 1) tempSettings.maxGenStartRetries = next;
        } else if (backupSetupSubFocusIdx == 4) {
          int next = tempSettings.chokeServoDegrees - 5;
          if (next >= 0) tempSettings.chokeServoDegrees = next;
        } else if (backupSetupSubFocusIdx == 5) {
          tempSettings.backupSchedEnabled = !tempSettings.backupSchedEnabled;
        } else if (backupSetupSubFocusIdx == 6) {
          tempSettings.backupSchedStartHour = (tempSettings.backupSchedStartHour - 1 + 24) % 24;
        } else if (backupSetupSubFocusIdx == 7) {
          tempSettings.backupSchedEndHour = (tempSettings.backupSchedEndHour - 1 + 24) % 24;
        }
      } else {
        backupSetupSubFocusIdx = (backupSetupSubFocusIdx + 1) % 10;

        // Skip non-applicable options based on backup source and state
        int attempts = 0;
        while (attempts < 10) {
          if (backupSetupSubFocusIdx == 1 && tempSettings.backupType == 1) { // Inverter: No Duration
            backupSetupSubFocusIdx = (backupSetupSubFocusIdx + 1) % 10;
          } else if (backupSetupSubFocusIdx == 2 && tempSettings.backupType != 2) { // Non-generator: No Choke
            backupSetupSubFocusIdx = (backupSetupSubFocusIdx + 1) % 10;
          } else if (backupSetupSubFocusIdx == 3 && tempSettings.backupType != 2) { // Non-generator: No Retries
            backupSetupSubFocusIdx = (backupSetupSubFocusIdx + 1) % 10;
          } else if (backupSetupSubFocusIdx == 4 && (tempSettings.backupType != 2 || !tempSettings.chokeServoEnabled)) { // Choke angle: Gen + Choke enabled only
            backupSetupSubFocusIdx = (backupSetupSubFocusIdx + 1) % 10;
          } else if ((backupSetupSubFocusIdx == 6 || backupSetupSubFocusIdx == 7) && !tempSettings.backupSchedEnabled) { // Hours: Sched enabled only
            backupSetupSubFocusIdx = (backupSetupSubFocusIdx + 1) % 10;
          } else {
            break;
          }
          attempts++;
        }
      }
      drawManualSettingsMenu("Backup Setup", 0.0f, "", 9);
    }
    return;
  }

  // RTC Clock Calendar adjustment
  if (activeTftScreen == SCREEN_MENU_TIME) {
    if (saveState != 0) return;

    if (okClicked) {
      if (clockSetupActiveIndex == 0) {
        // Toggle editing value on Clock Format
        isEditingValue = !isEditingValue;
        drawTimeAdjustmentMenu();
      } else if (clockSetupActiveIndex == 1) {
        // Adjust Time (Hour & Minute)
        if (!isEditingValue) {
          isEditingValue = true;
          clockTimeEditState = 0; // Starts with Hour
        } else {
          if (clockTimeEditState == 0) {
            clockTimeEditState = 1; // Move to Minute
          } else {
            isEditingValue = false; // Done adjusting
          }
        }
        drawTimeAdjustmentMenu();
      } else if (clockSetupActiveIndex == 2) {
        // Adjust Date (Day, Month, Year)
        if (!isEditingValue) {
          isEditingValue = true;
          clockDateEditState = 0; // Starts with Day
        } else {
          if (clockDateEditState == 0) {
            clockDateEditState = 1; // Move to Month
          } else if (clockDateEditState == 1) {
            clockDateEditState = 2; // Move to Year
          } else {
            isEditingValue = false; // Done adjusting
          }
        }
        drawTimeAdjustmentMenu();
      } else if (clockSetupActiveIndex == 3) {
        // Back -> Return to main option menu without saving
        activeTftScreen = SCREEN_MAIN_MENU;
        drawMainOptionMenu();
      } else if (clockSetupActiveIndex == 4) {
        // Save -> Set RTC and persist use24HourFormat
        saveState = 1;
        saveProgress = 0;
        drawTimeAdjustmentMenu();
        
        for (int p = 0; p <= 100; p += 10) {
          saveProgress = p;
          drawTimeAdjustmentMenu();
          delay(120);
        }
        
        // Assemble date-time and update RTC hardware chip
        RtcDateTime updatedDT(tempClockYear, tempClockMonth, tempClockDay, tempClockHour, tempClockMinute, 0);
        RtcModule.SetDateTime(updatedDT);
        
        currentSettings = tempSettings;
        saveSettingsToPreferences(currentSettings);
        
        saveState = 2; // SAVED
        drawTimeAdjustmentMenu();
        delay(1200);
        
        saveState = 0; // IDLE
        isEditingValue = false;
        activeTftScreen = SCREEN_MAIN_MENU;
        drawMainOptionMenu();
      }
    } else if (upClicked) {
      if (isEditingValue) {
        if (clockSetupActiveIndex == 0) {
          tempSettings.use24HourFormat = !tempSettings.use24HourFormat;
        } else if (clockSetupActiveIndex == 1) {
          if (clockTimeEditState == 0) {
            tempClockHour = (tempClockHour + 1) % 24;
          } else {
            tempClockMinute = (tempClockMinute + 1) % 60;
          }
        } else if (clockSetupActiveIndex == 2) {
          if (clockDateEditState == 0) {
            int mDays = daysInMonth(tempClockMonth, tempClockYear);
            tempClockDay = (tempClockDay % mDays) + 1;
          } else if (clockDateEditState == 1) {
            tempClockMonth = (tempClockMonth % 12) + 1;
          } else {
            tempClockYear++;
          }
        }
      } else {
        clockSetupActiveIndex = (clockSetupActiveIndex - 1 + 5) % 5;
      }
      drawTimeAdjustmentMenu();
    } else if (downClicked) {
      if (isEditingValue) {
        if (clockSetupActiveIndex == 0) {
          tempSettings.use24HourFormat = !tempSettings.use24HourFormat;
        } else if (clockSetupActiveIndex == 1) {
          if (clockTimeEditState == 0) {
            tempClockHour = (tempClockHour - 1 + 24) % 24;
          } else {
            tempClockMinute = (tempClockMinute - 1 + 60) % 60;
          }
        } else if (clockSetupActiveIndex == 2) {
          if (clockDateEditState == 0) {
            int mDays = daysInMonth(tempClockMonth, tempClockYear);
            tempClockDay = tempClockDay - 1;
            if (tempClockDay < 1) tempClockDay = mDays;
          } else if (clockDateEditState == 1) {
            tempClockMonth = tempClockMonth - 1;
            if (tempClockMonth < 1) tempClockMonth = 12;
          } else {
            tempClockYear--;
            if (tempClockYear < 2000) tempClockYear = 2000;
          }
        }
      } else {
        clockSetupActiveIndex = (clockSetupActiveIndex + 1) % 5;
      }
      drawTimeAdjustmentMenu();
    }
    return;
  }

  // Network Dashboard screen input routing
  if (activeTftScreen == SCREEN_NET_DASHBOARD) {
    int numOptions = currentSettings.isWifiConfigured ? 3 : 2;
    int maxSelections = numOptions + 1; // plus Back button
    
    if (upClicked) {
      netDashboardFocusIdx = (netDashboardFocusIdx - 1 + maxSelections) % maxSelections;
      drawNetworkDashboardScreen();
    } else if (downClicked) {
      netDashboardFocusIdx = (netDashboardFocusIdx + 1) % maxSelections;
      drawNetworkDashboardScreen();
    } else if (okClicked) {
      if (netDashboardFocusIdx == 0) {
        // Option "Scan wifi" selected. Verify configurations
        if (currentSettings.simAndWifiMode == 0 || currentSettings.simAndWifiMode == 3) {
          activeTftScreen = SCREEN_MENU_NET_SCAN_ERROR;
          drawNetScanErrorScreen();
        } else {
          activeTftScreen = SCREEN_MENU_NET_SCAN;
          wifiScanCount = -1;
          wifiNetSelection = 0;
          extern bool needNetScanFullRedraw;
          needNetScanFullRedraw = true;
          drawWifiScannerScreen();
          
          isScanningWifi = true; // Force background reconnects to pause
          WiFi.mode(WIFI_STA);
          WiFi.disconnect();
          delay(100);
          
          // Trigger non-blocking asynchronous WiFi scanner
          WiFi.scanNetworks(true); // true = async
        }
      } else if (netDashboardFocusIdx == 1) {
        // Option "sim & wifi" selected. Go to modes adjustment setup
        activeTftScreen = SCREEN_SIM_WIFI_SETUP;
        simWifiSetupFocusIdx = 0;
        tempSimAndWifiMode = currentSettings.simAndWifiMode;
        drawSimWifiSetupScreen();
      } else if (netDashboardFocusIdx == 2 && currentSettings.isWifiConfigured) {
        // Disconnect and delete all Wi-Fi credentials from NVRAM memory
        currentSettings.isWifiConfigured = false;
        memset(currentSettings.wifiSsid, 0, sizeof(currentSettings.wifiSsid));
        strcpy(selectedSsid, "None Selected");
        memset(typedPassword, 0, sizeof(typedPassword));
        
        saveSettingsToPreferences(currentSettings); // Persist erased memory to flash preferences
        
        WiFi.disconnect(); // actual wireless antenna release
        
        netDashboardFocusIdx = 0;
        drawNetworkDashboardScreen();
      } else if (netDashboardFocusIdx == numOptions) {
        // BACK red button option selected, go to Main Option Menu option 4
        activeTftScreen = SCREEN_MAIN_MENU;
        mainMenuIndex = 4;
        drawMainOptionMenu();
      }
    }
    return;
  }

  // SIM & WIFI setups parameter options screen routing
  if (activeTftScreen == SCREEN_SIM_WIFI_SETUP) {
    if (upClicked) {
      simWifiSetupFocusIdx = (simWifiSetupFocusIdx - 1 + 6) % 6;
      drawSimWifiSetupScreen();
    } else if (downClicked) {
      simWifiSetupFocusIdx = (simWifiSetupFocusIdx + 1) % 6;
      drawSimWifiSetupScreen();
    } else if (okClicked) {
      if (simWifiSetupFocusIdx <= 3) {
        tempSimAndWifiMode = simWifiSetupFocusIdx;
        drawSimWifiSetupScreen();
      } else if (simWifiSetupFocusIdx == 4) {
        // Back select, return to dashboard parameter list
        activeTftScreen = SCREEN_NET_DASHBOARD;
        netDashboardFocusIdx = 1;
        drawNetworkDashboardScreen();
      } else if (simWifiSetupFocusIdx == 5) {
        // Save current modifications to structure settings and EEPROM
        currentSettings.simAndWifiMode = tempSimAndWifiMode;
        saveSettingsToPreferences(currentSettings);
        
        saveState = 1; // SAVING
        saveProgress = 0;
        for (int p = 0; p <= 100; p += 10) {
          saveProgress = p;
          drawSimWifiSetupScreen();
          delay(40);
        }
        saveState = 2; // SAVED
        drawSimWifiSetupScreen();
        delay(1000);
        saveState = 0; // IDLE
        
        activeTftScreen = SCREEN_NET_DASHBOARD;
        netDashboardFocusIdx = 1;
        drawNetworkDashboardScreen();
      }
    }
    return;
  }

  // Network scanning blocked screen confirmation return
  if (activeTftScreen == SCREEN_MENU_NET_SCAN_ERROR) {
    if (okClicked || upClicked || downClicked) {
      activeTftScreen = SCREEN_NET_DASHBOARD;
      netDashboardFocusIdx = 0;
      drawNetworkDashboardScreen();
    }
    return;
  }

  // Wi-Fi Scanner AP menu routing
  if (activeTftScreen == SCREEN_MENU_NET_SCAN) {
    if (wifiScanCount <= 0) {
      if (okClicked || upClicked || downClicked) {
        activeTftScreen = SCREEN_NET_DASHBOARD;
        netDashboardFocusIdx = 0;
        drawNetworkDashboardScreen();
      }
      return;
    }

    int totalOptions = wifiScanCount + 2; // Scanned APs + BACK + REFRESH
    if (upClicked) {
      wifiNetSelection = (wifiNetSelection - 1 + totalOptions) % totalOptions;
      drawWifiScannerScreen();
    } else if (downClicked) {
      wifiNetSelection = (wifiNetSelection + 1) % totalOptions;
      drawWifiScannerScreen();
    } else if (okClicked) {
      if (wifiNetSelection == wifiScanCount) {
        // [ BACK TO MENU ] Selected! Return to network dashboard
        activeTftScreen = SCREEN_NET_DASHBOARD;
        netDashboardFocusIdx = 0;
        drawNetworkDashboardScreen();
      } else if (wifiNetSelection == wifiScanCount + 1) {
        // [ REFRESH SCAN ] Selected! Clear list and trigger async scan
        wifiScanCount = -1;
        wifiNetSelection = 0;
        drawWifiScannerScreen();
        
        isScanningWifi = true; // Suspend reconnect background loops
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);
        
        WiFi.scanNetworks(true); // Asynchronous background scan
      } else {
        // SSID selected. Copy ssid string & jump to wireless alphanumeric password keyboard screen index
        strncpy(selectedSsid, wifiScanBuffer[wifiNetSelection], 32);
        selectedSsid[32] = 0;
        
        // Clear keyed keyboard string to allow fresh password entries
        memset(typedPassword, 0, sizeof(typedPassword));
        kbCharIndex = 0;
        isShiftActive = false;
        needKeyboardFullRedraw = true;
        activeTftScreen = SCREEN_MENU_KEYBOARD;
        drawKeyboardScreen();
      }
    }
    return;
  }

  // Alphanumeric keyboard control routing logic
  if (activeTftScreen == SCREEN_MENU_KEYBOARD) {
    int totalKeys = 45; // 40 characters + SHIFT + SPACE + BS + CONNECT + BACK
    if (upClicked) {
      kbCharIndex = (kbCharIndex - 1 + totalKeys) % totalKeys;
      drawKeyboardScreen();
    } else if (downClicked) {
      kbCharIndex = (kbCharIndex + 1) % totalKeys;
      drawKeyboardScreen();
    } else if (okClicked) {
      if (kbCharIndex < 40) {
        char selectedChar = isShiftActive ? kbCharactersUpper[kbCharIndex] : kbCharactersLower[kbCharIndex];
        int pLen = strlen(typedPassword);
        if (pLen < 63) {
          typedPassword[pLen] = selectedChar;
          typedPassword[pLen + 1] = 0;
        }
        drawKeyboardScreen();
      } else if (kbCharIndex == 40) {
        // Toggle Shift State
        isShiftActive = !isShiftActive;
        // Fast update, redraw all keys to change character cases
        for (int i = 0; i < 45; i++) {
          drawSingleKeyboardKey(i, (i == kbCharIndex));
        }
      } else if (kbCharIndex == 41) {
        // Space
        int pLen = strlen(typedPassword);
        if (pLen < 63) {
          typedPassword[pLen] = ' ';
          typedPassword[pLen + 1] = 0;
        }
        drawKeyboardScreen();
      } else if (kbCharIndex == 42) {
        // BS backspace
        int pLen = strlen(typedPassword);
        if (pLen > 0) {
          typedPassword[pLen - 1] = 0;
        }
        drawKeyboardScreen();
      } else if (kbCharIndex == 43) {
        // Connect! Trigger connection wait screen
        activeTftScreen = SCREEN_WIFI_CONNECTING;
        wifiConnectStartTime = millis();
        needConnectingScreenFullRedraw = true;
        drawWifiConnectingScreen();
        
        WiFi.disconnect();
        WiFi.begin(selectedSsid, typedPassword);
      } else if (kbCharIndex == 44) {
        // BACK key to return back to Wifi networks scanner lists
        activeTftScreen = SCREEN_MENU_NET_SCAN;
        wifiNetSelection = 0;
        extern bool needNetScanFullRedraw;
        needNetScanFullRedraw = true;
        drawWifiScannerScreen();
      }
    }
    return;
  }
}

#endif // SCREEN_UI_H