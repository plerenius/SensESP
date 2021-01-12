#include "sensors/dhtxx.h"

#include "sensesp.h"
#include <RemoteDebug.h>


// DHTxx represents a DHT temperature & pressure sensor.
DHTxx::DHTxx(uint8_t pin, uint8_t type, String config_path) {
    pDHT_ = new DHT(pin, type);
    pDHT_->begin();
}

// TODO(Petter L): implement check_status.
void DHTxx::check_status() {
  return;
}

// DHTvalue reads and outputs the specified type of value of a DHTxx sensor
DHTValue::DHTValue(DHTxx* pDHTxx, DHTValType val_type,
                   uint read_delay, String config_path) :
                   NumericSensor(config_path), pDHTxx_{pDHTxx},
                   val_type_{val_type}, read_delay_{read_delay} {
      load_configuration();
}

// DHT outputs temp in Celsius. Need to convert to Kelvin
// before sending to Signal K.
void DHTValue::enable() {
  app.onRepeat(read_delay_, [this](){
      if (val_type_ == temperature) {
        // Kelvin is Celsius + 273.15
        output = pDHTxx_->pDHT_->readTemperature() + 273.15;
      } else if (val_type_ == humidity) {
        output = pDHTxx_->pDHT_->readHumidity();
      } else {
        output = 0.0;
      }

      notify();
  });
}

_VOID DHTValue::get_configuration(JsonObject& root) {
  root["read_delay"] = read_delay_;
}

static const char SCHEMA[] PROGMEM = R"###({
  "type": "object",
  "properties": {
      "read_delay": { "title": "Read delay", "type": "number", "description": "The time, in milliseconds, between each read of the input" }
  }
})###";


String DHTValue::get_config_schema() {
  return FPSTR(SCHEMA);
}

bool DHTValue::set_configuration(const JsonObject& config) {
  String expected[] = {"read_delay"};
  for (auto str : expected) {
    if (!config.containsKey(str)) {
      return false;
    }
  }
  read_delay_ = config["read_delay"];
  return true;
}
