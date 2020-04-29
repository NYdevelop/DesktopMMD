#include "ConvertJapaneseChar.h"

#include <vector>
#include <cmath>

using namespace std;

const vector<string> ONE_NUM_CALL =
{ "ぜろ", "いち", "に", "さん", "よん", "ご", "ろく", "なな", "はち", "きゅう" };

const vector<string> ALPHABET =
{
    "えー", "びー", "しー", "でぃー", "いー", "えふ", "じー", "えいち", "あい", "じぇい",
    "けー", "える", "えむ", "えぬ", "おー", "ぴー", "きゅー", "あーる", "えす", "てぃー",
    "ゆー", "ぶい", "えっくす", "だぶりゅう", "わい", "ぜっと"
};

const vector<string> SYMBLE0 =
{ "ころん", "せみころん", "しょうなり",  "いこーる",  "だいなり",  "",  "あっとまーく" };

const vector<string> SYMBLE1 =
{ "かっこ", "", "", "はっと", "あんだーばー", "" };


std::string ConvertUnderTen(int underTen)
{
    if (underTen >= 10)
    {
        throw "over 10";
    }

    return ONE_NUM_CALL[underTen];
}

std::string Add(int value, string kurai)
{
    string ret;
    if (value >= 2)
    {
        ret += ConvertUnderTen(value);
    }
    if (value >= 1)
    {
        ret += kurai;
    }
    return ret;
}

std::string Convert(float value)
{
    string ret;
    if (value < 0)
    {
        ret += "まいなす";
        value *= -1;
    }

    float integral_part;
    float fractional_part = std::modf(value, &integral_part);
    auto i = (int)integral_part;

    // 千の位まで変換
    auto s = i / 1000;
    ret += Add(s, "せん");

    auto h = i / 100 - s * 10;
    ret += Add(h, "ひゃく");

    auto j = i / 10 - s * 100 - h * 10;
    ret += Add(j, "じゅう");

    auto it = i % 10;
    ret += ConvertUnderTen(it);


    // 少数
    // 第2位まで変換
    value -= integral_part;
    i = static_cast<int>(value * 100);
    if (i == 0)
    {
        return ret;
    }
    ret += "てん";

    j = i / 10;
    ret += ConvertUnderTen(j);

    it = i % 10;
    if (it != 0)
    {
        ret += "ー" + ConvertUnderTen(it);
    }


    return ret;
}

std::string ConvertAlphabetAndSymbol(char value)
{
    vector<string> ALPHABET_SYMBLE = SYMBLE0;
    std::copy(ALPHABET.begin(), ALPHABET.end(), std::back_inserter(ALPHABET_SYMBLE));
    std::copy(SYMBLE1.begin(), SYMBLE1.end(), std::back_inserter(ALPHABET_SYMBLE));
    std::copy(ALPHABET.begin(), ALPHABET.end(), std::back_inserter(ALPHABET_SYMBLE));

    return ALPHABET_SYMBLE[value - 0x3A];
}
