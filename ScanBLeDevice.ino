#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// BLE scan settings
int scanTime = 5;  // In seconds
BLEScan *pBLEScan;

// TFT display settings
#define TFT_CS     5
#define TFT_RST    4
#define TFT_DC     2
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");

  // Initialize BLE
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();  // create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);  // active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value

  // Initialize TFT display
  tft.initR(INITR_BLACKTAB);  // Initialize using ST7735R chip, black tab
  tft.setRotation(1);         // Adjust rotation if needed (0, 1, 2, 3)
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
}

void loop() {
  BLEScanResults *foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices->getCount());
  Serial.println("Scan done!");

  // Display devices on TFT
  displayDevices(foundDevices);

  pBLEScan->clearResults();  // delete results from BLEScan buffer to release memory
  delay(2000);
}

void displayDevices(BLEScanResults *foundDevices) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(1);

  int y = 0;
  for (int i = 0; i < foundDevices->getCount(); i++) {
    BLEAdvertisedDevice device = foundDevices->getDevice(i);
    String deviceStr = device.toString().c_str();

    // Truncate string if it's too long for the display
    if (deviceStr.length() > 20) {
      deviceStr = deviceStr.substring(0, 20) + "*";
    }

    // Print each device in a new line
    tft.setCursor(0, y);
    tft.println(deviceStr);

    y += 10;  // Move to next line, adjust based on font size
    if (y > tft.height() - 10) {
      break;  // Prevent overflow if too many devices
    }
  }
}
