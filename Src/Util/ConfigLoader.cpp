#include "ConfigLoader.h"

// 2018/06/29

#include <fstream>
#include <iostream>

#include <vector>

using namespace std;

// 機能 : コンストラクタ
CConfigLoader::CConfigLoader(const std::string& fileName)
    :m_FileName(fileName)
{
}

// 機能 : デストラクタ
CConfigLoader::~CConfigLoader()
{
}

vector<string> Split(const string& src, char delim)
{
    size_t first = 0;
    int last = src.find_first_of(delim);

    std::vector<std::string> result;

    while (first < src.size()) {
        std::string subStr(src, first, last - first);

        result.push_back(subStr);

        first = last + 1;
        last = src.find_first_of(delim, first);

        if (last == std::string::npos)
        {
            last = src.size();
        }
    }

    return result;
}

// 機能 : キーワード文字列読み込み
std::string CConfigLoader::Load(const std::string& keyWord)
{
    ifstream ifs(m_FileName);
    string ret("");
    string lineStr;
    while (getline(ifs, lineStr))
    {
        // コメント削除
        auto pos = lineStr.find("//");
        if (pos != string::npos)
        {
            lineStr = lineStr.substr(0, pos);
        }
        // スペース削除
        pos = lineStr.find(" ");
        while (pos != string::npos)
        {
            lineStr = lineStr.substr(0, pos) + lineStr.substr(pos + 1, lineStr.size());
            pos = lineStr.find(" ");
        }
        // tab削除
        pos = lineStr.find("	");
        while (pos != string::npos)
        {
            lineStr = lineStr.substr(0, pos) + lineStr.substr(pos + 1, lineStr.size());
            pos = lineStr.find("	");
        }
        if (lineStr.size() == 0)
        {
            continue;
        }

        auto splitVec = Split(lineStr, '=');

        if (splitVec.size() > 1 && splitVec[0] == keyWord)
        {
            ret = splitVec[1];
            break;
        }
    }
    return ret;
}