#include "MMD/ManageMMD.h"

#include "Util/WinUtil.h"

#include "State/WaitState.h"
#include "State/RhythmState.h"
#include "State/ReadState.h"
#include "State/DanceState.h"

#include "Sound/fft.h"

using namespace std;


HRESULT ManageMMD::Initialize()
{
    m_Window.Init();

    SetUserWindow(m_Window.GetHWnd());
    m_mmd = shared_ptr<DrawMMD>(new DrawMMD());
    m_mmd->preInitialize();
    if (DxLib::DxLib_Init() == -1) return E_FAIL;
    m_mmd->afterInitialize();

    stateManager = shared_ptr< StateManager<EState> >(new StateManager<EState>());
    m_mmd->SetStateManager(stateManager);

    m_Window.SetDrawFunc([&](HDC hdc)
    {
        m_mmd->mainProcess();


        /// �L�[���͔���
        if (IsPress(VK_SHIFT) != 0)
        {
            if (IsPress(VK_RIGHT))
            {
                m_mmd->RotateY -= .1f;
            }

            if (IsPress(VK_LEFT))
            {
                m_mmd->RotateY += .1f;
            }

            if (IsPress(VK_UP))
            {
                m_mmd->Zoom -= .1f;
            }

            if (IsPress(VK_DOWN))
            {
                m_mmd->Zoom += .1f;
            }
            return;
        }

        auto pos = m_mmd->GetCharactorPos();
        if (IsPress(VK_RIGHT))
        {
            pos.x += .1f;
        }

        if (IsPress(VK_LEFT))
        {
            pos.x -= .1f;
        }

        if (IsPress(VK_UP))
        {
            pos.y += .1f;
        }

        if (IsPress(VK_DOWN))
        {
            pos.y -= .1f;
        }
        m_mmd->SetCharactorPos(pos);
    });

    m_Window.SetCallbackCommand([&](WPARAM wParam, LPARAM lParam)
    {
        switch ((EContextMenu)LOWORD(wParam)) {
        case EContextMenu::CONTEXT_EXIT: /* Exit���j���[ */
            SendMessageA(m_Window.GetHWnd(), WM_CLOSE, 0, 0);
            break;

        case EContextMenu::CONTEXT_MODE_WAIT:
            stateManager->Transrate(EState::STATE_WAIT);
            break;

        case EContextMenu::CONTEXT_MODE_RHYTHM:
            stateManager->Transrate(EState::STATE_RHYTHM);
            break;

        case EContextMenu::CONTEXT_MODE_READ:
            stateManager->Transrate(EState::STATE_READ);
            break;

        case EContextMenu::CONTEXT_MODE_DANCE:
            stateManager->Transrate(EState::STATE_DANCE);
            break;
        }
    });


    /// �^���f�o�C�X������
    WAVEFORMATEX wf;
    wf.wFormatTag = WAVE_FORMAT_PCM;       // PCM�`��
    wf.nChannels = 1;                      // �X�e���I�����m������
    wf.nSamplesPerSec = 22050;             // �T���v�����O���[�g 22050KHz
    wf.wBitsPerSample = 16;                // �ʎq�����x��
    wf.nBlockAlign = wf.wBitsPerSample * wf.nChannels / 8;      // �o�C�g������̃r�b�g��[PCM�̎d�l]
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;    // 1 �b������̃o�C�g��
                                                                // ���m����*�T���v��22050*�ʎq��16bit=1byte�Ȃ̂� ��b�Ԃ�44100byte�̃f�[�^����������
                                                                // �T���v�� * nBlockAlign = ��b�Ԃ̃f�[�^�ʂȂ̂� nBlockAlign��2

    m_Capture = shared_ptr<CaptureSound>(new CaptureSound());
    m_Capture->OpenDevice(0, wf, 4410 * 4);


    /// �����o�͏�����
    m_Output = shared_ptr<OutputSound>(new OutputSound());

    /// State������
    shared_ptr<State> wait(new WaitState());
    stateManager->AddState(EState::STATE_WAIT, move(wait));

    shared_ptr<State> rhythm(new RhythmState());
    auto rhythmPtr = (RhythmState*)rhythm.get();
    rhythmPtr->SetModel(m_mmd->GetModelHandle());
    rhythmPtr->OnceInital();
    stateManager->AddState(EState::STATE_RHYTHM, move(rhythm));

    shared_ptr<State> read(new ReadState());
    auto readPtr = (ReadState*)read.get();
    readPtr->SetOutputSound(m_Output);
    readPtr->SetManager(this);
    stateManager->AddState(EState::STATE_READ, move(read));

    shared_ptr<State> dance(new DanceState());
    auto dancePtr = (DanceState*)dance.get();
    dancePtr->SetOutputSound(m_Output);
    stateManager->AddState(EState::STATE_DANCE, dance);

    LoadModel();

    stateManager->Transrate(EState::STATE_WAIT);

    return S_OK;
}

HRESULT ManageMMD::Process()
{
    m_Window.Process(60);

    return S_OK;
}

void ManageMMD::Exit()
{
    // TODO: �L�����N�^�ʒu�ۑ�
    m_Window.SetDrawFunc(nullptr);
    stateManager->End();

    m_Output->Stop();
    m_Output->CloseDevice();

    m_Capture->Stop();
    m_Capture->CloseDevice();

    DxLib::DxLib_End(); // �c�w���C�u�����g�p�̏I������
}

void ManageMMD::LoadModel()
{
    m_mmd->LoadModel();
    auto map = stateManager->GetStateMap();
    for (auto& s : map)
    {
        auto sMMD = (StateMMD*)s.second.get();
        sMMD->SetModel(m_mmd->GetModelHandle());
        sMMD->ModelInitial();
    }
}

bool ManageMMD::IsRunning()
{
    return !m_Window.IsClose();
}


void ManageMMD::DrawFFT(WAVEFORMATEX wf)
{
    const int NUM_FREQUENCY_BUNDLES = 10;

    const int FFT_LENGTH = 2048;
    vector< int > ids;
    int n_level = lc_fft_calc_ids(FFT_LENGTH, &ids);

    const int devideNum = 11;
    chrono::milliseconds wait(3);
    float prev[22];
    memset(prev, 0, sizeof(float) * 22);
    auto Cr = GetColor(0, 0, 255);        // �F�̒l���擾
    auto eachWidth = 1920 / devideNum;
    auto halfHeight = 1080 / 2;
    thread t([&prev, devideNum, wait, Cr, eachWidth, halfHeight]()
    {
        while (true)
        {
            DxLib::ClearDrawScreen();
            for (int i = 0; i < devideNum; i++)
            {
                DrawBox(i*eachWidth, 1080 - prev[i], i*eachWidth + eachWidth - 2, 1030, Cr, TRUE);    // �l�p�`��`��
            }
            DxLib::ScreenFlip();
            this_thread::sleep_for(wait);
        }
    });

    m_Capture->SetCaptureCallback([&](WAVEHDR wh)
    {
        vector< complex<double> > signal;
        // setup "signal" as your FFT input.
        for (unsigned int i = 0; i < wh.dwBytesRecorded; i += 2)
        {
            complex<double> val(((short)wh.lpData[i] + (wh.lpData[i + 1] << 8)) * Humming(i, FFT_LENGTH), 0);
            signal.emplace_back(val);
        }

        vector< complex<double> > proj;
        lc_fft(signal, ids, n_level, &proj);// FFT�ϊ�

        for (int k = 0; k < devideNum; k++)
        {
            // �����I�Ȏ��g�������͒���g���ɏW�܂�₷���̂ŁA����g���̉��𒆐S�ɉ𑜓x���グ��
            auto indexFq = ((int)(round(FFT_LENGTH / 2 * pow(0.5, (double)(devideNum - 1 - k) * NUM_FREQUENCY_BUNDLES / devideNum)) - 1) << 1);
            //var indexFq = (int)(FFTLength / 2.0 / (devideNum * devideNum) * k * k);

            //���f���̑傫�����v�Z
            double diagonal = sqrt(proj[indexFq].real() * proj[indexFq].real() + proj[indexFq].imag() * proj[indexFq].imag());

            prev[k] = max((float)diagonal / 800.f, prev[k] - 8);
        }
    });


    m_Capture->OpenDevice(0, wf, FFT_LENGTH * 2);
}
