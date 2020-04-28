#include "CaptureSound.h"

#include <iostream>
#include <chrono>

#pragma comment(lib, "winmm")

using namespace std;

HRESULT CaptureSound::OpenDevice(UINT deviceIndex, WAVEFORMATEX format, int bufferSize)
{
    m_WaveFormat = format;
    if (waveInOpen(&hwi, deviceIndex, &format, (DWORD)Callback, (DWORD_PTR)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
        MessageBox(NULL, TEXT("WAVEデバイスのオープンに失敗しました。"), NULL, MB_ICONERROR);
        return -1;
    }

    //! データブロックを入力デバイスに登録する.
    memset(InHdr, 0, sizeof(InHdr));
    for (int i = 0; i < BUFFER_NUM; i++) {
        InHdr[i].lpData = new char[bufferSize];
        InHdr[i].dwBufferLength = bufferSize;
        InHdr[i].dwFlags = 0;
        if (waveInPrepareHeader(hwi, &InHdr[i], sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
            return FALSE;
        }
    }

    //! サウンド入力バッファをリセットする.
    waveInReset(hwi);

    //! データブロックをサウンド入力デバイスにキューイングする.
    for (DWORD i = 0; i < BUFFER_NUM; i++) {
        InHdr[i].dwBytesRecorded = 0;
        waveInAddBuffer(hwi, &InHdr[i], sizeof(WAVEHDR));
    }


    latestBuffer.lpData = new char[bufferSize];
    latestBuffer.dwBufferLength = bufferSize;

    return S_OK;
}

void CaptureSound::Start()
{
    // 録音開始
    waveInStart(hwi);
}

void CaptureSound::Stop()
{
    if (hwi == nullptr) return;

    // 録音終了
    waveInStop(hwi);
}

HRESULT CaptureSound::CloseDevice()
{
    if (hwi == nullptr) return S_OK;

    // 入力デバイスに登録したデータブロックを解放する.
    for (int i = 0; i < BUFFER_NUM; i++) {
        waveInUnprepareHeader(hwi, &InHdr[i], sizeof(WAVEHDR));

        // データバッファを解放する.
        if (InHdr[i].lpData) {
            delete[] InHdr[i].lpData;
            InHdr[i].lpData = NULL;
        }
    }

    if (latestBuffer.lpData)
    {
        delete[] latestBuffer.lpData;
        latestBuffer.lpData = NULL;
    }

    // 入力デバイスをクローズする.
    waveInClose(hwi);
    hwi = NULL;

    return S_OK;
}

void CaptureSound::SetCaptureCallback(const std::function<void(WAVEHDR)>& callback)
{
    m_CaptureCallback = callback;
}

WAVEFORMATEX CaptureSound::GetWaveFormatEx()
{
    return m_WaveFormat;
}

void CaptureSound::Callback(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    switch (uMsg)
    {
    case WIM_OPEN:
        cout << "waveInOpen" << endl;
        break;

    case WIM_CLOSE:
        cout << "waveInClose" << endl;
        break;

    case WIM_DATA:
        auto instance = (CaptureSound*)dwInstance;
        instance->SoundInputDone(hwi, (WAVEHDR*)dwParam1);
        break;
    }
}

void CaptureSound::SoundInputDone(HWAVEIN hDlg, WAVEHDR* wh)
{
    if (wh->dwBytesRecorded <= 0) return;

    //! callback実行
    memcpy(latestBuffer.lpData, wh->lpData, wh->dwBytesRecorded);
    latestBuffer.dwBytesRecorded = wh->dwBytesRecorded;
    m_CaptureCallback(latestBuffer);

    //! デバイスに空いた入力バッファをキューイングする.
    wh->dwBytesRecorded = 0;
    waveInAddBuffer(hwi, wh, sizeof(WAVEHDR));
}