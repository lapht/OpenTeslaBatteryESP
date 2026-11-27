#include <unity.h>
#include "can/AcceptanceFilter.h"

void setUp() {}
void tearDown() {}

static const uint32_t kBattery[] = {0x292, 0x352};

void test_two_ids_dual_exact() {
  AcceptanceFilter f = makeAcceptanceFilter(kBattery, 2);
  TEST_ASSERT_FALSE(f.single);
  TEST_ASSERT_EQUAL_HEX32((0x292u << 21) | (0x352u << 5), f.code);
  TEST_ASSERT_EQUAL_HEX32(0x001F001Fu, f.mask);
  TEST_ASSERT_TRUE(filterAccepts(f, 0x292));
  TEST_ASSERT_TRUE(filterAccepts(f, 0x352));
  // Exact: every other standard ID is rejected
  for (uint32_t id = 0; id < 0x800; id++) {
    if (id == 0x292 || id == 0x352) continue;
    if (filterAccepts(f, id)) TEST_FAIL_MESSAGE("unexpected ID accepted");
  }
}

void test_one_id() {
  const uint32_t ids[] = {0x132};
  AcceptanceFilter f = makeAcceptanceFilter(ids, 1);
  TEST_ASSERT_TRUE(filterAccepts(f, 0x132));
  TEST_ASSERT_FALSE(filterAccepts(f, 0x133));
}

void test_many_ids_single_mask_covers_all() {
  const uint32_t ids[] = {0x132, 0x292, 0x332, 0x352, 0x3D2};
  AcceptanceFilter f = makeAcceptanceFilter(ids, 5);
  TEST_ASSERT_TRUE(f.single);
  for (uint32_t id : ids) TEST_ASSERT_TRUE(filterAccepts(f, id));
  // All of them end in ...0010010 (0x12 in the low 5 bits): IDs that differ there are rejected
  TEST_ASSERT_FALSE(filterAccepts(f, 0x293));
  TEST_ASSERT_FALSE(filterAccepts(f, 0x100));
  // RTR and data bits stay don't care
  TEST_ASSERT_EQUAL_HEX32(0x001FFFFFu, f.mask & 0x001FFFFFu);
}

void test_empty_accepts_all() {
  AcceptanceFilter f = makeAcceptanceFilter(nullptr, 0);
  TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFFu, f.mask);
  TEST_ASSERT_TRUE(filterAccepts(f, 0x123));
}

void test_policy_filtered_stays_while_frames_arrive() {
  TEST_ASSERT_TRUE(FilterPolicy::wantFiltered(true, true, 200, 60000));
}

void test_policy_filtered_opens_when_frames_stop() {
  TEST_ASSERT_FALSE(FilterPolicy::wantFiltered(true, true, FilterPolicy::kOpenAfterMs, 60000));
}

void test_policy_filtered_without_any_frame_opens_after_timeout() {
  TEST_ASSERT_TRUE(FilterPolicy::wantFiltered(true, false, 0, 5000));
  TEST_ASSERT_FALSE(FilterPolicy::wantFiltered(true, false, 0, FilterPolicy::kOpenAfterMs));
}

void test_policy_open_narrows_when_frames_are_back() {
  TEST_ASSERT_TRUE(FilterPolicy::wantFiltered(false, true, 100, 3000));
}

void test_policy_open_waits_before_narrowing() {
  TEST_ASSERT_FALSE(FilterPolicy::wantFiltered(false, true, 100, 500));
}

void test_policy_open_stays_without_frames() {
  TEST_ASSERT_FALSE(FilterPolicy::wantFiltered(false, false, 0, 60000));
  TEST_ASSERT_FALSE(FilterPolicy::wantFiltered(false, true, 30000, 60000));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_two_ids_dual_exact);
  RUN_TEST(test_one_id);
  RUN_TEST(test_many_ids_single_mask_covers_all);
  RUN_TEST(test_empty_accepts_all);
  RUN_TEST(test_policy_filtered_stays_while_frames_arrive);
  RUN_TEST(test_policy_filtered_opens_when_frames_stop);
  RUN_TEST(test_policy_filtered_without_any_frame_opens_after_timeout);
  RUN_TEST(test_policy_open_narrows_when_frames_are_back);
  RUN_TEST(test_policy_open_waits_before_narrowing);
  RUN_TEST(test_policy_open_stays_without_frames);
  return UNITY_END();
}
