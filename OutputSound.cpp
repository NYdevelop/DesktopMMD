#include "OutputSound.h"

#include "ReadWavFile.h"

#pragma comment(lib, "winmm")

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

void OutputSound::Start(unsigned long bufSize)
{
    OutHdr = { 0 };
    //! データブロックを出力デバイスに登録する.
    TCHAR str[MAXERRORLENGTH];
    OutHdr.lpData = new char[bufSize];
    //OutHdr.dwBufferLength = bufSize;
    //OutHdr.dwFlags = 0;
    //OutHdr.dwBytesRecorded = 0;
    //OutHdr.dwLoops = 1;
    //OutHdr.lpNext = NULL;
    //OutHdr.dwUser = 0;
    //OutHdr.reserved = 0;
    OutHdr.dwBufferLength = 0;
    OutHdr.dwFlags = 0;
    OutHdr.dwBytesRecorded = 0;
    OutHdr.dwLoops = 0;
    OutHdr.lpNext = NULL;
    OutHdr.dwUser = 0;
    OutHdr.reserved = 0;

    auto mmRes = waveOutPrepareHeader(hwo, &OutHdr, sizeof(WAVEHDR));
    if (mmRes != MMSYSERR_NOERROR)
    {
        waveOutGetErrorText(mmRes, str, MAXERRORLENGTH);
        MessageBox(NULL, str, TEXT("output sound error"), MB_ICONERROR);
        return;
    }
}

void OutputSound::Stop()
{
    if (hwo == nullptr) return;
    waveOutReset(hwo);
}

HRESULT OutputSound::CloseDevice()
{
    if (hwo == nullptr) S_OK;

    //! 出力デバイスに登録したデータブロックを解放する.
    waveOutUnprepareHeader(hwo, &OutHdr, sizeof(WAVEHDR));

    //! データバッファを解放する.
    if (OutHdr.lpData) {
        delete[] OutHdr.lpData;
        OutHdr.lpData = NULL;
    }

    //! 出力デバイスをクローズする.
    waveOutClose(hwo);
    hwo = NULL;

    return S_OK;
}

void OutputSound::InputData(WAVEHDR wh)
{
    if (OutHdr.dwBufferLength == 0 && OutHdr.lpData)
    {
        memcpy(OutHdr.lpData, wh.lpData, wh.dwBytesRecorded);
        OutHdr.dwBufferLength = wh.dwBytesRecorded;
        waveOutWrite(hwo, &OutHdr, sizeof(WAVEHDR));
    }
}

void OutputSound::Output(const std::wstring & wavFileName, UINT deviceIndex)
{
    ReadWavFile reader;
    if (reader.ReadOpenWaveFile(wavFileName.c_str()) == FALSE)
    {
        MessageBox(NULL, TEXT("wav open error"), NULL, MB_ICONERROR);
        return;
    }

    auto header = reader.GetHeader();
    WAVEFORMATEX wf;
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = header.Channels;
    wf.nSamplesPerSec = header.SamplesPerSec;
    wf.wBitsPerSample = header.BitsPerSample;
    wf.nBlockAlign = wf.wBitsPerSample * wf.nChannels / 8;
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

    OpenDevice(&wf, deviceIndex);
    int size = header.FileSize - wf.nAvgBytesPerSec * 0.5;
    Start(size);

    WAVEHDR wh;
    wh.lpData = new char[size];
    wh.dwBytesRecorded = size;

    reader.ReadWaveFile(wh.lpData, size);
    InputData(wh);
}


void OutputSound::Callback(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    switch (uMsg) {
    case MM_WOM_DONE:
        ((WAVEHDR*)dwParam1)->dwBufferLength = 0;
        break;
    }
}
