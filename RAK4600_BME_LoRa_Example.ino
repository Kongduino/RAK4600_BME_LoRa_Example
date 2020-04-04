#include "Seeed_BME280.h"
#include <Wire.h>
#include <RadioLib.h>

// SX1278 has the following connections:
// NSS pin:   10
// DIO0 pin:  2
// RESET pin: 9
// DIO1 pin:  3
SX1276 lora = new Module(4, 28, 3, 31);

BME280 bme280;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  while (!Serial);
  while (Serial.available()) char c = Serial.read();
  delay(1000);
  Serial.print(F("[SX1276] Initializing ... "));
  // carrier frequency:           434.0 MHz
  // bandwidth:                   125.0 kHz
  // spreading factor:            9
  // coding rate:                 7
  // sync word:                   0x12
  // output power:                17 dBm
  // current limit:               100 mA
  // preamble length:             8 symbols
  // amplifier gain:              0 (automatic gain control)
  int state = lora.begin(869.525, 125.0, 9, 5, SX127X_REG_SYNC_WORD, 20, 80, 8, 0);
  // int16_t SX1276::begin(float freq, float bw, uint8_t sf, uint8_t cr, uint8_t syncWord, int8_t power, uint8_t currentLimit, uint16_t preambleLength, uint8_t gain) {
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
  Serial.println("BME280 test");
  if (!bme280.init()) {
    Serial.println("Device error!");
    while (1);
  }
  Serial.println("End of setup\n\n");
}

void loop() {
  float temp = bme280.getTemperature(), pressure = bme280.getPressure(), humidity = bme280.getHumidity(), alt = bme280.calcAltitude(pressure);
  Serial.print("[BME] Temp: ");
  Serial.print(temp);
  Serial.println("C");
  Serial.print("[BME] Pressure: ");
  Serial.print((pressure = pressure / 100));
  Serial.println(" HPa");
  Serial.print("[BME] Altitude: ");
  Serial.print(alt);
  Serial.println(" m");
  Serial.print("[BME] Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  char packet[256];
  memset(packet, 0, 256);
  String pp = "[BME] Temp: " + String(temp) + " *C. Pressure: " + \
              String(pressure) + " HPa. Altitude: " + String(alt) + \
              " m. Humidity: " + String(humidity) + "%";
  pp.toCharArray(packet, pp.length() + 1);
  int state = lora.transmit(packet);
  delay(3000);
}
