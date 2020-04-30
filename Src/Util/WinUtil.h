#pragma once

#include <string>


std::wstring StringToWString(std::string const& src);

std::string WStringToString(std::wstring const& src);

bool IsPress(int key);