#include "orientation_9dof_input.h"

#include <RemoteDebug.h>

#include "sensesp.h"

Orientation9DOF::Orientation9DOF(uint8_t pin_i2c_sda, uint8_t pin_i2c_scl) {
  sensor_fxos_fxas_ = new SensorNXP_FXOS8700_FXAS21002();
  if (!sensor_fxos_fxas_->connect(pin_i2c_sda, pin_i2c_scl)) {
    debugE(
        "No connection to FXOS8700/FXAS21002 sensor: check address & wiring");
  } else {
    sensor_fxos_fxas_->printSensorDetails();
  }
}

// Used only when calibrating. This method does not return.
void Orientation9DOF::stream_raw_values(void) {
  debugI("calling gatherCalibrationData()");
  while (true) {
    sensor_fxos_fxas_->gatherCalibrationDataOnce(true);
    delay(10);
  }
}


Read9DOF::Read9DOF(Orientation9DOF* orientation_9dof,
                   OrientationValType val_type, uint read_delay,
                   String config_path)
    : NumericSensor(config_path),
      orientation_9dof_{orientation_9dof},
      val_type_{val_type},
      read_delay_{read_delay} {
  load_configuration();
  orientation_9dof_->sensor_fxos_fxas_->initFilter(this->read_delay_);
}

// Setup repeated readings from combination sensor.
void Read9DOF::enable() {
  app.onRepeat(read_delay_, [this]() { this->update(); });
}

// Provides one parameter reading from the combination sensor.
// val_type determines which particular parameter is output.
// Note that since the filter algorithm works best when all 9-DOF
//  parameters are captured simultaneously, and at a specified
//  constant rate, there is only a single call to
//  sensor_fxos_fxas->gatherOrientationDataOnce() in this method.
//  Arbitrarily, this update happens when the compass heading
//  is requested; all the remaining parameters are retrieved
//  by calling the appropriate sensor_fxos_fxas->getter method.
// If the compass heading parameter is not the one that is set
//  to the fastest repetition rate (in main.cpp when calling
//  Read9DOF::Read9DOF()), then you should move the
//  gatherOrientationDataOnce() call into the case block
//  for whichever parameter _is_ updated the fastest - this
//  ensures that that parameter and all others are never stale.
void Read9DOF::update() {
  switch (val_type_) {
    case (compass_hdg):
      // sensor is read and filter called, only for compass_hdg
      // remaining parameters are obtained from most recent filter results
      orientation_9dof_->sensor_fxos_fxas_->gatherOrientationDataOnce(false);
      output = orientation_9dof_->sensor_fxos_fxas_->getHeadingRadians();
      break;
    case (roll):
      output = orientation_9dof_->sensor_fxos_fxas_->getRollRadians();
      break;
    case (pitch):
      output = orientation_9dof_->sensor_fxos_fxas_->getPitchRadians();
      break;
    case (acceleration_x):
      output = orientation_9dof_->sensor_fxos_fxas_->getAccelerationX();
      break;
    case (acceleration_y):
      output = orientation_9dof_->sensor_fxos_fxas_->getAccelerationY();
      break;
    case (acceleration_z):
      output = orientation_9dof_->sensor_fxos_fxas_->getAccelerationZ();
      break;
    case (rate_of_turn):
      output = orientation_9dof_->sensor_fxos_fxas_->getRateOfTurn();
      break;
    case (rate_of_pitch):
      output = orientation_9dof_->sensor_fxos_fxas_->getRateOfPitch();
      break;
    case (rate_of_roll):
      output = orientation_9dof_->sensor_fxos_fxas_->getRateOfRoll();
      break;
    default:
      output = 0.0;
  }
  notify();
}

void Read9DOF::get_configuration(JsonObject& doc) {
  doc["read_delay"] = read_delay_;
};

static const char SCHEMA[] PROGMEM = R"###({
    "type": "object",
    "properties": {
        "read_delay": { "title": "Read delay", "type": "number", "description": "The time, in milliseconds, between each read of the input" }
    }
  })###";

String Read9DOF::get_config_schema() { return FPSTR(SCHEMA); }

bool Read9DOF::set_configuration(const JsonObject& config) {
  String expected[] = {"read_delay"};
  for (auto str : expected) {
    if (!config.containsKey(str)) {
      return false;
    }
  }
  read_delay_ = config["read_delay"];
  return true;
}
