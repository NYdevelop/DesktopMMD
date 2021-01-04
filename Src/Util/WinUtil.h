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

std::vector<std::string> LoadContextNode(
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


// コンテキストメニューチェック関数のラッパー関数
inline DWORD ContextItemCheck(UINT id, HMENU contextMenu, bool isCheck)
{
    return CheckMenuItem(contextMenu, id, MF_BYCOMMAND | (isCheck ? MFS_CHECKED : MFS_UNCHECKED));
};

#include <functional>
// コンテキストメニューチェック実装関数
void ContextCheckFunc(
    UINT id,
    HMENU contextMenu,
    const std::function<void(bool)>& callbackFunc); // チェック時のコールバック関数
