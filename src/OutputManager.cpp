#include <OutputManager.hpp>
#include <Logger.hpp>
#include <AsyncTimer.h>
#include <TimeLib.h>

extern AsyncTimer asyncTimer;
_OutputManager OutputManager;

void _OutputManager::setup()
{
    LOG__DEBUG("Begin OutputManager::setup");
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    LOG__DEBUG("Begin OutputManager::setup [done]");

    int buzzerHighCounterSeconds = 0;
    asyncTimer.setInterval([&]() {
        int buzzerValue = digitalRead(BUZZER_PIN);
        LOG__INFO_F("Buzzer value is: %d counter is: %d", buzzerValue, buzzerHighCounterSeconds);
        if (buzzerValue == 0)  {
            buzzerHighCounterSeconds = 0;
        } else if (++buzzerHighCounterSeconds > 5) {
            LOG__INFO_F("Buzzer value is: %d counter is: %d call buzzerNoTone", buzzerValue, buzzerHighCounterSeconds);
            this->buzzerNoTone();
            LOG__INFO_F("Buzzer value is: %d counter is: %d call buzzerNoTone [done]", buzzerValue, buzzerHighCounterSeconds);
        }
        LOG__INFO_F("Buzzer value is: %d counter is: %d [done]", buzzerValue, buzzerHighCounterSeconds);
    }, 1 * 1000);
}

void _OutputManager::toggleRelay()
{
    setRelay(!relayState);
}

void _OutputManager::turnOnRelay()
{
    setRelay(true);
}

void _OutputManager::turnOffRelay()
{
    setRelay(false);
}

void _OutputManager::setRelay(bool state)
{
    relayState = state;
    digitalWrite(RELAY_PIN, relayState ? RELAY_MODE : !RELAY_MODE);
}

void _OutputManager::buzzerNoTone()
{
    noTone(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, LOW);
}

void _OutputManager::buzzerTone(unsigned int frequency)
{
    tone(BUZZER_PIN, frequency);
}

void _OutputManager::buzzerTone(unsigned int frequency, unsigned long duration)
{
    tone(BUZZER_PIN, frequency, duration);
}

void _OutputManager::buzzerTone(unsigned int frequency, unsigned long duration1, unsigned long delay, unsigned long duration2)
{
    this->buzzerTone(frequency, duration1);
    asyncTimer.setTimeout([&]() {
        this->buzzerTone(frequency, duration2);
    }, duration1 + delay);
}