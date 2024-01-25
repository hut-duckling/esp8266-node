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

	this->handleCardAccess(&cardUID, &cardType);

	LOG__DEBUG("Begin RFIDManager::scan [done]");
}

void _RFIDManager::handleCardAccess(String *uid, String *type)
{
	LOG__DEBUG_F("Begin RFIDManager::handleCardAccess uid: %s type: %s", uid->c_str(), type->c_str());

	String filename = "/P/";
	filename += uid->c_str();
	LOG__INFO_F("RFIDManager::handleCardAccess try open file: %s", filename.c_str());
	File file = LittleFS.open(filename, "r");
	if (file)
	{
		LOG__WARN(F("RFIDManager::handleCardAccess there is File!"));
		JsonDocument user;
		const String content = file.readString();
		LOG__INFO_F("_RFIDManager::handleCardAccess file content: %s %s", content.c_str(), file.readString().c_str());
		DeserializationError error = deserializeJson(user, content);
		if (error)
		{
			LOG__WARN_F("_RFIDManager::handleCardAccess: failed to parse json file, code: %d error: %s", error.code(), error.c_str());
			file.close();
			return;
		}
		
		String username = user["user"].as<String>();
		unsigned int accountType = user["acctype"].as<unsigned int>();
		if (accountType == 1)
		{
			unsigned long validL = user["validuntil"].as<unsigned long>();
			unsigned long nowL = now();
			if (validL > nowL)
			{
				LOG__DEBUG("Give Access");
				OutputManager.toggleRelay();
				OutputManager.buzzerTone(1, 500);
				LOG__DEBUG("Give Access [done]");

				WebSocketServer.textAll("{\"command\":\"giveAccess\"}");
			} else {
				LOG__DEBUG("Give Access: card expired");
				OutputManager.buzzerTone(1, 500);
				asyncTimer.setTimeout([]() {
					OutputManager.buzzerTone(1, 500);
				}, 1500);
			}
		}
		else if (accountType == 99)
		{
			LOG__DEBUG("Give Access");
			OutputManager.toggleRelay();
			OutputManager.buzzerTone(1, 500);
			LOG__DEBUG("Give Access [done]");

			WebSocketServer.textAll("{\"command\":\"giveAccess\"}");
		}
		else
		{
			LOG__WARN("Card has no access!");
			JsonDocument doc;
			doc["command"] = "piccscan";
			doc["uid"] = uid->c_str();
			doc["type"] = type->c_str();
			doc["known"] = 1;
			doc["acctype"] = 0;
			doc["user"] = username;

			String json;
			serializeJson(doc, json);
			WebSocketServer.textAll(json);
		}

		file.close();
	} else {
		LOG__WARN(F("RFIDManager::handleCardAccess there is no file!"));
		JsonDocument doc;
		doc["command"] = "piccscan";
		doc["uid"] = uid->c_str();
		doc["type"] = type->c_str();
		doc["known"] = 0;
		doc["user"] = "استاد جدید";
		String json;
		serializeJson(doc, json);
		WebSocketServer.textAll(json);

		OutputManager.buzzerTone(1, 400);
		asyncTimer.setTimeout([]() {
			OutputManager.buzzerTone(1, 500);
		}, 600);
	}

	LOG__DEBUG(F("Begin RFIDManager::handleCardAccess [done]"));
}
