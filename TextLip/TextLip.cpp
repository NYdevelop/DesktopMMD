#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <map>

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

vector<string> wait = { "ー", "っ" };

vector<string> baseStr =
{
    "かさたなはまやらわがざだばぱゃぁ",
    "きしちにひみりぎじぢびぴぃ",
    "くすつぬふむゆるぐずづぶぷゅぅ",
    "けせてねへめれげぜでべぺぇ",
    "こそとのほもよをろをごぞどぼぽょぉ"
};
#pragma endregion

#pragma region Func
void outputBinary(ofstream& ofs, int data)
{
    ofs.write(reinterpret_cast<char *>(&data), sizeof(int));
}

void outputBinary(ofstream& ofs, float data)
{
    ofs.write(reinterpret_cast<char *>(&data), sizeof(float));
}

void oneFrame(ofstream& ofs, char* morfName, int keyframe, float weight)
{
    ofs.write(morfName, 15);

    // keyframe
    outputBinary(ofs, keyframe);

    // weight
    outputBinary(ofs, weight);
}

void oneMove(ofstream& ofs, char* morfName, int& keyFrame, const string& next)
{
    if (morfName[1] == char(0xa6)) // 「え」の場合
    {
        morfName[1] = char(0xa2);
        oneMove(ofs, morfName, keyFrame, next);
        return;
    }


    if (keyFrame >= 4)
    {
        keyFrame -= 4;
    }
    oneFrame(ofs, morfName, keyFrame, 0);

    keyFrame += 4;
    float weight = 1.f;
    if (morfName[1] == char(0xa0)) // 「あ」の場合
        weight = 0.5f;
    oneFrame(ofs, morfName, keyFrame, weight);

    keyFrame += 6;
    oneFrame(ofs, morfName, keyFrame, 0);
}
#pragma endregion

int countnn(const string& str)
{
    int ret = 0;
    char nn = char(0xf1);
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

bool Coitain(char* c, const string& v)
{
    return find_if(
        v.begin(),
        v.end(),
        [c](char x) 
    {
        return x == c[1];
    }) != v.end();
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        return 0;
    }
    ifstream ifs(argv[1]);

    string buf;
    string text("");
    while (!ifs.eof())
    {
        getline(ifs, buf);
        if (buf == "") break;
        if (string(buf.end() - 3, buf.end() - 1) != "。")
        {
            buf += "。";
        }
        text += buf;
    }

    ofstream ofs("text.vmd", ios::binary);

    std::copy(header.begin(), header.end(), std::ostream_iterator<char>(ofs));
    char modelName[20] = "初音ミク";
    ofs.write(modelName, 20);

    int boneNum = 0;
    outputBinary(ofs, boneNum);

    //for (auto v : exclude)
    //{
    //    size_t pos = text.find(v);
    //    while (pos != string::npos)
    //    {
    //        text.erase(pos, 2);
    //        pos = text.find(v);
    //    }
    //}

    // キー数： 文字数 * 3
    int charNum = text.size() / 2;
    outputBinary(ofs, (charNum - countnn(text)) * 3);

    // 各行の基準データ生成
    //vector< vector<unsigned char> > aa(5);
    //aa[0] = a;
    //for (int i = 0; i<4; i++)
    //{
    //    auto& row0 = aa[i];
    //    auto& row1 = aa[i+1];
    //    for (int j = 0; j<8; j++)
    //    {
    //        row1.emplace_back(row0[j] + 2);
    //    }
    //}
    //aa[4].emplace_back(0xf0); // 「を」

    int keyFrame = 2+4;
    for (int i = 0; i < charNum; i++)
    {
        string search(text.begin() + i*2, text.begin() + i*2 + 2);
        if (exclude.find(search) != exclude.end())
        {
            keyFrame += exclude[search];
            continue;
        }

        char morfName[15] = { text[i*2], text[i*2+1] };
        for (int j = 0; j < 5; j++)
        {
            if (Coitain(morfName, baseStr[j]) == true)
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
        oneMove(ofs, morfName, keyFrame, next);
    }


    // footer カメラ等
    outputBinary(ofs, 0);
    outputBinary(ofs, 0);
    outputBinary(ofs, 0);

    return 0;
}
