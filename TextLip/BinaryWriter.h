#pragma once

#include <string>
#include <fstream>
#include <vector>

class BinaryWriter
{
public:
    void Open(const std::string& fileName)
    {
        ofs.open(fileName, std::ios::binary);
    }

    template<class T>
    void Write(T value)
    {
        ofs.write(reinterpret_cast<char*>(&value), sizeof(T));
    }

    void Write(char* value, size_t size)
    {
        ofs.write(value, size);
    }

    void Write(const std::vector<unsigned char>& vec)
    {
        std::copy(vec.begin(), vec.end(), std::ostream_iterator<char>(ofs));
    }



private:
    std::ofstream ofs;
};