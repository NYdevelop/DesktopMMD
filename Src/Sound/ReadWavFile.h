#pragma once

#include "ReadFile.h"
#include <vector>
#include "WaveFile.h"

class ReadWavFile : public IReadFile
{
public:
    bool Open(const std::wstring & fileName);
    void Dispose();

    /*!
    *	@brief	ファイルからWAVEデータを読み込む.
    *
    *	@param	[in]	data	WAVEデータを格納する領域のポインタ.
    *	@param	[in]	size	WAVEデータを格納する領域のバイト数.
    *
    *	@retval	ファイルから読み込んだバイト数.
    */
    unsigned long GetData(unsigned char* data, unsigned long size);
    RIFFHEADER GetHeader();
    RIFFFORMATCHUNK GetFormat();

private:
    HANDLE hFile = NULL;          //!< ファイル・ハンドル.
    RIFFHEADER Header = { 0 }; //!< WAVEファイルのヘッダ
    RIFFFORMATCHUNK Format = { 0 };
};