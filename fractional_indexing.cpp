#include "fractional_indexing.h"

#include <cmath>
#include <cassert>
#include <iostream>

namespace base {

namespace {
static const std::string kMinSentinel("A" + std::string(26, KBase62Digits[0]));

inline bool isValidFractionalPart(const std::string& fractionalPart) {
  // Fractional part cannot end with the minimum character '0' (unless empty)
  return fractionalPart.empty() || fractionalPart.back() != KBase62Digits[0];
}

inline int indexOfBase62Digit(char digit) {
  if (digit >= '0' && digit <= '9') {
    return digit - '0';  // 0-9
  } else if (digit >= 'A' && digit <= 'Z') {
    return digit - 'A' + 10;  // 10-35
  } else if (digit >= 'a' && digit <= 'z') {
    return digit - 'a' + 36;  // 36-61
  }
  return -1;
}

/*
find the midpoint between two keys.  returns "" if the keys are invalid, or
if they are not comparable.
a and b may be empty string.
*/
inline std::string midpoint(const std::string& a, const std::string& b) {
  if (!b.empty() && a >= b) {
    return "";
  }

  if (not isValidFractionalPart(a) || not isValidFractionalPart(b)) {
    return "";
  }

  if (!b.empty()) {
    // remove longest common prefix.  pad `a` with 0s as we
    // go.  note that we don't need to pad `b`, because it can't
    // end before `a` while traversing the common prefix.
    int n = 0;
    while (n < a.length() && n < b.length() && a[n] == b[n]) {
      n++;
    }

    if (n > 0) {
      return b.substr(0, n) + midpoint(a.substr(n), b.substr(n));
    }
  }

  const int a_index = a.empty() ? 0 : indexOfBase62Digit(a[0]);
  const int b_index =
      b.empty() ? KBase62Digits.length() : indexOfBase62Digit(b[0]);
  if (b_index - a_index > 1) {
    char c = KBase62Digits[std::round((a_index + b_index) / 2.0)];
    return std::string{c};
  } else {
    if (b.size() > 1) {
      return b.substr(0, 1);
    } else {
      return std::string{KBase62Digits[a_index]} +
             midpoint(a.empty() ? "" : a.substr(1), "");
    }
  }
}

inline int getIntegerLength(char head) {
  if (head >= 'a' && head <= 'z') {
    return head - 'a' + 2;
  } else if (head >= 'A' && head <= 'Z') {
    return 'Z' - head + 2;
  } else {
    return -1;
  }
}

inline std::string getIntegerPart(const std::string& key) {
  const int integerPartLength = getIntegerLength(key[0]);
  if (integerPartLength < 0) {
    return "";
  }

  if (key.length() < integerPartLength) {
    return "";
  }

  return key.substr(0, integerPartLength);
}

inline bool validateOrderKey(const std::string& key) {
  if (key == kMinSentinel) {
    return false;
  }
  std::string integerPart = getIntegerPart(key);
  if (integerPart.empty()) {
    return false;
  }

  std::string fractionalPart = key.substr(integerPart.length());
  if (!fractionalPart.empty() && fractionalPart.back() == KBase62Digits[0]) {
    // strictly constrain the tail minimum
    return false;
  }
  return true;
}

inline bool validateInteger(const std::string& integer) {
  if (integer.size() < 2 || integer.size() != getIntegerLength(integer[0])) {
    return false;
  }
  return true;
}

inline std::string decrementInteger(const std::string& integer) {
  if (!validateInteger(integer)) {
    return "";
  }

  std::string res = integer;
  bool borrow = true;
  for (int i = res.length() - 1; borrow && i >= 1; i--) {
    int idx = indexOfBase62Digit(res[i]) - 1;
    if (idx < 0) {
      res[i] = KBase62Digits.back();
    } else {
      res[i] = KBase62Digits[idx];
      borrow = false;
    }
  }
  char head = res[0];
  if (borrow) {
    if (head == 'a') {
      return std::string("Z") + KBase62Digits.back();
    }
    if (head == 'A') {
      return "";
    }
    res[0] = KBase62Digits[indexOfBase62Digit(head) - 1];
    if (res[0] < 'Z') {
      res.push_back(KBase62Digits.back());
    } else {
      res.pop_back();
    }
    return res;
  } else {
    return res;
  }

  return "";
}

inline std::string incrementInteger(const std::string& integer) {
  if (!validateInteger(integer)) {
    return "";
  }

  std::string res = integer;
  bool carry = true;
  char head = integer[0];
  for (int i = integer.length() - 1; carry && i >= 1; i--) {
    int idx = indexOfBase62Digit(integer[i]) + 1;
    if (idx == KBase62Digits.length()) {
      res[i] = KBase62Digits[0];
    } else {
      res[i] = KBase62Digits[idx];
      carry = false;
    }
  }
  if (carry) {
    if (head == 'Z') {
      return "a" + std::string{KBase62Digits[0]};
    }
    if (head == 'z') {
      return "";
    }
    res[0] = KBase62Digits[indexOfBase62Digit(head) + 1];
    if (res[0] > 'a') {
      res.push_back(KBase62Digits[0]);
    } else {
      res.pop_back();
    }
    return res;
  } else {
    return res;
  }
}

}  // namespace

std::string generateKeyBetween(const std::string& a, const std::string& b) {
  if (!a.empty() && !validateOrderKey(a)) {
    return "";
  }
  if (!b.empty() && !validateOrderKey(b)) {
    return "";
  }
  if (!a.empty() && !b.empty() && a >= b) {
    return "";
  }

  if (a.empty()) {
    if (b.empty()) {
      // start from the middle
      return std::string("a") + KBase62Digits[0];
    }

    std::string ib = getIntegerPart(b);
    std::string fb = b.substr(ib.length());
    if (ib == kMinSentinel) {
      if (fb.empty()) {
        return "";
      }

      return ib + midpoint("", fb);
    }
    if (ib < b) {
      return ib;
    }

    // Return the value before b's integer part
    return decrementInteger(ib);
  }

  if (b.empty()) {
    std::string ia = getIntegerPart(a);
    std::string fa = a.substr(ia.length());
    std::string res = incrementInteger(ia);
    return res.empty() ? ia + midpoint(fa, "") : res;
  }

  std::string ia = getIntegerPart(a);
  std::string fa = a.substr(ia.length());
  std::string ib = getIntegerPart(b);
  std::string fb = b.substr(ib.length());
  if (ia == ib) {
    std::string mid = midpoint(fa, fb);
    if (mid.empty()) {
      return "";
    }

    return ia + mid;
  }
  std::string i = incrementInteger(ia);
  if (i.empty()) {
    return "";
  }

  if (i < b) {
    return i;
  }

  return ia + midpoint(fa, "");
}

}  // namespace base

// int main() {
//   using namespace base;
//   std::string a = "PbVtCX7D1VfN0";
//   std::string b = "wKZ9wPvn5Cjm2iYGnPrGH2oODC34XOmV";
//   std::string result = generateKeyBetween(a, b);
//   std::cout << result << std::endl;
//   return 0;
// }