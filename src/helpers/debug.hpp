#include <WString.h>
#include <Arduino.h>
#include <Logger.hpp>

#define REQUIRED_BUFFER_FOR_HEXIFY(n) 2 + (n * 2) + 1

void dumpByteArray(byte *buffer, byte bufferSize);
void hexify(const uint8_t *input, size_t inputLength, char *output);
String hexify(const uint8_t *input, size_t inputLength);
bool isPrintableBuffer(const uint8_t *input, size_t inputLength);
void hexifyIfNeeded(const uint8_t *input, size_t inputLength, char *output);
String hexifyIfNeeded(const uint8_t *input, size_t inputLength);
