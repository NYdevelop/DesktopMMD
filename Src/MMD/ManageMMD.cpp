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

#include "Util\rapidxml-1.13\rapidxml_utils.hpp"
#include "Util\UtilXml.h"
#include "Util\ConfigLoader.h"

using namespace std;

HRESULT ManageMMD::Initialize()
{
    m_Window.Init();
    SetUserWindow(m_Window.GetHWnd());

    // コンテキストメニューの初期化
    rapidxml::xml_document<> doc;
    rapidxml::file<> input("config.xml");
    doc.parse<0>(input.data());
    auto base = doc.first_node("DesktopMMD");

    std::vector<std::tuple<HMENU, ULONG, UINT, std::wstring>> contextMenuConfig;
    auto contextNodeVec = LoadContextNode(base->first_node("menu")->first_node(), CreatePopupMenu(), contextMenuConfig);
    m_Window.InitContextMenu(contextMenuConfig);

    static const auto ANIM_PATH = GetAttribute(base->first_node("config")->first_node("anim"), "path");
    auto modelNode = base->first_node("model")->first_node();
    //for (int i = 1; i < 13; i++) modelNode = modelNode->next_sibling();
    static const auto modelPath = GetAttribute(modelNode, "path");
    CConfigLoader configPos("config_pos.txt");
    if (configPos.IsOpen())
    {
        m_mmd = shared_ptr<DrawMMD>(new DrawMMD(ANIM_PATH, modelPath,
            std::stof(configPos.Load("CHARA_POS_X")), std::stof(configPos.Load("CHARA_POS_Y")), std::stof(configPos.Load("CHARA_POS_Z")),
            std::stof(configPos.Load("CHARA_DIRECT")),
            std::stof(configPos.Load("CAMERA_POS_X")), std::stof(configPos.Load("CAMERA_POS_Y")), std::stof(configPos.Load("CAMERA_POS_Z"))));
    }
    else
    {
        m_mmd = shared_ptr<DrawMMD>(new DrawMMD(ANIM_PATH, modelPath));
    }
    if (DxLib::DxLib_Init() == -1) return E_FAIL;
    m_mmd->afterInitialize();

    stateManager = shared_ptr< StateManager<EState> >(new StateManager<EState>());
    m_mmd->SetStateManager(stateManager);

    // 外部操作初期化
    {
        m_Window.SetCallbackMsg(WM_MOUSEWHEEL, [&](WPARAM wParam, LPARAM lParam)
        {
            static const float VOLUME_DELTA = 0.05f;
            int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);	// 回転量
            int nNotch = zDelta / WHEEL_DELTA;    // ノッチ数を求める

            if (IsPress(VK_CONTROL))
            {
                auto v = m_Output->GetVolume();
                if (nNotch > 0)
                {
                    // ↑に回転（チルト）した
                    v += VOLUME_DELTA;
                    m_Output->SetVolume(v);
                }
                else if (nNotch < 0)
                {
                    // ↓に回転（チルト）した
                    v -= VOLUME_DELTA;
                    m_Output->SetVolume(v);
                }
                cout << "volume:" << v << endl;
                return;
            }

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

        // コンテキストメニュー操作設定
        static std::map<UINT, function<void(UINT)>> contextCommand;
        {
            static auto searchContextId = [](const std::vector<std::tuple<HMENU, ULONG, UINT, std::wstring>>& contextMenuConfig, const std::wstring& menuName)
            {
                auto itr = find_if(contextMenuConfig.begin(), contextMenuConfig.end(), [&menuName](auto& x) { return std::get<3>(x) == menuName; });
                if (itr == contextMenuConfig.end()) throw "not find...";
                return std::get<2>(*itr);
            };

            contextCommand[searchContextId(contextMenuConfig, L"Exit")] = [&](UINT)
            {
                m_Window.Close();
                SendMessageA(m_Window.GetHWnd(), WM_CLOSE, 0, 0);
            };

            // モード変更
            {
                auto modeChangeFunc = [&](UINT, EState mode)
                {
                    stateManager->Transrate(mode);
                };
                contextCommand[searchContextId(contextMenuConfig, L"Wait")] = std::bind(modeChangeFunc, std::placeholders::_1, EState::STATE_WAIT);
                contextCommand[searchContextId(contextMenuConfig, L"Rhythm")] = std::bind(modeChangeFunc, std::placeholders::_1, EState::STATE_RHYTHM);
                contextCommand[searchContextId(contextMenuConfig, L"Read")] = std::bind(modeChangeFunc, std::placeholders::_1, EState::STATE_READ);
                contextCommand[searchContextId(contextMenuConfig, L"手を振る")] = std::bind(modeChangeFunc, std::placeholders::_1, EState::STATE_WAVE_HAND);
            }

            // 移動指示
            {
                auto moveFunc = [&](UINT, float pos)
                {
                    auto CharaScreenPos = ConvWorldPosToScreenPos(m_mmd->GetCharactorPos());
                    WalkStart(VGet(pos, CharaScreenPos.y, -1), m_mmd.get(), &walkManager);
                };
                contextCommand[searchContextId(contextMenuConfig, L"左端へ移動")] = std::bind(moveFunc, std::placeholders::_1, 100.f);
                contextCommand[searchContextId(contextMenuConfig, L"右端へ移動")] = std::bind(moveFunc, std::placeholders::_1, 1920.f - 100.f);
            }
            contextCommand[searchContextId(contextMenuConfig, L"ランダム移動")] =
                std::bind(ContextCheckFunc, std::placeholders::_1, m_Window.GetContextMenu(), [&](bool isCheck)
            {
                ((WaitState*)(stateManager->GetStateMap()[EState::STATE_WAIT].get()))->SetRandomMove(isCheck);
            });

            // ライト設定
            {
                auto lightMode = GetAttrVal<bool>(base->first_node("config")->first_node("light"), "mode", false);
                m_mmd->SetEnableLight(lightMode);
                const auto lightContextId = searchContextId(contextMenuConfig, L"Light");
                ContextItemCheck(lightContextId, m_Window.GetContextMenu(), lightMode);
                contextCommand[lightContextId] =
                    std::bind(ContextCheckFunc, std::placeholders::_1, m_Window.GetContextMenu(), [&](bool isCheck)
                {
                    m_mmd->SetEnableLight(isCheck);
                });
            }

            // dance選択コマンド
            for (size_t i = 0; i<contextNodeVec.size(); i++)
            {
                if (contextNodeVec[i] != "dance") continue;
                auto contextConfig = contextMenuConfig[i];
                static const int FIRST_DANCE_INDEX = std::get<2>(contextConfig);
                contextCommand[std::get<2>(contextConfig)] = [&](UINT id)
                {
                    ((DanceState*)(stateManager->GetStateMap()[EState::STATE_DANCE].get()))->DanceIndex = (id - FIRST_DANCE_INDEX);
                    stateManager->Transrate(EState::STATE_DANCE);
                };
            }

            // model選択コマンド
            static std::vector<std::string> models;
            NodeApply(base->first_node("model")->first_node(), [&](auto child)
            {
                models.emplace_back(GetAttribute(child, "path"));
            });
            for (size_t i = 0; i < contextNodeVec.size(); i++)
            {
                if (contextNodeVec[i] != "model") continue;
                auto contextConfig = contextMenuConfig[i];
                static const int FIRST_MODEL_ID = std::get<2>(contextConfig);
                contextCommand[std::get<2>(contextConfig)] = [&](UINT id)
                {
                    stateManager->Transrate(EState::STATE_WAIT);

                    std::lock_guard<std::mutex> lock(modelMutex);
                    m_mmd->Exit();
                    m_mmd = nullptr;

                    CConfigLoader configPos("config_pos.txt");
                    if (configPos.IsOpen())
                    {
                        m_mmd = shared_ptr<DrawMMD>(new DrawMMD(
                            ANIM_PATH,
                            models[id - FIRST_MODEL_ID],
                            std::stof(configPos.Load("CHARA_POS_X")), std::stof(configPos.Load("CHARA_POS_Y")), std::stof(configPos.Load("CHARA_POS_Z")),
                            std::stof(configPos.Load("CHARA_DIRECT")),
                            std::stof(configPos.Load("CAMERA_POS_X")), std::stof(configPos.Load("CAMERA_POS_Y")), std::stof(configPos.Load("CAMERA_POS_Z"))));
                    }
                    m_mmd->afterInitialize();
                    m_mmd->SetStateManager(stateManager);
                    InitStateModel();
                    stateManager->Initialize(EState::STATE_WAIT);
                };
            }
        }

        m_Window.SetCallbackCommand([&](WPARAM wParam, LPARAM lParam)
        {
            walkManager.Cancel();
            auto id = (UINT)LOWORD(wParam);
            if (!contextCommand.empty() && contextCommand.find(id) != contextCommand.end())
            {
                contextCommand[id](id);
            }
        });

        InitMouseDrag();
    }

    m_Window.SetDrawFunc([&](HDC hdc)
    {
        if (m_Window.IsClose()) return;
        /// アニメ適用
        {
            std::lock_guard<std::mutex> lock(modelMutex);
            if (m_mmd->mainProcess() == S_FALSE) return;
            walkManager.Update();
            if (stateManager->GetCurrentStateIndex() != EState::STATE_DANCE)
            {
                animManager->Play();
            }
        }

        /// キー入力判定
        InputKey();
    });

    /// 録音デバイス初期化
    {
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
        auto isCapture = GetAttrVal(base->first_node("config")->first_node("capture"), "mode", false);
        if (isCapture) m_Capture->OpenDevice(0, wf, (wf.nSamplesPerSec / 5) * 4);
    }

    /// 音声出力初期化
    m_Output = shared_ptr<OutputSound>(new OutputSound());

    animManager = std::shared_ptr< ActionManager >(new ActionManager);

    /// State初期化
    InitStateModel();

    stateManager->Initialize(EState::STATE_WAIT);
    return S_OK;
}

HRESULT ManageMMD::Process()
{
    return m_Window.Process(60);
}

void ManageMMD::Exit()
{
    // キャラクタ位置保存
    m_mmd->Exit();

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
        sMMD->SetAnimManager(animManager);
        sMMD->ModelInitial();
    }

    {
        std::shared_ptr<PlayAnim> blink(new PlayAnim);
        blink->AttachAnime(m_mmd->GetModelHandle(), (int)EAnimIndex::ANIM_BLINK);
        blink->SetMaximumTime(250.f);
        blink->IsLoop(true);
        MV1SetAttachAnimBlendRate(m_mmd->GetModelHandle(), blink->GetAnimIndex(), 1);
        animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_BLINK)->AddAnim(blink);
    }

    {
        std::shared_ptr<PlayAnim> breath(new PlayAnim);
        breath->AttachAnime(m_mmd->GetModelHandle(), (int)EAnimIndex::ANIM_BREATH);
        breath->SetMaximumTime(120.f);
        breath->IsLoop(true);
        MV1SetAttachAnimBlendRate(m_mmd->GetModelHandle(), breath->GetAnimIndex(), 1);
        animManager->GetAnimQueue(ActionManager::EAnimQueue::QUEUE_BREATH)->AddAnim(breath);
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
    stateManager->Clear();

    shared_ptr<State> wait(new WaitState());
    auto waitPtr = (WaitState*)wait.get();
    waitPtr->SetDrawMMD(m_mmd);
    waitPtr->SetWalkStateManager(&walkManager);
    stateManager->AddState(EState::STATE_WAIT, move(wait));

    shared_ptr<State> rhythm(new RhythmState());
    auto rhythmPtr = (RhythmState*)rhythm.get();
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
    stateManager->AddState(EState::STATE_DANCE, dance);

    shared_ptr<State> walk(new WalkState());
    auto walkPtr = (WalkState*)walk.get();
    walkPtr->SetDrawMMD(m_mmd);
    stateManager->AddState(EState::STATE_WALK, walk);

    shared_ptr<State> waveHand(new WaveHandState());
    auto waveHandPtr = (WaveHandState*)waveHand.get();
    waveHandPtr->SetDrawMMD(m_mmd);
    stateManager->AddState(EState::STATE_WAVE_HAND, waveHand);

    LoadModel();

    walkManager.Initialize(m_mmd, stateManager, walkPtr->GetWalkAnimIndex());
    walkManager.SetNextState(EState::STATE_WAIT);

    waitPtr->OnceInitial();
}

void ManageMMD::InitMouseDrag()
{
    /// マウス左ドラッグ操作
    {
        VECTOR xVecLButton, yVecLButton;
        mouseDrag.SetButtonFunc(VK_LBUTTON,
            [&](LONG, LONG)
        {
            auto rayVec = m_mmd->GetRayVec();
            xVecLButton = VNorm(VCross(rayVec, VGet(0, 1, 0)));
            yVecLButton = VNorm(VCross(xVecLButton, rayVec));
        },
            [&](LONG diffX, LONG diffY)
        {
            VECTOR newPos = m_mmd->cameraPos;
            if (diffX != 0)
            {
                auto diff = VScale(xVecLButton, diffX / sqrt(m_mmd->GetZoom()) / 2.f);
                newPos = VAdd(newPos, diff);
                m_mmd->SetCharactorPos(VAdd(m_mmd->GetCharactorPos(), diff));
            }

            if (diffY != 0)
            {
                auto diff = VScale(yVecLButton, -diffY / sqrt(m_mmd->GetZoom()) / 2.f);
                newPos = VAdd(newPos, diff);
                m_mmd->SetCharactorPos(VAdd(m_mmd->GetCharactorPos(), diff));
            }
            m_mmd->cameraPos = newPos;
        });
    }

    /// マウス中ドラッグ操作
    {
        mouseDrag.SetButtonFunc(VK_MBUTTON,
            [](LONG, LONG) {},
            [&](LONG diffX, LONG diffY)
        {
            static const float CAMERA_MOVE_SPEED_A = .001f;
            static const float CAMERA_MOVE_SPEED_B = .624f;

            auto rayVec = m_mmd->GetRayVec();
            auto xVec = VNorm(VCross(rayVec, VGet(0, 1, 0)));
            VECTOR newPos = rayVec;
            if (diffX != 0)
            {
                float s = 1.f;
                if (diffX < 0)
                    s = -1.f;
                newPos =
                    VAdd(
                        m_mmd->cameraPos,
                        VScale(xVec, (CAMERA_MOVE_SPEED_A * pow(m_mmd->GetZoom(), 1.8f) + CAMERA_MOVE_SPEED_B) * s));
            }

            if (diffY != 0)
            {
                auto yVec = VNorm(VCross(xVec, rayVec));
                float s = 1.f;
                if (diffY < 0)
                    s = -1.f;
                newPos =
                    VAdd(
                        newPos,
                        VScale(yVec, (CAMERA_MOVE_SPEED_A * pow(m_mmd->GetZoom(), 1.8f) + CAMERA_MOVE_SPEED_B) * s));
            }

            newPos = VScale(VNorm(newPos), m_mmd->GetZoom());
            m_mmd->cameraPos = newPos;
        });
    }

    /// ドラッグ開始登録
    auto mouseClickFunc = [&](WPARAM, LPARAM, unsigned char button)
    {
        mouseDrag.ButtonDown(button);
    };
    m_Window.SetCallbackMsg(WM_LBUTTONDOWN, std::bind(mouseClickFunc, std::placeholders::_1, std::placeholders::_2, VK_LBUTTON));
    m_Window.SetCallbackMsg(WM_MBUTTONDOWN, std::bind(mouseClickFunc, std::placeholders::_1, std::placeholders::_2, VK_MBUTTON));
}

void ManageMMD::InputKey()
{
    if (IsPress(VK_CONTROL) == true)
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

    if (IsPress(VK_HOME))
    {
        int MouseX = 0, MouseY = 0;
        GetMousePoint(&MouseX, &MouseY);
        WalkStart(VGet((float)MouseX, (float)MouseY, -1), m_mmd.get(), &walkManager);
    }

    mouseDrag.Update();
}
