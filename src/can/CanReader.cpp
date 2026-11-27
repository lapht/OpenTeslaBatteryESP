#include "CanReader.h"

#include <Arduino.h>
#include "driver/twai.h"

#include "AcceptanceFilter.h"

bool CanReader::begin(int txPin, int rxPin, const uint32_t* ids, size_t n) {
  txPin_ = txPin;
  rxPin_ = rxPin;
  ids_ = ids;
  nIds_ = n;
  // Start unfiltered: the caller narrows once battery frames are seen.
  return install(false);
}

bool CanReader::install(bool filtered) {
  // LISTEN_ONLY: no ACK, no error frames, no TX. This is a hard project rule.
  twai_general_config_t g = TWAI_GENERAL_CONFIG_DEFAULT(
      (gpio_num_t)txPin_, (gpio_num_t)rxPin_, TWAI_MODE_LISTEN_ONLY);
  g.rx_queue_len = 64;
  twai_timing_config_t t = TWAI_TIMING_CONFIG_500KBITS();

  twai_filter_config_t f = TWAI_FILTER_CONFIG_ACCEPT_ALL();
  if (filtered && nIds_ > 0) {
    AcceptanceFilter af = makeAcceptanceFilter(ids_, nIds_);
    f.acceptance_code = af.code;
    f.acceptance_mask = af.mask;
    f.single_filter = af.single;
  } else {
    filtered = false;
  }

  if (twai_driver_install(&g, &t, &f) != ESP_OK) return false;
  if (twai_start() != ESP_OK) {
    twai_driver_uninstall();
    return false;
  }
  running_ = true;
  stats_.filtered = filtered;
  return true;
}

bool CanReader::setFiltered(bool on) {
  if (!running_) return false;
  if (on == stats_.filtered) return true;
  missedBase_ = stats_.missed;
  busErrorsBase_ = stats_.busErrors;
  end();
  if (install(on)) return true;
  return install(false);  // fall back to accept-all rather than going deaf
}

bool CanReader::read(CanFrame& out, uint32_t timeoutMs) {
  if (!running_) return false;
  twai_message_t msg;
  if (twai_receive(&msg, pdMS_TO_TICKS(timeoutMs)) != ESP_OK) return false;
  if (msg.extd || msg.rtr) return false;  // Tesla Vehicle CAN uses 11-bit data frames

  out.id = msg.identifier;
  out.dlc = msg.data_length_code > 8 ? 8 : msg.data_length_code;
  for (uint8_t i = 0; i < out.dlc; i++) out.data[i] = msg.data[i];
  out.timestampMs = millis();
  stats_.received++;
  stats_.lastFrameMs = out.timestampMs;

  twai_status_info_t s;
  if (twai_get_status_info(&s) == ESP_OK) {
    stats_.missed = missedBase_ + s.rx_missed_count;
    stats_.busErrors = busErrorsBase_ + s.bus_error_count;
  }
  return true;
}

void CanReader::end() {
  if (!running_) return;
  twai_stop();
  twai_driver_uninstall();
  running_ = false;
}
