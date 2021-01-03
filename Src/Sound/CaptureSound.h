#pragma once

// #define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <functional>

class CaptureSound
{
public:
    HRESULT OpenDevice(UINT deviceIndex, WAVEFORMATEX format, int bufferSize = 4056);

    void Start();
    void Stop();

    HRESULT CloseDevice();

    void SetCaptureCallback(const std::function<void(WAVEHDR)>& callback);

    WAVEFORMATEX GetWaveFormatEx();

private:
    static void CALLBACK Callback(HWAVEIN hwi,
        UINT uMsg,
        DWORD_PTR dwInstance,
        DWORD_PTR dwParam1,
        DWORD_PTR dwParam2);

    void SoundInputDone(HWAVEIN hDlg, WAVEHDR * wh);

    std::function<void(WAVEHDR)> m_CaptureCallback;

    static const int BUFFER_NUM = 5;
    HWAVEIN hwi = nullptr;
    WAVEHDR InHdr[BUFFER_NUM] = { 0 };               //!< サウンド入力のデータブロック構造体
    WAVEHDR latestBuffer;

    WAVEFORMATEX m_WaveFormat;
};