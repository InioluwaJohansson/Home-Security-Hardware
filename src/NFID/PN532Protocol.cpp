#include "PN532Protocol.h"
#include "../Hardware/Pins.h"
#include <cstring>

constexpr char PN532Protocol::Signature[];

PN532Protocol::PN532Protocol(TwoWire& wire)
    : _wire(wire)
{
    _nfc = new Adafruit_PN532(Pins::PN532_IRQ, Pins::PN532_RESET, &_wire);
}

bool PN532Protocol::Initialize()
{
    if (!_nfc)
        return false;

    _nfc->begin();

    if (_nfc->getFirmwareVersion() == 0)
        return false;

    _nfc->SAMConfig();
    return true;
}

bool PN532Protocol::CardPresent()
{
    if (!_nfc)
        return false;

    _uidLength = 0;

    return _nfc->readPassiveTargetID(
        PN532_MIFARE_ISO14443A,
        _uid,
        &_uidLength,
        50);
}

bool PN532Protocol::ReadPage(uint8_t page, uint8_t data[4])
{
    if (!_nfc || !data)
        return false;

    uint8_t buffer[16] = {};

    if (!_nfc->ntag2xx_ReadPage(page, buffer))
        return false;

    memcpy(data, buffer, 4);
    return true;
}

bool PN532Protocol::WritePage(uint8_t page, const uint8_t data[4])
{
    if (!_nfc || !data)
        return false;

    uint8_t writable[4];
    memcpy(writable, data, sizeof(writable));

    return _nfc->ntag2xx_WritePage(page, writable);
}

bool PN532Protocol::ReadSignature()
{
    uint8_t page[4] = {};

    if (!ReadPage(SignaturePage, page))
        return false;

    return memcmp(page, Signature, 4) == 0;
}

bool PN532Protocol::WriteSignature()
{
    const uint8_t page[4] = {
        static_cast<uint8_t>(Signature[0]),
        static_cast<uint8_t>(Signature[1]),
        static_cast<uint8_t>(Signature[2]),
        static_cast<uint8_t>(Signature[3])
    };

    return WritePage(SignaturePage, page);
}

bool PN532Protocol::WritePages(
    const uint8_t* data,
    uint16_t length)
{
    if (!_nfc || !data || length > UserMemorySize)
        return false;

    uint8_t page = FirstUserPage;
    uint16_t index = 0;

    while (index < length)
    {
        if (page > LastUserPage)
            return false;

        uint8_t block[4] = {0, 0, 0, 0};

        for (uint8_t i = 0; i < 4 && index < length; ++i)
            block[i] = data[index++];

        if (!WritePage(page, block))
            return false;

        ++page;
    }

    return true;
}

bool PN532Protocol::ReadPages(
    uint8_t* buffer,
    uint16_t& length)
{
    length = 0;

    if (!_nfc || !buffer)
        return false;

    for (uint8_t page = FirstUserPage;
         page <= LastUserPage;
         ++page)
    {
        uint8_t data[4] = {};

        if (!ReadPage(page, data))
            return false;

        for (uint8_t i = 0; i < 4; ++i)
        {
            if (data[i] == '\0')
                return true;

            if (length >= UserMemorySize)
                return false;

            buffer[length++] = data[i];
        }
    }

    return true;
}

bool PN532Protocol::ClearUserMemory()
{
    if (!_nfc)
        return false;

    const uint8_t blank[4] = {0, 0, 0, 0};

    for (uint8_t page = FirstUserPage;
         page <= LastUserPage;
         ++page)
    {
        if (!WritePage(page, blank))
            return false;
    }

    return true;
}

bool PN532Protocol::WriteCode(const String& code)
{
    if (code.isEmpty() ||
        code.length() + 1 > UserMemorySize)
    {
        return false;
    }

    // "Home" is the application signature. This is deliberately NOT
    // the irreversible NTAG lock bits: the card remains writable.
    if (!ReadSignature())
    {
        if (!ClearUserMemory())
            return false;

        if (!WriteSignature())
            return false;
    }

    if (!ClearUserMemory())
        return false;

    return WritePages(
        reinterpret_cast<const uint8_t*>(code.c_str()),
        code.length() + 1);
}

bool PN532Protocol::ReadCode(String& code)
{
    // Arbitrary NFC cards are rejected before their application data
    // is processed.
    if (!ReadSignature())
        return false;

    uint8_t buffer[UserMemorySize] = {};
    uint16_t length = 0;

    if (!ReadPages(buffer, length))
        return false;

    code = String();

    for (uint16_t i = 0; i < length; ++i)
        code += static_cast<char>(buffer[i]);

    return !code.isEmpty();
}
