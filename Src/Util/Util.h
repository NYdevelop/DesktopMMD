#pragma once

#include <string>

template<class T>
inline T String2Numeric(const std::string& str) {};// { _ASSERT_EXPR(true, "type error."); return T(0); };

template<>
inline unsigned int String2Numeric<unsigned int>(const std::string& str) { return std::stoi(str.c_str()); };

template<>
inline int String2Numeric<int>(const std::string& str) { return std::stoi(str.c_str()); };

template<>
inline float String2Numeric<float>(const std::string& str) { return std::stof(str.c_str()); };

template<>
inline double String2Numeric<double>(const std::string& str) { return std::stod(str.c_str()); };
