#include "ultrasonic_distance.h"

#include "Arduino.h"
#include "sensesp.h"

UltrasonicDistance::UltrasonicDistance(int8_t trig_pin, int8_t input_pin,
                                       uint read_delay, String config_path)
    : NumericSensor(config_path),
      trigger_pin_{trig_pin},
      input_pin_{input_pin},
      read_delay_{read_delay} {
  pinMode(trig_pin, OUTPUT);
  pinMode(input_pin, INPUT_PULLUP);
  load_configuration();
}

void UltrasonicDistance::enable() {
  app.onRepeat(read_delay_, [this]() {
    digitalWrite(trigger_pin_, HIGH);
    long last_time = micros();
    while (micros() - last_time < 100) {
      yield();
    }
    digitalWrite(trigger_pin_, LOW);
    this->emit(pulseIn(input_pin_, HIGH, 50000));  // 50 microsecond timeout
  });
}

void UltrasonicDistance::get_configuration(JsonObject& root) {
  root["read_delay"] = read_delay_;
};

static const char SCHEMA[] PROGMEM = R"###({
    "type": "object",
    "properties": {
        "read_delay": { "title": "Read delay", "type": "number", "description": "Number of milliseconds between each thermocouple read " }
    }
  })###";

String UltrasonicDistance::get_config_schema() { return FPSTR(SCHEMA); }

bool UltrasonicDistance::set_configuration(const JsonObject& config) {
  String expected[] = {"read_delay"};
  for (auto str : expected) {
    if (!config.containsKey(str)) {
      return false;
    }
  }
  read_delay_ = config["read_delay"];
  return true;
}
