#include <Arduino.h>
#include <Wire.h>

// CONFIGURATION

#include "Config/Config.h"

// PROVISIONING

#include "WiFi/Provisioning.h"

// MQTT

#include "MQTT/MQTT.h"

// MESSAGE ROUTER

#include "MQTT/MessageRouter.h"

// DEVICE MANAGERS

#include "Facilities/Devices.h"
#include "Facilities/Light.h"
#include "Facilities/Appliance.h"
#include "Facilities/Door.h"
#include "Facilities/Window.h"
#include "Facilities/Camera.h"
#include "Facilities/External.h"

// ACCESS

#include "Access/Access.h"

// FINGERPRINT

#include "Fingerprint/Fingerprint.h"

// NFID

#include "NFID/NFID.h"

// HEARTBEAT

#include "Managers/Heartbeat.h"

// SENSOR MONITORING

#include "Sensor/Sensor.h"


// PIN CONFIGURATION

constexpr uint8_t I2C_SDA = 21;
constexpr uint8_t I2C_SCL = 22;

constexpr uint8_t FINGERPRINT_RX = 16;
constexpr uint8_t FINGERPRINT_TX = 17;


// GLOBAL OBJECTS

ConfigurationManager configuration;

Provisioning* provisioning = nullptr;

MqttManager* mqtt = nullptr;

MessageRouter* messageRouter = nullptr;

Access* access = nullptr;

Fingerprint* fingerprint = nullptr;

NFID* nfid = nullptr;

Heartbeat* heartbeat = nullptr;

SensorMonitoring* sensorMonitoring = nullptr;


// DEVICE MANAGERS

LightManager* lights = nullptr;

ApplianceManager* appliances = nullptr;

DoorManager* doors = nullptr;

WindowManager* windows = nullptr;

CameraManager* cameras = nullptr;

ExternalInputManager* externals = nullptr;

DeviceManager* deviceManager = nullptr;


// FINGERPRINT SERIAL

HardwareSerial FingerprintSerial(2);


// FREE RTOS TASKS

TaskHandle_t NetworkTaskHandle = nullptr;

TaskHandle_t HardwareTaskHandle = nullptr;


// NETWORK TASK
// CORE 0

void NetworkTask(void* parameter)
{
    Serial.println(
        "[TASK] Network task started");

    Serial.print(
        "[TASK] Running on Core ");

    Serial.println(
        xPortGetCoreID());

    for(;;)
    {
        if(mqtt != nullptr)
        {
            mqtt->Loop();
        }

        if(heartbeat != nullptr)
        {
            heartbeat->Update();
        }

        vTaskDelay(
            pdMS_TO_TICKS(10));
    }
}


// HARDWARE TASK
// CORE 1

void HardwareTask(void* parameter)
{
    Serial.println(
        "[TASK] Hardware task started");

    Serial.print(
        "[TASK] Running on Core ");

    Serial.println(
        xPortGetCoreID());

    for(;;)
    {
        
        // FINGERPRINT
        

        if(fingerprint != nullptr)
        {
            fingerprint->Update();
        }

        
        // NFID / PN532

        if(nfid != nullptr)
        {
            nfid->Update();
        }
        
        // SENSOR MONITORING
        

        if(sensorMonitoring != nullptr)
        {
            sensorMonitoring->Update();
        }

        
        // ACCESS
        

        if(access != nullptr)
        {
            access->Update();
        }

        vTaskDelay(
            pdMS_TO_TICKS(10));
    }
}


// SETUP

void setup()
{
    
    // SERIAL
    

    Serial.begin(9600);

    delay(1000);

    Serial.println();
    Serial.println();

    Serial.println(
        "================================================");

    Serial.println(
        "              HOME SECURITY");

    Serial.println(
        "              ESP32 CONTROLLER");

    Serial.println(
        "================================================");


    Serial.println(
        "[BOOT] Initializing I2C...");

    Wire.begin(
        I2C_SDA,
        I2C_SCL);

    Wire.setClock(400000);

    Serial.println(
        "[BOOT] I2C initialized");

    Serial.println(
        "[BOOT] Initializing fingerprint serial...");

    FingerprintSerial.begin(
        57600,
        SERIAL_8N1,
        FINGERPRINT_RX,
        FINGERPRINT_TX);

    Serial.println(
        "[BOOT] Fingerprint serial initialized");


    Serial.println(
        "[BOOT] Loading configuration...");

    if(!configuration.Initialize())
    {
        Serial.println(
            "[BOOT] Configuration initialization FAILED");

        while(true)
        {
            delay(1000);
        }
    }

    Serial.println(
        "[BOOT] Configuration loaded");


    Serial.println(
        "[BOOT] Starting provisioning...");

    provisioning =
        new Provisioning(
            configuration);

    if(!provisioning->Begin())
    {
        Serial.println(
            "[BOOT] Provisioning initialization FAILED");

        while(true)
        {
            provisioning->Loop();

            delay(10);
        }
    }


    
    // PROVISIONING LOOP
    
    //
    // If provisioning is required, this keeps the
    // provisioning server alive until it finishes.
    //
    // If provisioning is already complete and Begin()
    // does not start the server, this exits immediately.
    //
    

    while(provisioning->IsRunning())
    {
        provisioning->Loop();

        delay(10);
    }

    Serial.println(
        "[BOOT] Provisioning complete");


    
    // CONFIGURATION AFTER PROVISIONING
    

    auto& config =
        configuration.GetConfiguration();

    Serial.print(
        "[BOOT] Hardware ID: ");

    Serial.println(
        config.HardwareId);

    Serial.print(
        "[BOOT] MQTT Host: ");

    Serial.println(
        config.MqttHost);

    Serial.print(
        "[BOOT] MQTT Port: ");

    Serial.println(
        config.MqttPort);


    
    // DEVICE MANAGERS
    

    Serial.println(
        "[BOOT] Creating device managers...");

    lights =
        new LightManager();

    appliances =
        new ApplianceManager();

    doors =
        new DoorManager();

    windows =
        new WindowManager();

    cameras =
        new CameraManager();

    externals =
        new ExternalInputManager();


    
    // DEVICE MANAGER
    

    deviceManager =
        new DeviceManager(
            *lights,
            *appliances,
            *doors,
            *windows,
            *cameras,
            *externals);


    
    // MQTT
    

    mqtt =
        new MqttManager(
            configuration);


    
    // ACCESS
    

    access =
        new Access(
            *deviceManager);


    
    // FINGERPRINT
    

    fingerprint =
        new Fingerprint(
            FingerprintSerial,
            *mqtt,
            configuration);


    
    // NFID
    

    nfid =
        new NFID(
            configuration,
            *mqtt,
            I2C_SDA,
            I2C_SCL);


    
    // HEARTBEAT
    

    heartbeat =
        new Heartbeat(
            configuration,
            *mqtt);


    
    // SENSOR MONITORING
    

    sensorMonitoring =
        new SensorMonitoring(
            configuration,
            *mqtt,
            *deviceManager);


    
    // MESSAGE ROUTER
    

    messageRouter =
        new MessageRouter(
            configuration,
            *deviceManager,
            *access,
            *fingerprint,
            *nfid);


    
    // CONNECT MQTT TO MESSAGE ROUTER
    

    mqtt->SetMessageRouter(
        *messageRouter);


    
    // INITIALIZE DEVICE MANAGERS
    

    Serial.println(
        "[BOOT] Initializing devices...");


    if(!lights->Initialize())
    {
        Serial.println(
            "[BOOT] Light initialization failed");
    }


    if(!appliances->Initialize())
    {
        Serial.println(
            "[BOOT] Appliance initialization failed");
    }


    if(!doors->Initialize())
    {
        Serial.println(
            "[BOOT] Door initialization failed");
    }


    if(!windows->Initialize())
    {
        Serial.println(
            "[BOOT] Window initialization failed");
    }


    if(!cameras->Initialize())
    {
        Serial.println(
            "[BOOT] Camera initialization failed");
    }


    if(!externals->Initialize())
    {
        Serial.println(
            "[BOOT] External input initialization failed");
    }


    
    // ACCESS
    

    if(!access->Initialize())
    {
        Serial.println(
            "[BOOT] Access initialization failed");
    }


    
    // FINGERPRINT
    

    if(!fingerprint->Initialize())
    {
        Serial.println(
            "[BOOT] Fingerprint initialization failed");
    }
    else
    {
        Serial.println(
            "[BOOT] Fingerprint initialized");
    }


    
    // NFID
    

    if(!nfid->Initialize())
    {
        Serial.println(
            "[BOOT] PN532/NFID initialization failed");
    }
    else
    {
        Serial.println(
            "[BOOT] PN532/NFID initialized");
    }


    
    // SENSOR MONITORING
    

    if(!sensorMonitoring->Initialize())
    {
        Serial.println(
            "[BOOT] Sensor monitoring initialization failed");
    }


    
    // HEARTBEAT
    

    if(!heartbeat->Initialize())
    {
        Serial.println(
            "[BOOT] Heartbeat initialization failed");
    }


    
    // MQTT
    

    Serial.println(
        "[BOOT] Connecting MQTT...");

    if(!mqtt->Connect())
    {
        Serial.println(
            "[BOOT] MQTT connection failed");

        Serial.println(
            "[BOOT] MQTT will retry automatically");
    }
    else
    {
        Serial.println(
            "[BOOT] MQTT connected");
    }


    
    // CORE 0
    // NETWORK TASK
    

    Serial.println(
        "[BOOT] Starting Core 0 network task...");

    BaseType_t networkResult =
        xTaskCreatePinnedToCore(
            NetworkTask,
            "NetworkTask",
            8192,
            nullptr,
            3,
            &NetworkTaskHandle,
            0);

    if(networkResult != pdPASS)
    {
        Serial.println(
            "[BOOT] NetworkTask creation FAILED");
    }
    else
    {
        Serial.println(
            "[BOOT] NetworkTask running on Core 0");
    }


    
    // CORE 1
    // HARDWARE TASK
    

    Serial.println(
        "[BOOT] Starting Core 1 hardware task...");

    BaseType_t hardwareResult =
        xTaskCreatePinnedToCore(
            HardwareTask,
            "HardwareTask",
            8192,
            nullptr,
            2,
            &HardwareTaskHandle,
            1);

    if(hardwareResult != pdPASS)
    {
        Serial.println(
            "[BOOT] HardwareTask creation FAILED");
    }
    else
    {
        Serial.println(
            "[BOOT] HardwareTask running on Core 1");
    }


    
    // BOOT COMPLETE
    

    Serial.println();

    Serial.println(
        "================================================");

    Serial.println(
        "          HOME SECURITY STARTED");

    Serial.println(
        "================================================");

    Serial.println(
        "[CORE 0] MQTT / Network / Heartbeat");

    Serial.println(
        "[CORE 1] Hardware / Fingerprint / NFID / Access");

    Serial.print(
        "[BOOT] Hardware ID: ");

    Serial.println(
        config.HardwareId);

    Serial.println(
        "================================================");
}


// LOOP

void loop()
{
    /*
        Continuous application processing is handled
        by the FreeRTOS tasks.

        Core 0:
            MQTT
            Heartbeat

        Core 1:
            Fingerprint
            NFID
            Sensors
            Access
    */

    vTaskDelay(
        pdMS_TO_TICKS(1000));
}