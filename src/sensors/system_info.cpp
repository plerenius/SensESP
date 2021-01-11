#include "system_info.h"

#include "Arduino.h"
#include "sensesp.h"

void SystemHz::tick() { tick_count_++; }

void SystemHz::update() {
  // getting sporadic divide by 0 exceptions, no harm in skipping a loop.
  if (elapsed_millis_ == 0) {
    return;
  }

  output = (tick_count_ * 1000) / elapsed_millis_;

  tick_count_ = 0;
  elapsed_millis_ = 0;

  this->notify();
}

void SystemHz::enable() {
  elapsed_millis_ = 0;

  app.onTick([this]() { this->tick(); });
  app.onRepeat(1000, [this]() { this->update(); });
}

void FreeMem::update() {
  this->emit(ESP.getFreeHeap());
}

void FreeMem::enable() {
  app.onRepeat(1000, [this]() { this->update(); });
}

void Uptime::update() {
  this->emit(millis() / 1000.);
}

void Uptime::enable() {
  app.onRepeat(1000, [this]() { this->update(); });
}

void IPAddrDev::update() {
  this->emit(WiFi.localIP().toString());
}

void IPAddrDev::enable() {
  app.onRepeat(10000, [this]() { this->update(); });
}

void WifiSignal::enable() {
  app.onRepeat(3000, [this]() { this->update(); });
}

void WifiSignal::update() {
  this->emit(WiFi.RSSI());
}