#pragma once

#include <Windows.h>
#include "WaveFile.h"

class ReadWavFile
{
public:
    /*!
    *	@brief	�t�@�C����ǂݍ��݃��[�h�ŃI�[�v������.
    *
    *	@param	[in]	file_name	�t�@�C�����̃|�C���^.
    *
    *	@retval	TRUE = OK. / FALSE = NG.
    */
    BOOL ReadOpenWaveFile(LPCTSTR file_name);

    void CloseWaveFile(void);

    /*!
    *	@brief	�t�@�C������WAVE�f�[�^��ǂݍ���.
    *
    *	@param	[in]	data	WAVE�f�[�^���i�[����̈�̃|�C���^.
    *	@param	[in]	size	WAVE�f�[�^���i�[����̈�̃o�C�g��.
    *
    *	@retval	�t�@�C������ǂݍ��񂾃o�C�g��.
    */
    DWORD	ReadWaveFile(void* data, DWORD size);
    WAVFILEHEADER GetHeader();

private:
    HANDLE hFile = NULL;          //!< �t�@�C���E�n���h��.
    WAVFILEHEADER Header = { 0 }; //!< WAVE�t�@�C���̃w�b�_�\����.
};