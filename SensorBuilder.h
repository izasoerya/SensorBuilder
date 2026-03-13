#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <cstdint>
#include <ModbusMaster.h>
#include <memory>

class CommunicationProperties
{
public:
    virtual ~CommunicationProperties() = default;
    virtual void connect() = 0;
    virtual uint16_t read() = 0;
};

class ModbusBase
{
public:
    uint8_t slaveId;
    uint8_t address;
    uint8_t functionCode;
    uint8_t lengthAddress;
};

class BuilderBaseModbus
{
public:
    virtual ~BuilderBaseModbus() = default;
    virtual BuilderBaseModbus &setSlaveId(uint8_t slaveId) = 0;
    virtual BuilderBaseModbus &setAddress(uint8_t address) = 0;
    virtual BuilderBaseModbus &setFunctionCode(uint8_t functionCode) = 0;
    virtual BuilderBaseModbus &setLengthAddress(uint8_t lengthAddress) = 0;
    virtual std::unique_ptr<ModbusBase> getResult() = 0;
};

class ModbusBuilder : public BuilderBaseModbus
{
private:
    std::unique_ptr<ModbusBase> base;

public:
    ModbusBuilder() : base(new ModbusBase()) {}

    ModbusBuilder &setSlaveId(uint8_t slaveId) override
    {
        base->slaveId = slaveId;
        return *this;
    }
    ModbusBuilder &setAddress(uint8_t address) override
    {
        base->address = address;
        return *this;
    }
    ModbusBuilder &setFunctionCode(uint8_t functionCode) override
    {
        base->functionCode = functionCode;
        return *this;
    }
    ModbusBuilder &setLengthAddress(uint8_t lengthAddress) override
    {
        base->lengthAddress = lengthAddress;
        return *this;
    }

    std::unique_ptr<ModbusBase> getResult() override
    {
        return std::move(base);
    }
};

class ModbusConnection : public CommunicationProperties
{
private:
    ModbusMaster node;
    std::unique_ptr<ModbusBase> base;
    uint8_t enablePin;

    static uint8_t &callbackPin()
    {
        static uint8_t pin = 255;
        return pin;
    }

    static void preTransmission()
    {
        digitalWrite(callbackPin(), HIGH);
    }

    static void postTransmission()
    {
        digitalWrite(callbackPin(), LOW);
    }

public:
    ModbusConnection(uint8_t enablePin) : enablePin(enablePin) {}

    void setConfig(std::unique_ptr<ModbusBase> config)
    {
        base = std::move(config);
    }

    void connect() override
    {
        callbackPin() = enablePin;
        pinMode(enablePin, OUTPUT);
        digitalWrite(enablePin, LOW);
        node.preTransmission(preTransmission);
        node.postTransmission(postTransmission);
        node.begin(base->slaveId, Serial);
    }

    uint16_t read() override
    {
        if (base->functionCode == 0x03)
        {
            return node.readHoldingRegisters(base->address, base->lengthAddress);
        }
        else if (base->functionCode == 0x04)
        {
            return node.readInputRegisters(base->address, base->lengthAddress);
        }
        return 0;
    }
};

#endif