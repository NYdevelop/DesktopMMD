#include "ReadMp3.h"

#include <msacm.h>

#pragma comment (lib, "msacm32.lib")

BOOL ReadMp3::DecodeToWave(LPWAVEFORMATEX lpwfSrc, LPBYTE lpSrcData, DWORD dwSrcSize, LPWAVEFORMATEX lpwfDest)
{
    //ACMFORMATTAGDETAILS tagDetails;
    //tagDetails.cbStruct = sizeof(ACMFORMATTAGDETAILS);
    //tagDetails.fdwSupport = 0;
    //tagDetails.dwFormatTag = WAVE_FORMAT_MPEGLAYER3;

    //if (acmFormatTagDetails(NULL, &tagDetails, ACM_FORMATTAGDETAILSF_FORMATTAG) != 0)
    //{
    //    MessageBox(NULL, TEXT("このフォーマットタグはサポートされていません。"), NULL, MB_ICONWARNING);
    //    return FALSE;
    //}

    HACMSTREAM      has;
    ACMSTREAMHEADER ash;
    LPBYTE          lpDestData;
    DWORD           dwDestSize;
    BOOL            bResult;

    lpwfDest->wFormatTag = WAVE_FORMAT_PCM;
    acmFormatSuggest(NULL, lpwfSrc, lpwfDest, sizeof(WAVEFORMATEX), ACM_FORMATSUGGESTF_WFORMATTAG);

    dwDestSize = GetDecodeSize(lpSrcData, dwSrcSize, lpwfDest);
    if (dwDestSize == 0)
    {
        return FALSE;
    }

    if (acmStreamOpen(&has, NULL, lpwfSrc, lpwfDest, NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME) != 0) {
        MessageBox(NULL, TEXT("変換ストリームのオープンに失敗しました。"), NULL, MB_ICONWARNING);
        return FALSE;
    }

    lpDestData = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, dwDestSize);

    ZeroMemory(&ash, sizeof(ACMSTREAMHEADER));
    ash.cbStruct = sizeof(ACMSTREAMHEADER);
    ash.pbSrc = lpSrcData;
    ash.cbSrcLength = dwSrcSize;
    ash.pbDst = lpDestData;
    ash.cbDstLength = dwDestSize;

    acmStreamPrepareHeader(has, &ash, 0);
    bResult = acmStreamConvert(has, &ash, 0) == 0;
    acmStreamUnprepareHeader(has, &ash, 0);

    acmStreamClose(has, 0);

    if (bResult) {
        m_CurrnetPtr = lpDestData;
        m_DataSize = ash.cbDstLengthUsed;
    }
    else {
        MessageBox(NULL, TEXT("変換に失敗しました。"), NULL, MB_ICONWARNING);
        m_CurrnetPtr = 0;
        m_DataSize = 0;
        HeapFree(GetProcessHeap(), 0, lpDestData);
    }

    return bResult;
}

BOOL ReadMp3::GetFrameHeader(LPBYTE lpData, LPFRAMEHEADER lpfh)
{
    BYTE  index;
    BYTE  version;
    BYTE  channel;
    BYTE  padding;
    WORD  wFrameSize;
    DWORD dwBitRate;
    DWORD dwSampleRate;
    DWORD dwBitTableLayer3[][16] = {
        { 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0 }, // MPEG1
        { 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0 } // MPEG2
    };
    DWORD dwSampleTable[][3] = {
        { 44100, 48000, 32000 }, // MPEG1
        { 22050, 24000, 16000 } // MPEG2
    };

    if (lpData[0] != 0xff || lpData[1] >> 5 != 0x07)
        return FALSE;

    switch (lpData[1] >> 3 & 0x03) {

    case 3:
        version = 1;
        break;

    case 2:
        version = 2;
        break;

    default:
        return FALSE;

    }

    if ((lpData[1] >> 1 & 0x03) != 1)
        return FALSE;

    index = lpData[2] >> 4;
    dwBitRate = dwBitTableLayer3[version - 1][index];

    index = lpData[2] >> 2 & 0x03;
    dwSampleRate = dwSampleTable[version - 1][index];

    padding = lpData[2] >> 1 & 0x01;
    channel = (lpData[3] >> 6) == 3 ? 1 : 2;

    wFrameSize = (WORD)((1152 * dwBitRate * 1000 / dwSampleRate) / 8) + padding;

    lpfh->version = version;
    lpfh->dwBitRate = dwBitRate;
    lpfh->dwSampleRate = dwSampleRate;
    lpfh->padding = padding;
    lpfh->channel = channel;
    lpfh->wFrameSize = wFrameSize;

    return TRUE;
}

void ReadMp3::GetMp3Format(LPFRAMEHEADER lpfh)
{
    m_Format.wfx.wFormatTag = WAVE_FORMAT_MPEGLAYER3;
    m_Format.wfx.nChannels = lpfh->channel;
    m_Format.wfx.nSamplesPerSec = lpfh->dwSampleRate;
    m_Format.wfx.nAvgBytesPerSec = (lpfh->dwBitRate * 1000) / 8;
    m_Format.wfx.nBlockAlign = 1;
    m_Format.wfx.wBitsPerSample = 0;
    m_Format.wfx.cbSize = MPEGLAYER3_WFX_EXTRA_BYTES;

    m_Format.wID = MPEGLAYER3_ID_MPEG;
    m_Format.fdwFlags = lpfh->padding ? MPEGLAYER3_FLAG_PADDING_ON : MPEGLAYER3_FLAG_PADDING_OFF;
    m_Format.nBlockSize = lpfh->wFrameSize;
    m_Format.nFramesPerBlock = 1;
    m_Format.nCodecDelay = 0x571;
}

DWORD ReadMp3::GetDecodeSize(LPBYTE lpMP3Data, DWORD dwMP3Size, LPWAVEFORMATEX lpwf)
{
    DWORD       dwFrameCount;
    DWORD       dwDecodeSize;
    LPBYTE      lp;
    DWORD       dwOffset;
    FRAMEHEADER frameHeader;
    double      dSecond;

    GetFrameHeader(lpMP3Data, &frameHeader);

    if (frameHeader.channel == 1)
        dwOffset = 17;
    else
        dwOffset = 31;

    lp = lpMP3Data + dwOffset + 4;
    {
        dwFrameCount = 0;
        dwOffset = 0;
        while (dwMP3Size > dwOffset) {
            if (GetFrameHeader(lpMP3Data + dwOffset, &frameHeader)) {
                dwFrameCount++;
                dwOffset += frameHeader.wFrameSize;
            }
            else {
                dwOffset++;
            }
        }
    }

    dSecond = (dwFrameCount * (double)(1152 / (double)lpwf->nSamplesPerSec));
    dwDecodeSize = (DWORD)(lpwf->nAvgBytesPerSec * dSecond);

    return dwDecodeSize;
}

BOOL ReadMp3::IsId3v2(LPBYTE lpData, DWORD dwDataSize, LPDWORD lpdwTagSize)
{
    BOOL          bResult;
    LPBYTE        lp;
    LPID3V2HEADER lpHeader = (LPID3V2HEADER)lpData;

    if (lpHeader->id[0] == 'I' && lpHeader->id[1] == 'D' && lpHeader->id[2] == '3') {
        *lpdwTagSize = ((lpHeader->size[0] << 21) | (lpHeader->size[1] << 14) | (lpHeader->size[2] << 7) | (lpHeader->size[3])) + 10;
        bResult = TRUE;
    }
    else {
        lp = (lpData + dwDataSize) - 128;
        if (lp[0] == 'T' && lp[1] == 'A' && lp[2] == 'G')
            *lpdwTagSize = 128;
        else
            *lpdwTagSize = 0;
        bResult = FALSE;
    }

    return bResult;
}

BOOL ReadMp3::ReadMP3File(const std::wstring& fileName, LPBYTE *lplpData, LPDWORD lpdwSize)
{
    HMMIO       hmmio;
    LPBYTE      lpData;
    LPBYTE      lpMp3Data;
    DWORD       dwSize;
    DWORD       dwTagSize;
    FRAMEHEADER frameHeader;

    hmmio = mmioOpen(const_cast<WCHAR*>((fileName).c_str()), NULL, MMIO_READ);
    if (hmmio == NULL) {
        MessageBox(NULL, TEXT("ファイルのオープンに失敗しました。"), NULL, MB_ICONWARNING);
        return FALSE;
    }

    dwSize = mmioSeek(hmmio, 0, SEEK_END);
    lpData = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, dwSize);
    mmioSeek(hmmio, 0, SEEK_SET);
    mmioRead(hmmio, (HPSTR)lpData, dwSize);
    mmioClose(hmmio, 0);

    if (IsId3v2(lpData, dwSize, &dwTagSize)) {
        dwTagSize += 384;
        dwSize -= dwTagSize;
        lpMp3Data = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, dwSize);
        CopyMemory(lpMp3Data, (LPBYTE)lpData + dwTagSize, dwSize);
        HeapFree(GetProcessHeap(), 0, lpData);
    }
    else {
        dwSize -= dwTagSize;
        lpMp3Data = lpData;
    }

    if (!GetFrameHeader(lpMp3Data, &frameHeader)) {
        HeapFree(GetProcessHeap(), 0, lpMp3Data);
        return FALSE;
    }

    GetMp3Format(&frameHeader);

    *lplpData = lpMp3Data;
    *lpdwSize = dwSize;

    return TRUE;
}

void SaveWave(LPTSTR lpszFileName, LPWAVEFORMATEX lpwf, LPBYTE lpData, DWORD dwSize)
{
    HMMIO    hmmio;
    MMCKINFO mmckRiff;
    MMCKINFO mmckFmt;
    MMCKINFO mmckData;

    hmmio = mmioOpen(lpszFileName, NULL, MMIO_CREATE | MMIO_WRITE);

    mmckRiff.fccType = mmioStringToFOURCC(TEXT("WAVE"), 0);
    mmioCreateChunk(hmmio, &mmckRiff, MMIO_CREATERIFF);

    mmckFmt.ckid = mmioStringToFOURCC(TEXT("fmt "), 0);
    mmioCreateChunk(hmmio, &mmckFmt, 0);
    mmioWrite(hmmio, (char *)lpwf, sizeof(WAVEFORMATEX));
    mmioAscend(hmmio, &mmckFmt, 0);

    mmckData.ckid = mmioStringToFOURCC(TEXT("data"), 0);
    mmioCreateChunk(hmmio, &mmckData, 0);
    mmioWrite(hmmio, (char *)lpData, dwSize);
    mmioAscend(hmmio, &mmckData, 0);

    mmioAscend(hmmio, &mmckRiff, 0);
    mmioClose(hmmio, 0);
}


bool ReadMp3::Open(const std::wstring & fileName)
{
    DWORD                dwMP3Size;
    LPBYTE               lpMP3Data;

    // mp3
    if (!ReadMP3File(fileName, &lpMP3Data, &dwMP3Size))
        return false;

    if (!DecodeToWave((LPWAVEFORMATEX)&m_Format, lpMP3Data, dwMP3Size, &m_WaveFormat)) {
        HeapFree(GetProcessHeap(), 0, lpMP3Data);
        return false;
    }
    HeapFree(GetProcessHeap(), 0, lpMP3Data);
    m_FirstPtr = m_CurrnetPtr;

    SaveWave(TEXT("test.wav"), &m_WaveFormat, m_FirstPtr, m_DataSize);

    return true;
}

unsigned long ReadMp3::GetData(unsigned char * data, unsigned long size)
{
    if (m_DataCount == m_DataSize) return 0;

    if (m_DataCount + size <= m_DataSize)
    {
        memcpy(data, m_CurrnetPtr, size);
        m_DataCount += size;
        m_CurrnetPtr += size;
        return size;
    }

    auto count = m_DataCount;
    m_DataCount = m_DataSize;
    m_CurrnetPtr += (m_DataSize - count);
    memcpy(data, m_CurrnetPtr, m_DataSize - count);
    return m_DataSize - count;
}

void ReadMp3::Dispose()
{
    HeapFree(GetProcessHeap(), 0, m_FirstPtr);
}
