#pragma once

#include <Windows.h>
#include <string>

class OutputSound
{
public:
    HRESULT OpenDevice(WAVEFORMATEX* format, UINT deviceIndex = WAVE_MAPPER);

    void Start(unsigned long bufSize);
    void Stop();

    HRESULT CloseDevice();

    void InputData(WAVEHDR wh);

    void Output(const std::wstring& wavFileName, UINT deviceIndex = WAVE_MAPPER);

private:
    static void CALLBACK Callback(HWAVEIN hwi,
        UINT uMsg,
        DWORD dwInstance,
        DWORD dwParam1,
        DWORD dwParam2);

    HWAVEOUT hwo;

    //static const int BUFFER_NUM = 5;
    //WAVEHDR OutHdr[BUFFER_NUM];               //!< サウンド入力のデータブロック構造体
    WAVEHDR OutHdr;
};