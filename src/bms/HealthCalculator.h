#pragma once
#include "BmsDecoder.h"

// Battery health figures derived from decoded BMS values.
// This is an estimate based on what the BMS reports, NOT Tesla's official
// battery health test.

struct Health {
  bool valid = false;
  float sohPct = 0;          // nominalFullPack / beginningOfLife * 100
  float degradationPct = 0;  // 100 - SOH
  float originalKWh = 0;
  float currentKWh = 0;
};

// staleMs: values older than this (relative to nowMs) are ignored.
Health computeHealth(const BmsState& s, uint32_t nowMs, uint32_t staleMs);
