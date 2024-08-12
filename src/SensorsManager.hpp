#ifndef SENSORSMANAGER_HPP
#define SENSORSMANAGER_HPP

#include <Logger.hpp>
#include <OneWire.h>
#include <DallasTemperature.h>

#ifndef TEMPRETURE_SENSOR_ONE_WIRE_PIN
#define TEMPRETURE_SENSOR_ONE_WIRE_PIN D0
#endif

#ifndef TEMPRETURE_SENSOR_SAMPLE_SIZE
#define TEMPRETURE_SENSOR_SAMPLE_SIZE 10
#endif

class _SensorsManager
{
public:
	_SensorsManager(_Logger *logger);
	void setup();
	float getLastTempreture();
	float getAvgTempreture();
	uint getLightPercent();

private:
	_Logger *logger;
	float lastTemperature = 0;
	int temperatureSamplesIndex = 0;
	float temperatureSamples[TEMPRETURE_SENSOR_SAMPLE_SIZE];
	DallasTemperature *tempSensor;
};

extern _SensorsManager SensorsManager;

#endif
