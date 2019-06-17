#include "angle_correction.h"

AngleCorrection::AngleCorrection(float offset, float min_angle, String config_path) :
    NumericTransform(config_path),
      offset{ offset }, min_angle{ min_angle } {
  load_configuration();
}


void AngleCorrection::set_input(float input, uint8_t inputChannel) {
  // first the correction
  float x = input + offset;

  // then wrap around the values
  x = fmod(x - min_angle, 2 * M_PI);
  if (x < 0)
      x += 2 * M_PI;
  output = x + min_angle;

  notify();
}


JsonObject& AngleCorrection::get_configuration(JsonBuffer& buf) {
  JsonObject& root = buf.createObject();
  root["offset"] = offset;
  root["min_angle"] = min_angle;
  root["value"] = output;
  return root;
}

static const char SCHEMA[] PROGMEM = R"###({
    "type": "object",
    "properties": {
        "offset": { "title": "Constant offset (radians)", "type": "number" },
        "min_angle": { "title": "Minimum angle value (radians, typically 0 or -3.15159265", "type": "number" },
        "value": { "title": "Last value", "type" : "number", "readOnly": true }
    }
  })###";

String AngleCorrection::get_config_schema() {
  return FPSTR(SCHEMA);
}

bool AngleCorrection::set_configuration(const JsonObject& config) {
  String expected[] = { "offset", "min_angle" };
  for (auto str : expected) {
    if (!config.containsKey(str)) {
      return false;
    }
  }
  offset = config["offset"];
  min_angle = config["min_angle"];
  return true;
}
