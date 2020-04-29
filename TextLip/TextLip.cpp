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
    { "ー", 3 },
    { "っ", 3 },
};

// TODO: 口の形をそのままに
vector<string> wait = { "ー", "っ" };

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
void oneFrame(char* morfName, int keyframe, float weight)
{
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

    keyFrame += 6;
    oneFrame(morfName, keyFrame, 0);
}

int countnn(const string& str)
{
    int ret = 0;
    char nn = char(0xf1);
    for (unsigned int i = 0; i<str.size()-2; i += 2)
    {
        string search(str.begin()+i, str.begin() + i+2);
        if (exclude.find(search) != exclude.end())
        {
            ret++;
        }
    }
    return ret;
}

bool Coitain(const string& src, const string& find)
{
    return find.find(src) != string::npos;
}

string oneWord(const string& word)
{
    string ret;
    /// 英字
    bool hit = false;
    for (auto c : word)
    {
        if (c >= 0x3A && c <= 0x7A)
        {
            hit = true;
            ret += ConvertAlphabetAndSymbol(c);
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
        buf = oneWord(buf);
        text.append(buf);
    }

    writer.Open("text.vmd");
    writer.Write(header);
    char modelName[20] = "初音ミク";
    writer.Write(modelName, MODEL_NAME_SIZE);

    int boneNum = 0;
    writer.Write(boneNum);

    // キー数： 文字数 * 3
    int charNum = text.size() / 2;
    writer.Write((charNum - countnn(text)) * 3);

    int keyFrame = 2+4;
    for (int i = 0; i < charNum; i++)
    {
        string search(text.begin() + i*2, text.begin() + i*2 + 2);
        if (exclude.find(search) != exclude.end())
        {
            keyFrame += exclude[search];
            continue;
        }

        char morfName[MORF_NAME_SIZE] = { text[i*2], text[i*2+1] };
        for (int j = 0; j < 5; j++)
        {
            if (Coitain(string(morfName), baseStr[j]) == true)
            {
                morfName[1] = char(0xa0 + j*2);
                break;
            }
        }

        string next;
        if (i != charNum - 1)
        {
            next = string(text.begin() + (i + 1) * 2, text.begin() + (i + 1) * 2 + 2);
        }
        oneMove(morfName, keyFrame, next);
    }


    // footer カメラ等
    writer.Write(0);
    writer.Write(0);
    writer.Write(0);

    return 0;
}
