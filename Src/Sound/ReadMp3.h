#pragma once

#include "ReadFile.h"

#include <mmreg.h>

// ref: http://eternalwindows.jp/winmm/mp3/mp300.html
class ReadMp3 : public IReadFile
{
public:
    struct FRAMEHEADER {
        BYTE  version;
        DWORD dwBitRate;
        DWORD dwSampleRate;
        BYTE  padding;
        BYTE  channel;
        WORD  wFrameSize;
    };
    typedef struct FRAMEHEADER FRAMEHEADER;
    typedef struct FRAMEHEADER *LPFRAMEHEADER;

    struct ID3V2HEADER {
        BYTE id[3];
        BYTE version[2];
        BYTE flag;
        BYTE size[4];
    };
    typedef struct ID3V2HEADER ID3V2HEADER;
    typedef struct ID3V2HEADER *LPID3V2HEADER;

    bool Open(const std::wstring& fileName);

    unsigned long GetData(unsigned char* data, unsigned long size);
    void Dispose();

private:
    BOOL DecodeToWave(LPWAVEFORMATEX lpwfSrc, LPBYTE lpSrcData, DWORD dwSrcSize, LPWAVEFORMATEX lpwfDest);

    BOOL GetFrameHeader(LPBYTE lpData, LPFRAMEHEADER lpfh);

    void GetMp3Format(LPFRAMEHEADER lpfh);

    DWORD GetDecodeSize(LPBYTE lpMP3Data, DWORD dwMP3Size, LPWAVEFORMATEX lpwf);

    BOOL IsId3v2(LPBYTE lpData, DWORD dwDataSize, LPDWORD lpdwTagSize);

    BOOL ReadMP3File(const std::wstring& fileName, LPBYTE *lplpData, LPDWORD lpdwSize);

    
    MPEGLAYER3WAVEFORMAT m_Format;
    unsigned long m_DataSize = 0;
    unsigned long m_DataCount = 0;
    unsigned char *m_CurrnetPtr = 0;
    unsigned char *m_FirstPtr = 0;
};
