#include <LCDManager.hpp>
#include <LiquidCrystal_I2C.h>
#include <Logger.hpp>
#include <AsyncTimer.h>
#include <SensorsManager.hpp>
#include <WifiManager.hpp>
#include <TimeLib.h>

extern AsyncTimer asyncTimer;

_LCDManager LCDManager(_LCDManager::getLcdAddress(), LCD_COLS, LCD_ROWS);

_LCDManager::_LCDManager(uint8_t lcd_address, uint8_t lcd_cols, uint8_t lcd_rows) : LiquidCrystal_I2C(lcd_address, lcd_cols, lcd_rows)
{
}

void _LCDManager::setup()
{
	LOG__DEBUG_F(
		"Begin LCDManager::setup with rows: %d and cols: %d at address: (0x%x)",
		this->getLcdRows(),
		this->getLcdRows(),
		this->getLcdAddress()
	);
	begin(LCD_COLS, LCD_ROWS);
	init();
	backlight();
	setCursor(0, 0);
	createChar(X_MARK_SYM, CHAR_X_MARK);
	createChar(WIFI_SIGNAL_A_SYM, CHAR_WIFI_SIGNAL_A);
	createChar(WIFI_SIGNAL_B_SYM, CHAR_WIFI_SIGNAL_B);
	createChar(WIFI_SIGNAL_C_SYM, CHAR_WIFI_SIGNAL_C);
	createChar(CELSIUS_SYM, CHAR_CELSIUS);
	LOG__DEBUG(F("Begin LCDManager::setup [done]"));

	asyncTimer.setInterval([&]() mutable {
		this->clearRow(0);

		int lastLength = 0;

		String light(SensorsManager.getLightPercent() + '%');
		this->setCursor(this->getLcdCols() - (lastLength += light.length()), 0);
		this->print(light.c_str());

		String temperature((int)SensorsManager.getAvgTempreture());
		this->setCursor(this->getLcdCols() - (lastLength += temperature.length()
			+ 1 // one for CELSIUS_SYM
			+ 1 // one space for space between light percent and temperature
		), 0);
		this->print(temperature);
		this->write(CELSIUS_SYM);

		if (this->getLcdCols() - lastLength >= 5) { // 03:30
			this->setCursor(0, 0);
			if (this->getLcdCols() - lastLength >= 8) { // 03:30:33]
				char time[9];
				sprintf(time, "%02d:%02d:%02d", hour(), minute(), second());
				LOG__TRACE_F("time is: %s", time);
				this->print(time);
			} else { // 03:30
				char time[6];
				sprintf(time, "%02d:%02d", hour(), minute());
				LOG__TRACE_F("time is: %s", time);
				this->print(time);
			}
		}
	}, 1 * 1000);

	int view = 0;
	asyncTimer.setInterval([&, view]() mutable {
		this->clearRow(1);
		if (view == 0)
		{
			this->setCursor(0, 1);
			this->write(WIFI_SIGNAL_C_SYM);

			String ip = WifiManager.getIpAddress().toString();
			if (ip.length() < this->getLcdCols() - 1) {
				this->setCursor(2, 1);
			} else {
				this->setCursor(1, 1);
			}
			this->print(WifiManager.getIpAddress().toString().c_str());
		}
		else if (view == 1)
		{
			this->setCursor(0, 1);
			this->write(WIFI_SIGNAL_C_SYM);
			String ssid = WifiManager.getWifiSSID();
			if (ssid.length() < this->getLcdCols() - 1) {
				this->setCursor(2, 1);
			} else {
				this->setCursor(1, 1);
			}
			if (ssid.length() > this->getLcdCols() - 1) {
				ssid = ssid.substring(0, this->getLcdCols() - 1);
			}
			this->print(ssid);
		}
		else if (view == 2)
		{
			this->setCursor(0, 1);
			this->print(F("FreeHeap: "));
			this->print(ESP.getFreeHeap());
		}

		if (++view > 2)
		{
			view = 0;
		}
	}, 1.5 * 1000);
}

void _LCDManager::clearRow(uint8_t row)
{
	for (int x = 0; x < this->getLcdCols(); x++)
	{
		this->setCursor(x, row);
		this->print(F(" "));
	}
}

uint8_t _LCDManager::getLcdAddress()
{
	return LCD_ADDRESS;
}

uint16_t _LCDManager::getLcdRows()
{
	return LCD_ROWS;
}

uint16_t _LCDManager::getLcdCols()
{
	return LCD_COLS;
}
