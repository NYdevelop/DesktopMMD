#pragma once

#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tuple>
#include "Util\rapidxml-1.13\rapidxml.hpp"
#include <vector>


std::wstring StringToWString(std::string const& src, unsigned int charCode = 65001);

std::string WStringToString(std::wstring const& src);

bool IsPress(int key);


std::tuple<HMENU, ULONG, UINT, std::wstring> LoadContextNode(rapidxml::xml_node<>* node, HMENU& context);

void LoadContextNode(
    rapidxml::xml_node<>* node,
    HMENU context,
    std::vector<std::tuple<HMENU, ULONG, UINT, std::wstring>>& config,
    std::tuple<HMENU, ULONG, UINT, std::wstring>* sub = nullptr);

template <typename Enumeration>
auto as_integer(Enumeration const value)
-> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}