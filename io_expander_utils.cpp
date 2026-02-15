#include "io_expander_utils.h"

namespace IOExpanderUtils
{

    void write8(IOExpander &expander, uint8_t value)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            expander.digitalWrite(i, (value >> i) & 0x01);
        }
    }

    void write16(IOExpander &expander, uint16_t value)
    {
        for (uint8_t i = 0; i < 16; i++)
        {
            expander.digitalWrite(i, (value >> i) & 0x01);
        }
    }

    uint8_t read8(IOExpander &expander)
    {
        uint8_t result = 0;
        for (uint8_t i = 0; i < 8; i++)
        {
            result |= (expander.digitalRead(i) & 0x01) << i;
        }
        return result;
    }

    uint16_t read16(IOExpander &expander)
    {
        uint16_t result = 0;
        for (uint8_t i = 0; i < 16; i++)
        {
            result |= (expander.digitalRead(i) & 0x01) << i;
        }
        return result;
    }

} // namespace IOExpanderUtils