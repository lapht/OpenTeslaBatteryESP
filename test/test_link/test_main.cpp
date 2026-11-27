#include <unity.h>
#include "bms/LinkState.h"

void setUp() {}
void tearDown() {}

void test_no_traffic_at_boot() {
  TEST_ASSERT_TRUE(classifyLink(0, 0, 0, 0, 100, 5000) == LinkState::NoTraffic);
}

void test_traffic_without_battery_frames() {
  TEST_ASSERT_TRUE(classifyLink(500, 9000, 0, 0, 10000, 5000) == LinkState::NoBatteryFrames);
}

void test_battery_frames_ok() {
  TEST_ASSERT_TRUE(classifyLink(500, 9000, 20, 8000, 10000, 5000) == LinkState::Ok);
}

void test_battery_frames_stopped_but_bus_alive() {
  TEST_ASSERT_TRUE(classifyLink(500, 9900, 20, 1000, 10000, 5000) == LinkState::NoBatteryFrames);
}

void test_everything_stopped() {
  TEST_ASSERT_TRUE(classifyLink(500, 1000, 20, 1000, 10000, 5000) == LinkState::NoTraffic);
}

void test_millis_wraparound() {
  // last frame just before the 32-bit millis() wrap, now just after it
  TEST_ASSERT_TRUE(classifyLink(1, 0xFFFFFF00u, 1, 0xFFFFFF00u, 0x00000100u, 5000) == LinkState::Ok);
}

void test_names() {
  TEST_ASSERT_EQUAL_STRING("ok", linkStateName(LinkState::Ok));
  TEST_ASSERT_EQUAL_STRING("no_battery_frames", linkStateName(LinkState::NoBatteryFrames));
  TEST_ASSERT_EQUAL_STRING("no_traffic", linkStateName(LinkState::NoTraffic));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_no_traffic_at_boot);
  RUN_TEST(test_traffic_without_battery_frames);
  RUN_TEST(test_battery_frames_ok);
  RUN_TEST(test_battery_frames_stopped_but_bus_alive);
  RUN_TEST(test_everything_stopped);
  RUN_TEST(test_millis_wraparound);
  RUN_TEST(test_names);
  return UNITY_END();
}
