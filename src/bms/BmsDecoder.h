#pragma once
#include <stdint.h>

// Decodes Tesla Model 3/Y BMS frames into physical values.
//
// Signal layouts come from joshwardell/model3dbc (Model3CAN.dbc, commit c56c113).
// They are community reverse-engineered and NOT yet validated on a real car
// by this project. See docs/can-mapping.md.

struct Value {
  float v = 0;
  bool valid = false;        // false if never received or SNA
  uint32_t lastSeenMs = 0;
};

struct BmsState {
  // 0x292 ID292BMS_SOC
  Value beginningOfLifeEnergyKWh;   // BattBeginningOfLifeEnergy292
  Value socUiPct;                   // SOCUI292
  // 0x352 ID352BMS_energyStatus
  Value nominalFullPackEnergyKWh;   // BMS_nominalFullPackEnergy
  Value nominalEnergyRemainingKWh;  // BMS_nominalEnergyRemaining
  Value energyBufferKWh;            // BMS_energyBuffer
};

class BmsDecoder {
 public:
  // Returns true if the frame ID is handled.
  bool decode(uint32_t id, const uint8_t* data, uint8_t dlc, uint32_t nowMs);
  const BmsState& state() const { return state_; }

  static constexpr uint32_t ID_BMS_SOC = 0x292;
  static constexpr uint32_t ID_BMS_ENERGY_STATUS = 0x352;

 private:
  void decode292(const uint8_t* data, uint8_t dlc, uint32_t nowMs);
  void decode352(const uint8_t* data, uint8_t dlc, uint32_t nowMs);
  BmsState state_;
};
