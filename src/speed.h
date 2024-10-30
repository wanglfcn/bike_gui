#include <Arduino.h>
#include <QMC5883LCompass.h>
#include <Adafruit_INA219.h>
#include <Preferences.h>
#include "ui/ui.h"

#define SPEED_PIN 38

QMC5883LCompass compass;
Adafruit_INA219 ina219;

unsigned long sensorLastUpdateTs = 0;
unsigned long counterLastUpdateTs = 0;
unsigned long speedLastUpdateTs = 0;
unsigned long mileageLastUpdateTs = 0;

long speedCounter = 0;
long totalCount = 0;
long totalDistance = 0;

Preferences prefs;

void IRAM_ATTR speedInterruptFunc();


void scanI2c() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    } else {

      Serial.print("Unknown error at address 0x");
      Serial.print(address,HEX);
      Serial.print("with code");
      Serial.println(error);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000);           // wait 5 seconds for next scan
}

void initSensor()
{
    prefs.begin("bike_gui");
    mileageLastUpdateTs = millis();
    totalDistance = prefs.getLong64("mileage", 0);

    bool result = Wire.setPins(1, 2);

    result = Wire.begin(1, 2, 0);

    Serial.print("begin i2c pins ");
    Serial.println(result);
    compass.init();
    // Try to initialize the INA219
    if (!ina219.begin())
    {
        Serial.println("Failed to find INA219 chip");
    }

    pinMode(SPEED_PIN, INPUT_PULLUP);                                              // Configure the pin as an input with an internal pull-up resistor
    attachInterrupt(digitalPinToInterrupt(SPEED_PIN), speedInterruptFunc, RISING); // Configure the interrupt
}

void IRAM_ATTR speedInterruptFunc()
{
    // Here you can add the code that will be executed when the interrupt occurs
    speedLastUpdateTs = millis();
    speedCounter++;
}

void updateTemperature()
{
}

void updateCompassDirection()
{

    compass.read();

    int azimuth = compass.getAzimuth();
    int x = compass.getX();

    // update compass offset
    int offset = (int) ((azimuth - 19.83 + 180) * 590/360) - 305;
    lv_obj_set_x(ui_compass, offset);

    Serial.print("compass direction: ");
    Serial.println(azimuth);
}

void updateBatteryLevel()
{
    // Read voltage and current from INA219.
    float shuntvoltage = ina219.getShuntVoltage_mV();
    float busvoltage = ina219.getBusVoltage_V();

    // Compute load voltage, power, and milliamp-hours.
    float loadvoltage = busvoltage + (shuntvoltage / 1000);
    // range 7.4-8.4?
    float minV = 7.4;
    float maxV = 8.4;
    int percentage = (int)((100 * (loadvoltage - minV)) / (maxV - minV));
    // update battery
    if (percentage > 100)  {
        percentage = 100;
    }
    Serial.print("battery voltage: ");
    Serial.println(loadvoltage);

    char data[20];
    sprintf(data, "%d%%", percentage);
    lv_label_set_text(ui_batteryLevel, data);

    lv_bar_set_value(ui_batteryInd, percentage, LV_ANIM_OFF);
    if (percentage < 25) {
        // red
        lv_obj_set_style_bg_color(ui_batteryInd, lv_color_hex(0xFF0000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    } else if (percentage < 50) {
        // yellow
        lv_obj_set_style_bg_color(ui_batteryInd, lv_color_hex(0xFFFF00), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    } else {
        lv_obj_set_style_bg_color(ui_batteryInd, lv_color_hex(0x008000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }
}

void updateSpeed(unsigned long now)
{
    int speed = 0;
    int distance = 0;

    int count = speedCounter;
    totalCount += count;
    speedCounter = 0;
    // speed is zero when no update after 10s
    if (now - counterLastUpdateTs < 10000 && count > 0)
    {
        unsigned long timeDiff = now - speedLastUpdateTs;
        speed = (int)((count * 1000 * 1.31) / (12 * timeDiff));
    }
    distance = (int)((totalCount * 1.31) / 12);

    speedLastUpdateTs = now;
}

void saveTotalDistance(unsigned long now)
{
    if (now - mileageLastUpdateTs > 60000)
    {
        long newTotalDistance = totalDistance + (int)((totalCount * 1.31) / 12);
        prefs.putLong64("mileage", newTotalDistance);
        mileageLastUpdateTs = now;
    }
}

void updateSensor(unsigned long now)
{
    updateSpeed(now);
    saveTotalDistance(now);
    if (now - sensorLastUpdateTs < 200)
    {
        return;
    }

    sensorLastUpdateTs = now;
    updateBatteryLevel();
    updateCompassDirection();
    updateTemperature();
}