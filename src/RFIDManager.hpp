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
	void handleCardAccess(String *uid, String *type);

protected:
	void dumpVersionToSerial();

private:
	_Logger *logger;
	MFRC522 *mfrc522;
	bool hasRFIDDisconnected = false;
	unsigned short intervalId = 0;
};

extern _RFIDManager RFIDManager;

#endif
