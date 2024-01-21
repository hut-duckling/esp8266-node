#include <OutputManager.hpp>
#include <Logger.hpp>
#include <AsyncTimer.h>

extern AsyncTimer asyncTimer;
_OutputManager OutputManager;

void _OutputManager::setup()
{
    LOG__DEBUG("Begin OutputManager::setup");
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    LOG__DEBUG("Begin OutputManager::setup [done]");
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
}

void _OutputManager::buzzerTone(unsigned int frequency)
{
    tone(BUZZER_PIN, frequency);
}

void _OutputManager::buzzerTone(unsigned int frequency, unsigned long duration)
{
    buzzerTone(frequency);
    asyncTimer.setTimeout([&]() {
        this->buzzerNoTone();
    }, duration);
}
