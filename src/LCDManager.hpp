#ifndef LCDMANAGER_HPP
#define LCDMANAGER_HPP

#include <LiquidCrystal_I2C.h>
#include <Logger.hpp>
#include <binary.h>

#ifndef LCD_ADDRESS
#define LCD_ADDRESS 0x27
#endif

#ifndef LCD_COLS
#define LCD_COLS 16
#endif

#ifndef LCD_ROWS
#define LCD_ROWS 2
#endif

#define X_MARK_SYM 0
#define WIFI_SIGNAL_A_SYM 1
#define WIFI_SIGNAL_B_SYM 2
#define WIFI_SIGNAL_C_SYM 3
#define CELSIUS_SYM 4

constexpr char PROGMEM CHAR_X_MARK[] = {B00000, B10001, B01010, B00100, B01010, B10001, B00000, B00000};
constexpr char PROGMEM CHAR_WIFI_SIGNAL_A[] = {B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00100};
constexpr char PROGMEM CHAR_WIFI_SIGNAL_B[] = {B00000, B00000, B00000, B00000, B01110, B01010, B00000, B00100};
constexpr char PROGMEM CHAR_WIFI_SIGNAL_C[] = {B01110, B11111, B10001, B00000, B01110, B01010, B00000, B00100};
constexpr char PROGMEM CHAR_CELSIUS[] = {B11000, B11000, B00000, B00111, B01000, B01000, B01000, B00111};

class _LCDManager : public LiquidCrystal_I2C
{
public:
	_LCDManager(uint8_t lcd_address, uint8_t lcd_cols, uint8_t lcd_rows);
	void setup();
	void clearRow(uint8_t row);
	static uint8_t getLcdAddress();
	uint16_t getLcdRows();
	uint16_t getLcdCols();
};

extern _LCDManager LCDManager;

#endif
