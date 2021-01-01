#include "WinUtil.h"

#include <vector>
#include <system_error>

using namespace std;

std::wstring StringToWString(std::string const& src, unsigned int charCode)
{
    const size_t len = src.size() + 1;
    TCHAR* tmp = new TCHAR[len];
    MultiByteToWideChar(charCode, 0, src.c_str(), -1, tmp, len);
    std::wstring ret(tmp);
    delete[] tmp;
    return ret;
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
    return (GetKeyState(key) & 0x8000) > 0;
}


#include "UtilXml.h"
std::tuple<HMENU, ULONG, UINT, std::wstring> LoadContextNode(rapidxml::xml_node<>* node, HMENU & context)
{
    std::wstring menuName(StringToWString(GetAttribute(node, "name")));
    UINT menuNum = std::stoi(GetAttribute(node, "num"));
    return std::tuple<HMENU, ULONG, UINT, std::wstring>(context, MF_STRING, menuNum, menuName);
}

void LoadContextNode(rapidxml::xml_node<>* node, HMENU context, std::vector<std::tuple<HMENU, ULONG, UINT, std::wstring>>& config, std::tuple<HMENU, ULONG, UINT, std::wstring>* sub)
{
    if (sub != nullptr)
    {
        std::get<1>(*sub) = MF_POPUP;
        auto subMenu = CreatePopupMenu();
        std::get<2>(*sub) = (UINT)subMenu;
        config.emplace_back(*sub);
        context = subMenu;
    }

    for (rapidxml::xml_node<>* child = node;
        child != nullptr;
        child = child->next_sibling())
    {
        auto info = LoadContextNode(child, context);

        // さらに子がいるかどうか
        auto childSub = child->first_node();
        if (childSub != nullptr)
        {
            LoadContextNode(childSub, context, config, &info);
            continue;
        }

        config.emplace_back(info);
    }
}