#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <map>

#include "BinaryWriter.h"
#include "ConvertJapaneseChar.h"

using namespace std;

#pragma region Define
vector<unsigned char> header =
{
    0x56, 0x6F, 0x63, 0x61, 0x6C, 0x6F, 0x69, 0x64, 0x20, 0x4D, 0x6F, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x44, 0x61, 0x74, 0x61, 0x20, 0x30, 0x30, 0x30, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00
};

map<string, int> exclude =
{
    { "ん", 6 },
    { "、", 23 },
    { "。", 38 },
};

// 口の形をそのままに
map<string, float> wait = 
{
    {"ー", 0.55f},
    {"っ", 0.5f},
};

vector<string> baseStr =
{
    "かさたなはまやらわがざだばぱゃぁゎ",
    "きしちにひみりぎじぢびぴぃゐ",
    "くすつぬふむゆるぐずづぶぷゅぅ",
    "けせてねへめれげぜでべぺぇゑ",
    "こそとのほもよをろをごぞどぼぽょぉ"
};

const unsigned int MORF_NAME_SIZE = 15;
const unsigned int MODEL_NAME_SIZE = 20;
#pragma endregion

BinaryWriter writer;

#pragma region Func
bool Coitain(const string& src, const string& find)
{
    return find.find(src) != string::npos;
}

void oneFrame(char* morfName, int keyframe, float weight)
{
    //cout << string(morfName) << ", frame: " << keyframe << ", weight: " << weight << endl;

    writer.Write(morfName, MORF_NAME_SIZE);

    // keyframe
    writer.Write(keyframe);

    // weight
    writer.Write(weight);
}

void oneMove(char* morfName, int& keyFrame, const string& next)
{
    if (morfName[1] == char(0xa6)) // 「え」の場合
    {
        morfName[1] = char(0xa2);
        oneMove(morfName, keyFrame, next);
        return;
    }


    if (keyFrame >= 4)
    {
        keyFrame -= 4;
    }
    oneFrame(morfName, keyFrame, 0);

    keyFrame += 4;
    float weight = 1.f;
    if (morfName[1] == char(0xa0)) // 「あ」の場合
        weight = 0.5f;
    oneFrame(morfName, keyFrame, weight);

    if (wait.find(next) != wait.end())
    {
        keyFrame += 6;
        weight = 1.f;
        if (morfName[1] == char(0xa0)) // 「あ」の場合
            weight = wait[next];
        oneFrame(morfName, keyFrame, weight);
    }

    keyFrame += 6;
    oneFrame(morfName, keyFrame, 0);
}

void oneChar(const string& chara, int& keyFrame, const string& nextChara)
{
    if (wait.find(chara) != wait.end())
    {
        return;
    }

    if (exclude.find(chara) != exclude.end())
    {
        keyFrame += exclude[chara];
        return;
    }


    char morfName[MORF_NAME_SIZE] = { chara[0], chara[1] };
    for (int j = 0; j < 5; j++)
    {
        if (Coitain(string(morfName), baseStr[j]) == true)
        {
            morfName[1] = char(0xa0 + j * 2);
            break;
        }
    }

    oneMove(morfName, keyFrame, nextChara);
}

int countnn(const string& str)
{
    int ret = 0;
    for (unsigned int i = 0; i<str.size(); i += 2)
    {
        string search(str.begin()+i, str.begin() + i+2);
        if (exclude.find(search) != exclude.end())
        {
            ret++;
        }
    }
    return ret;
}

int countWait(const string& str)
{
    int ret = 0;
    for (unsigned int i = 0; i < str.size() - 2; i += 2)
    {
        string search(str.begin() + i, str.begin() + i + 2);
        if (wait.find(search) != wait.end())
        {
            ret++;
        }
    }
    return ret;
}

/// trueならば2バイト文字
bool SJISMultiCheck(unsigned char c) {
    if (((c >= 0x81) && (c <= 0x9f)) || ((c >= 0xe0) && (c <= 0xfc)))
        return true;
    return false;
}

string ConvertOneWord(const string& word)
{
    string ret;
    /// 英字
    bool hit = false;
    for (unsigned int i = 0; i<word.size();)
    {
        if (!SJISMultiCheck(word[0]) && word[i] >= 0x3A && word[i] <= 0x7A)
        {
            // 半角文字
            hit = true;
            ret += ConvertAlphabetAndSymbol(word[i]);
            i++;
        }
        else
        {
            i += 2;
        }
    }
    if (hit == true)
    {
        return ret;
    }

    /// 数値
    try
    {
        auto i = stof(word);
        ret = Convert(i);
        return ret;
    }
    catch (const std::invalid_argument&)
    {
    }

    return word;
}
#pragma endregion

int main(int argc, char** argv)
{
    char tmp[] = { char(0x95), char(0x96) };
    string tmp2(tmp);
    if (argc != 2)
    {
        return 0;
    }
    ifstream ifs(argv[1]);

    /// 各単語処理
    string buf;
    string text("");
    while (!ifs.eof())
    {
        getline(ifs, buf);
        if (buf == "") break;

        /// 一文に連結
        buf = ConvertOneWord(buf);
        text.append(buf);
    }

    writer.Open("text.vmd");
    writer.Write(header);
    char modelName[MODEL_NAME_SIZE] = "初音ミク";
    writer.Write(modelName, MODEL_NAME_SIZE);

    int boneNum = 0;
    writer.Write(boneNum);

    // キー数： 文字数 * 3
    int charNum = text.size() / 2;
    int waitNum = countWait(text);
    writer.Write((charNum - countnn(text) - waitNum) * 3 + waitNum);

    int keyFrame = 0;// 2 + 4;
    for (int i = 0; i < charNum; i++)
    {
        string currentChar(text.begin() + i * 2, text.begin() + i * 2 + 2);
        if (i == charNum - 1)
        {
            oneChar(currentChar, keyFrame, string());
            continue;
        }
        string next(text.begin() + (i + 1) * 2, text.begin() + (i + 1) * 2 + 2);
        oneChar(currentChar, keyFrame, next);
    }


    // footer カメラ等
    writer.Write(0);
    writer.Write(0);
    writer.Write(0);

    //system("pause");

    return 0;
}
