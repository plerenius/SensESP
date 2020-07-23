#include "dhtxx.h"

#include "sensesp.h"
#include <RemoteDebug.h>


// DHTxx represents a DHT temperature & pressure sensor.
DHTxx::DHTxx(uint8_t pin, uint8_t type, String config_path) :
       Sensor(config_path) {
    load_configuration();
    pDHT = new DHT(pin, type);
    pDHT->begin();
}


// DHTvalue reads and outputs the specified type of value of a DHTxx sensor
DHTvalue::DHTvalue(DHTxx* pDHTxx, DHTValType val_type, uint read_delay, String config_path) :
                   NumericSensor(config_path), pDHTxx{pDHTxx}, val_type{val_type}, read_delay{read_delay} {
      load_configuration();
}

// DHT outputs temp in Celsius. Need to convert to Kelvin before sending to Signal K.
void DHTvalue::enable() {
  app.onRepeat(read_delay, [this](){ 
      if (val_type == temperature) {
          output = pDHTxx->pDHT->readTemperature() + 273.15; // Kelvin is Celsius + 273.15
      }
      else if (val_type == humidity) {
          output = pDHTxx->pDHT->readHumidity();
      }
      else output = 0.0;
      
      notify();
  });
}

_VOID DHTvalue::get_configuration(JsonObject& root) {
  root["read_delay"] = read_delay;
  root["value"] = output;
};

static const char SCHEMA[] PROGMEM = R"###({
  "type": "object",
  "properties": {
      "read_delay": { "title": "Read delay", "type": "number", "description": "The time, in milliseconds, between each read of the input" },
      "value": { "title": "Last value", "type" : "number", "readOnly": true }
  }
})###";


String DHTvalue::get_config_schema() {
  return FPSTR(SCHEMA);
}

bool DHTvalue::set_configuration(const JsonObject& config) {
  String expected[] = {"read_delay"};
  for (auto str : expected) {
    if (!config.containsKey(str)) {
      return false;
    }
  }
  read_delay = config["read_delay"];
  return true;
}
