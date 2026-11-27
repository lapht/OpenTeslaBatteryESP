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
};

class CanReader {
 public:
  // bitrate is fixed at 500 kbit/s (Tesla Vehicle CAN).
  bool begin(int txPin, int rxPin);
  // Non-blocking: returns true and fills `out` if a frame is available.
  bool read(CanFrame& out, uint32_t timeoutMs = 0);
  CanStats stats() const { return stats_; }
  void end();

 private:
  bool running_ = false;
  CanStats stats_{};
};
