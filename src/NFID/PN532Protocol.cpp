#include "PN532Protocol.h"

PN532Protocol::PN532Protocol(
    uint8_t sda,
    uint8_t scl)
    : _wire(0)
{
    _wire.begin(sda, scl);

    _nfc = new Adafruit_PN532(_wire);
}

bool PN532Protocol::Initialize()
{
    if(_nfc == nullptr)
        return false;

    _nfc->begin();

    uint32_t version =
        _nfc->getFirmwareVersion();

    if(version == 0)
        return false;

    _nfc->SAMConfig();

    return true;
}
bool PN532Protocol::CardPresent()
{
    return _nfc->readPassiveTargetID(
        PN532_MIFARE_ISO14443A,
        _uid,
        &_uidLength,
        50);
}
bool PN532Protocol::WritePages(
    const uint8_t* data,
    uint16_t length)
{
    uint8_t page = FirstUserPage;

    uint16_t index = 0;

    while(index < length)
    {
        if(page > LastUserPage)
            return false;

        uint8_t block[4] =
        {
            0,
            0,
            0,
            0
        };

        for(uint8_t i = 0;
            i < 4 && index < length;
            i++)
        {
            block[i] = data[index++];
        }

        if(!_nfc->ntag2xx_WritePage(
                page,
                block))
        {
            return false;
        }

        page++;
    }

    return true;
}
bool PN532Protocol::ReadPages(
    uint8_t* buffer,
    uint16_t& length)
{
    length = 0;

    for(uint8_t page = FirstUserPage;
        page <= LastUserPage;
        page += 4)
    {
        uint8_t data[16];

        if(!_nfc->ntag2xx_ReadPage(page, data))
        {
            return false;
        }

        for(uint8_t i = 0; i < 16; i++)
        {
            if(data[i] == '\0')
            {
                return true;
            }

            buffer[length++] = data[i];
        }
    }

    return true;
}
bool PN532Protocol::WriteCode(
    const String& code)
{

    if(!ClearUserMemory())
        return false;

    return WritePages(
        reinterpret_cast<const uint8_t*>(code.c_str()),
        code.length() + 1);
}
bool PN532Protocol::ReadCode(
    String& code)
{

    static constexpr uint16_t UserMemorySize =
    (LastUserPage - FirstUserPage + 1) * 4;

    uint16_t length = 0;

    if(!ReadPages(
            buffer,
            length))
    {
        return false;
    }

    code = "";

    for(uint16_t i = 0; i < length; i++)
    {
        code += (char)buffer[i];
    }

    return true;
}
bool PN532Protocol::ClearUserMemory()
{
    uint8_t blank[4] = {0, 0, 0, 0};

    for(uint8_t page = FirstUserPage;
        page <= LastUserPage;
        page++)
    {
        if(!_nfc->ntag2xx_WritePage(page, blank))
        {
            return false;
        }
    }

    return true;
}