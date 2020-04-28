#pragma once

#include <Windows.h>
#include <functional>

class CaptureSound
{
public:
    HRESULT OpenDevice(UINT deviceIndex, WAVEFORMATEX format, int bufferSize = 4056);

    void Start();
    void Stop();

    void SetCaptureCallback(const std::function<void(WAVEHDR)>& callback);

private:
    static void CALLBACK Callback(HWAVEIN hwi,
        UINT uMsg,
        DWORD dwInstance,
        DWORD dwParam1,
        DWORD dwParam2);

    void SoundInputDone(HWAVEIN hDlg, WAVEHDR * wh);

    std::function<void(WAVEHDR)> m_CaptureCallback;

    static const int BUFFER_NUM = 5;
    HWAVEIN hwi = nullptr;
    WAVEHDR InHdr[BUFFER_NUM];               //!< サウンド入力のデータブロック構造体
    WAVEHDR latestBuffer;
};