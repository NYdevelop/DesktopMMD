#include "ReadWavFile.h"

#include <stdio.h>
#include <tchar.h>

#define	RIFFCC( _x )	(((_x >> 24) & 0xFF) + ((_x >> 8) & 0xFF00) + ((_x << 8) & 0xFF0000) + ((_x << 24) & 0xFF000000))

BOOL ReadWavFile::ReadOpenWaveFile(LPCTSTR file_name)
{
    DWORD offset = 0;

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
        //! ファイルの先頭からストリームを読み込む.
        BYTE	buf[256];
        auto len = ReadWaveFile(buf, 256);
        if (len == 0) break;

        //! ファイルヘッダを取り出す.
        Header = *((WAVFILEHEADER*)&buf[offset]);
        offset += sizeof(Header);

        //! RIFFヘッダを調べる.
        if ((Header.RiffId != RIFFCC('RIFF'))
            || (Header.FileType != RIFFCC('WAVE'))
            || (Header.FileSize <= (sizeof(Header) - 8))) {
            break;
        }
        //! フォーマットヘッダを調べる.
        if ((Header.FormatId != RIFFCC('fmt '))
            || (Header.Channels == 0)
            || (Header.SamplesPerSec == 0)
            || (Header.AvgBytesPerSec == 0)
            || (Header.BlockAlign == 0)
            || (Header.BitsPerSample == 0)
            || (Header.FormatSize     <  16)) {
            break;
        }
        for (; offset < 256 && buf[offset] == 0; offset++);

        //! ファクトリチャンクを取り出す.
        WAVEFACTCHUNK fact;
        fact = *((WAVEFACTCHUNK*)&buf[offset]);
        if (fact.Id == RIFFCC('fact')) {
            offset += sizeof(WAVEFACTCHUNK);
        }
        else {
            fact.Samples = 0;
        }
        for (; offset < 256 && buf[offset] == 0; offset++);

        //! データヘッダを取り出す.
        WAVEDATACHUNK data;
        data = *((WAVEDATACHUNK*)&buf[offset]);
        offset += sizeof(WAVEDATACHUNK);

        //! データヘッダを調べる.
        if (data.Id != RIFFCC('data') || data.Size == 0) {
            break;
        }
        //! ファイルから読み込んだデータが少ない場合、エラーにする.
        if (len <= offset) {
            break;
        }
        //! ファイルの読み込み位置を先頭フレームに合わせておく.
        LARGE_INTEGER pos1, pos2;
        pos1.QuadPart = offset;
        SetFilePointerEx(hFile, pos1, &pos2, FILE_BEGIN);

        return TRUE;

    } while (FALSE);

    //! エラーの場合、ファイルをクローズして終了する.
    CloseWaveFile();
    return FALSE;
}

void ReadWavFile::CloseWaveFile(void)
{
    if (hFile) {
        //! オープン中のファイルがある場合、ファイルをクローズする.
        CloseHandle(hFile);
    }
    hFile = NULL;
}

DWORD ReadWavFile::ReadWaveFile(void * data, DWORD size)
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

WAVFILEHEADER ReadWavFile::GetHeader()
{
    return Header;
}

