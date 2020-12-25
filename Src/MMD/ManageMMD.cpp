#include "MMD/ManageMMD.h"

#include "Util/WinUtil.h"
#include "Util/UtilMMD.h"

#include "State/WaitState.h"
#include "State/RhythmState.h"
#include "State/ReadState.h"
#include "State/DanceState.h"
#include "State/WalkState.h"
#include "State/WaveHandState.h"

#include "Sound/fft.h"

using namespace std;


HRESULT ManageMMD::Initialize(const std::string& animPath, const std::string& modelPath)
{
    m_Window.Init();

    SetUserWindow(m_Window.GetHWnd());
    m_mmd = shared_ptr<DrawMMD>(new DrawMMD(animPath, modelPath));
    if (DxLib::DxLib_Init() == -1) return E_FAIL;
    m_mmd->afterInitialize();

    stateManager = shared_ptr< StateManager<EState> >(new StateManager<EState>());
    m_mmd->SetStateManager(stateManager);

    bool isPressMButton = false;
    int beginMousePosX = 0, beginMousePosY = 0;
    m_Window.SetDrawFunc([&](HDC hdc)
    {
        if (m_Window.IsClose()) return;
        m_mmd->mainProcess();
        walkManager.Update();

        /// キー入力判定
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
                m_mmd->SetZoom(m_mmd->GetZoom() + .1f);
            }

            if (IsPress(VK_DOWN))
            {
                m_mmd->SetZoom(m_mmd->GetZoom() - .1f);
            }
            return;
        }

        auto pos = m_mmd->GetCharactorPos();
        if (IsPress(VK_UP))
        {
            pos.y += .1f;
        }

        if (IsPress(VK_DOWN))
        {
            pos.y -= .1f;
        }
        m_mmd->SetCharactorPos(pos);

        if (IsPress('M'))
        {
            int MouseX = 0, MouseY = 0;
            GetMousePoint(&MouseX, &MouseY);
            WalkStart(VGet((float)MouseX, (float)MouseY, -1), m_mmd.get(), &walkManager);
        }

        // モデル上にカーソルがある場合反応するように
        if (!IsPress(VK_MBUTTON))
        {
            isPressMButton = false;
        }

        if (isPressMButton)
        {
            static const float CAMERA_MOVE_SPEED_A = .001f;
            static const float CAMERA_MOVE_SPEED_B = .624f;
            int mouseX, mouseY;
            GetMousePoint(&mouseX, &mouseY);

            auto rayVec = m_mmd->GetRayVec();
            auto xVec = VNorm(VCross(rayVec, VGet(0,1,0)));
            VECTOR newPos = rayVec;
            if (mouseX - beginMousePosX != 0)
            {
                float s = 1.f;
                if (mouseX - beginMousePosX < 0)
                    s = -1.f;
                newPos =
                    VAdd(
                        m_mmd->cameraPos,
                        VScale(xVec, (CAMERA_MOVE_SPEED_A * pow(m_mmd->GetZoom(), 1.8f) + CAMERA_MOVE_SPEED_B) * s));

                beginMousePosX = mouseX;
            }

            if (mouseY - beginMousePosY != 0)
            {
                auto yVec = VNorm(VCross(xVec, rayVec));
                float s = 1.f;
                if (mouseY - beginMousePosY < 0)
                    s = -1.f;
                newPos =
                    VAdd(
                        newPos,
                        VScale(yVec, (CAMERA_MOVE_SPEED_A * pow(m_mmd->GetZoom(), 1.8f) + CAMERA_MOVE_SPEED_B) * s));

                beginMousePosY = mouseY;
            }

            newPos = VScale(VNorm(newPos), m_mmd->GetZoom());
            m_mmd->cameraPos = newPos;
            return;
        }
    });

    m_Window.SetCallbackMsg(WM_MOUSEWHEEL, [&](WPARAM wParam, LPARAM lParam)
    {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);	// 回転量

                                                        // ノッチ数を求める
        int nNotch = zDelta / WHEEL_DELTA;

        if (nNotch > 0)
        {
            // ↑に回転（チルト）した
            m_mmd->SetZoom(m_mmd->GetZoom() - 5.f);
        }
        else if (nNotch < 0)
        {
            // ↓に回転（チルト）した
            m_mmd->SetZoom(m_mmd->GetZoom() + 5.f);
        }
    });

    m_Window.SetCallbackMsg(WM_MBUTTONDOWN, [&](WPARAM wParam, LPARAM lParam)
    {
        isPressMButton = true;
    });

    m_Window.SetCallbackCommand([&](WPARAM wParam, LPARAM lParam)
    {
        walkManager.Cancel();
        switch ((EContextMenu)LOWORD(wParam)) {
        case EContextMenu::CONTEXT_EXIT: /* Exitメニュー */
            m_Window.Close();
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

        case EContextMenu::CONTEXT_MODE_WAVE_HAND:
            stateManager->Transrate(EState::STATE_WAVE_HAND);
            break;

        case EContextMenu::CONTEXT_MOVE_LEFT:
        {
            auto CharaScreenPos = ConvWorldPosToScreenPos(m_mmd->GetCharactorPos());
            WalkStart(VGet(100.f, CharaScreenPos.y, -1), m_mmd.get(), &walkManager);
        }
            break;

        case EContextMenu::CONTEXT_MOVE_RIGHT:
        {
            auto CharaScreenPos = ConvWorldPosToScreenPos(m_mmd->GetCharactorPos());
            WalkStart(VGet(1920.f - 100.f, CharaScreenPos.y, -1), m_mmd.get(), &walkManager);
        }
            break;

        case EContextMenu::CONTEXT_MOVE_RANDOM:
        {
            UINT id = (UINT)EContextMenu::CONTEXT_MOVE_RANDOM;
            auto waitState = (WaitState*)stateManager->GetStateMap()[EState::STATE_WAIT].get();

            //チェック状態取得
            auto uState = GetMenuState(m_Window.GetContextMenu(), id, MF_BYCOMMAND);

            if (uState & MFS_CHECKED)
            {
                //チェックはずす
                waitState->SetRandomMove(false);
                CheckMenuItem(m_Window.GetContextMenu(), id, MF_BYCOMMAND | MFS_UNCHECKED);
                return;
            }

            //チェックする
            waitState->SetRandomMove(true);
            CheckMenuItem(m_Window.GetContextMenu(), id, MF_BYCOMMAND | MFS_CHECKED);
        }
        break;
        }
    });


    /// 録音デバイス初期化
    WAVEFORMATEX wf;
    wf.wFormatTag = WAVE_FORMAT_PCM;       // PCM形式
    wf.nChannels = 1;                      // ステレオかモノラルか
    wf.nSamplesPerSec = 22050;             // サンプリングレート 22050KHz
    wf.wBitsPerSample = 16;                // 量子化レベル
    wf.nBlockAlign = wf.wBitsPerSample * wf.nChannels / 8;      // バイトあたりのビット数[PCMの仕様]
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;    // 1 秒あたりのバイト数
                                                                // モノラル*サンプル22050*量子化16bit=1byteなので 一秒間に44100byteのデータが発生する
                                                                // サンプル * nBlockAlign = 一秒間のデータ量なので nBlockAlignは2

    m_Capture = shared_ptr<CaptureSound>(new CaptureSound());
    m_Capture->OpenDevice(0, wf, 4410 * 4);


    /// 音声出力初期化
    m_Output = shared_ptr<OutputSound>(new OutputSound());

    animQueue = std::shared_ptr< PlayAnimQueue >(new PlayAnimQueue);
    m_mmd->SetAnimQueue(animQueue);

    /// State初期化
    InitStateModel();

    stateManager->Initialize(EState::STATE_WAIT);
    return S_OK;
}

HRESULT ManageMMD::Process()
{
    m_Window.Process(60);

    return S_OK;
}

void ManageMMD::Exit()
{
    // TODO: キャラクタ位置保存
    m_Window.SetDrawFunc(nullptr);
    stateManager->End();

    m_Output->Stop();
    m_Output->CloseDevice();

    m_Capture->Stop();
    m_Capture->CloseDevice();

    DxLib::DxLib_End(); // ＤＸライブラリ使用の終了処理
}

void ManageMMD::LoadModel()
{
    m_mmd->LoadModel();
    auto map = stateManager->GetStateMap();
    for (auto& s : map)
    {
        auto sMMD = (StateMMD*)s.second.get();
        sMMD->SetModel(m_mmd->GetModelHandle());
        sMMD->SetAnimQueue(animQueue);
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
    auto Cr = GetColor(0, 0, 255);        // 青色の値を取得
    auto eachWidth = 1920 / devideNum;
    auto halfHeight = 1080 / 2;
    thread t([&prev, devideNum, wait, Cr, eachWidth, halfHeight]()
    {
        while (true)
        {
            DxLib::ClearDrawScreen();
            for (int i = 0; i < devideNum; i++)
            {
                DrawBox(i*eachWidth, 1080 - prev[i], i*eachWidth + eachWidth - 2, 1030, Cr, TRUE);    // 四角形を描画
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
        lc_fft(signal, ids, n_level, &proj);// FFT変換

        for (int k = 0; k < devideNum; k++)
        {
            // 特徴的な周波数成分は低周波数に集まりやすいので、低周波数の音を中心に解像度を上げる
            auto indexFq = ((int)(round(FFT_LENGTH / 2 * pow(0.5, (double)(devideNum - 1 - k) * NUM_FREQUENCY_BUNDLES / devideNum)) - 1) << 1);
            //var indexFq = (int)(FFTLength / 2.0 / (devideNum * devideNum) * k * k);

            //複素数の大きさを計算
            double diagonal = sqrt(proj[indexFq].real() * proj[indexFq].real() + proj[indexFq].imag() * proj[indexFq].imag());

            prev[k] = max((float)diagonal / 800.f, prev[k] - 8);
        }
    });


    m_Capture->OpenDevice(0, wf, FFT_LENGTH * 2);
}


void ManageMMD::InitStateModel()
{
    shared_ptr<State> wait(new WaitState());
    auto waitPtr = (WaitState*)wait.get();
    waitPtr->SetDrawMMD(m_mmd);
    waitPtr->SetWalkStateManager(&walkManager);
    stateManager->AddState(EState::STATE_WAIT, move(wait));

    shared_ptr<State> rhythm(new RhythmState());
    auto rhythmPtr = (RhythmState*)rhythm.get();
    rhythmPtr->SetModel(m_mmd->GetModelHandle());
    rhythmPtr->SetCapture(m_Capture);
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
    dancePtr->SetDrawMMD(m_mmd);
    stateManager->AddState(EState::STATE_DANCE, dance);

    shared_ptr<State> walk(new WalkState());
    auto walkPtr = (WalkState*)walk.get();
    walkPtr->SetDrawMMD(m_mmd);
    stateManager->AddState(EState::STATE_WALK, walk);
    walkManager.Initialize(walkPtr, m_mmd, stateManager);
    walkManager.SetNextState(EState::STATE_WAIT);

    shared_ptr<State> waveHand(new WaveHandState());
    auto waveHandPtr = (WaveHandState*)waveHand.get();
    waveHandPtr->SetDrawMMD(m_mmd);
    stateManager->AddState(EState::STATE_WAVE_HAND, waveHand);

    LoadModel();

    waitPtr->OnceInitial();
}
