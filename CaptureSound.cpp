#include "CaptureSound.h"

#include <iostream>
#include <chrono>

#pragma comment(lib, "winmm")

using namespace std;

std::chrono::system_clock::time_point start;
HRESULT CaptureSound::OpenDevice(UINT deviceIndex, WAVEFORMATEX format, int bufferSize)
{
    if (waveInOpen(&hwi, deviceIndex, &format, (DWORD)Callback, (DWORD_PTR)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
        MessageBox(NULL, TEXT("WAVE�f�o�C�X�̃I�[�v���Ɏ��s���܂����B"), NULL, MB_ICONWARNING);
        return -1;
    }

    //! �f�[�^�u���b�N����̓f�o�C�X�ɓo�^����.
    memset(InHdr, 0, sizeof(InHdr));
    for (int i = 0; i < BUFFER_NUM; i++) {
        InHdr[i].lpData = new char[bufferSize];
        InHdr[i].dwBufferLength = bufferSize;
        InHdr[i].dwFlags = 0;
        if (waveInPrepareHeader(hwi, &InHdr[i], sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
            return FALSE;
        }
    }

    //! �T�E���h���̓o�b�t�@�����Z�b�g����.
    waveInReset(hwi);

    //! �f�[�^�u���b�N���T�E���h���̓f�o�C�X�ɃL���[�C���O����.
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
    start = chrono::system_clock::now();
    // �^���J�n
    waveInStart(hwi);
}

void CaptureSound::Stop()
{
    if (hwi == nullptr) return;

    //! 1.�^���I��
    waveInStop(hwi);

    //! 2.���̓f�o�C�X�ɓo�^�����f�[�^�u���b�N���������.
    for (int i = 0; i < BUFFER_NUM; i++) {
        waveInUnprepareHeader(hwi, &InHdr[i], sizeof(WAVEHDR));

        //! 3.�f�[�^�o�b�t�@���������.
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

    //! -4.���̓f�o�C�X���N���[�Y����.
    waveInClose(hwi);
    hwi = NULL;
}

void CaptureSound::SetCaptureCallback(const std::function<void(WAVEHDR)>& callback)
{
    m_CaptureCallback = callback;
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
        //long long elapsedMicroSec = (chrono::system_clock::now() - start).count() / 10; //�����ɗv�������Ԃ��~���b�ɕϊ�
        //cout << "DATA " << elapsedMicroSec << endl;
        auto instance = (CaptureSound*)dwInstance;
        instance->SoundInputDone(hwi, (WAVEHDR*)dwParam1);
        break;
    }
}

void CaptureSound::SoundInputDone(HWAVEIN hDlg, WAVEHDR* wh)
{
    if (wh->dwBytesRecorded <= 0) return;

    //! callback���s
    memcpy(latestBuffer.lpData, wh->lpData, wh->dwBytesRecorded);
    latestBuffer.dwBytesRecorded = wh->dwBytesRecorded;
    m_CaptureCallback(latestBuffer);

    //! �f�o�C�X�ɋ󂢂����̓o�b�t�@���L���[�C���O����.
    wh->dwBytesRecorded = 0;
    waveInAddBuffer(hwi, wh, sizeof(WAVEHDR));
}