#pragma once
#include <stdint.h>
#include <stddef.h>

// Read-only CAN access through the ESP32 TWAI controller.
// The controller is started in TWAI_MODE_LISTEN_ONLY: it never transmits,
// not even ACK bits or error frames. There is intentionally no send() API.

struct CanFrame {
  uint32_t id;
  uint8_t dlc;
  uint8_t data[8];
  uint32_t timestampMs;
};

struct CanStats {
  uint32_t received;
  uint32_t missed;      // RX queue overruns
  uint32_t busErrors;
  uint32_t lastFrameMs;  // meaningful only if received > 0
  bool filtered;         // hardware acceptance filter active (see AcceptanceFilter.h)
};

class CanReader {
 public:
  // bitrate is fixed at 500 kbit/s (Tesla Vehicle CAN).
  // ids/n: IDs for the hardware acceptance filter, used while setFiltered(true).
  bool begin(int txPin, int rxPin, const uint32_t* ids = nullptr, size_t n = 0);
  // Switch between the hardware filter and accept-all. Reinstalls the TWAI driver:
  // call it from the task that calls read(). A few frames may be lost.
  bool setFiltered(bool on);
  bool filtered() const { return stats_.filtered; }
  // Non-blocking: returns true and fills `out` if a frame is available.
  bool read(CanFrame& out, uint32_t timeoutMs = 0);
  CanStats stats() const { return stats_; }
  void end();

 private:
  bool install(bool filtered);
  bool running_ = false;
  int txPin_ = -1, rxPin_ = -1;
  const uint32_t* ids_ = nullptr;
  size_t nIds_ = 0;
  uint32_t missedBase_ = 0, busErrorsBase_ = 0;  // counters survive driver reinstalls
  CanStats stats_{};
};
