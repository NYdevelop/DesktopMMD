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
    *	@brief	�t�@�C������WAVE�f�[�^��ǂݍ���.
    *
    *	@param	[in]	data	WAVE�f�[�^���i�[����̈�̃|�C���^.
    *	@param	[in]	size	WAVE�f�[�^���i�[����̈�̃o�C�g��.
    *
    *	@retval	�t�@�C������ǂݍ��񂾃o�C�g��.
    */
    unsigned long GetData(unsigned char* data, unsigned long size);
    RIFFHEADER GetHeader();
    RIFFFORMATCHUNK GetFormat();

private:
    HANDLE hFile = NULL;          //!< �t�@�C���E�n���h��.
    RIFFHEADER Header = { 0 }; //!< WAVE�t�@�C���̃w�b�_
    RIFFFORMATCHUNK Format = { 0 };
};