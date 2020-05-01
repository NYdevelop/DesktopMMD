#pragma once

#include <string>


std::wstring StringToWString(std::string const& src);

std::string WStringToString(std::wstring const& src);

bool IsPress(int key);


template <typename Enumeration>
auto as_integer(Enumeration const value)
-> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}