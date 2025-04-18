#ifndef RFIDMANAGER_HPP
#define RFIDMANAGER_HPP

#include <Logger.hpp>
#include <SPI.h>
#include <MFRC522.h>
#include <helpers/debug.hpp>

class _RFIDManager
{
public:
	void setup();
	void scan();
	void handleCardAccess(String &uid, String &type);
protected:
	bool hasLocalAccess(String &uid, String &type, String &username);
	bool hasRemoteAccess(String &uid, String &type, String &username);

	void handleCardHasAccess(String &uid, String &type, String &username);
	void handleCardHasNoAccess(String &uid, String &type);

private:
	_Logger *logger;
	MFRC522 *mfrc522;
	bool hasRFIDDisconnected = false;
	unsigned short intervalId = 0;
};

extern _RFIDManager RFIDManager;

#endif
