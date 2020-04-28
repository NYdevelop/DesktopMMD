#include "OutputSound.h"

#include "ReadWavFile.h"

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
    //! データブロックを出力デバイスに登録する.
//    const int BUFSIZE = 4096;
    //memset(&OutHdr, 0, bufSize);
    //for (int i = 0; i < BUFFER_NUM; i++) {
        OutHdr.lpData = new char[bufSize];
        if (waveOutPrepareHeader(hwo, &OutHdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
        {
            MessageBox(NULL, TEXT("output sound error"), NULL, MB_ICONERROR);
            return;
        }
    //}
}

void OutputSound::Stop()
{
    if (hwo == nullptr) return;
    waveOutReset(hwo);
}

HRESULT OutputSound::CloseDevice()
{
    if (hwo == nullptr) S_OK;

    //for (int i = 0; i < BUFFER_NUM; i++) {
        //! 出力デバイスに登録したデータブロックを解放する.
        waveOutUnprepareHeader(hwo, &OutHdr, sizeof(WAVEHDR));

        //! データバッファを解放する.
        if (OutHdr.lpData) {
            delete[] OutHdr.lpData;
            OutHdr.lpData = NULL;
        }
    //}

    //! 出力デバイスをクローズする.
    waveOutClose(hwo);
    hwo = NULL;

    return S_OK;
}

void OutputSound::InputData(WAVEHDR wh)
{
    //! 空いている出力データバッファを探して、入力音声データをコピーする.
    //for (DWORD i = 0; i < BUFFER_NUM; i++) {
        if (OutHdr.dwBufferLength == 0 && OutHdr.lpData)
        {
            memcpy(OutHdr.lpData, wh.lpData, wh.dwBytesRecorded);
            OutHdr.dwBufferLength = wh.dwBytesRecorded;
            waveOutWrite(hwo, &OutHdr, sizeof(WAVEHDR));
            //break;
        }
    //}
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
    Start(header.FileSize + 1);

    WAVEHDR wh;
    wh.lpData = new char[header.FileSize + 1];
    wh.lpData[header.FileSize] = 0;
    wh.dwBytesRecorded = header.FileSize + 1;

    reader.ReadWaveFile(wh.lpData, header.FileSize);
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
