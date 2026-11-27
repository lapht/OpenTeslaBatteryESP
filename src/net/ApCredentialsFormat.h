#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// Pure helpers for the SoftAP name (no Arduino deps, unit tested).

namespace otb {

// "<base>-XXXX" from the last two bytes of the SoftAP MAC, so several devices
// (or several cars in the same garage) can be told apart.
inline void formatSsid(const char* base, const uint8_t mac[6], char* out, size_t outLen) {
  snprintf(out, outLen, "%s-%02X%02X", base, mac[4], mac[5]);
}

}  // namespace otb
