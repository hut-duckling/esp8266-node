#include <Arduino.h>
#include <Logger.hpp>
#include <AsyncTimer.h>
#include <LCDManager.hpp>
#include <LittleFS.h>
#include <I2CScanner.hpp>
#include <SensorsManager.hpp>
#include <RFIDManager.hpp>
#include <OutputManager.hpp>
#include <HttpServer.hpp>
#include <WebSocketServer.hpp>

AsyncTimer asyncTimer(15U);
void setup()
{
  delay(1000);
  Logger.setup();
  LOG__INFO(F("Starting"));

  if (! LittleFS.begin()) {
    LOG__ERROR(F("Can not start LittleFS! so format it!"));
    LittleFS.format();
  }

  LOG__DEBUG("Starting I2CScanner Service");
  I2CScanner.setup();
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

  asyncTimer.setInterval([]() {
    LCDManager.setCursor(0, 1);
    LCDManager.print("RAM:" + String(ESP.getFreeHeap()) + "-" + String(millis()));
    // LOG__DEBUG_F("light sensor value: %s", SensorsManager.getLightPercent().c_str());
  }, 1 * 1000);
}

void loop()
{
  // LOG__DEBUG_F("availble memory before execute loop: %u bytes", ESP.getFreeHeap());

  // LOG__DEBUG("Handling async actions");
  asyncTimer.handle();
  // LOG__DEBUG("Handling async actions [done]");

  // LOG__DEBUG_F("availble memory after execute loop: %u bytes", ESP.getFreeHeap());
}
