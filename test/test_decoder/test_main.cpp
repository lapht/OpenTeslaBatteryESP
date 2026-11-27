#include <unity.h>
#include "bms/BmsDecoder.cpp"
#include "bms/Signal.h"

void setUp() {}
void tearDown() {}

// Helper: write an unsigned LE signal into a payload (inverse of extractLE).
static void put(uint8_t* d, uint16_t start, uint8_t len, uint32_t v) {
  for (uint8_t i = 0; i < len; i++) {
    uint16_t b = start + i;
    if (v & (1u << i)) d[b / 8] |= (1u << (b % 8));
    else d[b / 8] &= ~(1u << (b % 8));
  }
}

void test_extract_le_roundtrip() {
  uint8_t d[8] = {0};
  put(d, 11, 11, 0x5A5);
  TEST_ASSERT_EQUAL_UINT32(0x5A5, extractLE(d, 8, 11, 11));
}

void test_extract_out_of_range() {
  uint8_t d[4] = {0xFF, 0xFF, 0xFF, 0xFF};
  TEST_ASSERT_EQUAL_UINT32(0xFFFFFFFFu, extractLE(d, 4, 40, 10));
}

void test_decode_292_bol() {
  uint8_t d[8] = {0};
  put(d, 40, 10, 820);  // 82.0 kWh (synthetic test value, not from a real car)
  BmsDecoder dec;
  TEST_ASSERT_TRUE(dec.decode(0x292, d, 8, 100));
  TEST_ASSERT_TRUE(dec.state().beginningOfLifeEnergyKWh.valid);
  TEST_ASSERT_FLOAT_WITHIN(0.01, 82.0, dec.state().beginningOfLifeEnergyKWh.v);
}

void test_decode_292_sna() {
  uint8_t d[8] = {0};
  put(d, 40, 10, 1023);
  BmsDecoder dec;
  dec.decode(0x292, d, 8, 100);
  TEST_ASSERT_FALSE(dec.state().beginningOfLifeEnergyKWh.valid);
}

void test_decode_352() {
  uint8_t d[8] = {0};
  put(d, 0, 11, 755);   // 75.5 kWh
  put(d, 11, 11, 400);  // 40.0 kWh
  put(d, 55, 8, 32);    // 3.2 kWh
  BmsDecoder dec;
  TEST_ASSERT_TRUE(dec.decode(0x352, d, 8, 100));
  TEST_ASSERT_FLOAT_WITHIN(0.01, 75.5, dec.state().nominalFullPackEnergyKWh.v);
  TEST_ASSERT_FLOAT_WITHIN(0.01, 40.0, dec.state().nominalEnergyRemainingKWh.v);
  TEST_ASSERT_FLOAT_WITHIN(0.01, 3.2, dec.state().energyBufferKWh.v);
}

void test_unknown_id_ignored() {
  uint8_t d[8] = {0};
  BmsDecoder dec;
  TEST_ASSERT_FALSE(dec.decode(0x123, d, 8, 100));
  TEST_ASSERT_EQUAL_UINT32(0, dec.state().batteryFrames);
}

void test_battery_frames_counted_even_if_sna() {
  uint8_t d[8] = {0};
  put(d, 40, 10, 1023);  // SNA
  BmsDecoder dec;
  dec.decode(0x292, d, 8, 100);
  dec.decode(0x352, d, 8, 250);
  TEST_ASSERT_EQUAL_UINT32(2, dec.state().batteryFrames);
  TEST_ASSERT_EQUAL_UINT32(250, dec.state().lastBatteryFrameMs);
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_extract_le_roundtrip);
  RUN_TEST(test_extract_out_of_range);
  RUN_TEST(test_decode_292_bol);
  RUN_TEST(test_decode_292_sna);
  RUN_TEST(test_decode_352);
  RUN_TEST(test_unknown_id_ignored);
  RUN_TEST(test_battery_frames_counted_even_if_sna);
  return UNITY_END();
}
