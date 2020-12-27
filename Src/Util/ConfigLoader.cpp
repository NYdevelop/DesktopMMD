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
    if (last == string::npos)
    {
        result.emplace_back(src);
        return result;
    }


    while (first < src.size()) {
        std::string subStr(src, first, last - first);

        result.emplace_back(subStr);

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
        if (lineStr.size() == 0)
        {
            continue;
        }

        auto splitVec = Split(lineStr, '=');
        if (splitVec.size() > 1)
        {
            splitVec[0] = RemoveSpace(splitVec[0]);
            if (splitVec[0] == keyWord)
            {

                auto splitVecQuat = Split(splitVec[1], '\"');
                if (splitVecQuat.size() < 2)
                {
                    // ダブルクォーテーションない場合
                    ret = RemoveSpace(splitVec[1]);
                    break;
                }

                // ダブルクォーテーション内のみRemoveSpaceしない
                ret = RemoveSpace(splitVecQuat[0]);
                ret += "\"" + splitVecQuat[1] + "\"";
                if (splitVecQuat.size() >= 3)
                {
                    ret += RemoveSpace(splitVecQuat[2]);
                }
                break;

            }
        }
    }

    // ダブルクォーテーション除去
    auto pos = ret.find("\"");
    if (m_IsRemoveDoubleQuatation && pos == 0 && ret.find("\"", pos + 1) == ret.length()-1)
    {
        ret = ret.erase(0, 1);
        ret = ret.erase(ret.length() - 1, 1);
    }

    return ret;
}

void CConfigLoader::SetRemoveDoubleQuatation(bool isRemove)
{
    m_IsRemoveDoubleQuatation = isRemove;
}

std::string CConfigLoader::RemoveSpace(const std::string & str)
{
    std::string ret(str);
    // スペース削除
    auto pos = ret.find(" ");
    while (pos != string::npos)
    {
        ret = ret.substr(0, pos) + ret.substr(pos + 1, ret.size());
        pos = ret.find(" ");
    }
    // tab削除
    pos = ret.find("	");
    while (pos != string::npos)
    {
        ret = ret.substr(0, pos) + ret.substr(pos + 1, ret.size());
        pos = ret.find("	");
    }
    return ret;
}
