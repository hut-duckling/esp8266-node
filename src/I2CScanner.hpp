#ifndef I2CSCANNER_HPP
#define I2CSCANNER_HPP

class _I2CScanner
{
public:
	void setup();
	void start();
	void stop();

private:
	unsigned short intervalId = 0;

	void scan() const;
};

extern _I2CScanner I2CScanner;

#endif