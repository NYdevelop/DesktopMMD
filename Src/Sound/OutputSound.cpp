#include "OutputSound.h"

#pragma comment(lib, "winmm")

#include <iostream>

#include "ReadMp3.h"
#include "ReadWavFile.h"
#include "ReadWssFile.h"

HRESULT OutputSound::OpenDevice(WAVEFORMATEX* format, UINT deviceIndex)
{
    //! �o�̓f�o�C�X���I�[�v������.
    auto mmRes = waveOutOpen(&hwo, deviceIndex, format, (DWORD_PTR)&Callback, (DWORD_PTR)this, CALLBACK_FUNCTION);
    if (mmRes != MMSYSERR_NOERROR) {
        TCHAR str[MAXERRORLENGTH];
        waveOutGetErrorText(mmRes, str, MAXERRORLENGTH);
        MessageBox(NULL, str, L"output soud open error", MB_ICONERROR);
        return E_FAIL;
    }
    return S_OK;
}

void OutputSound::Start(const std::wstring& fileName, UINT deviceIndex)
{
    auto dotPos = fileName.find_last_of(L".");
    if (dotPos == std::string::npos)
    {
        std::cerr << "file name error" << std::endl;
        throw "file name error";
    }
    auto ext = fileName.substr(dotPos+1);
    if (ext == L"wav")
    {
        m_FileReader = std::unique_ptr<IReadFile>(new ReadWavFile());
    }
    else if (ext == L"mp3")
    {
        m_FileReader = std::unique_ptr<IReadFile>(new ReadMp3());
    }
    else if (ext == L"wss")
    {
        m_FileReader = std::unique_ptr<IReadFile>(new ReadWssFile());
    }

    StartWavFile(fileName, deviceIndex);
}

void OutputSound::StartWavFile(const std::wstring & fileName, UINT deviceIndex)
{
    m_IsStop = false;

    if (m_FileReader->Open(fileName) == FALSE)
    {
        MessageBox(NULL, TEXT("file open error"), NULL, MB_ICONERROR);
        return;
    }

    m_FileWaveFormat = m_FileReader->m_WaveFormat;

    StartWav(deviceIndex);
}

void OutputSound::StartWav(UINT deviceIndex)
{
    OpenDevice(&m_FileWaveFormat, deviceIndex);
    m_ReadSize = m_FileWaveFormat.nAvgBytesPerSec / READ_HZ;
    InitializeBuffer(m_ReadSize);

    ReadNext();
    InputData();

    ReadNext();
    Sleep(1000 / READ_HZ - 200);
    InputData();
}

void OutputSound::Stop()
{
    if (hwo == nullptr) return;
    m_IsStop = true;
    waveOutReset(hwo);
    m_FileReader->Dispose();
    m_CurrentBuffer = 0;
}

HRESULT OutputSound::CloseDevice()
{
    if (hwo == nullptr) S_OK;
    Sleep(1000 / READ_HZ + 100);
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

void OutputSound::InputData()
{
    if (OutHdr[m_CurrentBuffer].lpData == NULL) return;

    waveOutWrite(hwo, &OutHdr[m_CurrentBuffer], sizeof(WAVEHDR));
    if (++m_CurrentBuffer >= BUFFER_NUM)
    {
        m_CurrentBuffer = 0;
    }
}

int OutputSound::ReadNext()
{
    unsigned long size = m_FileReader->GetData((unsigned char*)OutHdr[m_CurrentBuffer].lpData, m_ReadSize);
    OutHdr[m_CurrentBuffer].dwBufferLength = size;
    if (size == 0)
    {
        return S_FALSE;
    }
    DoVolume(OutHdr[m_CurrentBuffer].lpData, size, &m_FileWaveFormat);
    return S_OK;
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

double OutputSound::GetDuration()
{
    return m_FileReader->m_Size / (double)m_FileWaveFormat.nAvgBytesPerSec;
}

// private
void OutputSound::Callback(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    switch (uMsg) {
    case MM_WOM_DONE:
        OutputSound* outputSound = (OutputSound*)dwInstance;
        if (((WAVEHDR*)dwParam1)->dwUser || outputSound->m_IsStop)
        {    //waveOutReset�֐��Œ�~����
            outputSound->m_CurrentBuffer = 0;
            return;
        }

        if (outputSound->ReadNext() == S_FALSE) break;
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
