#pragma once

#include <string>

template<typename T>
T String2Numeric(const std::string& str) { _ASSERT("type error.") }

template<> int String2Numeric(const std::string& str) { return std::stoi(str.c_str()); }
template<> double String2Numeric(const std::string& str) { return std::stof(str.c_str()); }
