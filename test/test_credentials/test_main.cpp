#include <unity.h>
#include "net/ApCredentialsFormat.h"

void setUp() {}
void tearDown() {}

void test_ssid_suffix() {
  const uint8_t mac[6] = {0x24, 0x6F, 0x28, 0xAB, 0x3F, 0xA2};
  char ssid[48];
  otb::formatSsid("OpenTeslaBattery", mac, ssid, sizeof(ssid));
  TEST_ASSERT_EQUAL_STRING("OpenTeslaBattery-3FA2", ssid);
}

void test_ssid_truncated_safely() {
  const uint8_t mac[6] = {0, 0, 0, 0, 0x01, 0x02};
  char ssid[8];
  otb::formatSsid("OpenTeslaBattery", mac, ssid, sizeof(ssid));
  TEST_ASSERT_EQUAL_STRING("OpenTes", ssid);
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_ssid_suffix);
  RUN_TEST(test_ssid_truncated_safely);
  return UNITY_END();
}
