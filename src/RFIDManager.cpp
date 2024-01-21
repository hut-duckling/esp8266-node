#include <RFIDManager.hpp>
#include <Logger.hpp>
#include <AsyncTimer.h>
#include <MFRC522.h>
#include <helpers/debug.hpp>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <OutputManager.hpp>

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
		LOG__ERROR("Communication FAILURE!, Is the MFRC522 properly connected?");
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

	// int AccType = 0;
	String filename = "/P/";
	filename += *uid;
	File file = LittleFS.open(filename, "r");
	if (file)
	{
		LOG__WARN(F("RFIDManager::handleCardAccess there is File!"));
		// JsonDocument json;
		// size_t size = file.size();
		// std::unique_ptr<char[]> buf(new char[size]);
		// file.readBytes(buf.get(), size);
		// DeserializationError error = deserializeJson(json, buf);
		// if (error)
		// {
		// 	LOG__WARN(F("RFIDManager::handleCardAccess failed to parse users file"));
		// 	return;
		// }
		// OutputManager.toggleRelay();
	} else {
		LOG__WARN(F("RFIDManager::handleCardAccess there is no file!"));
	}

	LOG__DEBUG("Begin RFIDManager::handleCardAccess toggleRelay");
	OutputManager.toggleRelay();
	LOG__DEBUG("Begin RFIDManager::handleCardAccess toggleRelay [done]");

	LOG__DEBUG(F("Begin RFIDManager::handleCardAccess [done]"));
}
