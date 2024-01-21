#include <SensorsManager.hpp>
#include <Logger.hpp>
#include <AsyncTimer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

extern AsyncTimer asyncTimer;
_SensorsManager SensorsManager(nullptr);

_SensorsManager::_SensorsManager(_Logger *logger)
{
	this->logger = logger;
}

void _SensorsManager::setup()
{
	LOG__DEBUG("Begin SensorsManager::setup");

	OneWire oneWire(TEMPRETURE_SENSOR_ONE_WIRE_PIN);
	tempSensor = new DallasTemperature(&oneWire);
	tempSensor->begin();
	int deviceCount = tempSensor->getDeviceCount();
	LOG__INFO_F("The temperature sensors count is: %d", deviceCount);

	asyncTimer.setInterval([&]() {
		float tempSum = 0;
		tempSensor->requestTemperatures();

		int deviceCount = tempSensor->getDeviceCount();
		// LOG__DEBUG_F("Update temperature: sensors count is: %d", deviceCount);

		for (int i = 0; i < deviceCount; i++)
		{
			float it = tempSensor->getTempCByIndex(i);
			// LOG__DEBUG_F("The temperature of sensor at index: %d is %f", i, it);
			tempSum += it;
		}
		if (deviceCount) {
			this->lastTemperature = tempSum / deviceCount;
			this->temperatureSamples[this->temperatureSamplesIndex] = this->lastTemperature;
			this->temperatureSamplesIndex = (this->temperatureSamplesIndex + 1) % TEMPRETURE_SENSOR_SAMPLE_SIZE;

			LOG__DEBUG_F("The temperature average is: %f", this->lastTemperature);
		}
	}, 1000);

	LOG__DEBUG("Begin SensorsManager::setup [done]");
}

float _SensorsManager::getLastTempreture()
{
	return this->lastTemperature;
}

float _SensorsManager::getAvgTempreture()
{
	float avg = 0;
	for (int x = 0; x < TEMPRETURE_SENSOR_SAMPLE_SIZE; x++) {
		avg += this->temperatureSamples[x];
	}
	avg /= TEMPRETURE_SENSOR_SAMPLE_SIZE;
	return avg;
}

String _SensorsManager::getLightPercent()
{
	int voltage = analogRead(A0);
	int percent = map(voltage, 0, 1023, 0, 99);
	return String(percent);
}
