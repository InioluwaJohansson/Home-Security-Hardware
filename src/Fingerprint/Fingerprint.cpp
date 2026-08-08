#include "Fingerprint.h"

Fingerprint::Fingerprint(HardwareSerial& serial, MqttManager& mqtt, ConfigurationManager& configuration)
    : _serial(serial), _mqtt(mqtt), _configuration(configuration), _protocol(serial) {}

bool Fingerprint::Initialize() {
    _context = FingerprintContext{};
    return _protocol.Initialize();
}

void Fingerprint::Update() {
    switch (_context.Operation) {
        case FingerprintOperation::Registration: CaptureFingerprint(); break;
        case FingerprintOperation::Verification: VerifyFingerprint(); break;
        default: break;
    }
}

void Fingerprint::RequestCapture(int personId) {
    if (personId < 0) { SetFailed(); return; }
    _context.Operation = FingerprintOperation::Registration;
    _context.State = FingerprintState::WaitingForFinger;
    _context.PersonId = personId;
    _context.StartedAt = millis();
}

void Fingerprint::RequestVerification() {
    _context.Operation = FingerprintOperation::Verification;
    _context.State = FingerprintState::WaitingForFinger;
    _context.PersonId = -1;
    _context.StartedAt = millis();
}

bool Fingerprint::CaptureFingerprint() {
    if (!_protocol.IsFingerDetected()) return true;
    _context.State = FingerprintState::Capturing;
    if (!_protocol.CaptureImage()) { SetFailed(); return false; }
    _context.State = FingerprintState::Uploading;
    uint8_t* image = nullptr; uint32_t length = 0;
    if (!_protocol.UploadImage(image, length)) { _protocol.Cancel(); SetFailed(); return false; }
    (void)image; (void)length;
    _context.State = FingerprintState::Success;
    _context.Operation = FingerprintOperation::None;
    _context.StartedAt = 0;
    _protocol.Cancel();
    return true;
}

bool Fingerprint::VerifyFingerprint() {
    if (!_protocol.IsFingerDetected()) return true;
    _context.State = FingerprintState::Capturing;
    if (!_protocol.CaptureImage()) { SetFailed(); return false; }
    uint8_t* image = nullptr; uint32_t length = 0;
    if (!_protocol.UploadImage(image, length)) { _protocol.Cancel(); SetFailed(); return false; }
    (void)image; (void)length;
    _context.State = FingerprintState::Success;
    _context.Operation = FingerprintOperation::None;
    _context.StartedAt = 0;
    _protocol.Cancel();
    return true;
}

void Fingerprint::SetFailed() {
    _protocol.Cancel();
    _context.State = FingerprintState::Failed;
    _context.Operation = FingerprintOperation::None;
    _context.StartedAt = 0;
}
