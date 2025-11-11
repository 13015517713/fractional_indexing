#include "gtest/gtest.h"
#include "fractional_indexing.h"

using base::generateKeyBetween;

const int kBase62DigitsLength = 62;

/*
test(null, null, "a0");
test(null, "a0", "Zz");
test(null, "Zz", "Zy");
test("a0", null, "a1");
test("a1", null, "a2");
test("a0", "a1", "a0V");
test("a1", "a2", "a1V");
test("a0V", "a1", "a0l");
test("Zz", "a0", "ZzV");
test("Zz", "a1", "a0");
test(null, "Y00", "Xzzz");
test("bzz", null, "c000");
test("a0", "a0V", "a0G");
test("a0", "a0G", "a08");
test("b125", "b129", "b127");
test("a0", "a1V", "a1");
test("Zz", "a01", "a0");
test(null, "a0V", "a0");
test(null, "b999", "b99");
test(
  null,
  "A00000000000000000000000000",
  "invalid order key: A00000000000000000000000000"
);
test(null, "A000000000000000000000000001", "A000000000000000000000000000V");
test("zzzzzzzzzzzzzzzzzzzzzzzzzzy", null, "zzzzzzzzzzzzzzzzzzzzzzzzzzz");
test("zzzzzzzzzzzzzzzzzzzzzzzzzzz", null, "zzzzzzzzzzzzzzzzzzzzzzzzzzzV");
test("a00", null, "invalid order key: a00");
test("a00", "a1", "invalid order key: a00");
test("0", "1", "invalid order key head: 0");
test("a1", "a0", "a1 >= a0");
*/

TEST(FractionalIndexingTest, GenerateKeyBetween) {
  EXPECT_EQ(generateKeyBetween("", ""), "a0");
  EXPECT_EQ(generateKeyBetween("", "a0"), "Zz");
  EXPECT_EQ(generateKeyBetween("", "Zz"), "Zy");
  EXPECT_EQ(generateKeyBetween("a0", ""), "a1");
  EXPECT_EQ(generateKeyBetween("a1", ""), "a2");
  EXPECT_EQ(generateKeyBetween("a0", "a1"), "a0V");
  EXPECT_EQ(generateKeyBetween("a1", "a2"), "a1V");
  EXPECT_EQ(generateKeyBetween("a0V", "a1"), "a0l");
  EXPECT_EQ(generateKeyBetween("Zz", "a0"), "ZzV");
  EXPECT_EQ(generateKeyBetween("Zz", "a1"), "a0");
  EXPECT_EQ(generateKeyBetween("", "Y00"), "Xzzz");
  EXPECT_EQ(generateKeyBetween("bzz", ""), "c000");
  EXPECT_EQ(generateKeyBetween("a0", "a0V"), "a0G");
  EXPECT_EQ(generateKeyBetween("a0", "a0G"), "a08");
  EXPECT_EQ(generateKeyBetween("b125", "b129"), "b127");
  EXPECT_EQ(generateKeyBetween("a0", "a1V"), "a1");  // a0V
  EXPECT_EQ(generateKeyBetween("Zz", "a01"), "a0");  // ZzV
  EXPECT_EQ(generateKeyBetween("", "a0V"), "a0");
  EXPECT_EQ(generateKeyBetween("", "b999"), "b99");
  EXPECT_EQ(generateKeyBetween("", "A00000000000000000000000000"),
            "");  // invalid order key
  EXPECT_EQ(generateKeyBetween("", "A000000000000000000000000001"),
            "A000000000000000000000000000V");
  EXPECT_EQ(generateKeyBetween("zzzzzzzzzzzzzzzzzzzzzzzzzzy", ""),
            "zzzzzzzzzzzzzzzzzzzzzzzzzzz");
  EXPECT_EQ(generateKeyBetween("zzzzzzzzzzzzzzzzzzzzzzzzzzz", ""),
            "zzzzzzzzzzzzzzzzzzzzzzzzzzzV");
  EXPECT_EQ(generateKeyBetween("a00", ""), "");    // invalid order key
  EXPECT_EQ(generateKeyBetween("a00", "a1"), "");  // invalid order key
  EXPECT_EQ(generateKeyBetween("0", "1"), "");     // invalid order key head
  EXPECT_EQ(generateKeyBetween("a1", "a0"), "");   // a1 >= a0
}

static int GetIntegerLength(char head) {
  if (head >= 'a' && head <= 'z') {
    return head - 'a' + 2;
  } else if (head >= 'A' && head <= 'Z') {
    return 'Z' - head + 2;
  } else {
    return -1;
  }
}

static inline std::string RandomBase62String(int length,
                                             bool contain_zero = true) {
  std::string result;
  result.reserve(length);
  for (int i = 0; i < length; i++) {
    if (contain_zero) {
      result.push_back(base::KBase62Digits[rand() % kBase62DigitsLength]);
    } else {
      result.push_back(
          base::KBase62Digits[rand() % (kBase62DigitsLength - 1) + 1]);
    }
  }
  return result;
}

static inline std::string generateRandomKey() {
  std::string res;
  res = base::KBase62Digits[rand() % 52 + 10];
  res.append(RandomBase62String(GetIntegerLength(res[0]) -1 ));

  bool is_fractional = rand() & (1 << 2);
  if (is_fractional) {
    res.append(RandomBase62String(rand() % 10, false));
  }
  return res;
}

TEST(FractionalIndexingTest, RandomTest) {
  const uint64_t run_s = 1200;

  uint64_t time_s = time(0);
  while (true) {
    for (int i = 0; i < 1000; i++) {
      std::string key1 = generateRandomKey();
      std::string key2 = generateRandomKey();
      if (key1 == key2) {
        continue;
      } else if (key1 > key2) {
        std::swap(key1, key2);
      }

      std::string result = generateKeyBetween(key1, key2);
      ASSERT_GT(result, key1)
          << "key1: " << key1 << ", key2: " << key2 << ", result: " << result;
      ASSERT_LT(result, key2)
          << "key1: " << key1 << ", key2: " << key2 << ", result: " << result;
    }

    if (time(0) - time_s > run_s) {
      break;
    }
  }
}