#include <LCDManager.hpp>
#include <LiquidCrystal_I2C.h>
#include <Logger.hpp>
#include <AsyncTimer.h>
#include <SensorsManager.hpp>
#include <WifiManager.hpp>
#include <TimeLib.h>
#include <DateConvL.h>

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

	asyncTimer.setInterval([&]() {
		const uint8_t dateAndTimeRow = 0;
		this->setCursor(0, dateAndTimeRow);
		DateConvL dateC;
		dateC.ToShamsi(year(), month(), day());
		this->printf("%04d/%02d/%02d  %02d:%02d:%02d", dateC.global_year, dateC.global_month, dateC.global_day, hour(), minute(), second());

		const uint8_t lightAndTempRow = 1;
		this->setCursor(0, lightAndTempRow);
		this->printf("Light %02d%% Temp %.2fc", SensorsManager.getLightPercent(), SensorsManager.getAvgTempreture());
	}, 1 * 1000);

	int view = 0;
	asyncTimer.setInterval([&, view]() mutable {
		const uint8_t line = 2;
		this->clearRow(line);
		if (view == 0)
		{
			this->setCursor(0, line);
			this->write(WIFI_SIGNAL_C_SYM);

			String ip = WifiManager.getIpAddress().toString();
			if ((unsigned)ip.length() < this->getLcdCols() - 1) {
				this->setCursor(2, line);
			} else {
				this->setCursor(1, line);
			}
			this->print(WifiManager.getIpAddress().toString().c_str());
		}
		else if (view == 1)
		{
			this->setCursor(0, line);
			this->write(WIFI_SIGNAL_C_SYM);
			String ssid = WifiManager.getWifiSSID();
			if ((unsigned)ssid.length() < this->getLcdCols() - 1) {
				this->setCursor(2, line);
			} else {
				this->setCursor(1, line);
			}
			if ((unsigned)ssid.length() > this->getLcdCols() - 1) {
				ssid = ssid.substring(0, this->getLcdCols() - 1);
			}
			this->print(ssid);
		}

		if (++view > 1)
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
