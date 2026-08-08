#include "R50xProtocol.h"
#include "R50xPacket.h"

R50xProtocol::R50xProtocol(HardwareSerial& serial) : _serial(serial) {}
R50xProtocol::~R50xProtocol() { Cancel(); }

const R50xSystemParameters& R50xProtocol::GetSystemParameters() const { return _systemParameters; }

bool R50xProtocol::Initialize() {
    _serial.begin(57600, SERIAL_8N1);
    while (_serial.available()) _serial.read();
    if (!VerifyPassword()) return false;
    return ReadSystemParameters();
}

bool R50xProtocol::VerifyPassword() {
    R50xPacket packet;
    packet.SetInstruction(R50x::Instruction::VerifyPassword);
    const uint8_t password[4] = {0,0,0,0};
    packet.SetPayload(password, sizeof(password));
    return packet.Send(_serial) &&
           packet.Receive(_serial) &&
           packet.IsAckPacket() &&
           packet.ConfirmationCode() == R50x::Confirmation::Success;
}

bool R50xProtocol::ReadSystemParameters() {
    R50xPacket packet;
    packet.SetInstruction(R50x::Instruction::ReadSysPara);
    if (!packet.Send(_serial) || !packet.Receive(_serial) ||
        !packet.IsAckPacket() || packet.ConfirmationCode() != R50x::Confirmation::Success)
        return false;
    if (packet.PayloadLength() < 17) return false;
    const uint8_t* p = packet.Payload();
    _systemParameters.StatusRegister = (uint16_t(p[1])<<8)|p[2];
    _systemParameters.SystemIdentifier = (uint16_t(p[3])<<8)|p[4];
    _systemParameters.LibraryCapacity = (uint16_t(p[5])<<8)|p[6];
    _systemParameters.SecurityLevel = (uint16_t(p[7])<<8)|p[8];
    _systemParameters.DeviceAddress = (uint32_t(p[9])<<24)|(uint32_t(p[10])<<16)|(uint32_t(p[11])<<8)|p[12];
    _systemParameters.PacketLengthCode = (uint16_t(p[13])<<8)|p[14];
    switch (_systemParameters.PacketLengthCode) {
        case 0: _systemParameters.PacketLength=32; break;
        case 1: _systemParameters.PacketLength=64; break;
        case 2: _systemParameters.PacketLength=128; break;
        case 3: _systemParameters.PacketLength=256; break;
        default: _systemParameters.PacketLength=128; break;
    }
    const uint16_t baudMultiplier=(uint16_t(p[15])<<8)|p[16];
    _systemParameters.BaudRate=uint32_t(baudMultiplier)*9600UL;
    return true;
}

bool R50xProtocol::GenerateImage() {
    R50xPacket packet; packet.SetInstruction(R50x::Instruction::GenImg);
    return packet.Send(_serial) && packet.Receive(_serial) && packet.IsAckPacket() &&
           packet.ConfirmationCode() == R50x::Confirmation::Success;
}

bool R50xProtocol::IsFingerDetected() {
    R50xPacket packet; packet.SetInstruction(R50x::Instruction::GenImg);
    if (!packet.Send(_serial) || !packet.Receive(_serial) || !packet.IsAckPacket()) return false;
    return packet.ConfirmationCode() == R50x::Confirmation::Success;
}

bool R50xProtocol::CaptureImage() { return GenerateImage(); }

bool R50xProtocol::UploadImage(uint8_t*& image, uint32_t& length) {
    image=nullptr; length=0;
    R50xPacket packet; packet.SetInstruction(R50x::Instruction::UpImage);
    if (!packet.Send(_serial) || !packet.Receive(_serial) || !packet.IsAckPacket() ||
        packet.ConfirmationCode()!=R50x::Confirmation::Success) return false;
    return ReceiveImagePackets(image,length);
}

bool R50xProtocol::ReceiveImagePackets(uint8_t*& image, uint32_t& imageLength) {
    Cancel();
    _imageBuffer=new uint8_t[MaxImageSize];
    if (!_imageBuffer) return false;
    imageLength=0;
    for (;;) {
        R50xPacket packet;
        if (!packet.Receive(_serial,5000)) { Cancel(); return false; }
        if (!packet.IsDataPacket() && !packet.IsEndDataPacket()) { Cancel(); return false; }
        uint16_t n=packet.PayloadLength();
        if (imageLength+n>MaxImageSize) { Cancel(); return false; }
        memcpy(_imageBuffer+imageLength,packet.Payload(),n);
        imageLength+=n;
        if (packet.IsEndDataPacket()) break;
    }
    _imageLength=imageLength; image=_imageBuffer; return true;
}

void R50xProtocol::Cancel() {
    if (_imageBuffer) { delete[] _imageBuffer; _imageBuffer=nullptr; }
    _imageLength=0;
}
