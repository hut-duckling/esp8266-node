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

	bool isWarmedUp = false;
	do {
		OneWire oneWire(TEMPRETURE_SENSOR_ONE_WIRE_PIN);
		tempSensor = new DallasTemperature(&oneWire);
		tempSensor->begin();

		int deviceCount = tempSensor->getDeviceCount();
		LOG__INFO_F("The temperature sensors count is: %d", deviceCount);
		if (deviceCount > 0) {
			isWarmedUp = true;

			asyncTimer.setTimeout([&]() {
				asyncTimer.setInterval([&]() {
					float tempSum = 0;
					auto response = tempSensor->requestTemperatures();
					if (response.result) {
						// LOG__INFO_F("tempSensor->requestTemperatures at (%d) is true!", response.timestamp);
					} else {
						LOG__INFO_F("tempSensor->requestTemperatures at (%ld) is false!", response.timestamp);
					}

					int deviceCount = tempSensor->getDeviceCount();
					int dS18Count = tempSensor->getDS18Count();
					LOG__INFO_F("Update temperature: deviceCount is: %d dS18Count is: %d pin is: GPIO%d",
						deviceCount, dS18Count, TEMPRETURE_SENSOR_ONE_WIRE_PIN);

					for (int i = 0; i < deviceCount; i++)
					{
						float it = tempSensor->getTempCByIndex(i);
						if (DEVICE_DISCONNECTED_C == it) {
							LOG__ERROR_F("Error reading temperature of sensor at index: %d", i);
						} else {
							LOG__INFO_F("The temperature of sensor at index: %d is %f", i, it);
							tempSum += it;
						}
					}
					if (deviceCount > 0 && tempSum > 0) {
						this->lastTemperature = tempSum / deviceCount;
						this->temperatureSamples[this->temperatureSamplesIndex] = this->lastTemperature;
						this->temperatureSamplesIndex = (this->temperatureSamplesIndex + 1) % TEMPRETURE_SENSOR_SAMPLE_SIZE;

						LOG__INFO_F("The temperature average is: %f", this->lastTemperature);
					}
				}, 1 * 1000);

				LOG__DEBUG("Begin SensorsManager::setup [done]");
			}, tempSensor->millisToWaitForConversion());
		} else {
			delete tempSensor;
		}

	} while (!isWarmedUp);
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

uint _SensorsManager::getLightPercent()
{
	int voltage = analogRead(A0);
	uint percent = map(voltage, 0, 1023, 0, 99);
	return percent;
}
