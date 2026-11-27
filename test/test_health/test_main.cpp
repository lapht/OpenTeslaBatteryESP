#include <unity.h>
#include "bms/HealthCalculator.cpp"

void setUp() {}
void tearDown() {}

static BmsState state(float bol, float full, uint32_t t) {
  BmsState s;
  s.beginningOfLifeEnergyKWh = {bol, true, t};
  s.nominalFullPackEnergyKWh = {full, true, t};
  return s;
}

void test_soh_basic() {
  Health h = computeHealth(state(80.0f, 72.0f, 1000), 1500, 5000);
  TEST_ASSERT_TRUE(h.valid);
  TEST_ASSERT_FLOAT_WITHIN(0.01, 90.0, h.sohPct);
  TEST_ASSERT_FLOAT_WITHIN(0.01, 10.0, h.degradationPct);
}

void test_missing_value() {
  BmsState s = state(80.0f, 72.0f, 1000);
  s.nominalFullPackEnergyKWh.valid = false;
  TEST_ASSERT_FALSE(computeHealth(s, 1500, 5000).valid);
}

void test_stale_value() {
  TEST_ASSERT_FALSE(computeHealth(state(80.0f, 72.0f, 1000), 7000, 5000).valid);
}

void test_zero_bol() {
  TEST_ASSERT_FALSE(computeHealth(state(0.0f, 72.0f, 1000), 1500, 5000).valid);
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_soh_basic);
  RUN_TEST(test_missing_value);
  RUN_TEST(test_stale_value);
  RUN_TEST(test_zero_bol);
  return UNITY_END();
}
