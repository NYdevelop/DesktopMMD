#include "OutputSound.h"

#include "ReadWavFile.h"

HRESULT OutputSound::OpenDevice(WAVEFORMATEX* format, UINT deviceIndex)
{
    //! �o�̓f�o�C�X���I�[�v������.
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
    //! �f�[�^�u���b�N���o�̓f�o�C�X�ɓo�^����.
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
        //! �o�̓f�o�C�X�ɓo�^�����f�[�^�u���b�N���������.
        waveOutUnprepareHeader(hwo, &OutHdr, sizeof(WAVEHDR));

        //! �f�[�^�o�b�t�@���������.
        if (OutHdr.lpData) {
            delete[] OutHdr.lpData;
            OutHdr.lpData = NULL;
        }
    //}

    //! �o�̓f�o�C�X���N���[�Y����.
    waveOutClose(hwo);
    hwo = NULL;

    return S_OK;
}

void OutputSound::InputData(WAVEHDR wh)
{
    //! �󂢂Ă���o�̓f�[�^�o�b�t�@��T���āA���͉����f�[�^���R�s�[����.
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
