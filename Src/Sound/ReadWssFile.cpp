#include "ReadWssFile.h"
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;


#define LOG10	2.3025850929940456840	//ln(10)
#define LOG2	1.4426950408889634070	//log2(e)
#define MAGIC_NUMBER	((LOG10*LOG2)/28.12574042515172)

short* DeByteCompress(const char *CompressedData, int len, std::vector<char>& buf)
{
    short *snap, *OutputData;

    if (!(snap = OutputData = new short[len]))  return 0;
    short LastVal = 0;
    for (; len--; CompressedData++)
    {
        if (*CompressedData)
        {
            double asFloat = abs(*CompressedData) *MAGIC_NUMBER;
            double rnd = round(asFloat);
            asFloat = pow(2.0, asFloat - rnd) * pow(2, rnd);
            if (*CompressedData < 0) asFloat *= -1;
            int asInt = round(asFloat) + LastVal;
            if (asInt > SHRT_MAX) asInt = SHRT_MAX;
            if (asInt < SHRT_MIN) asInt = SHRT_MIN;
            LastVal = (short)asInt;
        }
        *OutputData = LastVal;
        char up = *OutputData >> 8;
        char dn = *OutputData;
        buf.emplace_back(dn);
        buf.emplace_back(up);
        OutputData++;
    }
    return snap;
}


bool ReadWssFile::Open(const std::wstring & file_name)
{
    index = 0;
    ifstream ifs(file_name, ios::binary | ios::in);

    ifs.seekg(0, std::ios_base::end);
    auto filesize = ifs.tellg();
    ifs.seekg(0, std::ios_base::beg);

    auto size = static_cast<unsigned int>(filesize) - sizeof(WSSHEADER);
    vector<char> tmp(size);

    ifs.read(&(tmp[0]), sizeof(WSSHEADER));
    header = *((WSSHEADER*)&tmp[0]);
    //if (header.Size == 0)
    //{
        header.Size = size;
    //}
    ifs.read(&(tmp[0]), header.Size);

    DeByteCompress(&(tmp[0]), header.Size, buf);
    header.Size = buf.size();
    m_Size = header.Size;

    m_WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    m_WaveFormat.nChannels = 1;//header.Channels;
    m_WaveFormat.nSamplesPerSec = header.SamplesPerSec;
    m_WaveFormat.wBitsPerSample = header.BytesPerSecond * 8 / header.SamplesPerSec;
    m_WaveFormat.nBlockAlign = m_WaveFormat.wBitsPerSample * m_WaveFormat.nChannels / 8;
    m_WaveFormat.nAvgBytesPerSec = m_WaveFormat.nSamplesPerSec * m_WaveFormat.nBlockAlign;

    currentPtr = buf.data();
    return true;
}

void ReadWssFile::Dispose()
{
}

unsigned long ReadWssFile::GetData(unsigned char* data, unsigned long size)
{
    unsigned long ret = size;
    if (index >= buf.size())
    {
        return 0;
    }

    memcpy(data, currentPtr, size);
    if (size > buf.size())
    {
        ret = buf.size();
    }
    index += ret;
    return ret;
}
