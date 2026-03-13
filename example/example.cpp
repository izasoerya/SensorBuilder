#include <Arduino.h>
#include "SensorBuilder.h"

void setup()
{
    ModbusBuilder shtObject;
    shtObject.setSlaveId(1).setFunctionCode(0x03).setAddress(0x00).setLengthAddress(2);

    ModbusConnection shtConnection(13); // 13 is DE / RE PIN
    shtConnection.setConfig(shtObject.getResult());
    shtConnection.connect();
    uint16_t result = shtConnection.read();
}

void loop()
{
    // put your main code here, to run repeatedly:
}
