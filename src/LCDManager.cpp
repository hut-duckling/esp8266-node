#include <LCDManager.hpp>
#include <LiquidCrystal_I2C.h>
#include <Logger.hpp>
#include <AsyncTimer.h>
#include <SensorsManager.hpp>

extern AsyncTimer asyncTimer;

_LCDManager LCDManager(_LCDManager::getLcdAddress(), LCD_COLS, LCD_ROWS);

_LCDManager::_LCDManager(uint8_t lcd_address, uint8_t lcd_cols, uint8_t lcd_rows) : LiquidCrystal_I2C(lcd_address, lcd_cols, lcd_rows)
{
}

void _LCDManager::setup()
{
	LOG__DEBUG(F("Begin LCDManager::setup"));
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

	int view = 0;
	asyncTimer.setInterval([&, view]() mutable {
		int lastLength = 0;

		if (view == 0)
		{
			this->clearRow(0);
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
		}
		else if (view == 1)
		{
			this->clearRow(0);
			String ip("255.255.255.255");
			this->setCursor(this->getLcdCols() - (lastLength += ip.length() + 1), 0);
			this->write(WIFI_SIGNAL_C_SYM);
			this->print(ip.c_str());
		}

		if (++view > 1)
		{
			view = 0;
		}
	}, 2 * 1000);
}

void _LCDManager::clearRow(uint8_t row)
{
	for (int x = 0; x < this->getLcdCols(); x++)
	{
		this->setCursor(x, row);
		this->print(' ');
	}
}

uint8_t _LCDManager::getLcdAddress()
{
	return LCD_ADDRESS;
}

uint8_t _LCDManager::getLcdRows()
{
	return LCD_ROWS;
}

uint8_t _LCDManager::getLcdCols()
{
	return LCD_COLS;
}
