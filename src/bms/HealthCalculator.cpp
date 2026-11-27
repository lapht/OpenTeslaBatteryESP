#include "HealthCalculator.h"

static bool fresh(const Value& v, uint32_t nowMs, uint32_t staleMs) {
  return v.valid && (nowMs - v.lastSeenMs) <= staleMs;
}

Health computeHealth(const BmsState& s, uint32_t nowMs, uint32_t staleMs) {
  Health h;
  const Value& bol = s.beginningOfLifeEnergyKWh;
  const Value& full = s.nominalFullPackEnergyKWh;
  if (!fresh(bol, nowMs, staleMs) || !fresh(full, nowMs, staleMs)) return h;
  if (bol.v <= 0.0f) return h;

  h.originalKWh = bol.v;
  h.currentKWh = full.v;
  h.sohPct = full.v / bol.v * 100.0f;
  h.degradationPct = 100.0f - h.sohPct;
  h.valid = true;
  return h;
}
