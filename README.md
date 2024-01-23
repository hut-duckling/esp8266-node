# node-worker
The slave node of the project


PINS-D1-MINI:
    A0: Light Sensor (Pulldown it with resistor)

    D0: Thermocouple Yellow Wire

    D1: LCD -> SCL
    D2: LCD -> SDA

    D3: Output Relay Pin (For class lights and cooling system, I use a LED with resistor)
    D4: LED_BUILTIN (Connect + of Buzzer to this pin, this pin also connected to builtin led of the Wemos)

    D5: RFID-M522 -> SCK
    D6: RFID-M522 -> MISO
    D7: RFID-M522 -> MOSI
    D8: RFID-M522 -> SS (SDA written on board)

