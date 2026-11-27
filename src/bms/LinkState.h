#pragma once
#include <stdint.h>

// What the page should tell the user about the data source (issue #9).
// "Device unreachable" is decided by the browser (fetch fails), not here.
enum class LinkState : uint8_t {
  NoTraffic,        // no CAN frame received within staleMs (not wired, car asleep)
  NoBatteryFrames,  // CAN traffic, but no 0x292/0x352 within staleMs (wrong bus?)
  Ok,               // battery frames are arriving
};

inline bool isRecent(uint32_t count, uint32_t lastMs, uint32_t nowMs, uint32_t staleMs) {
  return count > 0 && (uint32_t)(nowMs - lastMs) <= staleMs;  // wrap-safe
}

inline LinkState classifyLink(uint32_t canFrames, uint32_t lastCanMs, uint32_t batteryFrames,
                              uint32_t lastBatteryMs, uint32_t nowMs, uint32_t staleMs) {
  if (isRecent(batteryFrames, lastBatteryMs, nowMs, staleMs)) return LinkState::Ok;
  if (isRecent(canFrames, lastCanMs, nowMs, staleMs)) return LinkState::NoBatteryFrames;
  return LinkState::NoTraffic;
}

inline const char* linkStateName(LinkState s) {
  switch (s) {
    case LinkState::Ok: return "ok";
    case LinkState::NoBatteryFrames: return "no_battery_frames";
    default: return "no_traffic";
  }
}
