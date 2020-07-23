#ifndef _dhtxx_H_
#define _dhtxx_H_

#include <DHT.h>

#include "sensor.h"

// DHTxx represents a temperature / humidity sensor.
// The constructore creates a pointer to the instance, and starts up the sensor. The pointer is
// passed to DHTvalue, which retrieves the specified value.
class DHTxx : public Sensor {
  public:
    DHTxx(uint8_t pin, uint8_t type, String config_path = "");
    DHT* pDHT;

  private:
    void check_status();
};


// Pass one of these in the constructor to DHTvalue() to tell which type of value you want to output
enum DHTValType { temperature, humidity };

// DHTvalue reads and outputs the specified value of a DHT sensor.
class DHTvalue : public NumericSensor {
  public:
    DHTvalue(DHTxx* pDHTxx, DHTValType val_type, uint read_delay = 500, String config_path="");
    void enable() override final;
    DHTxx* pDHTxx;

  private:
    
    DHTValType val_type;
    uint read_delay;
    virtual void get_configuration(JsonObject& doc) override;
    virtual bool set_configuration(const JsonObject& config) override;
    virtual String get_config_schema() override;

};

#endif
