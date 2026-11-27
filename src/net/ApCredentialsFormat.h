#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// Pure helpers for the SoftAP name and default password (no Arduino deps, unit tested).

namespace otb {

// Lowercase letters and digits without look-alikes (0/o, 1/l/i): easy to type on a phone.
constexpr char kPasswordAlphabet[] = "23456789abcdefghjkmnpqrstuvwxyz";
constexpr size_t kPasswordAlphabetLen = sizeof(kPasswordAlphabet) - 1;  // 31
constexpr size_t kPasswordChars = 12;                                   // ~59 bits of entropy
constexpr size_t kPasswordBufLen = kPasswordChars + 2 /* dashes */ + 1; // "xxxx-xxxx-xxxx\0"

// Formats 12 random words as "xxxx-xxxx-xxxx". The modulo bias on 32-bit words is < 1e-8.
inline void formatPassword(const uint32_t rnd[kPasswordChars], char out[kPasswordBufLen]) {
  size_t o = 0;
  for (size_t i = 0; i < kPasswordChars; i++) {
    if (i > 0 && i % 4 == 0) out[o++] = '-';
    out[o++] = kPasswordAlphabet[rnd[i] % kPasswordAlphabetLen];
  }
  out[o] = '\0';
}

// "<base>-XXXX" from the last two bytes of the SoftAP MAC, so several devices
// (or several cars in the same garage) can be told apart.
inline void formatSsid(const char* base, const uint8_t mac[6], char* out, size_t outLen) {
  snprintf(out, outLen, "%s-%02X%02X", base, mac[4], mac[5]);
}

// WPA2 passphrase rule: 8..63 printable ASCII characters.
inline bool isValidPassword(const char* pw) {
  if (pw == nullptr) return false;
  size_t n = 0;
  for (; pw[n] != '\0'; n++) {
    if (pw[n] < 0x20 || pw[n] > 0x7E) return false;
    if (n > 63) return false;
  }
  return n >= 8 && n <= 63;
}

}  // namespace otb
