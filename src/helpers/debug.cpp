#include "debug.hpp"
#include <cstdio>
#include <Logger.hpp>

void dumpByteArray(byte *buffer, byte bufferSize)
{
	for (byte i = 0; i < bufferSize; i++)
	{
		LOG__INFO_F(
			"dumpByteArray: %s %s",
			buffer[i] < 0x10 ? " 0" : " ",
			String(buffer[i], HEX).c_str()
		);
	}
}

void hexify(const uint8_t *input, size_t inputLength, char *output)
{
	output[0] = '0';
	output[1] = 'x';
	for (size_t x = 0; x < inputLength; x++)
	{
		sprintf(output + (x * 2) + 2, "%02x", input[x]);
	}
}

String hexify(const uint8_t *input, size_t inputLength)
{
	String result;
	result.reserve(REQUIRED_BUFFER_FOR_HEXIFY(inputLength) - 1);
	hexify(input, inputLength, (char *)result.c_str());
	return result;
}

bool isPrintableBuffer(const uint8_t *input, size_t inputLength)
{
	bool result = true;
	for (size_t x = 0; x < inputLength; x++)
	{
		if (!isprint(input[x]))
		{
			result = false;
		}
	}
	return result;
}

void hexifyIfNeeded(const uint8_t *input, size_t inputLength, char *output)
{
	if (isPrintableBuffer(input, inputLength))
	{
		memcpy(output, input, inputLength);
		if (input[inputLength - 1] != 0)
		{
			output[inputLength] = 0;
		}
		return;
	}
	hexify(input, inputLength, output);
}

String hexifyIfNeeded(const uint8_t *input, size_t inputLength)
{
	if (isPrintableBuffer(input, inputLength))
	{
		String s;
		s.reserve(inputLength);
		s.concat((const char *)input, inputLength);
		return s;
	}
	return hexify(input, inputLength);
}