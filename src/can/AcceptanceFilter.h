#pragma once
#include <stddef.h>
#include <stdint.h>

// TWAI hardware acceptance filter for a set of 11-bit IDs (issue #7).
// Pure logic, no ESP-IDF types, so it can be unit tested on the host.
//
// TWAI acceptance mask convention: a 1 bit in `mask` means "don't care".
// - 1 or 2 IDs: dual filter mode, each filter matches one ID exactly.
//   Standard frames, dual mode: filter 1 = code[31:21] ID, [20] RTR, [19:16]+[3:0] data byte 1;
//   filter 2 = code[15:5] ID, [4] RTR. RTR and data bits are left as don't care.
// - 3+ IDs: single filter mode with the tightest single mask that covers them all
//   (bits that differ between the IDs become don't care). It may let a few other
//   IDs through; the decoder ignores those in software.
struct AcceptanceFilter {
  uint32_t code;
  uint32_t mask;
  bool single;  // true = single filter mode, false = dual
};

inline AcceptanceFilter makeAcceptanceFilter(const uint32_t* ids, size_t n) {
  if (n == 0) return {0, 0xFFFFFFFFu, true};  // accept all
  if (n <= 2) {
    uint32_t a = ids[0] & 0x7FF, b = (n == 2 ? ids[1] : ids[0]) & 0x7FF;
    return {(a << 21) | (b << 5), 0x001F001Fu, false};
  }
  uint32_t andBits = 0x7FF, orBits = 0;
  for (size_t i = 0; i < n; i++) {
    andBits &= ids[i] & 0x7FF;
    orBits |= ids[i] & 0x7FF;
  }
  uint32_t differ = andBits ^ orBits;
  // Single mode, standard frame: ID at [31:21], RTR [20], data bytes 1-2 at [15:0].
  return {andBits << 21, (differ << 21) | 0x001FFFFFu, true};
}

// True if `id` passes the filter (used by the tests to check coverage).
inline bool filterAccepts(const AcceptanceFilter& f, uint32_t id) {
  id &= 0x7FF;
  if (f.single) return (((id << 21) ^ f.code) & ~f.mask & 0xFFE00000u) == 0;
  bool f1 = (((id << 21) ^ f.code) & ~f.mask & 0xFFE00000u) == 0;
  bool f2 = (((id << 5) ^ f.code) & ~f.mask & 0x0000FFE0u) == 0;
  return f1 || f2;
}

// When to narrow the hardware filter and when to open it again.
// With the filter on, the controller drops every other frame, so the page could
// no longer tell "no CAN traffic" from "traffic but no battery frames (wrong bus?)".
// So: run filtered while battery frames arrive; if they stop for kOpenAfterMs,
// open the filter (accept all) to diagnose; narrow again once they are back.
struct FilterPolicy {
  static constexpr uint32_t kOpenAfterMs = 10000;   // filtered, no battery frame this long -> open
  static constexpr uint32_t kNarrowAfterMs = 2000;  // open at least this long before narrowing
  static constexpr uint32_t kRecentMs = 1000;       // battery frame "recent" for narrowing

  // batterySeen: at least one battery frame since `sinceBatteryMs` started counting.
  // Returns the desired state: true = filtered.
  static bool wantFiltered(bool filtered, bool batterySeen, uint32_t sinceBatteryMs,
                           uint32_t sinceModeChangeMs) {
    if (filtered) {
      uint32_t quiet = batterySeen ? sinceBatteryMs : sinceModeChangeMs;
      return quiet < kOpenAfterMs;
    }
    return batterySeen && sinceBatteryMs <= kRecentMs && sinceModeChangeMs >= kNarrowAfterMs;
  }
};
