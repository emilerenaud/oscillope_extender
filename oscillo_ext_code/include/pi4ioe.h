#ifndef IO_MUX_H
#define IO_MUX_H

#include <Arduino.h>
#include <Wire.h>

namespace PI4IOE
{
    class PI4IOE
    {
    public:
        PI4IOE();
        void begin(uint8_t address, TwoWire *wire);
        void configure_pin(uint8_t pin, uint8_t in_out);
        void set_pin(uint8_t pin, uint8_t state);
        uint8_t read_register(byte reg);
        void write_register(byte reg, byte data);
        void check_id();

    private:
        uint8_t _address;
        TwoWire *_wire;
    };

    // Register addresses
    // B7:5 = ID. B4:2 = Firmware. B1 = Reset Interrupt. B0 = Soft reset
    const uint8_t ID_CONTROL = 0x01;
    const uint8_t IO_DIRECTION = 0x03;
    const uint8_t OUTPUT_STATE = 0x05;
    const uint8_t OUTPUT_HZ = 0x07;
    const uint8_t INPUT_DEFAULT_STATE = 0x09;
    const uint8_t PULL_UP_DOWN_ENABLE = 0x0B;
    const uint8_t PULL_UP_DOWN_SELECT = 0x0D;
    const uint8_t INPUT_STATUS = 0x0F;
    const uint8_t INTERRUPT_MASK = 0x11;
    const uint8_t INTERRUPT_STATUS = 0x13;

};

#endif
