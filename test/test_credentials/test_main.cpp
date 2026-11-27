#include <string.h>
#include <unity.h>
#include "net/ApCredentialsFormat.h"

using namespace otb;

void setUp() {}
void tearDown() {}

void test_password_format() {
  uint32_t rnd[kPasswordChars];
  for (size_t i = 0; i < kPasswordChars; i++) rnd[i] = 0x9E3779B9u * (i + 1);
  char pw[kPasswordBufLen];
  formatPassword(rnd, pw);
  TEST_ASSERT_EQUAL_size_t(14, strlen(pw));
  TEST_ASSERT_EQUAL_CHAR('-', pw[4]);
  TEST_ASSERT_EQUAL_CHAR('-', pw[9]);
  for (size_t i = 0; i < 14; i++) {
    if (i == 4 || i == 9) continue;
    TEST_ASSERT_NOT_NULL(strchr(kPasswordAlphabet, pw[i]));
  }
  TEST_ASSERT_TRUE(isValidPassword(pw));
}

void test_password_depends_on_rng() {
  uint32_t a[kPasswordChars] = {0}, b[kPasswordChars] = {0};
  b[11] = 1;
  char pa[kPasswordBufLen], pb[kPasswordBufLen];
  formatPassword(a, pa);
  formatPassword(b, pb);
  TEST_ASSERT_EQUAL_STRING("2222-2222-2222", pa);
  TEST_ASSERT_EQUAL_STRING("2222-2222-2223", pb);
}

void test_alphabet_has_no_lookalikes() {
  TEST_ASSERT_EQUAL_size_t(31, kPasswordAlphabetLen);
  for (const char* c = "01oli"; *c; c++) TEST_ASSERT_NULL(strchr(kPasswordAlphabet, *c));
}

void test_ssid_suffix() {
  const uint8_t mac[6] = {0x24, 0x6F, 0x28, 0xAB, 0x3F, 0xA2};
  char ssid[48];
  formatSsid("OpenTeslaBattery", mac, ssid, sizeof(ssid));
  TEST_ASSERT_EQUAL_STRING("OpenTeslaBattery-3FA2", ssid);
}

void test_password_validation() {
  TEST_ASSERT_FALSE(isValidPassword(nullptr));
  TEST_ASSERT_FALSE(isValidPassword(""));
  TEST_ASSERT_FALSE(isValidPassword("short"));
  TEST_ASSERT_TRUE(isValidPassword("12345678"));
  char longPw[70];
  memset(longPw, 'a', 64);
  longPw[64] = '\0';
  TEST_ASSERT_FALSE(isValidPassword(longPw));
  longPw[63] = '\0';
  TEST_ASSERT_TRUE(isValidPassword(longPw));
  TEST_ASSERT_FALSE(isValidPassword("abc\tdefgh"));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_password_format);
  RUN_TEST(test_password_depends_on_rng);
  RUN_TEST(test_alphabet_has_no_lookalikes);
  RUN_TEST(test_ssid_suffix);
  RUN_TEST(test_password_validation);
  return UNITY_END();
}
