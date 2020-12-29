#include "OutputSound.h"

#pragma comment(lib, "winmm")

#include <iostream>

HRESULT OutputSound::OpenDevice(WAVEFORMATEX* format, UINT deviceIndex)
{
    //! 出力デバイスをオープンする.
    TCHAR str[MAXERRORLENGTH];
    auto mmRes = waveOutOpen(&hwo, deviceIndex, format, (DWORD)&Callback, (DWORD_PTR)this, CALLBACK_FUNCTION);
    if (mmRes != MMSYSERR_NOERROR) {
        waveOutGetErrorText(mmRes, str, MAXERRORLENGTH);
        MessageBox(NULL, str, L"output soud open error", MB_ICONERROR);
        return E_FAIL;
    }
    return S_OK;
}

void OutputSound::Start(const std::wstring & wavFileName, UINT deviceIndex)
{
    m_IsStop = false;
    if (m_FileReader.ReadOpenWaveFile(wavFileName.c_str()) == FALSE)
    {
        MessageBox(NULL, TEXT("wav open error"), NULL, MB_ICONERROR);
        return;
    }

    auto format = m_FileReader.GetFormat();
    m_FileWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    m_FileWaveFormat.nChannels = format.Channels;
    m_FileWaveFormat.nSamplesPerSec = format.SamplesPerSec;
    m_FileWaveFormat.wBitsPerSample = format.BitsPerSample;
    m_FileWaveFormat.nBlockAlign = m_FileWaveFormat.wBitsPerSample * m_FileWaveFormat.nChannels / 8;
    m_FileWaveFormat.nAvgBytesPerSec = m_FileWaveFormat.nSamplesPerSec * m_FileWaveFormat.nBlockAlign;

    OpenDevice(&m_FileWaveFormat, deviceIndex);
    auto header = m_FileReader.GetHeader();
    unsigned long size = m_FileWaveFormat.nAvgBytesPerSec;
    InitializeBuffer(size);

    ReadNext();
    InputData();

    ReadNext();
    Sleep(400);
    InputData();
}

void OutputSound::Stop()
{
    if (hwo == nullptr) return;
    m_IsStop = true;
    //auto ret = waveOutReset(hwo);
    ////auto ret = waveOutClose(hwo);
    //if (ret != MMSYSERR_NOERROR)
    //{
    //    std::cerr << "waveOutReset error." << std::endl;
    //}
    m_CurrentBuffer = 0;
}

HRESULT OutputSound::CloseDevice()
{
    if (hwo == nullptr) S_OK;
    Sleep(1100);
    for (int i = 0; i < BUFFER_NUM; i++)
    {
        //! 出力デバイスに登録したデータブロックを解放する.
        waveOutUnprepareHeader(hwo, &OutHdr[i], sizeof(WAVEHDR));

        //! データバッファを解放する
        if (OutHdr[i].lpData) {
            delete[] OutHdr[i].lpData;
            OutHdr[i].lpData = NULL;
        }
    }

    //! 出力デバイスをクローズする.
    waveOutClose(hwo);
    hwo = NULL;

    return S_OK;
}

void OutputSound::InputData()//WAVEHDR wh)
{
    if (OutHdr[m_CurrentBuffer].lpData == NULL) return;

    waveOutWrite(hwo, &OutHdr[m_CurrentBuffer], sizeof(WAVEHDR));
    if (++m_CurrentBuffer >= BUFFER_NUM)
    {
        m_CurrentBuffer = 0;
    }
}

void OutputSound::ReadNext()
{
    unsigned long size = m_FileWaveFormat.nAvgBytesPerSec;
    size = m_FileReader.ReadWaveFile(OutHdr[m_CurrentBuffer].lpData, size);
    DoVolume(OutHdr[m_CurrentBuffer].lpData, size, &m_FileWaveFormat);
    OutHdr[m_CurrentBuffer].dwBufferLength = size;
}

void OutputSound::SetVolume(float val)
{
    if (val > 1.f)
    {
        val = 1.f;
    }
    if (val < 0.f)
    {
        val = 0.f;
    }
    m_Volume = val;
}

// private
void OutputSound::Callback(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    switch (uMsg) {
    case MM_WOM_DONE:
        OutputSound* outputSound = (OutputSound*)dwInstance;
        if (((WAVEHDR*)dwParam1)->dwUser || outputSound->m_IsStop) {    //waveOutReset関数で停止した
            outputSound->m_CurrentBuffer = 0;
            return;
        }

        outputSound->ReadNext();
        outputSound->InputData();
        break;
    }
}

void OutputSound::InitializeBuffer(unsigned long bufSize)
{
    for (int i = 0; i < BUFFER_NUM; i++)
    {
        //memset(&OutHdr[bufIndex], '\0', sizeof(WAVEHDR));
        OutHdr[i] = { 0 };

        //! データブロックを出力デバイスに登録する.
        TCHAR str[MAXERRORLENGTH];
        OutHdr[i].lpData = new char[bufSize];
        OutHdr[i].dwBufferLength = 0;
        OutHdr[i].dwFlags = 0;
        OutHdr[i].dwBytesRecorded = 0;
        OutHdr[i].dwLoops = 0;
        OutHdr[i].lpNext = NULL;
        OutHdr[i].dwUser = 0;
        OutHdr[i].reserved = 0;

        auto mmRes = waveOutPrepareHeader(hwo, &OutHdr[i], sizeof(WAVEHDR));
        if (mmRes != MMSYSERR_NOERROR)
        {
            waveOutGetErrorText(mmRes, str, MAXERRORLENGTH);
            MessageBox(NULL, str, TEXT("output sound error"), MB_ICONERROR);
            return;
        }
    }
}

//音量調節
//pBuffer：音源データが格納されているバッファ
//nBufSize：バッファのバイト数
//pwfx：音源データの演奏形式を格納したWAVEFORMATEX構造体
BOOL OutputSound::DoVolume(LPVOID pBuffer, UINT nBufSize, WAVEFORMATEX* pwfx)
{
    //ASSERT(::AfxIsValidAddress(pBuffer,nBufSize));
    //ASSERT(!::IsBadReadPtr(pwfx,sizeof(WAVEFORMATEX)));

    //音量が左右とも最大なら、何もせずに終了します。
    if (m_Volume == 1.f) return TRUE;

    //左音量を取得します。
    DWORD dwVolL = (DWORD)(m_Volume * 0xFFFF);
    //右音量を取得します。
    DWORD dwVolR = (DWORD)(m_Volume * 0xFFFF);
    //サンプル数を算出します。
    int nSamples = nBufSize / pwfx->nBlockAlign;

    //演奏するサンプルあたりのバイト数で個別処理します。
    switch (pwfx->nBlockAlign) {
    case 1://１バイト（８ビット音源）の場合。
           //音量が最大ではない場合。
        if (dwVolL<0xFFFF) {
            //音源バッファへのポインタを取得します。
            PBYTE src = (PBYTE)pBuffer;
            PBYTE dst = src;
            for (int i = 0; i<nSamples; i++) {
                BYTE b = *src++;
                //音量を掛けて出力音源を算出します。
                *dst++ = (BYTE)(b*dwVolL / 0xFFFF);

            }
        }
        break;
    case 2://２バイトの場合。
           //１６ビットモノラルの場合。
        if (pwfx->nChannels == 1) {
            //音量が最大ではない場合。
            if (dwVolL<0xFFFF) {
                //音源バッファへのポインタを取得します。
                WORD* src = (WORD*)pBuffer;
                WORD* dst = src;
                for (int i = 0; i<nSamples; i++) {
                    DWORD dw = (DWORD)*src++;
                    //音量を掛けて出力音源を算出します。
                    if (dw & 0x8000) {    //負数の場合
                        dw |= 0xFFFF0000;
                        dw = 0 - abs((LONG)dw)*dwVolL / 0xFFFF;
                    }
                    else dw = dw*dwVolL / 0xFFFF;
                    *dst++ = (WORD)dw;
                }
            }
        }
        //８ビットステレオの場合
        else {
            //音源バッファへのポインタを取得します。
            PBYTE src = (PBYTE)pBuffer;
            PBYTE dst = src;
            for (int i = 0; i<nSamples; i++) {
                //左音量が最大ではない場合。
                if (dwVolL<0xFFFF) {
                    BYTE b = *src++;
                    //左音量を掛けて左出力音源を算出します。
                    *dst++ = (BYTE)(b*dwVolL / 0xFFFF);
                }
                //左音量が最大の場合は、何もせずにポインタだけ進めます。
                else {
                    src++;
                    dst++;
                }
                //右音量が最大ではない場合。
                if (dwVolR<0xFFFF) {
                    BYTE b = *src++;
                    //右音量を掛けて右出力音源を算出します。
                    *dst++ = (BYTE)(b*dwVolR) / 0xFFFF;
                }
                //右音量が最大の場合は、何もせずにポインタだけ進めます。
                else {
                    src++;
                    dst++;
                }
            }
        }
        break;

    case 4: //４バイトの場合。
            //音源バッファへのポインタを取得します。
        WORD* src = (WORD*)pBuffer;
        WORD* dst = src;
        for (int i = 0; i<nSamples; i++) {
            //左音量が最大ではない場合。
            if (dwVolL<0xFFFF) {
                DWORD dw = (DWORD)*src++;
                //左音量を掛けて左出力音源を算出します。
                if (dw & 0x8000) {    //負数の場合
                    dw |= 0xFFFF0000;
                    dw = 0 - abs((LONG)dw)*dwVolL / 0xFFFF;
                }
                else dw = dw*dwVolL / 0xFFFF;
                *dst++ = (WORD)dw;
            }
            //左音量が最大の場合は、何もせずにポインタだけ進めます。
            else {
                src++;
                dst++;
            }
            //右音量が最大ではない場合。
            if (dwVolR<0xFFFF) {
                DWORD dw = (DWORD)*src++;
                //右音量を掛けて右出力音源を算出します。
                if (dw & 0x8000) {    //負数の場合
                    dw |= 0xFFFF0000;
                    dw = 0 - abs((LONG)dw)*dwVolR / 0xFFFF;
                }
                else dw = dw*dwVolR / 0xFFFF;
                *dst++ = (WORD)dw;
            }
            //右音量が最大の場合は、何もせずにポインタだけ進めます。
            else {
                src++;
                dst++;
            }
        }
        break;
    }
    return TRUE;
}