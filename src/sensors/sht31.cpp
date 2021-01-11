#include "sht31.h"

#include <RemoteDebug.h>

#include "sensesp.h"

// SHT31 represents an ADAfruit (or compatible) SHT31 temperature & humidity
// sensor.
SHT31::SHT31(uint8_t addr) {
  adafruit_sht31_ = new Adafruit_SHT31();
  if (!adafruit_sht31_->begin(addr)) {
    debugE("Could not find a valid SHT31 sensor: check address and wiring");
  }
}

// SHT31Value reads and outputs the specified type of value of a SHT31 sensor
SHT31Value::SHT31Value(SHT31* sht31, SHT31ValType val_type, uint read_delay,
                       String config_path)
    : NumericSensor(config_path),
      sht31_{sht31},
      val_type_{val_type},
      read_delay_{read_delay} {
  load_configuration();
}

// SHT31 outputs temp in Celsius. Need to convert to Kelvin before sending to
// Signal K. Humidity is output in relative humidity (0 - 100%)
void SHT31Value::enable() {
  app.onRepeat(read_delay_, [this]() {
    if (val_type_ == temperature) {
      output = sht31_->adafruit_sht31_->readTemperature() +
               273.15;  // Kelvin is Celsius + 273.15
    } else if (val_type_ == humidity) {
      output = sht31_->adafruit_sht31_->readHumidity();
    } else {
      output = 0.0;
    }

    notify();
  });
}

void SHT31Value::get_configuration(JsonObject& root) {
  root["read_delay"] = read_delay_;
};

static const char SCHEMA[] PROGMEM = R"###({
    "type": "object",
    "properties": {
        "read_delay": { "title": "Read delay", "type": "number", "description": "The time, in milliseconds, between each read of the input" }
    }
  })###";

String SHT31Value::get_config_schema() { return FPSTR(SCHEMA); }

bool SHT31Value::set_configuration(const JsonObject& config) {
  String expected[] = {"read_delay"};
  for (auto str : expected) {
    if (!config.containsKey(str)) {
      return false;
    }
  }
  read_delay_ = config["read_delay"];
  return true;
}
