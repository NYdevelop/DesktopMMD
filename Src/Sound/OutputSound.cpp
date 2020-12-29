#include "OutputSound.h"

#pragma comment(lib, "winmm")

#include <iostream>

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
        //! �o�̓f�o�C�X�ɓo�^�����f�[�^�u���b�N���������.
        waveOutUnprepareHeader(hwo, &OutHdr[i], sizeof(WAVEHDR));

        //! �f�[�^�o�b�t�@���������
        if (OutHdr[i].lpData) {
            delete[] OutHdr[i].lpData;
            OutHdr[i].lpData = NULL;
        }
    }

    //! �o�̓f�o�C�X���N���[�Y����.
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
        if (((WAVEHDR*)dwParam1)->dwUser || outputSound->m_IsStop) {    //waveOutReset�֐��Œ�~����
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

        //! �f�[�^�u���b�N���o�̓f�o�C�X�ɓo�^����.
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

//���ʒ���
//pBuffer�F�����f�[�^���i�[����Ă���o�b�t�@
//nBufSize�F�o�b�t�@�̃o�C�g��
//pwfx�F�����f�[�^�̉��t�`�����i�[����WAVEFORMATEX�\����
BOOL OutputSound::DoVolume(LPVOID pBuffer, UINT nBufSize, WAVEFORMATEX* pwfx)
{
    //ASSERT(::AfxIsValidAddress(pBuffer,nBufSize));
    //ASSERT(!::IsBadReadPtr(pwfx,sizeof(WAVEFORMATEX)));

    //���ʂ����E�Ƃ��ő�Ȃ�A���������ɏI�����܂��B
    if (m_Volume == 1.f) return TRUE;

    //�����ʂ��擾���܂��B
    DWORD dwVolL = (DWORD)(m_Volume * 0xFFFF);
    //�E���ʂ��擾���܂��B
    DWORD dwVolR = (DWORD)(m_Volume * 0xFFFF);
    //�T���v�������Z�o���܂��B
    int nSamples = nBufSize / pwfx->nBlockAlign;

    //���t����T���v��������̃o�C�g���Ōʏ������܂��B
    switch (pwfx->nBlockAlign) {
    case 1://�P�o�C�g�i�W�r�b�g�����j�̏ꍇ�B
           //���ʂ��ő�ł͂Ȃ��ꍇ�B
        if (dwVolL<0xFFFF) {
            //�����o�b�t�@�ւ̃|�C���^���擾���܂��B
            PBYTE src = (PBYTE)pBuffer;
            PBYTE dst = src;
            for (int i = 0; i<nSamples; i++) {
                BYTE b = *src++;
                //���ʂ��|���ďo�͉������Z�o���܂��B
                *dst++ = (BYTE)(b*dwVolL / 0xFFFF);

            }
        }
        break;
    case 2://�Q�o�C�g�̏ꍇ�B
           //�P�U�r�b�g���m�����̏ꍇ�B
        if (pwfx->nChannels == 1) {
            //���ʂ��ő�ł͂Ȃ��ꍇ�B
            if (dwVolL<0xFFFF) {
                //�����o�b�t�@�ւ̃|�C���^���擾���܂��B
                WORD* src = (WORD*)pBuffer;
                WORD* dst = src;
                for (int i = 0; i<nSamples; i++) {
                    DWORD dw = (DWORD)*src++;
                    //���ʂ��|���ďo�͉������Z�o���܂��B
                    if (dw & 0x8000) {    //�����̏ꍇ
                        dw |= 0xFFFF0000;
                        dw = 0 - abs((LONG)dw)*dwVolL / 0xFFFF;
                    }
                    else dw = dw*dwVolL / 0xFFFF;
                    *dst++ = (WORD)dw;
                }
            }
        }
        //�W�r�b�g�X�e���I�̏ꍇ
        else {
            //�����o�b�t�@�ւ̃|�C���^���擾���܂��B
            PBYTE src = (PBYTE)pBuffer;
            PBYTE dst = src;
            for (int i = 0; i<nSamples; i++) {
                //�����ʂ��ő�ł͂Ȃ��ꍇ�B
                if (dwVolL<0xFFFF) {
                    BYTE b = *src++;
                    //�����ʂ��|���č��o�͉������Z�o���܂��B
                    *dst++ = (BYTE)(b*dwVolL / 0xFFFF);
                }
                //�����ʂ��ő�̏ꍇ�́A���������Ƀ|�C���^�����i�߂܂��B
                else {
                    src++;
                    dst++;
                }
                //�E���ʂ��ő�ł͂Ȃ��ꍇ�B
                if (dwVolR<0xFFFF) {
                    BYTE b = *src++;
                    //�E���ʂ��|���ĉE�o�͉������Z�o���܂��B
                    *dst++ = (BYTE)(b*dwVolR) / 0xFFFF;
                }
                //�E���ʂ��ő�̏ꍇ�́A���������Ƀ|�C���^�����i�߂܂��B
                else {
                    src++;
                    dst++;
                }
            }
        }
        break;

    case 4: //�S�o�C�g�̏ꍇ�B
            //�����o�b�t�@�ւ̃|�C���^���擾���܂��B
        WORD* src = (WORD*)pBuffer;
        WORD* dst = src;
        for (int i = 0; i<nSamples; i++) {
            //�����ʂ��ő�ł͂Ȃ��ꍇ�B
            if (dwVolL<0xFFFF) {
                DWORD dw = (DWORD)*src++;
                //�����ʂ��|���č��o�͉������Z�o���܂��B
                if (dw & 0x8000) {    //�����̏ꍇ
                    dw |= 0xFFFF0000;
                    dw = 0 - abs((LONG)dw)*dwVolL / 0xFFFF;
                }
                else dw = dw*dwVolL / 0xFFFF;
                *dst++ = (WORD)dw;
            }
            //�����ʂ��ő�̏ꍇ�́A���������Ƀ|�C���^�����i�߂܂��B
            else {
                src++;
                dst++;
            }
            //�E���ʂ��ő�ł͂Ȃ��ꍇ�B
            if (dwVolR<0xFFFF) {
                DWORD dw = (DWORD)*src++;
                //�E���ʂ��|���ĉE�o�͉������Z�o���܂��B
                if (dw & 0x8000) {    //�����̏ꍇ
                    dw |= 0xFFFF0000;
                    dw = 0 - abs((LONG)dw)*dwVolR / 0xFFFF;
                }
                else dw = dw*dwVolR / 0xFFFF;
                *dst++ = (WORD)dw;
            }
            //�E���ʂ��ő�̏ꍇ�́A���������Ƀ|�C���^�����i�߂܂��B
            else {
                src++;
                dst++;
            }
        }
        break;
    }
    return TRUE;
}