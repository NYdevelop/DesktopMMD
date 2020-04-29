#include "ConvertJapaneseChar.h"

#include <vector>
#include <cmath>

using namespace std;

const vector<string> ONE_NUM_CALL =
{ "����", "����", "��", "����", "���", "��", "�낭", "�Ȃ�", "�͂�", "���イ" };

const vector<string> ALPHABET =
{
    "���[", "�с[", "���[", "�ł��[", "���[", "����", "���[", "������", "����", "������",
    "���[", "����", "����", "����", "���[", "�ҁ[", "����[", "���[��", "����", "�Ă��[",
    "��[", "�Ԃ�", "��������", "���Ԃ�イ", "�킢", "������"
};

const vector<string> SYMBLE0 =
{ "�����", "���݂����", "���傤�Ȃ�",  "�����[��",  "�����Ȃ�",  "",  "�����Ƃ܁[��" };

const vector<string> SYMBLE1 =
{ "������", "", "", "�͂���", "���񂾁[�΁[", "" };


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
        ret += "�܂��Ȃ�";
        value *= -1;
    }

    float integral_part;
    float fractional_part = std::modf(value, &integral_part);
    auto i = (int)integral_part;

    // ��̈ʂ܂ŕϊ�
    auto s = i / 1000;
    ret += Add(s, "����");

    auto h = i / 100 - s * 10;
    ret += Add(h, "�ЂႭ");

    auto j = i / 10 - s * 100 - h * 10;
    ret += Add(j, "���イ");

    auto it = i % 10;
    ret += ConvertUnderTen(it);


    // ����
    // ��2�ʂ܂ŕϊ�
    value -= integral_part;
    i = static_cast<int>(value * 100);
    if (i == 0)
    {
        return ret;
    }
    ret += "�Ă�";

    j = i / 10;
    ret += ConvertUnderTen(j);

    it = i % 10;
    if (it != 0)
    {
        ret += "�[" + ConvertUnderTen(it);
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
