#include "WinUtil.h"

#include <Windows.h>

#include <vector>
#include <system_error>

using namespace std;

std::wstring StringToWString(std::string const& src)
{
    auto const dest_size = ::MultiByteToWideChar(CP_ACP, 0U, src.data(), -1, nullptr, 0U);
    std::vector<wchar_t> dest(dest_size, L'\0');
    if (::MultiByteToWideChar(CP_ACP, 0U, src.data(), -1, dest.data(), dest_size) == 0) {
        MessageBox(NULL, TEXT("StringToWString error. GetLastError: " + static_cast<int>(::GetLastError())), NULL, MB_ICONERROR);
        // throw std::system_error{ static_cast<int>(::GetLastError()), std::system_category() };
        return nullptr;
    }
    dest.resize(std::char_traits<wchar_t>::length(dest.data()));
    dest.shrink_to_fit();
    return std::wstring(dest.begin(), dest.end());
}


std::string WStringToString(std::wstring const& src)
{
    UINT codepage = 932;
    // wstring → SJIS
    const auto dest_size = WideCharToMultiByte(codepage, 0, src.data(), -1, nullptr, 0, NULL, NULL);

    // バッファの取得
    // CHAR* cpMultiByte = new CHAR[iBufferSize];
    vector<char> dest(dest_size, '\0');

    // wstring → SJIS
    //WideCharToMultiByte(CP_OEMCP, 0, src.c_str(), -1, cpMultiByte, iBufferSize, NULL, NULL);
    if (0 == WideCharToMultiByte(codepage, 0, src.data(), -1, dest.data(), dest_size, NULL, NULL))
    {
        MessageBox(NULL, TEXT("StringToWString error. GetLastError: " + static_cast<int>(::GetLastError())), NULL, MB_ICONERROR);
        //throw std::system_error{ static_cast<int>(::GetLastError()), std::system_category() };
        return nullptr;
    }

    dest.resize(std::char_traits<char>::length(dest.data()));
    dest.shrink_to_fit();

    // 変換結果を返す
    return string(dest.begin(), dest.end());
}


bool IsPress(int key)
{
    return GetKeyState(key) & 0x8000;
}