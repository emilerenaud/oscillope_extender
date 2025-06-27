#include "pi4ioe.h"

namespace PI4IOE
{
    PI4IOE::PI4IOE() {}

    void PI4IOE::begin(uint8_t address, TwoWire *wire)
    {
        _wire = wire;
        _address = address;

        read_register(ID_CONTROL);
        write_register(ID_CONTROL, 0b00000001);
        delay(100);
        read_register(ID_CONTROL);

        configure_pin(0, INPUT);
        configure_pin(1, INPUT);
        configure_pin(2, INPUT);
        configure_pin(3, INPUT);
        configure_pin(4, INPUT);
        configure_pin(5, INPUT);
        configure_pin(6, INPUT);
        configure_pin(7, INPUT);
        write_register(OUTPUT_HZ, 0x00);
    }

    void PI4IOE::check_id()
    {
        Serial.println(read_register(ID_CONTROL));
    
    }

    uint8_t PI4IOE::read_register(byte reg)
    {
        _wire->beginTransmission(_address);
        _wire->write(reg);
        _wire->endTransmission();
        _wire->requestFrom(_address, 1);
        byte data = _wire->read();
        uint8_t err = _wire->endTransmission(1);
        return data;
    }

    void PI4IOE::write_register(byte reg, byte data)
    {
        _wire->beginTransmission(_address);
        _wire->write(reg);
        _wire->write(data);
        _wire->endTransmission();
    }

    void PI4IOE::configure_pin(uint8_t pin, uint8_t in_out)
    {
        uint8_t reg = read_register(IO_DIRECTION);
        reg = (in_out == INPUT) ? (reg & ~(1 << pin)) : (reg | (1 << pin));
        write_register(IO_DIRECTION, reg);
    }

    void PI4IOE::set_pin(uint8_t pin, uint8_t state)
    {
        uint8_t reg = read_register(OUTPUT_STATE);
        reg = (state == LOW) ? (reg & ~(1 << pin)) : (reg | (1 << pin));
        write_register(OUTPUT_STATE, reg);
    }
};
