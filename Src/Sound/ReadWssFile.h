#pragma once

#include "ReadFile.h"
#include <vector>
#include "WssFile.h"

class ReadWssFile : public IReadFile
{
public:
    bool Open(const std::wstring& file_name);

    void Dispose();

    unsigned long GetData(unsigned char* data, unsigned long size);

private:
    std::vector<char> buf;
    WSSHEADER header;
    char* currentPtr;
    unsigned long index;
};