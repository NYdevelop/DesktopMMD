#include "ReadWavFile.h"

#include <stdio.h>
#include <tchar.h>

#define	RIFFCC( _x )	(((_x >> 24) & 0xFF) + ((_x >> 8) & 0xFF00) + ((_x << 8) & 0xFF0000) + ((_x << 24) & 0xFF000000))

BOOL ReadWavFile::ReadOpenWaveFile(LPCTSTR file_name)
{
    DWORD offset = 0;

    //! ���Ƀt�@�C���E�I�[�v�����̏ꍇ�A�G���[�ɂ���.
    if (hFile) {
        return FALSE;
    }
    //! �������s���ȏꍇ�A�G���[�ɂ���.
    if (!file_name || _tcslen(file_name) == 0) {
        return FALSE;
    }
    //! �t�@�C����ǂݍ��݃��[�h�ŃI�[�v������.
    hFile = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        hFile = NULL;
        return FALSE;
    }

    //! �t�@�C���w�b�_����͂���.
    do {
        const int CHECK_LENGTH = 256;
        //! �t�@�C���̐擪����X�g���[����ǂݍ���.
        BYTE	buf[CHECK_LENGTH];
        auto len = ReadWaveFile(buf, CHECK_LENGTH);
        if (len == 0) break;

        //! �t�@�C���w�b�_�����o��.
        Header = *((RIFFHEADER*)&buf[offset]);
        offset += sizeof(Header);

        //! RIFF�w�b�_�𒲂ׂ�.
        if ((Header.ID != RIFFCC('RIFF'))
            || (Header.FileType != RIFFCC('WAVE'))
            || (Header.Size <= (sizeof(Header) - 8))) {
            break;
        }

        while (offset < len)
        {
            RIFFCHUNCK chunk = *((RIFFCHUNCK*)&buf[offset]);
            offset += sizeof(chunk);

            if (chunk.ID == RIFFCC('fmt '))
            {
                Format = *((RIFFFORMATCHUNK*)&buf[offset]);
                //! �t�H�[�}�b�g�w�b�_�𒲂ׂ�.
                if ((Format.Channels == 0)
                    || (Format.SamplesPerSec == 0)
                    || (Format.AvgBytesPerSec == 0)
                    || (Format.BlockAlign == 0)
                    || (Format.BitsPerSample == 0))
                {
                    throw "wave format error";
                }
            }
            else if (chunk.ID == RIFFCC('fact'))
            {
                //! �t�@�N�g���`�����N�����o��.
                WAVEFACTCHUNK fact;
                fact = *((WAVEFACTCHUNK*)&buf[offset]);
                if (fact.Id != RIFFCC('fact')) {
                    fact.Samples = 0;
                }
            }
            else if (chunk.ID == RIFFCC('data'))
            {
                //! �f�[�^�w�b�_�𒲂ׂ�.
                if (chunk.Size == 0) {
                    throw "wav data size error";
                }
                break;
            }
            offset += chunk.Size;
        }

        //! �t�@�C������ǂݍ��񂾃f�[�^�����Ȃ��ꍇ�A�G���[�ɂ���.
        if (len <= offset) {
            break;
        }
        //! �t�@�C���̓ǂݍ��݈ʒu��擪�t���[���ɍ��킹�Ă���.
        LARGE_INTEGER pos1, pos2;
        pos1.QuadPart = offset;
        SetFilePointerEx(hFile, pos1, &pos2, FILE_BEGIN);

        return TRUE;

    } while (FALSE);

    //! �G���[�̏ꍇ�A�t�@�C�����N���[�Y���ďI������.
    CloseWaveFile();
    return FALSE;
}

void ReadWavFile::CloseWaveFile(void)
{
    if (hFile) {
        //! �I�[�v�����̃t�@�C��������ꍇ�A�t�@�C�����N���[�Y����.
        CloseHandle(hFile);
    }
    hFile = NULL;
}

DWORD ReadWavFile::ReadWaveFile(void * data, DWORD size)
{
    DWORD len = 0;

    //! �t�@�C������f�[�^��ǂݍ���.
    if (hFile) {
        if (ReadFile(hFile, data, size, &len, NULL)) {
            return len;
        }
    }
    return 0;
}

RIFFHEADER ReadWavFile::GetHeader()
{
    return Header;
}

RIFFFORMATCHUNK ReadWavFile::GetFormat()
{
    return Format;
}

