#pragma once
#include <stdint.h>

// Extract an unsigned little-endian (Intel, DBC "@1") signal from a CAN payload.
// startBit and length follow DBC conventions: startBit is the LSB position,
// counted from bit 0 of byte 0.
inline uint32_t extractLE(const uint8_t* data, uint8_t dlc, uint16_t startBit, uint8_t length) {
  uint32_t value = 0;
  for (uint8_t i = 0; i < length; i++) {
    uint16_t bit = startBit + i;
    uint8_t byteIdx = bit / 8;
    if (byteIdx >= dlc) return 0xFFFFFFFFu;  // signal does not fit in this frame
    if (data[byteIdx] & (1u << (bit % 8))) value |= (1u << i);
  }
  return value;
}
