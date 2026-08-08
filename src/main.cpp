#include <Arduino.h>
#include <Wire.h>

#include "Config/Config.h"
#include "WiFi/Provisioning.h"
#include "MQTT/MQTT.h"
#include "MQTT/MessageRouter.h"

#include "Hardware/Pins.h"
#include "Hardware/Hardware.h"
#include "Hardware/HardwareController.h"
#include "Hardware/MCP23017.h"
#include "Hardware/PCA9685.h"
#include "Hardware/Multiplexer.h"
#include "Hardware/ADS1115.h"

#include "Facilities/Devices.h"
#include "Facilities/Light.h"
#include "Facilities/Appliance.h"
#include "Facilities/Door.h"
#include "Facilities/Window.h"
#include "Facilities/Camera.h"
#include "Facilities/External.h"
#include "Facilities/StateCache.h"

#include "Access/Access.h"
#include "Fingerprint/Fingerprint.h"
#include "NFID/NFID.h"
#include "Managers/Heartbeat.h"
#include "Sensor/Sensor.h"

ConfigurationManager configuration;

Provisioning* provisioning = nullptr;
MqttManager* mqtt = nullptr;
MessageRouter* messageRouter = nullptr;

Access* access = nullptr;
Fingerprint* fingerprint = nullptr;
NFID* nfid = nullptr;
Heartbeat* heartbeat = nullptr;
SensorController* sensorController = nullptr;

LightManager* lights = nullptr;
ApplianceManager* appliances = nullptr;
DoorManager* doors = nullptr;
WindowManager* windows = nullptr;
CameraManager* cameras = nullptr;
ExternalManager* externals = nullptr;
DeviceManager* deviceManager = nullptr;

StateCache stateCache;

HardwareManager hardwareManager;
MCP23017Controller mcp;
PCA9685Controller pwm;
MultiplexerController multiplexer;
ADS1115Controller adc;
HardwareController* hardware = nullptr;

HardwareSerial FingerprintSerial(2);

TaskHandle_t NetworkTaskHandle = nullptr;
TaskHandle_t HardwareTaskHandle = nullptr;

void NetworkTask(void*)
{
    for (;;)
    {
        if (mqtt)
            mqtt->Loop();

        if (heartbeat)
            heartbeat->Update();

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void HardwareTask(void*)
{
    for (;;)
    {
        if (hardware)
            hardware->Update();

        if (fingerprint)
            fingerprint->Update();

        if (nfid)
            nfid->Update();

        if (sensorController)
            sensorController->Update();

        if (access)
            access->Update();

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static bool InitializeApplication()
{
    hardwareManager.Initialize();

    hardware = new HardwareController(
        hardwareManager,
        mcp,
        pwm,
        multiplexer,
        adc);

    hardware->Initialize();

    lights = new LightManager(*hardware);
    appliances = new ApplianceManager(*hardware);
    doors = new DoorManager(*hardware);
    windows = new WindowManager(*hardware);
    cameras = new CameraManager(*hardware);
    externals = new ExternalManager(*hardware);

    lights->Initialize();
    appliances->Initialize();
    doors->Initialize();
    windows->Initialize();
    cameras->Initialize();
    externals->Initialize();

    deviceManager = new DeviceManager(
        *lights,
        *appliances,
        *doors,
        *windows,
        *cameras,
        *externals);

    access = new Access(*doors);

    fingerprint = new Fingerprint(
        FingerprintSerial,
        *mqtt,
        configuration);

    nfid = new NFID(
        configuration,
        *mqtt,
        Wire);

    heartbeat = new Heartbeat(
        configuration,
        *mqtt);

    sensorController = new SensorController(
        *hardware,
        configuration,
        *deviceManager,
        stateCache,
        *mqtt);

    messageRouter = new MessageRouter(
        configuration,
        *deviceManager,
        *access,
        *fingerprint,
        *nfid);

    mqtt->SetMessageRouter(*messageRouter);

    if (!access->Initialize())
        return false;

    if (!fingerprint->Initialize())
        Serial.println("[BOOT] Fingerprint initialization failed");

    if (!nfid->Initialize())
        Serial.println("[BOOT] PN532/NFID initialization failed");

    sensorController->Initialize();

    return true;
}

void setup()
{
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println("==============================================");
    Serial.println("        HOME SECURITY ESP32 CONTROLLER");
    Serial.println("==============================================");

    if (!configuration.Initialize())
    {
        Serial.println("[BOOT] Configuration initialization FAILED");
        while (true)
            delay(1000);
    }

    provisioning = new Provisioning(configuration);

    if (!provisioning->Begin())
    {
        Serial.println("[BOOT] Provisioning initialization FAILED");
        while (true)
        {
            provisioning->Loop();
            delay(10);
        }
    }

    while (provisioning->IsRunning())
    {
        provisioning->Loop();
        delay(10);
    }

    Wire.begin(Pins::I2C_SDA, Pins::I2C_SCL);
    Wire.setClock(400000);

    FingerprintSerial.begin(
        57600,
        SERIAL_8N1,
        Pins::FP_RX,
        Pins::FP_TX);

    mqtt = new MqttManager(configuration);

    if (!InitializeApplication())
        Serial.println("[BOOT] Application initialization failed");

    if (!mqtt->Connect())
        Serial.println("[BOOT] MQTT connection failed; retrying automatically");

    xTaskCreatePinnedToCore(
        NetworkTask,
        "NetworkTask",
        8192,
        nullptr,
        3,
        &NetworkTaskHandle,
        0);

    xTaskCreatePinnedToCore(
        HardwareTask,
        "HardwareTask",
        8192,
        nullptr,
        3,
        &HardwareTaskHandle,
        1);

    Serial.println("[BOOT] Home Security controller started");
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}
