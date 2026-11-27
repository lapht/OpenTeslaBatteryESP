#include "BmsDecoder.h"
#include "Signal.h"

namespace {
constexpr uint32_t kNoSna = 0xFFFFFFFEu;  // signal has no SNA value in the DBC

// Store raw*scale unless the raw value is the DBC "SNA" (signal not available) marker.
void set(Value& dst, uint32_t raw, uint32_t sna, float scale, uint32_t nowMs) {
  if (raw == 0xFFFFFFFFu || raw == sna) {
    dst.valid = false;
    return;
  }
  dst.v = raw * scale;
  dst.valid = true;
  dst.lastSeenMs = nowMs;
}
}  // namespace

bool BmsDecoder::decode(uint32_t id, const uint8_t* data, uint8_t dlc, uint32_t nowMs) {
  switch (id) {
    case ID_BMS_SOC: decode292(data, dlc, nowMs); break;
    case ID_BMS_ENERGY_STATUS: decode352(data, dlc, nowMs); break;
    default: return false;
  }
  state_.batteryFrames++;
  state_.lastBatteryFrameMs = nowMs;
  return true;
}

// DBC: SG_ BattBeginningOfLifeEnergy292 : 40|10@1+ (0.1,0) "kWh", SNA=1023
//      SG_ SOCUI292 : 10|10@1+ (0.1,0) "%"
void BmsDecoder::decode292(const uint8_t* d, uint8_t dlc, uint32_t now) {
  set(state_.beginningOfLifeEnergyKWh, extractLE(d, dlc, 40, 10), 1023, 0.1f, now);
  set(state_.socUiPct, extractLE(d, dlc, 10, 10), kNoSna, 0.1f, now);
}

// DBC: SG_ BMS_nominalFullPackEnergy : 0|11@1+ (0.1,0) "KWh", SNA=2047
//      SG_ BMS_nominalEnergyRemaining : 11|11@1+ (0.1,0) "KWh", SNA=2047
//      SG_ BMS_energyBuffer : 55|8@1+ (0.1,0) "KWh", SNA=255
// NOTE: some firmware versions are reported to multiplex 0x352 differently.
// To be verified against Scan My Tesla on the reference car (see issue tracker).
void BmsDecoder::decode352(const uint8_t* d, uint8_t dlc, uint32_t now) {
  set(state_.nominalFullPackEnergyKWh, extractLE(d, dlc, 0, 11), 2047, 0.1f, now);
  set(state_.nominalEnergyRemainingKWh, extractLE(d, dlc, 11, 11), 2047, 0.1f, now);
  set(state_.energyBufferKWh, extractLE(d, dlc, 55, 8), 255, 0.1f, now);
}
