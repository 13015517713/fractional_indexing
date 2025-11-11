#pragma once

#include <string>

namespace base {

const std::string KBase62Digits =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

std::string generateKeyBetween(const std::string& a, const std::string& b);

}  // namespace base