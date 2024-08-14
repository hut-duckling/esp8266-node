#include <RFIDManager.hpp>
#include <Logger.hpp>
#include <AsyncTimer.h>
#include <MFRC522.h>
#include <helpers/debug.hpp>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <OutputManager.hpp>
#include <TimeLib.h>
#include <WebSocketServer.hpp>
#include <LCDManager.hpp>

extern AsyncTimer asyncTimer;
_RFIDManager RFIDManager;

void _RFIDManager::setup()
{
	if (intervalId)
	{
		return;
	}
	LOG__DEBUG("Begin RFIDManager::setup");
	mfrc522 = new MFRC522(PIN_SPI_SS);
	SPI.begin();
	mfrc522->PCD_Init();
	mfrc522->PCD_DumpVersionToSerial();
	intervalId = asyncTimer.setInterval(std::bind(&_RFIDManager::scan, this), 500);
	LOG__DEBUG("Begin RFIDManager::setup [done]");
}

void _RFIDManager::scan()
{
	// LOG__DEBUG("Begin RFIDManager::scan");
	byte v = mfrc522->PCD_ReadRegister(mfrc522->PCD_Register::VersionReg);
	if ((v == 0x00) || (v == 0xFF))
	{
		hasRFIDDisconnected = true;
		LOG__ERROR("Communication FAILURE!, Is the MFRC522 properly connected?");
	}
	else if (hasRFIDDisconnected)
	{
		mfrc522->PCD_Init();
		hasRFIDDisconnected = false;
	}

	if (!mfrc522->PICC_IsNewCardPresent())
	{
		return;
	}
	if (!mfrc522->PICC_ReadCardSerial())
	{
		return;
	}
	mfrc522->PICC_HaltA();

	String cardUID = hexify(mfrc522->uid.uidByte, mfrc522->uid.size);

	MFRC522::PICC_Type piccType = mfrc522->PICC_GetType(mfrc522->uid.sak);
	String cardType = mfrc522->PICC_GetTypeName(piccType);

	LOG__DEBUG_F(
		"RFIDManager::scan cardUID: %s cardTye: %s",
		cardUID.c_str(),
		cardType.c_str()
	);

	this->handleCardAccess(cardUID, cardType);

	LOG__DEBUG("Begin RFIDManager::scan [done]");
}

void _RFIDManager::handleCardAccess(String &uid, String &type)
{
	LOG__DEBUG_F("Begin RFIDManager::handleCardAccess uid: %s type: %s", uid.c_str(), type.c_str());

	String username;
	if (this->hasLocalAccess(uid, type, username)) {
		this->handleCardHasAccess(uid, type, username);
	}
	else if (this->hasRemoteAccess(uid, type, username)) {
		this->handleCardHasAccess(uid, type, username);
	} else {
		this->handleCardHasNoAccess(uid, type);
	}

	LOG__DEBUG(F("Begin RFIDManager::handleCardAccess [done]"));
}

bool _RFIDManager::hasLocalAccess(String &uid, String &type, String &username)
{
	LOG__DEBUG_F("Begin RFIDManager::hasLocalAccess uid: %s type: %s", uid.c_str(), type.c_str());

	String filename = "/P/";
	filename += uid.c_str();
	LOG__INFO_F("RFIDManager::hasLocalAccess try open file: %s", filename.c_str());
	File file = LittleFS.open(filename, "r");
	if (!file) {
		LOG__WARN(F("RFIDManager::hasLocalAccess there is NO File!"));
		return false;
	}
	LOG__WARN(F("RFIDManager::hasLocalAccess there is File!"));

	JsonDocument user;
	const String content = file.readString();
	LOG__INFO_F("_RFIDManager::hasLocalAccess file content: %s %s", content.c_str(), file.readString().c_str());
	DeserializationError error = deserializeJson(user, content);
	if (error)
	{
		LOG__WARN_F("_RFIDManager::hasLocalAccess: failed to parse json file, code: %d error: %s", error.code(), error.c_str());
		file.close();
		return false;
	}

	username = user["user"].as<String>();

	unsigned int accountType = user["acctype"].as<unsigned int>();
	if (accountType == 1)
	{
		unsigned long validL = user["validuntil"].as<unsigned long>();
		unsigned long nowL = now();
		if (validL > nowL)
		{
			return true;
		}
	}
	else if (accountType == 99)
	{
		return true;
	}

	return false;
}

bool _RFIDManager::hasRemoteAccess(String &uid, String &type, String &username)
{
	LOG__DEBUG_F("Begin RFIDManager::hasRemoteAccess uid: %s type: %s", uid.c_str(), type.c_str());
	return false;
	LOG__DEBUG("Begin RFIDManager::hasRemoteAccess [done]");
}

void _RFIDManager::handleCardHasAccess(String &uid, String &type, String &username)
{
	LOG__WARN(F("RFIDManager::handleCardHasAccess start!"));

	LOG__DEBUG_F("RFIDManager::handleCardHasAccess: Give Access to: %s", username.c_str());
	OutputManager.toggleRelay();
	OutputManager.buzzerTone(1, 500);

	LCDManager.setCursor(0, LCDManager.getLcdRows() - 1);
	LCDManager.print("Hi, " + username);
	asyncTimer.setTimeout([]() {
		LCDManager.clearRow(LCDManager.getLcdRows() - 1);
	}, 3000);

	LOG__DEBUG("Give Access [done]");

	String data = String(username.c_str());
	data += " " + String(uid.c_str());

	Logger.writeEvent("WARN", "rfid", "Say hello to teacher!", data);

	WebSocketServer.textAll("{\"command\":\"giveAccess\"}");

	LOG__WARN(F("RFIDManager::handleCardHasAccess start! [done]"));
}

void _RFIDManager::handleCardHasNoAccess(String &uid, String &type)
{
	LOG__WARN(F("RFIDManager::handleCardHasNoAccess"));
	JsonDocument doc;
	doc["command"] = "piccscan";
	doc["uid"] = uid.c_str();
	doc["type"] = type.c_str();
	doc["known"] = 0;
	doc["user"] = "استاد جدید";
	String json;
	serializeJson(doc, json);
	WebSocketServer.textAll(json);

	String data = String(uid.c_str());
	data += " " + String(type.c_str());
	Logger.writeEvent("WARN", "rfid", "Unknown Card!", data);

	LCDManager.setCursor(0, LCDManager.getLcdRows() - 1);
	LCDManager.print("Unauthorized card!");
	asyncTimer.setTimeout([]() {
		LCDManager.clearRow(LCDManager.getLcdRows() - 1);
	}, 3000);

	OutputManager.buzzerTone(1, 4000, 6000, 5000);
	LOG__WARN(F("RFIDManager::handleCardHasNoAccess [done]"));
}
