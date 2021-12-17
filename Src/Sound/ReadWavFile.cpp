#include "ReadWavFile.h"

#include <stdio.h>
#include <tchar.h>

#define	RIFFCC( _x )	(((_x >> 24) & 0xFF) + ((_x >> 8) & 0xFF00) + ((_x << 8) & 0xFF0000) + ((_x << 24) & 0xFF000000))

bool ReadWavFile::Open(const std::wstring & fileName)
{
    DWORD offset = 0;
    auto file_name = const_cast<WCHAR*>((fileName).c_str());

    //! 既にファイル・オープン中の場合、エラーにする.
    if (hFile) {
        return FALSE;
    }
    //! 引数が不正な場合、エラーにする.
    if (!file_name || _tcslen(file_name) == 0) {
        return FALSE;
    }
    //! ファイルを読み込みモードでオープンする.
    hFile = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        hFile = NULL;
        return FALSE;
    }

    //! ファイルヘッダを解析する.
    do {
        const int CHECK_LENGTH = 256;
        //! ファイルの先頭からストリームを読み込む.
        BYTE	buf[CHECK_LENGTH];
        auto len = GetData(buf, CHECK_LENGTH);
        if (len == 0) break;

        //! ファイルヘッダを取り出す.
        Header = *((RIFFHEADER*)&buf[offset]);
        offset += sizeof(Header);

        //! RIFFヘッダを調べる.
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
                //! フォーマットヘッダを調べる.
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
                //! ファクトリチャンクを取り出す.
                WAVEFACTCHUNK fact;
                fact = *((WAVEFACTCHUNK*)&buf[offset]);
                if (fact.Id != RIFFCC('fact')) {
                    fact.Samples = 0;
                }
            }
            else if (chunk.ID == RIFFCC('data'))
            {
                //! データヘッダを調べる.
                if (chunk.Size == 0) {
                    throw "wav data size error";
                }
                break;
            }
            offset += chunk.Size;
        }

        //! ファイルから読み込んだデータが少ない場合、エラーにする.
        if (len <= offset) {
            break;
        }
        //! ファイルの読み込み位置を先頭フレームに合わせておく.
        LARGE_INTEGER pos1, pos2;
        pos1.QuadPart = offset;
        SetFilePointerEx(hFile, pos1, &pos2, FILE_BEGIN);

        auto format = GetFormat();
        m_WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
        m_WaveFormat.nChannels = format.Channels;
        m_WaveFormat.nSamplesPerSec = format.SamplesPerSec;
        m_WaveFormat.wBitsPerSample = format.BitsPerSample;
        m_WaveFormat.nBlockAlign = m_WaveFormat.wBitsPerSample * m_WaveFormat.nChannels / 8;
        m_WaveFormat.nAvgBytesPerSec = m_WaveFormat.nSamplesPerSec * m_WaveFormat.nBlockAlign;

        return TRUE;

    } while (FALSE);

    //! エラーの場合、ファイルをクローズして終了する.
    Dispose();
    return FALSE;
}

void ReadWavFile::Dispose()
{
    if (hFile) {
        //! オープン中のファイルがある場合、ファイルをクローズする.
        CloseHandle(hFile);
    }
    hFile = NULL;
}

unsigned long ReadWavFile::GetData(unsigned char* data, unsigned long size)
//DWORD ReadWavFile::ReadWaveFile(void * data, DWORD size)
{
    DWORD len = 0;

    //! ファイルからデータを読み込む.
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

