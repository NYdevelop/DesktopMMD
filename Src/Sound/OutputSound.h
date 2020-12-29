#pragma once

#include <Windows.h>
#include <string>

#include "ReadWavFile.h"

class OutputSound
{
public:
    HRESULT OpenDevice(WAVEFORMATEX* format, UINT deviceIndex = WAVE_MAPPER);

    void Start(const std::wstring& wavFileName, UINT deviceIndex = WAVE_MAPPER);

    void Stop();

    HRESULT CloseDevice();

    void InputData();

    void ReadNext();

    inline float GetVolume() { return m_Volume; }

    void SetVolume(float val);

private:
    static void CALLBACK Callback(
        HWAVEIN hwi,
        UINT uMsg,
        DWORD dwInstance,
        DWORD dwParam1,
        DWORD dwParam2);

    void InitializeBuffer(unsigned long bufSize);

    BOOL DoVolume(LPVOID pBuffer, UINT nBufSize, WAVEFORMATEX * pwfx);

    HWAVEOUT hwo;

    static const int BUFFER_NUM = 2;
    WAVEHDR OutHdr[BUFFER_NUM];               //!< サウンド入力のデータブロック構造体
    char* m_DataBuffer[BUFFER_NUM];
    int m_CurrentBuffer = 0;

    ReadWavFile m_FileReader;
    WAVEFORMATEX m_FileWaveFormat;

    float m_Volume = 0.0625;
    bool m_IsStop = false;
};