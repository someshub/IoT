#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// BLE settings
BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristic = nullptr;
bool deviceConnected = false;
uint32_t value = 0;

// UUIDs for the service and characteristic
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// TFT display settings
#define TFT_CS     5
#define TFT_RST    4
#define TFT_DC     2
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// LED settings
#define LED_PIN 21

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Device connected");
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device disconnected");
    pServer->startAdvertising(); // Restart advertising
  }
};

void setup() {
  Serial.begin(115200);

  // Initialize BLE
  BLEDevice::init("ESP32_BLE");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting for a client connection to notify...");

  // Initialize TFT display
  tft.initR(INITR_BLACKTAB);  // Initialize using ST7735R chip, black tab
  tft.setRotation(1);         // Adjust rotation if needed (0, 1, 2, 3)
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);

  tft.setCursor(0, 0);
  tft.println("BLE Access Point");
  tft.println("Waiting for connection...");

  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  if (deviceConnected) {
    // Notify client with the current value
    pCharacteristic->setValue((uint8_t*)&value, 4);
    pCharacteristic->notify();
    value++;
    delay(1000); // Send notification every second

    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.println("BLE Access Point");
    tft.println("Device connected");
    tft.print("Value: ");
    tft.println(value);

    // Heartbeat-like LED blinking
    digitalWrite(LED_PIN, HIGH);
    delay(100); // LED on for 100ms
    digitalWrite(LED_PIN, LOW);
    delay(900); // LED off for 900ms
  } else {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.println("BLE Access Point");
    tft.println("Waiting for connection...");

    // Fast LED blinking
    digitalWrite(LED_PIN, HIGH);
    delay(100); // LED on for 100ms
    digitalWrite(LED_PIN, LOW);
    delay(100); // LED off for 100ms
  }
}
