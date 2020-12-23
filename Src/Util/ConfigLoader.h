#ifndef _CONFIG_LOADER_H_
#define _CONFIG_LOADER_H_

// 2018/06/29

#include <string>

class CConfigLoader                             // 設定ファイル読み込みクラス
{
public:
    CConfigLoader(const std::string& fileName);   // コンストラクタ
    ~CConfigLoader();                             // デストラクタ

    std::string Load(const std::string& keyWord); // キーワード文字列読み込み

private:
    std::string m_FileName;                       // 設定ファイル名
};

#endif
