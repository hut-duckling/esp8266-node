#include <Arduino.h>
#include <Ticker.h>
#include <AsyncTimer.h>
#include <LittleFS.h>
#include <Logger.hpp>
#include <ConfigManager.hpp>
#include <LCDManager.hpp>
#include <I2CScanner.hpp>
#include <SensorsManager.hpp>
#include <RFIDManager.hpp>
#include <OutputManager.hpp>
#include <WifiManager.hpp>
#include <HttpServer.hpp>
#include <WebSocketServer.hpp>

#ifdef DEBUG_ESP_PORT // to debug WiFi
// #define DEBUG_ESP_PORT Serial
#endif

AsyncTimer asyncTimer(15U);
void setup()
{
  delay(1000);
  Logger.setup();
  LOG__INFO("Starting");

  if (! LittleFS.begin()) {
    LOG__ERROR("Can not start LittleFS! so format it!");
    LittleFS.format();
  }

  LOG__DEBUG("Loading ConfigManager");
  ConfigManager.load();
  LOG__DEBUG("Loading ConfigManager [done]");

  LOG__DEBUG("Setuping ConfigManager");
  ConfigManager.setup();
  LOG__DEBUG("Setuping ConfigManager [done]");

  LOG__DEBUG("Setuping WifiManager");
  WifiManager.setup();
  LOG__DEBUG("Setuping WifiManager [done]");

  LOG__DEBUG("Starting I2CScanner Service");
  // I2CScanner.setup();
  // I2CScanner.start();
  LOG__DEBUG("Starting I2CScanner Service [done]");

  LOG__DEBUG("Setuping SensorsManager");
  SensorsManager.setup();
  LOG__DEBUG("Setuping SensorsManager [done]");

  LOG__DEBUG("Setuping RFIDManager");
  RFIDManager.setup();
  LOG__DEBUG("Setuping RFIDManager [done]");

  LOG__DEBUG("Setuping OutputManager");
  OutputManager.setup();
  LOG__DEBUG("Setuping OutputManager [done]");

  LOG__DEBUG("Setuping LCDManager");
  LCDManager.setup();
  LOG__DEBUG("Setuping LCDManager [done]");

  LOG__DEBUG("Setuping WebSocketServer");
  WebSocketServer.setup();
  LOG__DEBUG("Setuping WebSocketServer [done]");

  LOG__DEBUG("Setuping HttpServer");
  HttpServer.setup();
  LOG__DEBUG("Setuping HttpServer [done]");
}

void loop()
{
  LOG__TRACE_F("availble memory before execute loop: %u bytes", ESP.getFreeHeap());
  asyncTimer.handle();
  LOG__TRACE_F("availble memory after execute loop: %u bytes", ESP.getFreeHeap());
}
