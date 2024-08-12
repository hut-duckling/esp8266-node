#ifndef OUTPUTMANAGER_HPP
#define OUTPUTMANAGER_HPP

#include <Logger.hpp>

#ifndef RELAY_PIN
#define RELAY_PIN D3
#endif

#ifndef RELAY_MODE
#define RELAY_MODE HIGH
#endif

#ifndef BUZZER_PIN
#define BUZZER_PIN LED_BUILTIN
#endif

class _OutputManager
{
public:
	void setup();

	void toggleRelay();
	void turnOnRelay();
	void turnOffRelay();
	void setRelay(bool turnOn);

	void buzzerNoTone();
	void buzzerTone(unsigned int frequency);
	void buzzerTone(unsigned int frequency, unsigned long duration);
	void buzzerTone(unsigned int frequency, unsigned long duration1, unsigned long delay, unsigned long duration2);
private:
	bool relayState = false;
	bool buzzerState = false;
};

extern _OutputManager OutputManager;

#endif
