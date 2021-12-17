#pragma once

#include <Windows.h>
#include <string>

class IReadFile
{
public:
    virtual bool Open(const std::wstring & fileName) = 0;
    virtual unsigned long GetData(unsigned char* data, unsigned long size) = 0;
    virtual void Dispose() = 0;

    WAVEFORMATEX m_WaveFormat;
    unsigned long m_Size;
};