#pragma once

#include <Windows.h>
#include "WaveFile.h"

class ReadWavFile
{
public:
    /*!
    *	@brief	ファイルを読み込みモードでオープンする.
    *
    *	@param	[in]	file_name	ファイル名のポインタ.
    *
    *	@retval	TRUE = OK. / FALSE = NG.
    */
    BOOL ReadOpenWaveFile(LPCTSTR file_name);

    void CloseWaveFile(void);

    /*!
    *	@brief	ファイルからWAVEデータを読み込む.
    *
    *	@param	[in]	data	WAVEデータを格納する領域のポインタ.
    *	@param	[in]	size	WAVEデータを格納する領域のバイト数.
    *
    *	@retval	ファイルから読み込んだバイト数.
    */
    DWORD	ReadWaveFile(void* data, DWORD size);
    WAVFILEHEADER GetHeader();

private:
    HANDLE hFile = NULL;          //!< ファイル・ハンドル.
    WAVFILEHEADER Header = { 0 }; //!< WAVEファイルのヘッダ構造体.
};