#include "MMD/ManageMMD.h"

#include "Util/WinUtil.h"

#include "State/WaitState.h"
#include "State/RhythmState.h"
#include "State/ReadState.h"
#include "State/DanceState.h"
#include "State/WalkState.h"

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

    int beginMousePosX = 0, beginMousePosY = 0;
    m_Window.SetDrawFunc([&](HDC hdc)
    {
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
                m_mmd->Zoom += .1f;
            }

            if (IsPress(VK_DOWN))
            {
                m_mmd->Zoom -= .1f;
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
            // ConvScreenPosToWorldPos に渡す座標の z を 0.0f にした場合は画面の指定の座標のワールド座標での
            // 一番手前(画面に近い側)の座標(カメラから SetCameraNearFar の Near で指定した分だけ離れた距離の位置)を取得することができ、
            // z を 1.0f にした場合は画面の指定の座標のワールド座標での画面奥側の座標(カメラから SetCameraNearFar の Far で
            // 指定した分だけ離れた距離の位置)を取得することができます

            // https://dxlib.xsrv.jp/function/dxfunc_3d_camera.html#R12N11

            int MouseX = 0, MouseY = 0;
            GetMousePoint(&MouseX, &MouseY);
            auto ScreenPos = VGet(0.f, 0.f, 0.f);
            ScreenPos.x = (float)MouseX;
            ScreenPos.y = (float)MouseY;

            ScreenPos.z = 0.0f;
            auto Start3DPos = ConvScreenPosToWorldPos(ScreenPos);

            auto pos = m_mmd->GetCharactorPos();
            walkManager.Start(VGet(Start3DPos.x, pos.y, 1.f));
        }

        if (IsPress(VK_MBUTTON))
        {
            static const float CAMERA_MOVE_SPEED = 44.f;
            int mouseX, mouseY;
            GetMousePoint(&mouseX, &mouseY);

            auto rayVec = m_mmd->cameraPos; //VSub(m_mmd->cameraPos, VGet(0,0,0));
            auto xVec = VNorm(VCross(rayVec, VGet(0,1,0)));
            VECTOR newPos = rayVec;
            if (mouseX - beginMousePosX != 0)
            {
                float s = 1.f;
                if (mouseX - beginMousePosX < 0)
                    s = -1.f;
                newPos = VAdd(m_mmd->cameraPos, VScale(xVec, CAMERA_MOVE_SPEED / m_mmd->Zoom * s));

                beginMousePosX = mouseX;
            }

            if (mouseY - beginMousePosY != 0)
            {
                //rayVec = m_mmd->cameraPos; //VSub(m_mmd->cameraPos, VGet(0,0,0));
                //xVec = VNorm(VCross(rayVec, VGet(0, 1, 0)));

                auto yVec = VNorm(VCross(xVec, rayVec));
                float s = 1.f;
                if (mouseY - beginMousePosY < 0)
                    s = -1.f;
                newPos = VAdd(newPos, VScale(yVec, CAMERA_MOVE_SPEED / m_mmd->Zoom * s));

                beginMousePosY = mouseY;
            }

            newPos = VScale(VNorm(newPos), 10.f);
            m_mmd->cameraPos = newPos;
            return;
        }
    });

    m_Window.SetCallbackWheel([&](WPARAM wParam, LPARAM lParam)
        {
            DWORD fwKeys = GET_KEYSTATE_WPARAM(wParam);	// 同時に押されているキー情報
            int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);	// 回転量

            //int x = GET_X_LPARAM(lParam);	// 回転時のマウスカーソルのクライアント座標Ｘ
            //int y = GET_Y_LPARAM(lParam);	// 回転時のマウスカーソルのクライアント座標Ｙ

            // 前回の端数を追加
            //zDelta += nWheelFraction;

            // ノッチ数を求める
            int nNotch = zDelta / WHEEL_DELTA;

            // 端数を保存する
            //nWheelFraction = zDelta % WHEEL_DELTA;

            if (nNotch > 0)
            {
                // ↑に回転（チルト）した
                m_mmd->Zoom -= .5f;
            }
            else if (nNotch < 0)
            {
                // ↓に回転（チルト）した
                m_mmd->Zoom += .5f;
            }
        });

    m_Window.SetCallbackCommand([&](WPARAM wParam, LPARAM lParam)
    {
        switch ((EContextMenu)LOWORD(wParam)) {
        case EContextMenu::CONTEXT_EXIT: /* Exitメニュー */
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

        case EContextMenu::CONTEXT_MOVE_LEFT:
        {
            auto ScreenPos = VGet(100.f, 0.f, 0.f);
            auto worldPos = ConvScreenPosToWorldPos(ScreenPos);

            auto pos = m_mmd->GetCharactorPos();
            walkManager.Start(VGet(worldPos.x, pos.y, 1.f));
        }
            break;

        case EContextMenu::CONTEXT_MOVE_RIGHT:
        {
            auto ScreenPos = VGet(1920.f - 100.f, 0.f, 0.f);
            auto worldPos = ConvScreenPosToWorldPos(ScreenPos);

            auto pos = m_mmd->GetCharactorPos();
            walkManager.Start(VGet(worldPos.x, pos.y, 1.f));
        }
            break;

        case EContextMenu::CONTEXT_MOVE_OVER:
        {
            auto pos = m_mmd->GetCharactorPos();
            walkManager.Start(VGet(11.f, pos.y, 11.f));
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

    /// State初期化
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
    dancePtr->SetDrawMMD(m_mmd);
    stateManager->AddState(EState::STATE_DANCE, dance);

    shared_ptr<State> walk(new WalkState());
    auto walkPtr = (WalkState*)walk.get();
    walkPtr->SetDrawMMD(m_mmd);
    stateManager->AddState(EState::STATE_WALK, walk);
    walkManager.Initialize(walkPtr, m_mmd, stateManager);
    walkManager.SetNextState(EState::STATE_WAIT);

    LoadModel();

    stateManager->SetState(EState::STATE_WAIT);

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
